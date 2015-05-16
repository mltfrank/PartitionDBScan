#ifndef _DOMAIN_PARTITION_H_
#define _DOMAIN_PARTITION_H_

#include "DefineMain.h"

typedef struct PartitionPos{
	int x_pos;
	int y_pos;
	int z_pos;
	int part_pos;
} PartitionPos;

#define bufferMaxSize N

/*
根据预定参数初始化划分
*/
void initPartition();

/*
统计每个划分当中点的数量
*/
void getPointCountInPartition();

/*
判定划分是否有效，如果无效则重新划分
不保证划分的结果有效
*/
int partitionValidation();


/*
统计获取当前划分的数量
*/
int getPartitionCount();

/*
将划分后的结果由高维数组转移到线性数组当中
*/
void getPartitionMap();

/*
根据划分的一维数组，将一个划分中的点存入同一个数组当中
*/
void movePointsIntoPartition(int partitionCount);

/*
打印划分结果，用于验证
*/
void printPartition();

#endif