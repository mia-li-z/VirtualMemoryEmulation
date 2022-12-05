#!/bin/bash
EXEC=./virtmem
ALGORITHM="rand fifo lru"
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
    # # attempt to generate graphs
    # if command -v gnuplot > /dev/null; then
    #   command gnuplot -e "set term png size 800,600;\
    #     set output './csv-files/$algorithm-$benchmark.png';\
    #     set datafile separator ',';\
    #     set key autotitle columnhead;\
    #     set title '$EXEC 100 <Frames> $algorithm $benchmark';\
    #     set xlabel 'Frames';\
    #     set pointsize 1.0;\
    #     plot '$FILE' using 2:3 title 'Page Faults/Disk Reads', '' using 2:5"
    # else
    #   echo "Could not generate graph b/c GNUplot is not installed"
    # fi
  done
done


