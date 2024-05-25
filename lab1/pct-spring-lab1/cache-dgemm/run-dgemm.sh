#!/bin/sh

sudo perf stat -e cache-references,cache-misses ./dgemm
