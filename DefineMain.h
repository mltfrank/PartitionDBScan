#ifndef _DEFINE_MAIN_H_
#define _DEFINE_MAIN_H_

/*
* 用于表示划分出的一个域
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
* 作为划分后的数据结构
*/
typedef struct PartitionData{
	double x;		//点的x坐标
	double y;		//点的y坐标
	double z;		//点的z坐标
	int pos;		//点原来的下标
	int flags = 0;	//点的属性flag，最后两位表示位于外域还是内域  11：外域  10：内域，冲突   00： 内域，不冲突
	//倒数第3位表示是否是核点 1：是  0：不是
	//倒数第4位表示是否已经访问过 1是  0：不是
} PartitionData;

//原问题定义
#define MIN_X 0	
#define MAX_X 1
#define MIN_Y 0
#define MAX_Y 1
#define MIN_Z 0
#define MAX_Z 1

#define EPS 0.06		//邻域范围
#define MINPOINTS 120	//临近点个数
#define N 5061			//数据集大小

//初始化划分的参数
#define STEP_X 0.5
#define STEP_Y 0.5
#define STEP_Z 0.5

//算法参数
#define MAX_DATA_PER_BLOCK 1024		//每块中点的最大数目，根据程序中需要的内存与GPU结构而定
#define PART_X (int)((MAX_X-MIN_X)/STEP_X)		//X维度上的划分数量
#define PART_Y (int)((MAX_Y-MIN_Y)/STEP_Y)		//y维度上的划分数量
#define PART_Z (int)((MAX_Z-MIN_Z)/STEP_Z)		//z维度上的划分数量
#define MAX_PART_Z 16								//允许进行z轴上的进一步划分，划分最多为8个
#define MAX_LOCAL_CLUSTER 64


extern double dataMatrix[N][3];													//数据集 声明
extern PartitionData* localDataMatrix;											//划分后的数组
extern Partition** partPartition;													//划分后的域
extern int* partitionLength;													//划分后的域长度

extern int conflictMatrix[MAX_LOCAL_CLUSTER][MAX_LOCAL_CLUSTER];				//冲突矩阵,元素为0时不发生冲突，元素为1是为冲突 声明
extern int clusterGlobalMap[MAX_LOCAL_CLUSTER];									//局部簇号到全局簇号的映射 声明
extern int clusterResult[N];													//聚类的结果，先后存放局部簇号和全局簇号 声明
extern int clusterIfCore[N];													//全局变量，存放是否是核点，用于判定冲突 声明
extern int localClusterCount;													//局部簇的全局统计，同时作为局部簇号
#endif