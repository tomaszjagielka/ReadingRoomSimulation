# Reading Room Simulation

This project simulates a reading room where a fixed number of processes (N) can either read or write messages. 

## Requirements

The simulation follows these requirements:

1. Each process alternates between two phases: a relaxation phase and a phase of using the reading room.
2. At any point during the relaxation phase, a process may (but doesn't have to) change its role: from a writer to a reader or from a reader to a writer.
3. When moving to the phase of using the reading room, a process must gain access in the mode appropriate for its current role.
4. A writer places the result of their work - their creation - in the form of a message in a message queue. This message remains in the queue until all the processes, which were in the reader role at the moment the work was issued, have read it. After being read by all required processes, the message is removed.
5. The capacity of the message queue - representing a bookshelf - is limited, i.e., it cannot store more than K works.
6. While in the reading room, a process (including a writer) reads at most one work, after which the reader leaves the reading room, and the writer waits for a place in the queue to publish another work.

## Algorithm

The algorithm uses multithreading to simulate the processes. Each process is represented by a thread. The threads use mutexes and condition variables to synchronize their access to the reading room.

Please note that each process is in an infinite loop, continuously alternating between relaxation and reading room phases. This is by design to simulate the ongoing nature of the reading room.

Also, be aware that there is a potential scenario (which you are likely to run into) where all processes become writers. In this case, there are no readers to read and remove messages from the queue, which could lead to writers being indefinitely blocked waiting for the message queue to become free.

## Compiling and running the program

To compile the program, you need a C++ compiler. Here are the steps to compile and run the program using CMake:

1. Open a terminal in the project directory.
2. Generate the build files and compile the program with the following commands:
```sh
cmake -B build/
cd build
make
```
3. Run the program with the following command:
```sh
./ReadingRoomSimulation
```

Please note that the number of processes and the capacity of the message queue are defined as constants in the program (`NUMBER_OF_PROCESSES` and `MESSAGE_QUEUE_CAPACITY` respectively). You can change these values to simulate different scenarios.