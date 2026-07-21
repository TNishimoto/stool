#!/bin/bash
#bash download.sh

dataset_folder_path=$1
output_folder_path=$2

nohup bash sa.sh $1 $2 &
nohup bash dsa.sh $1 $2 &
nohup bash delta.sh $1 $2 &
nohup bash dsa_delta.sh $1 $2 &