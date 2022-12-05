#!/bin/bash
EXEC=./virtmem
ALGORITHM="rand fifo custom"
BENCHMARK="scan sort focus"

if [ ! -e $EXEC ]; then
  make
fi

if [ ! -d "./csv-files" ]; then
  mkdir ./csv-files
fi

# Run each benchmark 100 times with each replacement algorithm
for algorithm in $ALGORITHM; do
  for benchmark in $BENCHMARK; do
    FILE="./csv-files/$algorithm-$benchmark.csv"
    echo $FILE
    echo "Pages,Frames,Page Faults,Disk Reads,Disk Writes" > $FILE
    for ((i = 1 ; i <= 100 ; i++)); do
      output=$($EXEC 100 $i $algorithm $benchmark | grep page)
      words=( $output )
      if [ ! ${#words[@]} -eq "0" ]; then
        echo "100,$i,${words[0]},${words[3]},${words[6]}" >> $FILE
      fi
    done
  done
done


