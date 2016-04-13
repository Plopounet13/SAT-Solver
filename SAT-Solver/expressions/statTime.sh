#!/bin/bash
echo "Test heuristique normale :"
time ./resol $*

echo "Test heuristique RAND :"
time ./resol $* -rand

echo "Test heuristique MOMS :"
time ./resol $* -moms

echo "Test heuristique DLIS :"
time ./resol $* -dlis

exit 0