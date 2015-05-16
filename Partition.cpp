#include "Partition.h"
#include "stdio.h"
#include "stdlib.h"
#include <memory.h>

//���ֹ�������Ҫ�õ����м����
int partNumMap[PART_X][PART_Y][PART_Z][MAX_PART_Z];						//����ǰ���ӳ���ϵ
int partition_Count;														//������ĸ���
Partition partition[PART_X][PART_Y][PART_Z][MAX_PART_Z];				//���ַ���
int partitionCount[PART_X][PART_Y][PART_Z][MAX_PART_Z];					//ÿ�����ֵ�ͳ������

/*
*��ʼ������
*/
void initPartition()
{
	int x_c, y_c, z_c;
	double i, j, k;
	for (i = MIN_X, x_c = 0; i < MAX_X; i += STEP_X)		//X��
	{
		for (j = MIN_Y, y_c = 0; j < MAX_Y; j += STEP_Y)		//Y��
		{
			for (k = MIN_Z, z_c = 0; k < MAX_Z; k += STEP_Z)		//Z��
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
	partition_Count = x_c * y_c * z_c;	//��ʼ�����ֵĸ���
}

/*
ͳ�Ƹ���
*/
void getPointCountInPartition()
{
	memset(partitionCount, 0, PART_X*PART_Y*PART_Z*MAX_PART_Z*sizeof(int));	//��ʼ��Ϊ0
	for (int m = 0; m < N; m++)	//����Ե�������������ڵĿ顣
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
		if (partition[i][j][k][idx].z_max == partition[i][j][k][idx].z_min)	//��û��ռ��
			return idx;
	}

	return 0;
}

int resetPartition(int i, int j, int k, int l, int* newPartition)
{
	double step = (partition[i][j][k][l].z_max - partition[i][j][k][l].z_min) / 2;
	double init = partition[i][j][k][l].z_min;
	partition[i][j][k][l].z_max = init + step;
	newPartition[0] = l;	//��һ������ԭ���Ŀ�

	int domPos = l;

	if (domPos = findFreePartition(i, j, k, domPos))	//�ҵ����¿飬���¸�z��ķ�Χ��ֵ
	{
		newPartition[1] = domPos;	//����¿���±꣬z�᷶Χ�����������
		Partition* newPartition = &partition[i][j][k][domPos];
		*newPartition = partition[i][j][k][l];	//�ȸ��ƣ��̳�ԭ���ڵ����Ϣ
		init += step;
		newPartition->z_min = init;
		newPartition->z_max = init + step;
	}
	else
	{											//�Ҳ����¿飬������
		printf("û�п���飬����ʧ��!\n");
		return 0;
	}


	return 1;
}



/*
*�õ���ĸ���
*/
int getPartitionCount()
{
	int count = 0;
	for (int i = 0; i < PART_X; i++)
		for (int j = 0; j < PART_Y; j++)
			for (int k = 0; k < PART_Z; k++)
				for (int l = 0; l < MAX_PART_Z; l++)
					if (partitionCount[i][j][k][l] != 0)	//�������е�
						count++;
	return count;
}


/*
�����ֵĽ���ƶ������Ե����鵱��
*/
void getPartitionMap()
{
	int count = 0;
	for (int i = 0; i < PART_X; i++)
		for (int j = 0; j < PART_Y; j++)
			for (int k = 0; k < PART_Z; k++)
				for (int l = 0; l < MAX_PART_Z; l++)
					if (partitionCount[i][j][k][l] != 0)	//�������е�
					{
						partNumMap[i][j][k][l] = count;
						partPartition[count] = &partition[i][j][k][l];
						partitionLength[count] = partitionCount[i][j][k][l];
						count++;
					}
}




int partitionValidation()
{
	int flag = 1;		//�Ƿ�ϸ�
	//�鿴������������Ƿ񳬹���ͳ��
	for (int i = 0; i < PART_X; i++)
	{
		for (int j = 0; j < PART_Y; j++)
		{
			for (int k = 0; k < PART_Z; k++)
			{
				for (int l = 0; l < MAX_PART_Z; l++)
				{
					int dim = (partitionCount[i][j][k][l] - 1) / MAX_DATA_PER_BLOCK;
					if (dim > 0)	//˵������鳬���ˣ�����Ҫ�ָ��dim+1��
					{
						flag = 0;	//��ǳɲ��ϸ�
						int newPartition[2];

						if (!resetPartition(i, j, k, l, newPartition))		//�Կ�����и���ʧ�ܣ��򱨴���
							return 1;
					}
				}
			}
		}
	}

	return flag;
}

/*
���ݻ��ֵ�һά���飬��һ�������еĵ����ͬһ�����鵱��
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
					printf("�鳬�����ƣ�\n");
					break;
				}

				PartitionData* data2 = localDataMatrix + j * MAX_DATA_PER_BLOCK + partitionLength[j];	//�õ��Ӧ���ڴ�
				data2->x = x;
				data2->y = y;
				data2->z = z;
				data2->pos = i;
				if (x <= xmin2 || x > xmax2 || y <= ymin2 || y >= ymax2 || z <= zmin2 || z >= zmax2)
					data2->flags = 3;	//��ʾλ������
				else if (x < xmax3 && x > xmin3 && y < ymax3 && y > ymin3 && z < zmax3 && z > zmin3)
					data2->flags = 0;	//���򲻳�ͻ
				else
					data2->flags = 2;	//�����ͻ
				partitionLength[j] ++;
			}
		}
	}

}












void printPartition()
{

	//����鿴���
	printf("������Ľ����\n");
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
	printf("�������ͳ�ƽ��\n");
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
