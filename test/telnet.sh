#!/bin/bash

(
    sleep 2;
    echo "name"$1;
    sleep 2;
    echo "/create room"$1;
    sleep 2;
    echo "/join default";
    sleep 2;
    echo "hello";
    sleep 2;
    echo "world";
    sleep 2;
    echo "a test string";
    sleep 2;
    echo "/who"
    sleep 2;
    echo "/whisper name0 secret";
    sleep 2;
    echo "/leave";
    sleep 2;
    echo "/quit";
) | telnet localhost 8237
