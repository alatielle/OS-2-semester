#!/bin/bash

set -e

host='192.168.0.1'
rate=$1
if [ $4 -eq 0 ]
then
	x=$(xwininfo -root | awk '/.*-geometry.*/ {match ($2,"[0-9]+x"); print substr($2,RSTART,RLENGTH-1)}')
else
	x=$4
fi
if [ $5 -eq 0 ]
then
	y=$(xwininfo -root | awk '/.*-geometry.*/ {match ($2,"x[0-9]+"); print substr($2,RSTART+1,RLENGTH)}')
else
	y=$5
fi
screen=':'$2'.'$3
size=$x'x'$y

ssh $host << EOP
ffmpeg -f x11grab -r $rate -s $size -i $screen -f avi pipe:1
EOP
