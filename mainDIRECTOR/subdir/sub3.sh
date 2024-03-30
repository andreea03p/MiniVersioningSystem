#!/bin/bash

if [ $# -ne 3 ]
then
	echo "wrong args"
	exit 1
else
	echo "Usage: $1 $2 $3"
fi

is_number() 
{
    [[ "$1" =~ ^[0-9]+$ ]]
}

if ! is_number "$1" || ! is_number "$2" || ! is_number "$3"
then
    echo "Error: All arguments must be numeric."
    exit 1
fi


a=$1
b=$2
c=$3

if [ "$a" -gt "$b" ]
then 
    if [ "$a" -gt "$c" ]
    then
        max=$a
    else
        max=$c
    fi 
else 
    if [ "$b" -gt "$c" ]
    then
        max=$b
    else
        max=$c
    fi 
fi

echo "The biggest number is: $max"

