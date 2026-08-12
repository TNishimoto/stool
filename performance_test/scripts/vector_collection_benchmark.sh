#!/bin/bash
cd -- "$(dirname -- "${BASH_SOURCE[0]}")" || exit 1

list=("none" "vector" "block16" "block32" "block64" "block128" "block256" "block512" "block1024")
list2=("8" "16" "32")

case "$(uname -s)" in
  Darwin)
    echo "computing delta on macOS"
    for item in "${list[@]}" ; do
        for item2 in "${list2[@]}" ; do
            echo "Computing vector collection benchmark - container: ${item} - value_bits: ${item2}"
            echo "Computing vector collection benchmark - container: ${item} - value_bits: ${item2}" >> ../build/vector_collection_benchmark_read.log 2>&1
            echo "Computing vector collection benchmark - container: ${item} - value_bits: ${item2}" >> ../build/vector_collection_benchmark_write.log 2>&1
            /usr/bin/time -l ../build/vector_collection_benchmark -c ${item} -b ${item2} -a read >> ../build/vector_collection_benchmark_read.log 2>&1
            /usr/bin/time -l ../build/vector_collection_benchmark -c ${item} -b ${item2} -a write >> ../build/vector_collection_benchmark_write.log 2>&1
            ../build/vector_collection_benchmark -c ${item} -b ${item2} -a read >> ../build/vector_collection_benchmark_read.csv 2>&1
            ../build/vector_collection_benchmark -c ${item} -b ${item2} -a write >> ../build/vector_collection_benchmark_write.csv 2>&1

        done
    done
    ;;

  Linux)
    echo "computing delta on Linux"
    ;;

  *)
    echo "unsupported OS: $(uname -s)" >&2
    exit 1
    ;;
esac
