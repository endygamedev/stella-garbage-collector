#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "runtime.h"
#include "gc.h"

/** Total allocated number of bytes (over the entire duration of the program). */
int total_allocated_bytes = 0;
int total_requested_bytes = 0;

/** Total allocated number of objects (over the entire duration of the program). */
int total_allocated_objects = 0;

int max_allocated_bytes = 0;
int max_allocated_objects = 0;

int total_reads = 0;
int total_writes = 0;

/** Total count of collected garbage collected for the entire duration of program  */
int total_gc_collect = 0;

#define MAX_ALLOC_SIZE (24 * 64)
#define GEN_SIZE_MULTIPLIER 4

#define EXIT_OUT_OF_MEMORY 137

#define MAX_GC_ROOTS 1024

int gc_roots_max_size = 0;
int gc_roots_top = 0;
void **gc_roots[MAX_GC_ROOTS];

#define MAX_CHANGED_NODES 1024
int changed_nodes_top = 0;
void *changed_nodes[MAX_CHANGED_NODES];

space g0_space_from, g1_space_from, g1_space_to;

int generation_count = 2;
generation g0, g1;
generation* generations[] = { &g0, &g1 };

size_t get_stella_object_size(const stella_object* object) {
  const int field_count = STELLA_OBJECT_HEADER_FIELD_COUNT(object->object_header);
  return (1 + field_count) * sizeof(void*);
}

/** Stella Object Wrapper functions. */

size_t get_stella_object_wrapper_size(const stella_object_wrapper* object) {
  const int field_count = STELLA_OBJECT_HEADER_FIELD_COUNT(object->object.object_header);
  return (2 + field_count) * sizeof(void*);
}

stella_object_wrapper* get_object(void* pointer) {
  return pointer - sizeof(void*);
}

stella_object* get_stella_object(stella_object_wrapper* pointer) {
  return &pointer->object;
}

/** Space functions. */

space initialize_generation_space(int generation, int size) {
  space initialized_space = (space){
    .generation = generation,
    .size = size,
    .heap = malloc(size),
    .next = NULL
  };

  assert(initialized_space.heap != NULL);

  initialized_space.next = initialized_space.heap;

  return initialized_space;
}

bool space_contains(space* space, const void* pointer) {
  return pointer >= space->heap && pointer < space->heap + space->size;
}

stella_object_wrapper* space_allocate(space* space, const size_t requsted_size) {
  const size_t total_size = requsted_size + sizeof(void*);

  if (space->next + total_size > space->heap + space->size) {
    return NULL;
  }

  stella_object_wrapper* result = (stella_object_wrapper*)space->next;
  *result = (stella_object_wrapper){
    .forwarded = NULL,
    .object.object_header = 0
  };

  space->next += total_size;

  return result;
}

void print_space(const space* space) {
  printf("Objects:\n");
  for (void* start = space->heap; start < space->next; start += get_stella_object_wrapper_size(start)) {
    const stella_object_wrapper* object = start;
    printf("\tAddress: %-15p | Fields: ", object);

    const int field_count = STELLA_OBJECT_HEADER_FIELD_COUNT(object->object.object_header);
    for (int i = 0; i < field_count; ++i) {
      printf("%-15p", object->object.object_fields[i]);
      if (i < field_count - 1) {
        printf(" ");
      }
    }

    printf("\n");
  }

  printf(
    "From: %-15p | To: %-15p | Size: %d bytes\n",
    space->heap,
    space->heap + space->size,
    space->size
  );
}

/** Generation functions. */

generation initialize_generation(int id, space *from, space *to) {
  return (generation){
    .id = id,
    .from = from,
    .to = to
  };
}

