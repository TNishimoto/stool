#!/bin/bash
#bash download.sh

dataset_folder_path=$1
output_folder_path=$2

bash sa.sh $1 $2
bash dsa.sh $1 $2
bash delta.sh $1 $2
bash dsa_delta.sh $1 $2