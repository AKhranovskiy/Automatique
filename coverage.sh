#!/bin/bash

if [ ! -d "$CLANG_BIN" ]
then
  echo "CLANG_BIN(${CLANG_BIN}) is invalid path"
  exit
fi

key="$1"
echo $key
case $key in
  report)
    TYPE=report
    shift
    ;;
  show)
    TYPE=show
    shift
    ;;
  *)
    echo "Unknown report type. Use report or show"
    exit
    ;;
esac

APP=$1
shift

PROFRAW=default.profraw
PROFDATA=default.profdata
$APP && \
$CLANG_BIN/llvm-profdata merge -sparse $PROFRAW -o $PROFDATA && \
$CLANG_BIN/llvm-cov $TYPE $APP -instr-profile=$PROFDATA $@

