#!/bin/bash
echo 'Sending files to RPI ...' 
scp  ./* pi@10.3.141.1:/home/pi/strategyOpt/