void initialize_generations() {
  if (g0.from != NULL) return;

  g0_space_from = initialize_generation_space(0, MAX_ALLOC_SIZE);

  const int g1_space_size = MAX_ALLOC_SIZE * GEN_SIZE_MULTIPLIER;
  g1_space_from = initialize_generation_space(1, g1_space_size);
  g1_space_to = initialize_generation_space(1, g1_space_size);

  g0 = initialize_generation(0, &g0_space_from, &g1_space_from);
  g1 = initialize_generation(1, &g1_space_from, &g1_space_to);
}

void print_generation(const generation* g) {
  printf("G%d state\n", g->from->generation);
  printf("Collect count %d\n", g->collect_count);
  print_space(g->from);

  if (g->to->generation == g->from->generation) {
    printf("To\n");
    print_space(g->to);
  }
}

bool chase(generation* g, stella_object_wrapper* p) {
  do {
    /** Allocate memory in "to" space for the object. */
    stella_object_wrapper* q = space_allocate(g->to, get_stella_object_size(&p->object));
    if (q == NULL) {
      return false;
    }

    const int field_count = STELLA_OBJECT_HEADER_FIELD_COUNT(p->object.object_header);
    void* r = NULL;

    /** Initialize the allocated object. */
    q->forwarded = NULL;
    q->object.object_header = p->object.object_header;

    /** Copy the fields of the Stella object. */
    for (int i = 0; i < field_count; ++i) {
      q->object.object_fields[i] = p->object.object_fields[i];

      /** Check if the field references an object in the "from" space. */
      if (space_contains(g->from, q->object.object_fields[i])) {
        stella_object_wrapper* forwarded = get_object(q->object.object_fields[i]);

        /** Check if the object needs to be forwarded. */
        if (!space_contains(g->to, forwarded->forwarded)) {
          r = forwarded;
        }
      }
    }

    /** Update the "forwarded" pointer and move to the next object. */
    p->forwarded = q;
    p = r;
  } while(p != NULL);

  return true;
}

void* forward(generation* g, void* pointer) {
  if (!space_contains(g->from, pointer)) {
    return pointer;
  }

  stella_object_wrapper* object = get_object(pointer);
  if (space_contains(g->to, object->forwarded)) {
    return get_stella_object(object->forwarded);
  }

  if (!chase(g, object)) {
    if (g->to->generation == g->from->generation) {
      exit(EXIT_OUT_OF_MEMORY);
    }

    collect(generations[g->to->generation]);

    if (!chase(g, object)) {
      exit(EXIT_OUT_OF_MEMORY);
    }
  }

  return get_stella_object(object->forwarded);
}

void forward_roots(generation* g) {
  for (int i = 0; i < gc_roots_top; ++i) {
      void** root = gc_roots[i];

      /** Forward root pointer */
      *root = forward(g, *root);
  }
}

void forward_object_fields(generation* g, stella_object_wrapper* object) {
    const int field_count = STELLA_OBJECT_HEADER_FIELD_COUNT(object->object.object_header);
    for (int i = 0; i < field_count; ++i) {
        object->object.object_fields[i] = forward(g, object->object.object_fields[i]);
    }
}

void forward_previous_generation_objects(generation* g) {
    for (int i = 0; i < g->id; i++) {
        const generation* previous = generations[i];
        for (void* pointer = previous->from->heap; pointer < previous->from->next; pointer += get_stella_object_wrapper_size(pointer)) {
            stella_object_wrapper* object = pointer;
            forward_object_fields(g, object);
        }
    }
}

void forward_changed_objects(generation* g) {
    for (int i = 0; i < changed_nodes_top; ++i) {
        stella_object_wrapper* obj = changed_nodes[i];
        forward_object_fields(g, obj);
    }

    /** Reset changed nodes counter. */
    changed_nodes_top = 0;
}

