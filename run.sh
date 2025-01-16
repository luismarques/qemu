#!/bin/bash

scripts/opentitan/pyot.py \
    -c luis.hjson         \
    -g cfg.ini            \
    -R                    \
    -w /tmp/results.csv   \
    -L qemu.log           \
    -N GUI                \
    -V -vvv               \
    --trace=trace.txt

#    --trace=trace.txt

#    --opts=-S --opts=-s
