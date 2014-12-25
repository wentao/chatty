#!/bin/bash

rm output*.log

i="0"
while  [ $i -lt $1 ]
do
    ./telnet.sh $i >> output$i.log &
    i=$[$i+1]
done

wait