void handle_generation_switch(generation* g) {
    if (g->from->generation == g->to->generation) {
        /** Swap "from" and "to" spaces. */
        void* tmp = g->from;
        g->from = g->to;
        g->to = tmp;

        /** Reset "next" to the beginning of the "to" space. */
        g->to->next = g->to->heap; 

        /** Update the previous generation's "to" and "scan". */
        generation* previous = generations[g->from->generation - 1];
        previous->to = g->from;

        /** Start from the beginning (since "from" structure may change). */
        previous->scan = g->from->heap;
    } else {
        generation* current = generations[g->from->generation];
        const generation* next = generations[g->to->generation];
        
        /** Set up the new "from" and "to" pointers */
        current->from->next = g->from->heap;
        current->to = next->from;
    }
}

void scan_and_forward_objects(generation* g) {
    while (g->scan < g->to->next) {
        stella_object_wrapper* object = g->scan;
        forward_object_fields(g, object);
        g->scan += get_stella_object_wrapper_size(object);
    }
}

void collect(generation* g) {
  printf(
    "Before garbage collect G%d number %d\n",
    g->from->generation,
    g->collect_count
  );
  print_gc_state();

  g->collect_count++;
  total_gc_collect++;

  /** Initialize the scan pointer for the current generation's "to" space. */
  g->scan = g->to->next;

  /** Forward all root pointers. */
  forward_roots(g);

  /** Forward objects from previous generations. */
  forward_previous_generation_objects(g);

  /** Forward fields of changed objects. */
  forward_changed_objects(g);

  /** Scan and forward all objects in the current generation's "to" space. */
  scan_and_forward_objects(g);

  /** Handle copying and generation switching logic. */
  handle_generation_switch(g);
}

void* try_allocate(generation* g, const size_t size) {
  void* allocated = space_allocate(g->from, size);
  return allocated == NULL ? NULL : allocated + sizeof(void*);
}

void* gc_alloc(size_t size_in_bytes) {
  initialize_generations();

  void* result = try_allocate(&g0, size_in_bytes);
  if (result == NULL) {
    collect(&g0);

    printf("After garbage collecting:\n");
    print_gc_state();
    print_gc_alloc_stats();

    result = try_allocate(&g0, size_in_bytes);
  }

  if (result == NULL) {
    exit(EXIT_OUT_OF_MEMORY);
  }
  
  total_requested_bytes += size_in_bytes;
  total_allocated_bytes += size_in_bytes + sizeof(void*);
  total_allocated_objects += 1;
  max_allocated_bytes = total_allocated_bytes;
  max_allocated_objects = total_allocated_objects;

  return result;
}

void print_gc_roots() {
  printf("Roots: ");
  for (int i = 0; i < gc_roots_top; ++i) {
    printf("\tIndex: %-5d | Address: %-15p | %-15p\n", i, gc_roots[i], *gc_roots[i]);
  }
}

void print_gc_alloc_stats() {
  printf("Total memory requested:   %d bytes (%d objects)\n", total_requested_bytes, total_allocated_objects);
  printf("Total memory allocation: %'d bytes (%'d objects)\n", total_allocated_bytes, total_allocated_objects);
  printf("Total garbage collecting: %d. G_0: %d. G_1: %d\n", total_gc_collect, g0.collect_count, g1.collect_count);
  printf("Maximum residency:       %'d bytes (%'d objects)\n", max_allocated_bytes, max_allocated_objects);
  printf("Total memory use:        %'d reads and %'d writes\n", total_reads, total_writes);
  printf("Max GC roots stack size: %'d roots\n", gc_roots_max_size);
}

void print_gc_state() {
  print_generation(&g0);
  print_generation(&g1);
  print_gc_roots();
}

void gc_read_barrier(void *object, int field_index) {
  total_reads += 1;
}

void gc_write_barrier(void *object, int field_index, void *contents) {
  total_writes += 1;
}

void gc_push_root(void **ptr){
  gc_roots[gc_roots_top++] = ptr;
  if (gc_roots_top > gc_roots_max_size) { gc_roots_max_size = gc_roots_top; }
}

void gc_pop_root(void **ptr){
  gc_roots_top--;
}
