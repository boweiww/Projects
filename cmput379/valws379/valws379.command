#!/bin/bash

echo


wrongformat=1
typett=0

if [ "$#" -eq 3 ]; then
pgsize="$2”
wsize="$3”
wrongformat=0
type=1
if [ "$1" = "-i" ]; then
wrongformatt=1
fi
fi



if [ "$#" -eq 2 ]; then
pgsize="$1"
wsize="$2"
wrongformat=0
fi

if [ "$wrongformat" = 1 ]; then
echo "Incorrect number of parameters"
echo "./valws379 [-i] ,<pgsize> ,<window-size> "
exit 1
fi
echo "processing...."

if  [ "$type" -eq 1 ]; then
exec > $logfile 2>&1 | ./valws379.bin -i $2 $3
else
exec > $logfile 2>&1 | ./valws379.bin $pgsize $wsize
fi

