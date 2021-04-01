#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

//缓冲区最大1M,最小2K
#define MAX_CHUNK_SIZE 1048576
#define MIN_CHUNK_SIZE 2048
/*
//to use clock_gettime, need gcc -lrt when compiling
int rand_array[MAX_NUMBER];

//random delete an item from the array and add it to before tail
void generate_rand(){
  int i, j, rn, ca;
  for (i = 0; i < MAX_NUMBER; i++){
    rand_array[i] = i;
  }
  for (i = 0; i < MAX_NUMBER - 1; i++){
    rn = rand() % (MAX_NUMBER - i);
    ca = rand_array[rn];
    for (j = rn; j < MAX_NUMBER - i - 1; j++){
      rand_array[j] = rand_array[j + 1];
    }
    rand_array[j] = ca;
  }
}
*/
static void fill_buf(char *buf, int len){
  int i;
  for (i = 0; i < len; i++){
    buf[i] = '\0' + ((rand() % 127) + 1);
  }
}

int main(int argc, char *argv[]){
  int inter_dup_percent, intra_dup_percent;
  char file_name[256];

  int opt;
  while ((opt = getopt(argc, argv, "f:r:a:")) != -1){
    switch (opt){
      case 'f':
        sprintf(file_name, "%s", optarg);
        break;
      case 'r':
        inter_dup_percent = atoi(optarg);
        if (inter_dup_percent < 0){
          inter_dup_percent = 0;
        } else if (inter_dup_percent > 100){
          inter_dup_percent = 100;
        }
        break;
      case 'a':
        intra_dup_percent = atoi(optarg);
        if (intra_dup_percent < 0){
          intra_dup_percent = 0;
        } else if (intra_dup_percent > 100){
          intra_dup_percent = 100;
        }
        break;
      case '?':
        printf("unknown option: %c\n", optopt);
        printf("Usage: generate_dup -f file_name -r inter_dup_percent -a intra_dup_percent\n");
        break;
    }
  }

  srand(time(0));

  int fd;
  unsigned long file_size;

  fd = open(file_name, O_RDWR, 0);//O_RDWR
  file_size = lseek(fd, 0, SEEK_END);

  //rec_size为1%的文件内容大小
  //num_rec = rec_size / chunk_size = 每写1%的文件大小需要写的chunk次数
  printf("@file:%s,size=%lu,inter_dup_percent=%d,intra_dup_percent=%d\n",
      file_name, file_size, inter_dup_percent, intra_dup_percent);

  /*
     |--------------file_size(100%)--------------|
     |-------a--------|--b0--|--b1--|.....|--bn--|
     |-100-intra_dup_percent-|-intra_dup_percent-|
     文件内部去重后:
     |-------a--------|--b0--|

     a是内部无任何重复数据块且与b无重复数据块的部分,
     b是单个重复数据的大小,n+1是重复数据的份数,去掉冗余的n份,剩余1份b0
     其中整个文件的重复率,指的就是n*b占整个文件的大小百分比
    //产生方法:先用完全随机的缓冲区以最小chunk_size为单位填充(a+b0)
    //再将b0部分复制n份填充到b1-bn区
  */

  //根据文件外部的重复率来决定需要改写的文件内容大小
  unsigned long overwritten_file_size;
  overwritten_file_size = file_size * (100 - inter_dup_percent) / 100;
  if (overwritten_file_size < MIN_CHUNK_SIZE){
    overwritten_file_size = MIN_CHUNK_SIZE;
  }

  unsigned long average_chunk_size, target_chunk_size;
  //先将要改写的区域划分成100份,看它的大小,再上对齐到离它最近的2^n块大小
  //target_chunk_size就是要使用的chunk大小
  average_chunk_size = overwritten_file_size / 100;
  target_chunk_size = MIN_CHUNK_SIZE;
  while (average_chunk_size > target_chunk_size){
    target_chunk_size <<= 1;
  }
  if (target_chunk_size > MAX_CHUNK_SIZE){
    target_chunk_size = MAX_CHUNK_SIZE;
  }
  //按照target_chunk_size计算,总共需要写的块数量
  unsigned long total_chunk_to_write;
  total_chunk_to_write = (overwritten_file_size + target_chunk_size - 1) / target_chunk_size;

  //分配缓冲区
  char *buf;
  int buf_size;
  //确定缓冲区大小:
  buf_size = target_chunk_size;
  buf = (char *)malloc(buf_size);
  if (buf == NULL){
    printf("@failed to allocate buf of size %d\n", buf_size);
    return -ENOMEM;
  }

  //开始写数据
  //分两个阶段,先写non_dup_percent,再写dup_percent
  //num_dup_chunk,num_non_dup_chunk:非重复和重复的区域块数量
  unsigned long num_dup_chunk, num_non_dup_chunk;
  num_dup_chunk = total_chunk_to_write * intra_dup_percent / 100;
  if (num_dup_chunk < 1){
    num_dup_chunk = 1;
  }
  num_non_dup_chunk = total_chunk_to_write - num_dup_chunk;
  //FIXME
  printf("num_dup_chunk=%lu,num_non_dup_chunk=%lu\n",
      num_dup_chunk, num_non_dup_chunk);

  unsigned long chunk_written = 0;
  unsigned long base, offset;

  base = 0;//FIXME:目前就从文件偏移0开始写
  lseek(fd, base, SEEK_SET);

  while (chunk_written < num_non_dup_chunk){
    fill_buf(buf, target_chunk_size);
    //写一个chunk
    offset = base + (chunk_written * target_chunk_size);
    write(fd, buf, target_chunk_size);
    chunk_written++;
  }
  base = offset;
  chunk_written = 0;
  while (chunk_written < num_dup_chunk){
    //不需要再重新填充buf,直接用上一次填充过的buf来进行接下来的所有chunk写
    //写一个chunk
    offset = base + (chunk_written * target_chunk_size);
    //lseek(fd, base + offset, SEEK_SET);
    write(fd, buf, target_chunk_size);
    chunk_written++;
  }

  //关闭文件前将它同步回磁盘
  fsync(fd);

  close(fd);

  //释放缓冲区
  if (buf){
    free(buf);
  }
  return 0;
}

