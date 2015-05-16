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
����Ԥ��������ʼ������
*/
void initPartition();

/*
ͳ��ÿ�����ֵ��е������
*/
void getPointCountInPartition();

/*
�ж������Ƿ���Ч�������Ч�����»���
����֤���ֵĽ����Ч
*/
int partitionValidation();


/*
ͳ�ƻ�ȡ��ǰ���ֵ�����
*/
int getPartitionCount();

/*
�����ֺ�Ľ���ɸ�ά����ת�Ƶ��������鵱��
*/
void getPartitionMap();

/*
���ݻ��ֵ�һά���飬��һ�������еĵ����ͬһ�����鵱��
*/
void movePointsIntoPartition(int partitionCount);

/*
��ӡ���ֽ����������֤
*/
void printPartition();

#endif