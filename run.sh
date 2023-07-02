#!/bin/sh

set -ex

gcc -g -Wall main.c -o main
./main
rm main
