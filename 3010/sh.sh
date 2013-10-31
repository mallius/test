#!/bin/bash
service nfs start
ifconfig
ifconfig eth0 up 192.168.1.85
ifconfig
ls /dev/ | grep ttyUSB
kermit -c
