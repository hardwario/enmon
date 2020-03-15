#!/bin/sh

set -eux

counter=0

while true
do
    echo "Counter: $counter"
    sudo ./out/enmon
    counter=$((counter+1))
    sleep 0.1
done
