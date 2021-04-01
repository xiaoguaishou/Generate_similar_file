#!/bin/sh
#Usage: ./generate_dup.sh input_dir output_dir
#要求的源数据格式:input_dir/intra_dup_percent-file_name
dirname=$1
output_dir=$2
#先创建输出文件的所有文件夹
#for inter_dup_percent in 50
for inter_dup_percent in 0 2 4 8 10 20 40 60 80 100
do
  #mkdir ../dupfile-$inter_dup_percent
  mkdir $output_dir/dupfile-$inter_dup_percent
done

for orig_file in ${dirname}/*
do
  file_name=`basename $orig_file`
  #获取文件名头部存放的intra_dup_percent信息
  intra_dup_percent=`echo $file_name | awk -F "-" '{printf "%d",$1}'`
  #echo $intra_dup_percent
  #for inter_dup_percent in 50
  for inter_dup_percent in 0 2 4 8 10 20 40 60 80 100
  do
    target_file="$output_dir/dupfile-$inter_dup_percent/$inter_dup_percent-$file_name"
    echo $target_file
    cp $orig_file $target_file
    ./generate_dup -f $target_file -r $inter_dup_percent -a $intra_dup_percent
    #for srand work
    sleep 1
  done
done

