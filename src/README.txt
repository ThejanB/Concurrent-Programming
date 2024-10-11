# Prerequisites ->
    1. GCC Compiler
    2. A POSIX-compliant operating system (Linux or macOS |  also can run in windows using wsl terminal) 
    3. Make 
    4. Pthreads Library

# Files ->
    Makefile        : Automates the build and run processes.
    serial.c        : Source code for the serial version.
    mutex.c         : Source code using mutex locks.
    rw_lock.c       : Source code using read-write locks.
    linked_list.c and linked_list.h   : Implementation of the linked list data structure.

# How to run -> 
    1. open a terminal in the project directory and run the following command, 
        - make all
    
    2. To run the program for all test cases,
        - make run_all
    (Execution times for each case will be saved in results folder.)

    3. To generate graphs,
        - cd diagrams
        - python3 draw_diagrams.py  ( for windows without using wsl terminal -> python draw_diagrams.py )
    (Generated graphs will be saved in diagrams folder.)

# Additionally, 
    ## Makefile Commands Summary ->
        make all         : Compiles all programs. (serial,mutex and read-write lock programs)
        make serial      : Compiles the serial program.
        make mutex       : Compiles the mutex program.
        make rw_lock     : Compiles the read-write lock program.
        make run_all     : Runs all programs with predefined arguments.
        make run_serial  : Runs the serial program with predefined arguments.
        make run_mutex   : Runs the mutex program with predefined arguments.
        make run_rw_lock : Runs the read-write lock program with predefined arguments.
        make clean       : Removes compiled executables.

    ## you can compile the program for individual case by providing below command line arguments,
        gcc -o serial serial.c linked_list.c -lm
        gcc -o mutex mutex.c linked_list.c -lpthread -lm
        gcc -o rw_lock rw_lock.c linked_list.c -lpthread -lm

    ## you can run the program for individual case by providing below command line arguments,
        1. Thread Count: (only relevant for mutex and read_write)
        2. mMember: The probability of the member operation.
        3. mInsert: The probability of the insert operation.
        4. mDelete: The probability of the delete operation.

        i.e.
            ./serial <mMember> <mInsert> <mDelete>
            ./mutex <thread_count> <mMember> <mInsert> <mDelete>
            ./rw_lock <thread_count> <mMember> <mInsert> <mDelete>

        As an example you can run instances as,
            ./serial 0.99 0.005 0.005
            ./serial 0.9 0.05 0.05
            ./mutex 1 0.99 0.005 0.005
            ./mutex 2 0.9 0.05 0.05
            ./rw_lock 4 0.5 0.25 0.25
            ./rw_lock 8 0.9 0.05 0.05

    Execution times for each case will be saved in results folder.
    Generated Graphs will be saved in diagrams folder.


