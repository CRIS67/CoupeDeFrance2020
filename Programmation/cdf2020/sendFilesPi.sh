#!/bin/bash

echo "Sending files : $@" 
scp  ./$@ pi@10.3.141.1:/home/pi/strategyOpt/

