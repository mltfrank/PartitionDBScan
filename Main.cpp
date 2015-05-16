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

double dataMatrix[N][3];												//���ݼ� ����
PartitionData* localDataMatrix;											//���ֺ������
Partition** partPartition;												//���ֺ����
int* partitionLength;												//���ֺ���򳤶�

int conflictMatrix[MAX_LOCAL_CLUSTER][MAX_LOCAL_CLUSTER];				//��ͻ����,Ԫ��Ϊ0ʱ��������ͻ��Ԫ��Ϊ1��Ϊ��ͻ
int clusterGlobalMap[MAX_LOCAL_CLUSTER];							//�ֲ��غŵ�ȫ�ִغŵ�ӳ��
int clusterResult[N];											//����Ľ�����Ⱥ��žֲ��غź�ȫ�ִغ�
int clusterIfCore[N];											//ȫ�ֱ���������Ƿ��Ǻ˵㣬�����ж���ͻ
int localClusterCount = 0;										//�ֲ��ص�ȫ��ͳ�ƣ�ͬʱ��Ϊ�ֲ��غ�

void costTime()
{
	int i = 0x3fffffff;
	while (i--);
}

int main()
{
//	costTime();
	load_data(dataMatrix);	//��������
//	printf("Finish reading data!\n");

	initPartition();		//��ʼ�����ַ���
//	printf("Init partition finish!\n");
	getPointCountInPartition();		//ͳ�ƻ��ֵ�����
	while (!partitionValidation())
	{
		getPointCountInPartition();		//ͳ�ƻ��ֵ�����
	}

	int partitionCount = getPartitionCount();
//	printf("��������%d������\n", partitionCount);
	localDataMatrix = (PartitionData *)malloc(partitionCount*MAX_DATA_PER_BLOCK*sizeof(PartitionData));
	partPartition = (Partition**)malloc(partitionCount * sizeof(Partition*));
	partitionLength = (int *)malloc(partitionCount * sizeof(int));
	if (!localDataMatrix || !partPartition || !partitionLength)
	{
		printf("Out of memory!\n");
		return 0;
	}

	printPartition();

	getPartitionMap();				//�����ֽ���ƶ������Խṹ����
	movePointsIntoPartition(partitionCount);		//����������Ϣ�ƶ����ֿ�����ݵ���
//	printf("�ƶ����ֲ�������ɣ���ʼ���оֲ�����\n");

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
		localDBSCAN(data, dataLen);								//�ֲ�����dbscan�㷨����д��ͻ����
	}
	//printf("�ֲ�������ɣ���ʼ�ϲ�����\n");
	calConflict(localClusterCount);				//���ݳ�ͻ������дȫ�ִغ�ӳ��
	refillCluster();							//���ȫ��ӳ��
	//printf("�㷨��ɣ�\n");

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