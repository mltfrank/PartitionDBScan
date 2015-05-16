#include <stdio.h>
#include "stdlib.h"
#include "assert.h"
#include <time.h>
#include "windows.h"

#include "DefineMain.h"
#include "FileRW.h"
#include "Partition.h"
#include "LocalDBScan.h"
#include "ConflictMapRefill.h"

double dataMatrix[N][3];												//数据集 定义
PartitionData* localDataMatrix;											//划分后的数组
Partition** partPartition;												//划分后的域
int* partitionLength;												//划分后的域长度

int conflictMatrix[MAX_LOCAL_CLUSTER][MAX_LOCAL_CLUSTER];				//冲突矩阵,元素为0时不发生冲突，元素为1是为冲突
int clusterGlobalMap[MAX_LOCAL_CLUSTER];							//局部簇号到全局簇号的映射
int clusterResult[N];											//聚类的结果，先后存放局部簇号和全局簇号
int clusterIfCore[N];											//全局变量，存放是否是核点，用于判定冲突
int localClusterCount = 0;										//局部簇的全局统计，同时作为局部簇号

void costTime()
{
	int i = 0x3fffffff;
	while (i--);
}

int main()
{
//	costTime();
	load_data(dataMatrix);	//读入数据
//	printf("Finish reading data!\n");

	initPartition();		//初始化划分方案
//	printf("Init partition finish!\n");
	getPointCountInPartition();		//统计划分的数量
	while (!partitionValidation())
	{
		getPointCountInPartition();		//统计划分的数量
	}

	int partitionCount = getPartitionCount();
//	printf("共产生了%d个划分\n", partitionCount);
	localDataMatrix = (PartitionData *)malloc(partitionCount*MAX_DATA_PER_BLOCK*sizeof(PartitionData));
	partPartition = (Partition**)malloc(partitionCount * sizeof(Partition*));
	partitionLength = (int *)malloc(partitionCount * sizeof(int));
	if (!localDataMatrix || !partPartition || !partitionLength)
	{
		printf("Out of memory!\n");
		return 0;
	}

	printPartition();

	getPartitionMap();				//将划分结果移动到线性结构当中
	movePointsIntoPartition(partitionCount);		//将点坐标信息移动到分块的数据当中
//	printf("移动到局部数组完成！开始进行局部聚类\n");

	memset(conflictMatrix, 0, MAX_LOCAL_CLUSTER * MAX_LOCAL_CLUSTER * sizeof(int));
	memset(clusterIfCore, 0, N * sizeof(int));
	memset(clusterResult, 0xff, N * sizeof(int));
	memset(clusterGlobalMap, 0xff, MAX_LOCAL_CLUSTER * sizeof(int));

	for (int i = 0; i < partitionCount; i++)
	{
		PartitionData * data = localDataMatrix + i * MAX_DATA_PER_BLOCK;
		int dataLen = *(partitionLength + i);
		writeConflictPointIfCore(data, dataLen);
	}

	for (int i = 0; i < partitionCount; i++)
	{
		PartitionData * data = localDataMatrix + i * MAX_DATA_PER_BLOCK;
		int dataLen = *(partitionLength + i);
		localDBSCAN(data, dataLen);								//局部进行dbscan算法，填写冲突矩阵
	}
	//printf("局部聚类完成，开始合并重填\n");
	calConflict(localClusterCount);				//根据冲突矩阵，填写全局簇号映射
	refillCluster();							//完成全局映射
	//printf("算法完成！\n");

	int * clusterMainCounter = (int *)malloc(localClusterCount * sizeof(int));
	memset(clusterMainCounter, 0, localClusterCount * sizeof(int));
	for (int i = 0; i < N; i++)
	{
		if (clusterResult[i] >= 0)
		{
			clusterMainCounter[clusterResult[i]] ++;
		}

	}
	
	for (int i = 0; i < localClusterCount; i++)
	{
		if (clusterMainCounter[i] > 0)
			printf("Cluster %d : %d\n", i, clusterMainCounter[i]);
	}

	free(localDataMatrix);
	free(partPartition);
	free(partitionLength);

	free(clusterMainCounter);
	printf("end");

	getchar();

	return 0;
}