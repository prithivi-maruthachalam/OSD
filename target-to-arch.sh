#!/bin/bash
# set -e

if echo $1 | grep -Eq 'i[[:digit:]]86-'
then
    echo i386
else
    echo $1
fi