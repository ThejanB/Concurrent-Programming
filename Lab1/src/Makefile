# Compiler
CC = gcc

# Targets to build
all: serial mutex rw_lock

serial: serial.c linked_list.c
	$(CC) -o serial serial.c linked_list.c -lm

mutex: mutex.c linked_list.c
	$(CC) -o mutex mutex.c linked_list.c -lpthread -lm

rw_lock: rw_lock.c linked_list.c
	$(CC) -o rw_lock rw_lock.c linked_list.c -lpthread -lm

# Clean up compiled files
clean:
	rm -f serial mutex rw_lock

# Run the programs with the predefined arguments
run_all: run_serial run_mutex run_rw_lock

run_serial: serial
	./serial 0.99 0.005 0.005
	./serial 0.9 0.05 0.05
	./serial 0.5 0.25 0.25

run_mutex: mutex
	./mutex 1 0.99 0.005 0.005
	./mutex 2 0.99 0.005 0.005
	./mutex 4 0.99 0.005 0.005
	./mutex 8 0.99 0.005 0.005
	./mutex 1 0.9 0.05 0.05
	./mutex 2 0.9 0.05 0.05
	./mutex 4 0.9 0.05 0.05
	./mutex 8 0.9 0.05 0.05
	./mutex 1 0.5 0.25 0.25
	./mutex 2 0.5 0.25 0.25
	./mutex 4 0.5 0.25 0.25
	./mutex 8 0.5 0.25 0.25

run_rw_lock: rw_lock
	./rw_lock 1 0.99 0.005 0.005
	./rw_lock 2 0.99 0.005 0.005
	./rw_lock 4 0.99 0.005 0.005
	./rw_lock 8 0.99 0.005 0.005
	./rw_lock 1 0.9 0.05 0.05
	./rw_lock 2 0.9 0.05 0.05
	./rw_lock 4 0.9 0.05 0.05
	./rw_lock 8 0.9 0.05 0.05
	./rw_lock 1 0.5 0.25 0.25
	./rw_lock 2 0.5 0.25 0.25
	./rw_lock 4 0.5 0.25 0.25
	./rw_lock 8 0.5 0.25 0.25
