#!/bin/bash

(
    echo "name"$1;
    sleep 2;
    echo "/join 1";
    sleep 2;
    echo "hello";
    sleep 2;
    echo "world";
    sleep 2;
    echo "a test string";
    sleep 2;
    echo "/leave";
    sleep 2;
    echo "/quit";
) | telnet localhost 8237