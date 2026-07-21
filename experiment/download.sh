#!/bin/bash
cd -- "$(dirname -- "${BASH_SOURCE[0]}")" || exit 1

mkdir -p ../build/experiment
mkdir -p ../build/experiment/datasets

list=("cere" "coreutils" "einstein.de.txt" "einstein.en.txt" "Escherichia_Coli" "influenza" "kernel" "para" "world_leaders" )

for item in "${list[@]}" ; do
  set -- $item
  echo $1
  if [ -f ../build/experiment/datasets/$1 ]; then
    echo "../build/experiment/datasets/$1 exists."
  else
    echo "../build/experiment/datasets/$1 does not exist."
    wget https://pizzachili.dcc.uchile.cl/repcorpus/real/$1.gz -O ../build/experiment/datasets/$1.gz
    gunzip ../build/experiment/datasets/$1.gz
  fi
done
