#!/bin/bash


echo " Cleaning the temporaries and outputs"
make clean
echo " Start building..."
make -B it

if [[ "$?" -ne 0 ]]; then
  echo "Build failed."
  exit 1
fi


for i in test/*.cmm; do
  BASENAME=$(basename $i .cmm)
  cat $i | ./it --show >test/$BASENAME.s 2>test/$BASENAME.err


done

echo "Build was successfull."
