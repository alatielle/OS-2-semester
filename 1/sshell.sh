#!/bin/bash

set -e

host='192.168.0.1'
rate=$1
if [ $4 -eq 0 -o $5 -eq 0 ]
then
	size=$(xwininfo -root | awk '/.*-geometry.*/ {match ($2,"[0-9]+x[0-9]+"); print substr($2,RSTART,RLENGTH)}')
else
	size=$4'x'$5
fi
screen=':'$2'.'$3

ssh $host << EOP
ffmpeg -f x11grab -r $rate -s $size -i $screen -f avi pipe:1
EOP
