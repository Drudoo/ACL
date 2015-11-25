#!/bin/bash
echo count to 4:
COUNTER=0
while [  $COUNTER -lt 5 ]; do
echo $COUNTER
let COUNTER=COUNTER+1
done
echo counter is done!
