#!/bin/bash

set -e

FR=10
D=0
S=0
X=0
Y=0
args=("$@")
i=0
while [ $i -lt $# ]
do
	if [[ ${args[$i]} =~ ^[-][Ff][Rr]$ && ${args[i+1]} =~ ^[0-9]+ ]]
	then
		FR=${args[$i+1]}
		let "i+=2"
	elif [[ ${args[$i]} =~ ^[-][Dd]$ && ${args[i+1]} =~ ^[0-9]+$ ]]
	then
		D=${args[i+1]}
		let "i+=2"
	elif [[ ${args[$i]} =~ ^[-][sS]$ && ${args[i+1]} =~ ^[0-9]+$ ]]
	then
		S=${args[i+1]}
		let "i+=2"
	elif [[ ${args[$i]} =~ ^[-][xX]$ && ${args[i+1]} =~ ^[0-9]+$ ]]
	then
		X=${args[i+1]}
		let "i+=2"
	elif [[ ${args[$i]} =~ ^[-][yY]$ && ${args[i+1]} =~ ^[0-9]+$ ]]
	then
		Y=${args[i+1]}
		let "i+=2"
	else 
		echo "Incorrect command line args"
		exit
	fi
done

./sshell.sh $FR $D $S $X $Y | ffplay -f avi -
