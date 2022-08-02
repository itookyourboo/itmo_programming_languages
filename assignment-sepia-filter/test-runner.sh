#!/bin/bash

MAIN=./build/main
SSE_KEY=-sse
TESTS_FOLDER=./tests

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'
result=0

for i in {1..4}
do
  echo -e "${BLUE}==============TEST $i==============${NC}"

  echo "---------C IMPLEMENTATION---------"
  $MAIN $TESTS_FOLDER/$i/input.bmp $TESTS_FOLDER/$i/output_c.bmp
  cmp $TESTS_FOLDER/$i/output_c.bmp $TESTS_FOLDER/$i/output_expected.bmp

  if [ $? -eq 0 ]; then
    echo -e "${GREEN}OK. Result = expected.${NC}"
  else
    echo -e "${RED}ERROR. Result != expected.${NC}"
    result=1
  fi

  echo "--------ASM IMPLEMENTATION--------"
  $MAIN $SSE_KEY $TESTS_FOLDER/$i/input.bmp $TESTS_FOLDER/$i/output_sse.bmp
  cmp $TESTS_FOLDER/$i/output_sse.bmp $TESTS_FOLDER/$i/output_expected.bmp

  if [ $? -eq 0 ]; then
    echo -e "${GREEN}OK. Result = expected.${NC}"
  else
    echo -e "${RED}ERROR. Result != expected.${NC}"
    result=1
  fi

  echo -e "\n"
done

exit $result