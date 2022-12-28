#!/bin/sh
gcc src/master.c -o bin/master -lrt
gcc src/processA.c -lncurses -o bin/processA -lrt -pthread
gcc src/processB.c -lncurses -o bin/processB -lrt -pthread
./bin/master
