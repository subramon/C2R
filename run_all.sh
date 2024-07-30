#!/bin/bash
set -e
iter=1
while [ $iter -le 5 ]; do
  bash test/ut3.sh $iter ./data2/
  iter=`expr $iter + 1`
done
echo "All done"
