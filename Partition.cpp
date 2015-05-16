#include "Partition.h"
#include "stdio.h"
#include "stdlib.h"
#include <memory.h>

//划分过程中需要用到的中间变量
int partNumMap[PART_X][PART_Y][PART_Z][MAX_PART_Z];						//整理前后的映射关系
int partition_Count;														//划分域的个数
Partition partition[PART_X][PART_Y][PART_Z][MAX_PART_Z];				//划分方案
int partitionCount[PART_X][PART_Y][PART_Z][MAX_PART_Z];					//每个划分的统计数量

/*
*初始化划分
*/
void initPartition()
{
	int x_c, y_c, z_c;
	double i, j, k;
	for (i = MIN_X, x_c = 0; i < MAX_X; i += STEP_X)		//X轴
	{
		for (j = MIN_Y, y_c = 0; j < MAX_Y; j += STEP_Y)		//Y轴
		{
			for (k = MIN_Z, z_c = 0; k < MAX_Z; k += STEP_Z)		//Z轴
			{
				partition[x_c][y_c][z_c][0].x_min = i;
				partition[x_c][y_c][z_c][0].x_max = i + STEP_X;

				partition[x_c][y_c][z_c][0].y_min = j;
				partition[x_c][y_c][z_c][0].y_max = j + STEP_Y;

				partition[x_c][y_c][z_c][0].z_min = k;
				partition[x_c][y_c][z_c][0].z_max = k + STEP_Z;

				z_c++;
			}
			y_c++;
		}
		x_c++;
	}
	partition_Count = x_c * y_c * z_c;	//初始化划分的个数
}

/*
统计个数
*/
void getPointCountInPartition()
{
	memset(partitionCount, 0, PART_X*PART_Y*PART_Z*MAX_PART_Z*sizeof(int));	//初始化为0
	for (int m = 0; m < N; m++)	//逐个对点遍历，查找所在的块。
	{
		double x = dataMatrix[m][0];
		double y = dataMatrix[m][1];
		double z = dataMatrix[m][2];

		for (int i = 0; i < PART_X; i++)
		{
			for (int j = 0; j < PART_Y; j++)
			{
				for (int k = 0; k < PART_Z; k++)
				{
					for (int l = 0; l < MAX_PART_Z; l++)
					{
						double xmax = partition[i][j][k][l].x_max + EPS;
						double xmin = partition[i][j][k][l].x_min - EPS;
						double ymax = partition[i][j][k][l].y_max + EPS;
						double ymin = partition[i][j][k][l].y_min - EPS;
						double zmax = partition[i][j][k][l].z_max + EPS;
						double zmin = partition[i][j][k][l].z_min - EPS;

						if (x <= xmax && x >= xmin && y <= ymax && y >= ymin && z <= zmax && z >= zmin)
						{
							partitionCount[i][j][k][l] ++;
						}
					}
				}
			}
		}
	}
}



int findFreePartition(int i, int j, int k, int l)
{
	for (int idx = l + 1; idx < MAX_PART_Z; idx++)
	{
		if (partition[i][j][k][idx].z_max == partition[i][j][k][idx].z_min)	//还没有占用
			return idx;
	}

	return 0;
}

int resetPartition(int i, int j, int k, int l, int* newPartition)
{
	double step = (partition[i][j][k][l].z_max - partition[i][j][k][l].z_min) / 2;
	double init = partition[i][j][k][l].z_min;
	partition[i][j][k][l].z_max = init + step;
	newPartition[0] = l;	//第一个块是原来的块

	int domPos = l;

	if (domPos = findFreePartition(i, j, k, domPos))	//找到了新块，重新给z轴的范围赋值
	{
		newPartition[1] = domPos;	//存放新块的下标，z轴范围是逐个增长的
		Partition* newPartition = &partition[i][j][k][domPos];
		*newPartition = partition[i][j][k][l];	//先复制，继承原来节点的信息
		init += step;
		newPartition->z_min = init;
		newPartition->z_max = init + step;
	}
	else
	{											//找不到新块，报错返回
		printf("没有空余块，划分失败!\n");
		return 0;
	}


	return 1;
}



/*
*得到域的个数
*/
int getPartitionCount()
{
	int count = 0;
	for (int i = 0; i < PART_X; i++)
		for (int j = 0; j < PART_Y; j++)
			for (int k = 0; k < PART_Z; k++)
				for (int l = 0; l < MAX_PART_Z; l++)
					if (partitionCount[i][j][k][l] != 0)	//该域中有点
						count++;
	return count;
}


/*
将划分的结果移动到线性的数组当中
*/
void getPartitionMap()
{
	int count = 0;
	for (int i = 0; i < PART_X; i++)
		for (int j = 0; j < PART_Y; j++)
			for (int k = 0; k < PART_Z; k++)
				for (int l = 0; l < MAX_PART_Z; l++)
					if (partitionCount[i][j][k][l] != 0)	//该域中有点
					{
						partNumMap[i][j][k][l] = count;
						partPartition[count] = &partition[i][j][k][l];
						partitionLength[count] = partitionCount[i][j][k][l];
						count++;
					}
}




