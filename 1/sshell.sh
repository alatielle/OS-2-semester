#!/bin/bash

set -e

host='192.168.0.1'
rate=10
size=$(xwininfo -root | awk '/.*-geometry.*/ {match ($2,"[0-9]+x[0-9]+"); print substr($2,RSTART,RLENGTH)}')
screen=':0.0'

ssh $host << EOP
ffmpeg -f x11grab -r $rate -s $size -i $screen -f avi pipe:1
EOP
