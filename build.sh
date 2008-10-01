#!/bin/sh

mkdir obj 2> /dev/null
cd obj && cmake .. && make
