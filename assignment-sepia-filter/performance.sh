#!/bin/bash

MAIN=./build/main
SSE_KEY=-sse
TESTS_FOLDER=./tests
TEST=4

C_RUN="$MAIN $TESTS_FOLDER/$TEST/input.bmp $TESTS_FOLDER/$TEST/output_c.bmp"
SSE_RUN="$MAIN $SSE_KEY $TESTS_FOLDER/$TEST/input.bmp $TESTS_FOLDER/$TEST/output_c.bmp"

c_sum=0
c_min=999
c_max=0

sse_sum=0
sse_min=999
sse_max=0

NUM=20

for ((i=1; i<=$NUM; i++))
do
  start=$(date +%s.%N)
  `$C_RUN`
  dur=$(echo "$(date +%s.%N) - $start" | bc)
  c_sum=$(echo $c_sum+$dur | bc)
  if (( $(echo "$dur < $c_min" | bc -l) )); then
    c_min=$dur
  fi
  if (( $(echo "$dur > $c_max" | bc -l) )); then
    c_max=$dur
  fi

  start=$(date +%s.%N)
  `$SSE_RUN`
  dur=$(echo "$(date +%s.%N) - $start" | bc)
  sse_sum=$(echo $sse_sum+$dur | bc)
  if (( $(echo "$dur < $sse_min" | bc -l) )); then
    sse_min=$dur
  fi
  if (( $(echo "$dur > $sse_max" | bc -l) )); then
    sse_max=$dur
  fi

  echo -ne "$i/$NUM\r"
done

c_avg=$(echo "$c_sum / $NUM" | bc -l)
sse_avg=$(echo "$sse_sum / $NUM" | bc -l)

line=$(printf "%0.s=" {1..66})

echo -ne "$line\n"
echo -e "|| \t\t|| MIN\t\t|| MAX\t\t|| AVG\t\t||"
echo $line
printf "|| C\t\t|| %.6fs\t|| %.6fs\t|| %.6fs\t||\n" $c_min $c_max $c_avg
echo $line
printf "|| SSE\t\t|| %.6fs\t|| %.6fs\t|| %.6fs\t||\n" $sse_min $sse_max $sse_avg
echo $line
