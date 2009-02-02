#!/bin/sh

echo "Emacs/gdb debugger usage:"
echo "- at startup, press enter"
echo "- F12 to compile (then enter)"
echo "- in the gdb window, type 'r eblearn_tester -nocolor'"
emacs -l .debug.el &
