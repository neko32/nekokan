#!/bin/bash

# デフォルト値の設定
DEFAULT_SHORTNAME="NA" # 例：デフォルト値をcanon_prtとする

# オプションの処理
while getopts "i:" opt; do
  case $opt in
    i)
      INPUT_SHORTNAME="$OPTARG"
      ;;
    \?)
      echo "Usage: $0 [-i <shortname>]" >&2
      exit 1
      ;;
  esac
done

# 入力ShortNameの決定 (オプションが指定されていなければ、デフォルト値を使用)
if [ -z "$INPUT_SHORTNAME" ]; then
  INPUT_SHORTNAME="$DEFAULT_SHORTNAME"
fi

# CSVファイルの読み込みとIPアドレスの検索

IP_ADDR=$(awk -F, '{
    gsub(/^ +| +$/, "", $5)
    sub(/\r$/, "", $5)
}
$5 == "nekox_pi" {print $3}' /mnt/c/dev/internal_pc_mapper/mapper.csv)

# 結果の出力
if [ -n "$IP_ADDR" ]; then
  echo "$IP_ADDR"
else
  echo "ShortName \"$INPUT_SHORTNAME\" not found in the CSV file." >&2
  exit 1
fi

