#ifndef _DEFINE_MAIN_H_
#define _DEFINE_MAIN_H_

/*
* ���ڱ�ʾ���ֳ���һ����
*/
typedef struct Partition{
	double x_min;
	double x_max;
	double y_min;
	double y_max;
	double z_min;
	double z_max;
} Partition;

/*
* ��Ϊ���ֺ�����ݽṹ
*/
typedef struct PartitionData{
	double x;		//���x����
	double y;		//���y����
	double z;		//���z����
	int pos;		//��ԭ�����±�
	int flags = 0;	//�������flag�������λ��ʾλ������������  11������  10�����򣬳�ͻ   00�� ���򣬲���ͻ
	//������3λ��ʾ�Ƿ��Ǻ˵� 1����  0������
	//������4λ��ʾ�Ƿ��Ѿ����ʹ� 1��  0������
} PartitionData;

//ԭ���ⶨ��
#define MIN_X 0	
#define MAX_X 1
#define MIN_Y 0
#define MAX_Y 1
#define MIN_Z 0
#define MAX_Z 1

#define EPS 0.06		//����Χ
#define MINPOINTS 120	//�ٽ������
#define N 5061			//���ݼ���С

//��ʼ�����ֵĲ���
#define STEP_X 0.5
#define STEP_Y 0.5
#define STEP_Z 0.5

//�㷨����
#define MAX_DATA_PER_BLOCK 1024		//ÿ���е�������Ŀ�����ݳ�������Ҫ���ڴ���GPU�ṹ����
#define PART_X (int)((MAX_X-MIN_X)/STEP_X)		//Xά���ϵĻ�������
#define PART_Y (int)((MAX_Y-MIN_Y)/STEP_Y)		//yά���ϵĻ�������
#define PART_Z (int)((MAX_Z-MIN_Z)/STEP_Z)		//zά���ϵĻ�������
#define MAX_PART_Z 16								//�������z���ϵĽ�һ�����֣��������Ϊ8��
#define MAX_LOCAL_CLUSTER 64


extern double dataMatrix[N][3];													//���ݼ� ����
extern PartitionData* localDataMatrix;											//���ֺ������
extern Partition** partPartition;													//���ֺ����
extern int* partitionLength;													//���ֺ���򳤶�

extern int conflictMatrix[MAX_LOCAL_CLUSTER][MAX_LOCAL_CLUSTER];				//��ͻ����,Ԫ��Ϊ0ʱ��������ͻ��Ԫ��Ϊ1��Ϊ��ͻ ����
extern int clusterGlobalMap[MAX_LOCAL_CLUSTER];									//�ֲ��غŵ�ȫ�ִغŵ�ӳ�� ����
extern int clusterResult[N];													//����Ľ�����Ⱥ��žֲ��غź�ȫ�ִغ� ����
extern int clusterIfCore[N];													//ȫ�ֱ���������Ƿ��Ǻ˵㣬�����ж���ͻ ����
extern int localClusterCount;													//�ֲ��ص�ȫ��ͳ�ƣ�ͬʱ��Ϊ�ֲ��غ�
#endif