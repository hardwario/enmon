#!/bin/sh

set -eux

counter=0

while true
do
    echo "Counter: $counter"
    ./enmon
    counter=$((counter+1))
    sleep 0.1
done