int partitionValidation()
{
	int flag = 1;		//是否合格
	//查看各个域的数量是否超过了统计
	for (int i = 0; i < PART_X; i++)
	{
		for (int j = 0; j < PART_Y; j++)
		{
			for (int k = 0; k < PART_Z; k++)
			{
				for (int l = 0; l < MAX_PART_Z; l++)
				{
					int dim = (partitionCount[i][j][k][l] - 1) / MAX_DATA_PER_BLOCK;
					if (dim > 0)	//说明这个块超大了，至少要分割成dim+1份
					{
						flag = 0;	//标记成不合格
						int newPartition[2];

						if (!resetPartition(i, j, k, l, newPartition))		//对块进行切割，如果失败，则报错返回
							return 1;
					}
				}
			}
		}
	}

	return flag;
}

/*
根据划分的一维数组，将一个划分中的点存入同一个数组当中
*/
void movePointsIntoPartition(int partitionCount){
	for (int i = 0; i < partitionCount; i++)
	{
		partitionLength[i] = 0;
	}

	for (int i = 0; i < N; i++)
	{
		double x = dataMatrix[i][0];
		double y = dataMatrix[i][1];
		double z = dataMatrix[i][2];

		for (int j = 0; j < partitionCount; j++)
		{
			double xmax = partPartition[j]->x_max + EPS;
			double xmin = partPartition[j]->x_min - EPS;
			double ymax = partPartition[j]->y_max + EPS;
			double ymin = partPartition[j]->y_min - EPS;
			double zmax = partPartition[j]->z_max + EPS;
			double zmin = partPartition[j]->z_min - EPS;

			double xmax2 = partPartition[j]->x_max;
			double xmin2 = partPartition[j]->x_min;
			double ymax2 = partPartition[j]->y_max;
			double ymin2 = partPartition[j]->y_min;
			double zmax2 = partPartition[j]->z_max;
			double zmin2 = partPartition[j]->z_min;

			double xmax3 = partPartition[j]->x_max - EPS;
			double xmin3 = partPartition[j]->x_min + EPS;
			double ymax3 = partPartition[j]->y_max - EPS;
			double ymin3 = partPartition[j]->y_min + EPS;
			double zmax3 = partPartition[j]->z_max - EPS;
			double zmin3 = partPartition[j]->z_min + EPS;

			if (x <= xmax && x >= xmin && y <= ymax && y >= ymin && z <= zmax && z >= zmin)
			{
				if (partitionLength[j] > MAX_DATA_PER_BLOCK)
				{
					printf("块超出限制！\n");
					break;
				}

				PartitionData* data2 = localDataMatrix + j * MAX_DATA_PER_BLOCK + partitionLength[j];	//该点对应的内存
				data2->x = x;
				data2->y = y;
				data2->z = z;
				data2->pos = i;
				if (x <= xmin2 || x > xmax2 || y <= ymin2 || y >= ymax2 || z <= zmin2 || z >= zmax2)
					data2->flags = 3;	//表示位于外域
				else if (x < xmax3 && x > xmin3 && y < ymax3 && y > ymin3 && z < zmax3 && z > zmin3)
					data2->flags = 0;	//内域不冲突
				else
					data2->flags = 2;	//内域冲突
				partitionLength[j] ++;
			}
		}
	}

}












void printPartition()
{

	//输出查看结果
	printf("划分域的结果：\n");
	for (int i = 0; i < PART_X; i++)
	{
		for (int j = 0; j < PART_Y; j++)
		{
			for (int k = 0; k < PART_Z; k++)
			{
				for (int l = 0; l < MAX_PART_Z; l++)
				{
					printf(" %1.2f, %1.2f, %1.2f ", partition[i][j][k][l].x_max, partition[i][j][k][l].y_max, partition[i][j][k][l].z_max);
					printf("  |  ");
				}
				printf("\n");
			}
			printf("\n\n");
		}
		printf("***********************");
		printf("\n\n\n");
	}

	printf("----------------------------------------------------------------------------");
	printf("----------------------------------------------------------------------------");
	printf("划分域的统计结果\n");
	for (int i = 0; i < PART_X; i++)
	{
		for (int j = 0; j < PART_Y; j++)
		{
			for (int k = 0; k < PART_Z; k++)
			{
				for (int l = 0; l < MAX_PART_Z; l++)
				{
					printf(" %d ", partitionCount[i][j][k][l]);
					printf("   |   ");
				}
				printf("\n");
			}
			printf("\n\n");
		}
		printf("***********************");
		printf("\n\n\n");
	}
}
