#!/bin/bash

# Check if a number is provided as an argument
if [ $# -ne 1 ]; then
  echo "Usage: $0 <number_of_times>"
  exit 1
fi

# Get the number of times to run the command
n=$1

# Run `gc_cli` n times in parallel with wait
for ((i=0; i<$n; i++)); do
  gc_cli in.gc &
done
wait

echo "All $n commands finished!"

