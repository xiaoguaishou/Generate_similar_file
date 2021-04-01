#!/bin/bash
declare -i num1=1
declare -i num2=1
declare -i num3=1
#while ((num1 < 500))
#do
  #dd if=/dev/urandom of=/home/ly/fhw/data_fhw3/random_data/2K-$num1 count=2 bs=1024
  #dd if=/dev/urandom of=/home/ly/fhw/data_fhw3/random_data/4K-$num1 count=4 bs=1024
  #dd if=/dev/urandom of=/home/ly/fhw/data_fhw3/random_data/8K-$num1 count=8 bs=1024
  #dd if=/dev/urandom of=/home/ly/fhw/data_fhw3/random_data/16K-$num1 count=16 bs=1024
  #dd if=/dev/urandom of=/home/ly/fhw/data_fhw3/random_data/32K-$num1 count=32 bs=1024
 # num1=$(($num1+1))
#done

while (($num2 < 300))
do
  dd if=/dev/urandom of=/home/ly/fhw/data_fhw3/random_data/64K-$num2 count=64 bs=1024
  dd if=/dev/urandom of=/home/ly/fhw/data_fhw3/random_data/128K-$num2 count=128 bs=1024
  dd if=/dev/urandom of=/home/ly/fhw/data_fhw3/random_data/256K-$num2 count=256 bs=1024
  dd if=/dev/urandom of=/home/ly/fhw/data_fhw3/random_data/512K-$num2 count=512 bs=1024
  num2=$(($num2+1))
done

while (($num3 < 60))
do
  dd if=/dev/urandom of=/home/ly/fhw/data_fhw3/random_data/1M-$num3 count=1024 bs=1024
  dd if=/dev/urandom of=/home/ly/fhw/data_fhw3/random_data/2M-$num3 count=2048 bs=1024
  dd if=/dev/urandom of=/home/ly/fhw/data_fhw3/random_data/4M-$num3 count=4096 bs=1024
  dd if=/dev/urandom of=/home/ly/fhw/data_fhw3/random_data/8M-$num3 count=8192 bs=1024
  dd if=/dev/urandom of=/home/ly/fhw/data_fhw3/random_data/16M-$num3 count=16384 bs=1024
  num3=$(($num3+1))
done
