#!/bin/bash

echo "n = 100:"
./main 100 100
./main 100 64
./main 100 32
./main 100 16
./main 100 8
./main 100 4
./main 100 2

echo ""
echo "n = 300:"
./main 300 300
./main 300 256
./main 300 128
./main 300 64
./main 300 32
./main 300 16
./main 300 8

echo ""
echo "n = 500:"
./main 500 500
./main 500 256
./main 500 128
./main 500 64
./main 500 32
./main 500 16
./main 500 8

echo ""
echo "Finish"