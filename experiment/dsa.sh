
#!/bin/bash
cd -- "$(dirname -- "${BASH_SOURCE[0]}")" || exit 1

dataset_folder_path=$1
output_folder_path=$2
output_data_path=$2/stool_data
output_log_path=$2/stool_logs

if [[ ! -d "${dataset_folder_path}" ]]; then
  echo "directory not found: ${dataset_folder_path}"
  exit 1
fi

if [[ ! -d "${output_folder_path}" ]]; then
  echo "directory not found: ${output_folder_path}"
  exit 1
fi

if [[ ! -d "${output_data_path}" ]]; then
  mkdir -p ${output_data_path}
fi

if [[ ! -d "${output_log_path}" ]]; then
  mkdir -p ${output_log_path}
fi

#list=("world_leaders")
list=("cere" "coreutils" "einstein.de.txt" "einstein.en.txt" "Escherichia_Coli" "influenza" "kernel" "para" "world_leaders" )

case "$(uname -s)" in
  Darwin)
    echo "building DSA on macOS"
    for item in "${list[@]}" ; do
      set -- $item
      echo $1
      if [[ -f "${output_data_path}/$1.dsa" ]]; then
        echo "skipped: ${output_data_path}/$1.dsa"
      else
        echo "Building DSA - text: $1"
        /usr/bin/time -l ../build/build_dsa -i ${output_data_path}/$1.sa -o ${output_data_path}/$1.dsa -c uint64_t >> ${output_log_path}/dsa.log 2>&1
      fi
    done
    ;;

  Linux)
    echo "building DSA on Linux"
    # Linux固有のコマンド
    for item in "${list[@]}" ; do
      set -- $item
      echo $1
      if [[ -f "${output_data_path}/$1.dsa" ]]; then
        echo "skipped: ${output_data_path}/$1.dsa"
      else
        echo "Building DSA - text: $1"
        nohup /usr/bin/time -f "#build dsa, $1, %e sec, %M KB" ../build/build_dsa -i ${output_data_path}/$1.sa -o ${output_data_path}/$1.dsa -c uint64_t >> ${output_log_path}/dsa.log 2>&1
      fi
    done
    ;;

  *)
    echo "unsupported OS: $(uname -s)" >&2
    exit 1
    ;;
esac