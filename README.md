stella-garbage-collector
========================

Реализация копирующего сборщика с двумя поколениями G0 и G1.

Зависимости
-----------

- [gcc](https://gcc.gnu.org/)
- [CMake 3.22](https://cmake.org/) или старше
- [Docker](https://www.docker.com/)

Запуск
------

1.  Назначить директорию "build" для сборки проекта:

    ```shell
    cmake -B build
    ```

2.  Транслировать программу на языке Stella в язык C:

    ```shell
    docker run -i fizruk/stella compile < tests/${PROGRAM} > main.c
    ```

    где `${PROGRAM}` -- программа на языке Stella:

    - `tests/factorial.st` -- программа вычисления факториала;
    - `tests/fibonacci.st` -- программа вычисления числа Фибоначчи;
    - `tests/exponent.st` -- программа вычисления экспоненты с основанием 2 (2^n).

3.  Скомпилировать программу со средой выполнения и сборщиком мусора:

    ```shell
    cmake \
        --build build \
        --clean-first
    ```

4.  Запустить результируюущую программу.

    Пример:

    ```
    $ echo 6 | build/stella_garbage_collector 

    Before garbage collect G0 number 0
    G0 state
    Collect count 0
    Objects:
            Address: 0x5c546a65a740  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65a758  | Fields: 0x5c546a65a748 
            Address: 0x5c546a65a770  | Fields: 0x5c546a65a760 
            Address: 0x5c546a65a788  | Fields: 0x5c546a65a778 
            Address: 0x5c546a65a7a0  | Fields: 0x5c546a65a790 
            Address: 0x5c546a65a7b8  | Fields: 0x5c546a65a7a8 
            Address: 0x5c546a65a7d0  | Fields: 0x5c546a65a7c0  0x5c5468eea1e8  0x5c546a65a800 
            Address: 0x5c546a65a7f8  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65a810  | Fields: 0x5c5468ee3a0b 
            Address: 0x5c546a65a828  | Fields: 0x5c5468ee3806 
            Address: 0x5c546a65a840  | Fields: 0x5c546a65a8e8  0x5c546a65a800  0x5c546a65a900 
            Address: 0x5c546a65a868  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65a880  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65a898  | Fields: 0x5c546a65a888 
            Address: 0x5c546a65a8b0  | Fields: 0x5c546a65a8a0 
            Address: 0x5c546a65a8c8  | Fields: 0x5c546a65a8b8 
            Address: 0x5c546a65a8e0  | Fields: 0x5c546a65a8d0 
            Address: 0x5c546a65a8f8  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65a910  | Fields: 0x5c5468ee3806 
            Address: 0x5c546a65a928  | Fields: 0x5c546a65a9b8  0x5c546a65a900  0x5c546a65a9e8 
            Address: 0x5c546a65a950  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65a968  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65a980  | Fields: 0x5c546a65a970 
            Address: 0x5c546a65a998  | Fields: 0x5c546a65a988 
            Address: 0x5c546a65a9b0  | Fields: 0x5c546a65a9a0 
            Address: 0x5c546a65a9c8  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65a9e0  | Fields: 0x5c546a65a9d0 
            Address: 0x5c546a65a9f8  | Fields: 0x5c5468ee3806 
            Address: 0x5c546a65aa10  | Fields: 0x5c546a65aa88  0x5c546a65a9e8  0x5c546a65aad0 
            Address: 0x5c546a65aa38  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65aa50  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65aa68  | Fields: 0x5c546a65aa58 
            Address: 0x5c546a65aa80  | Fields: 0x5c546a65aa70 
            Address: 0x5c546a65aa98  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65aab0  | Fields: 0x5c546a65aaa0 
            Address: 0x5c546a65aac8  | Fields: 0x5c546a65aab8 
            Address: 0x5c546a65aae0  | Fields: 0x5c5468ee3806 
            Address: 0x5c546a65aaf8  | Fields: 0x5c546a65ab58  0x5c546a65aad0  0x5c546a65abd0 
            Address: 0x5c546a65ab20  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65ab38  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65ab50  | Fields: 0x5c546a65ab40 
            Address: 0x5c546a65ab68  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65ab80  | Fields: 0x5c546a65ab70 
            Address: 0x5c546a65ab98  | Fields: 0x5c546a65ab88 
            Address: 0x5c546a65abb0  | Fields: 0x5c546a65aba0 
            Address: 0x5c546a65abc8  | Fields: 0x5c546a65abb8 
            Address: 0x5c546a65abe0  | Fields: 0x5c5468ee3806 
            Address: 0x5c546a65abf8  | Fields: 0x5c546a65ac40  0x5c546a65abd0  0x5c546a65ad00 
            Address: 0x5c546a65ac20  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65ac38  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65ac50  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65ac68  | Fields: 0x5c546a65ac58 
            Address: 0x5c546a65ac80  | Fields: 0x5c546a65ac70 
            Address: 0x5c546a65ac98  | Fields: 0x5c546a65ac88 
            Address: 0x5c546a65acb0  | Fields: 0x5c546a65aca0 
            Address: 0x5c546a65acc8  | Fields: 0x5c546a65acb8 
            Address: 0x5c546a65ace0  | Fields: 0x5c546a65acd0 
            Address: 0x5c546a65acf8  | Fields: 0x5c546a65ace8 
            Address: 0x5c546a65ad10  | Fields: 0x5c5468ee3806 
    From: 0x5c546a65a740  | To: 0x5c546a65ad40  | Size: 1536 bytes
    G1 state
    Collect count 0
    Objects:
    From: 0x5c546a65ad50  | To: 0x5c546a65c550  | Size: 6144 bytes
    To
    Objects:
    From: 0x5c546a65c560  | To: 0x5c546a65dd60  | Size: 6144 bytes
    Roots:  Index: 0     | Address: 0x7fff42040bd8  | 0x5c5468ee6090 
            Index: 1     | Address: 0x7fff42040be0  | 0x5c546a65a7c0 
            Index: 2     | Address: 0x7fff42040bc0  | 0x5c546a65a7c0 
            Index: 3     | Address: 0x7fff42040b78  | 0x5c546a65a7c0 
            Index: 4     | Address: 0x7fff42040b80  | 0x5c5468ee6080 
            Index: 5     | Address: 0x7fff42040b88  | 0x5c546a65a7d8 
            Index: 6     | Address: 0x7fff42040b90  | 0x5c546a65a7d8 
            Index: 7     | Address: 0x7fff42040b60  | 0x5c546a65a7c0 
            Index: 8     | Address: 0x7fff42040b28  | 0x5c546a65a7c0 
            Index: 9     | Address: 0x7fff42040b30  | 0x5c546a65a7d8 
            Index: 10    | Address: 0x7fff42040b38  | 0x5c546a65a818 
            Index: 11    | Address: 0x7fff42040b40  | 0x5c546a65a818 
            Index: 12    | Address: 0x7fff42040b10  | 0x5c546a65a7d8 
            Index: 13    | Address: 0x7fff42040ae8  | 0x5c5468eea1e8 
            Index: 14    | Address: 0x7fff42040ae0  | 0x5c546a65ac00 
            Index: 15    | Address: 0x7fff42040ad8  | 0x5c546a65a818 
            Index: 16    | Address: 0x7fff42040a88  | 0x7fff42040ab0 
            Index: 17    | Address: 0x7fff42040a90  | 0x7fff42040ac0 
            Index: 18    | Address: 0x7fff42040a98  | 0x5c5468ee3a88 
            Index: 19    | Address: 0x7fff42040aa0  | 0x5c5468eea1e8 
            Index: 20    | Address: 0x7fff42040a70  | 0x5c546a65ac00 
    After garbage collecting:
    G0 state
    Collect count 1
    Objects:
    From: 0x5c546a65a740  | To: 0x5c546a65ad40  | Size: 1536 bytes
    G1 state
    Collect count 0
    Objects:
            Address: 0x5c546a65ad50  | Fields: 0x5c546a65ad70 
            Address: 0x5c546a65ad68  | Fields: 0x5c546a65ad88 
            Address: 0x5c546a65ad80  | Fields: 0x5c546a65ada0 
            Address: 0x5c546a65ad98  | Fields: 0x5c546a65adb8 
            Address: 0x5c546a65adb0  | Fields: 0x5c546a65add0 
            Address: 0x5c546a65adc8  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65ade0  | Fields: 0x5c546a65ad58  0x5c5468eea1e8  0x5c546a65ae10 
            Address: 0x5c546a65ae08  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65ae20  | Fields: 0x5c5468ee3a0b 
            Address: 0x5c546a65ae38  | Fields: 0x5c546a65af28  0x5c546a65af40  0x5c546a65ae68 
            Address: 0x5c546a65ae60  | Fields: 0x5c546a65ae80 
            Address: 0x5c546a65ae78  | Fields: 0x5c546a65ae98 
            Address: 0x5c546a65ae90  | Fields: 0x5c546a65aeb0 
            Address: 0x5c546a65aea8  | Fields: 0x5c546a65aec8 
            Address: 0x5c546a65aec0  | Fields: 0x5c546a65aee0 
            Address: 0x5c546a65aed8  | Fields: 0x5c546a65aef8 
            Address: 0x5c546a65aef0  | Fields: 0x5c546a65af10 
            Address: 0x5c546a65af08  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65af20  | Fields: 0x5c5468eea1e8 
            Address: 0x5c546a65af38  | Fields: 0x5c546a65af58 
            Address: 0x5c546a65af50  | Fields: 0x5c546a65af70 
            Address: 0x5c546a65af68  | Fields: 0x5c546a65af88 
            Address: 0x5c546a65af80  | Fields: 0x5c546a65afa0 
            Address: 0x5c546a65af98  | Fields: 0x5c5468eea1e8 
    From: 0x5c546a65ad50  | To: 0x5c546a65c550  | Size: 6144 bytes
    To
    Objects:
    From: 0x5c546a65c560  | To: 0x5c546a65dd60  | Size: 6144 bytes
    Roots:  Index: 0     | Address: 0x7fff42040bd8  | 0x5c5468ee6090 
            Index: 1     | Address: 0x7fff42040be0  | 0x5c546a65ad58 
            Index: 2     | Address: 0x7fff42040bc0  | 0x5c546a65ad58 
            Index: 3     | Address: 0x7fff42040b78  | 0x5c546a65ad58 
            Index: 4     | Address: 0x7fff42040b80  | 0x5c5468ee6080 
            Index: 5     | Address: 0x7fff42040b88  | 0x5c546a65ade8 
            Index: 6     | Address: 0x7fff42040b90  | 0x5c546a65ade8 
            Index: 7     | Address: 0x7fff42040b60  | 0x5c546a65ad58 
            Index: 8     | Address: 0x7fff42040b28  | 0x5c546a65ad58 
            Index: 9     | Address: 0x7fff42040b30  | 0x5c546a65ade8 
            Index: 10    | Address: 0x7fff42040b38  | 0x5c546a65ae28 
            Index: 11    | Address: 0x7fff42040b40  | 0x5c546a65ae28 
            Index: 12    | Address: 0x7fff42040b10  | 0x5c546a65ade8 
            Index: 13    | Address: 0x7fff42040ae8  | 0x5c5468eea1e8 
            Index: 14    | Address: 0x7fff42040ae0  | 0x5c546a65ae40 
            Index: 15    | Address: 0x7fff42040ad8  | 0x5c546a65ae28 
            Index: 16    | Address: 0x7fff42040a88  | 0x7fff42040ab0 
            Index: 17    | Address: 0x7fff42040a90  | 0x7fff42040ac0 
            Index: 18    | Address: 0x7fff42040a98  | 0x5c5468ee3a88 
            Index: 19    | Address: 0x7fff42040aa0  | 0x5c5468eea1e8 
            Index: 20    | Address: 0x7fff42040a70  | 0x5c546a65ae40 
    Total memory requested:   1040 bytes (59 objects)
    Total memory allocation: 1 512 bytes (59 objects)
    Total garbage collecting: 1. G_0: 1. G_1: 0
    Maximum residency:       1 512 bytes (59 objects)
    Total memory use:        85 reads and 0 writes
    Max GC roots stack size: 22 roots
    8

    ------------------------------------------------------------
    Garbage collector (GC) statistics:
    Total memory requested:   1296 bytes (74 objects)
    Total memory allocation: 1 888 bytes (74 objects)
    Total garbage collecting: 1. G_0: 1. G_1: 0
    Maximum residency:       1 888 bytes (74 objects)
    Total memory use:        113 reads and 0 writes
    Max GC roots stack size: 22 roots

    ------------------------------------------------------------
    Stella runtime statistics:
    Total allocated fields in Stella objects: 88 fields
    ```

    __Примечание.__ В результате выполнения выводится статистика состояния
    поколений и корней, а также в результате, статистика работы сборщика мусора.
