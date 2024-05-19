#!/bin/sh

sudo perf stat -e branch-misses -- taskset --cpu-list 0 ./prog

