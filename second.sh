#!/bin/sh
gcc src/master.c -o bin/master -lrt -lm
gcc src/processA.c -lncurses -o bin/processA -lbmp -lrt -pthread -lm
gcc src/processB.c -lncurses -o bin/processB -lrt -pthread -lm
./bin/master
