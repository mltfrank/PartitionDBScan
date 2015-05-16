#include "DefineMain.h"
#include "stdio.h"

#define EPS_SQU EPS*EPS
#define GET_DIS_SQU(p1, p2) (p1->x - p2->x)*(p1->x - p2->x) + (p1->y - p2->y)*(p1->y - p2->y) + (p1->z - p2->z)*(p1->z - p2->z)
#define NOISE -1

//���ؾ����ƽ��
double inline getDistanceSqu(PartitionData * p1, PartitionData * p2)
{
	double dis1 = (p1->x - p2->x);
	double dis2 = (p1->y - p2->y);
	double dis3 = (p1->z - p2->z);

	return dis1 * dis1 + dis2 * dis2 + dis3 * dis3;

}

/*
����seedList
*/
void addIntoSeedList(int pt, int * seedList, int * seedCount, PartitionData * data, int * localClusterRes){
	if (!(data[pt].flags & 0x00000008) || localClusterRes[pt] == NOISE)	//�����û���ʹ�������noise���ͼ���seedList
		//����noise��ԭ������Ҫ����Ϊ���Ǵ�
	{
		seedList[*seedCount] = pt;
		*seedCount += 1;
		data[pt].flags |= 0x00000008;		//��Ƿ��ʹ�����ֹ�ظ�����
	}
}

/*
���Ʋ���seedList������
*/
void addNearIntoSeedList(int pos, int dataLen, PartitionData * data, int * seedList, int * seedCount, int * nearBuffer, int * nearCount, int * localClusterRes)
{
	for (int j = 0; j < dataLen; j++)		//����룬��ʼ��seedlist
	{
		if (getDistanceSqu(&data[pos], &data[j]) <= EPS_SQU)
		{
			if (*nearCount < MINPOINTS + 1)	//��ǰ��������˵�����������nearBuffer
			{
				nearBuffer[*nearCount] = j;
			}
			else{							//�Ѿ������ˣ������ӵ�seedList
				addIntoSeedList(j, seedList, seedCount, data, localClusterRes);
			}
			*nearCount += 1;					//�ٽ�����+1
		}
	}
}

/*
����ͻ���ĵ��ж��Ƿ��Ǻ˵㣬д��ȫ���ڴ�
*/
void writeConflictPointIfCore(PartitionData * data, int dataLen)
{
	for (int i = 0; i < dataLen; i++)
	{
		if ((data[i].flags & 0x00000003) == 2)	//ֻ������ĳ�ͻ��
		{
			int nearCount = 0;
			for (int j = 0; j < dataLen; j++)		//����룬ͳ�Ƹ���
			{
				if (getDistanceSqu(&data[i], &data[j]) <= EPS_SQU)
					nearCount++;
			}
			if (nearCount >= MINPOINTS + 1)		//�����˵��ж���Χ����Ǻ˵�
			{
				clusterIfCore[data[i].pos] = 1;		//����ȫ���ڴ�
			}
		}
	}
}

/*
�ֲ���DBSCAN�㷨
*/
void localDBSCAN(PartitionData * data, int dataLen)
{
	int nearBuffer[MINPOINTS + 1];		//�ٽ����buffer����1����Ϊ���������˱���ĵ�
	int seedList[MAX_DATA_PER_BLOCK];	//seedList
	int seedCount = 0;					//��ǰseedList����
	int nearCount = 0;					//�ٽ�������
	int localClusterNo;					//�ֲ��غ�
	int localClusterRes[MAX_DATA_PER_BLOCK];	//�ֲ��ؽ��

	for (int i = 0; i < dataLen; i++)
	{
		localClusterRes[i] = -2;
	}

	for (int i = 0; i < dataLen; i++)
	{

		if (data[i].flags & 0x00000008)	//�鿴��4λ�ǲ���1������ǣ����Ѿ����ʹ�
			continue;
		data[i].flags |= 0x00000008;			//��Ƿ��ʹ�

		addNearIntoSeedList(i, dataLen, data, seedList, &seedCount, nearBuffer, &nearCount, localClusterRes);	//������룬���ҽ��ٽ������nearBuffer��seedList

		//����ͬ����Ҫ�����Ƿ��Ǻ˵���
		if (nearCount >= MINPOINTS + 1)			//�������Լ�������Ҫ��һ��
		{
			localClusterNo = localClusterCount++;	//����ֲ��غ�
			//data[i].flags |= 0x00000004;			//����Ǻ˵�
			localClusterRes[i] = localClusterNo;	//��������Ĵغ�
			for (int m = 0; m < MINPOINTS + 1; m++)		//��nearBuffer�еĵ��ƶ���seedList
			{
				addIntoSeedList(nearBuffer[m], seedList, &seedCount, data, localClusterRes);
			}
			nearCount = 0;							//nearBuffer���㣬��ʼ��seedList���

			while (seedCount){	//seedList��Ϊ��
				int seed = seedList[--seedCount];

				data[seed].flags |= 0x00000008;				//��Ƿ��ʹ���
				localClusterRes[seed] = localClusterNo;		//��������Ĵغ�

				//������벢����seedList
				addNearIntoSeedList(seed, dataLen, data, seedList, &seedCount, nearBuffer, &nearCount, localClusterRes);
				if (nearCount >= MINPOINTS + 1)				//������Ǻ˵�
				{
					//data[i].flags |= 0x00000004;			//����Ǻ˵�

					for (int m = 0; m < MINPOINTS + 1; m++)		//��nearBuffer�еĵ��ƶ���seedList
						addIntoSeedList(nearBuffer[m], seedList, &seedCount, data, localClusterRes);
				}
				nearCount = 0;							//���nearBuffer����addNearIntoSeedList��Ӧ
			}
		}
		else
		{
			localClusterRes[i] = NOISE;			//��ʱ���غű��Ϊnoise���ȱ�Ĵ���д��
		}

		nearCount = 0;							//���nearBuffer����addNearIntoSeedList��Ӧ

	}



	//���ؾ����������ȫ�ֽ����д��ͬʱ��д��ͻ����
	for (int i = 0; i < dataLen; i++)
	{
		if (localClusterRes[i] >= 0){		//����noise����Ҫд��
			int res = clusterResult[data[i].pos];		//ǰ��Ľ��
			int ifCore = clusterIfCore[data[i].pos];	//�Ƿ��Ǻ˵�
			if (res < 0)	//NOISE�����޼ɵ�����д
			{
				clusterResult[data[i].pos] = localClusterRes[i];
			}
			else{	//�����Ѿ�д��������ж����			
				if (ifCore == 1)			//�Ǻ˵㣬������ͻ��localClusterRes[i]��������б�ע��ÿһ�����Ƿ��ͻ
				{	//����Ͳ��ǳ�ͻ����Ϊ��ʱ�˵���֮ǰд�õģ���һ����ȷ��
					int a, b;
					if (localClusterRes[i] < res)		//ͨ�����ַ������γ������Ǿ���
					{
						a = localClusterRes[i];
						b = res;
					}
					else if (localClusterRes[i] > res)
					{
						a = res;
						b = localClusterRes[i];
					}
					conflictMatrix[a][b] = 1;
				}
			}
		}
	}
}
