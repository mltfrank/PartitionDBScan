#include "DefineMain.h"
#include "stdio.h"

#define EPS_SQU EPS*EPS
#define GET_DIS_SQU(p1, p2) (p1->x - p2->x)*(p1->x - p2->x) + (p1->y - p2->y)*(p1->y - p2->y) + (p1->z - p2->z)*(p1->z - p2->z)
#define NOISE -1

//返回距离的平方
double inline getDistanceSqu(PartitionData * p1, PartitionData * p2)
{
	double dis1 = (p1->x - p2->x);
	double dis2 = (p1->y - p2->y);
	double dis3 = (p1->z - p2->z);

	return dis1 * dis1 + dis2 * dis2 + dis3 * dis3;

}

/*
插入seedList
*/
void addIntoSeedList(int pt, int * seedList, int * seedCount, PartitionData * data, int * localClusterRes){
	if (!(data[pt].flags & 0x00000008) || localClusterRes[pt] == NOISE)	//如果还没访问过或者是noise，就加入seedList
		//加入noise的原因是需要重新为其标记簇
	{
		seedList[*seedCount] = pt;
		*seedCount += 1;
		data[pt].flags |= 0x00000008;		//标记访问过，防止重复访问
	}
}

/*
控制插入seedList的条件
*/
void addNearIntoSeedList(int pos, int dataLen, PartitionData * data, int * seedList, int * seedCount, int * nearBuffer, int * nearCount, int * localClusterRes)
{
	for (int j = 0; j < dataLen; j++)		//算距离，初始化seedlist
	{
		if (getDistanceSqu(&data[pos], &data[j]) <= EPS_SQU)
		{
			if (*nearCount < MINPOINTS + 1)	//当前还不满足核点条件，加入nearBuffer
			{
				nearBuffer[*nearCount] = j;
			}
			else{							//已经满足了，则增加到seedList
				addIntoSeedList(j, seedList, seedCount, data, localClusterRes);
			}
			*nearCount += 1;					//临近个数+1
		}
	}
}

/*
将冲突区的点判断是否是核点，写入全局内存
*/
void writeConflictPointIfCore(PartitionData * data, int dataLen)
{
	for (int i = 0; i < dataLen; i++)
	{
		if ((data[i].flags & 0x00000003) == 2)	//只算内域的冲突点
		{
			int nearCount = 0;
			for (int j = 0; j < dataLen; j++)		//算距离，统计个数
			{
				if (getDistanceSqu(&data[i], &data[j]) <= EPS_SQU)
					nearCount++;
			}
			if (nearCount >= MINPOINTS + 1)		//超出核点判定范围，标记核点
			{
				clusterIfCore[data[i].pos] = 1;		//访问全局内存
			}
		}
	}
}

/*
局部的DBSCAN算法
*/
void localDBSCAN(PartitionData * data, int dataLen)
{
	int nearBuffer[MINPOINTS + 1];		//临近点的buffer，多1是因为可能算入了本身的点
	int seedList[MAX_DATA_PER_BLOCK];	//seedList
	int seedCount = 0;					//当前seedList长度
	int nearCount = 0;					//临近点数量
	int localClusterNo;					//局部簇号
	int localClusterRes[MAX_DATA_PER_BLOCK];	//局部簇结果

	for (int i = 0; i < dataLen; i++)
	{
		localClusterRes[i] = -2;
	}

	for (int i = 0; i < dataLen; i++)
	{

		if (data[i].flags & 0x00000008)	//查看第4位是不是1，如果是，则已经访问过
			continue;
		data[i].flags |= 0x00000008;			//标记访问过

		addNearIntoSeedList(i, dataLen, data, seedList, &seedCount, nearBuffer, &nearCount, localClusterRes);	//计算距离，并且将临近点加入nearBuffer和seedList

		//块内同步，要计算是否是核点了
		if (nearCount >= MINPOINTS + 1)			//算上了自己，所以要多一个
		{
			localClusterNo = localClusterCount++;	//分配局部簇号
			//data[i].flags |= 0x00000004;			//标记是核点
			localClusterRes[i] = localClusterNo;	//标记这个点的簇号
			for (int m = 0; m < MINPOINTS + 1; m++)		//将nearBuffer中的点移动到seedList
			{
				addIntoSeedList(nearBuffer[m], seedList, &seedCount, data, localClusterRes);
			}
			nearCount = 0;							//nearBuffer清零，初始化seedList完成

			while (seedCount){	//seedList不为空
				int seed = seedList[--seedCount];

				data[seed].flags |= 0x00000008;				//标记访问过了
				localClusterRes[seed] = localClusterNo;		//标记这个点的簇号

				//计算距离并加入seedList
				addNearIntoSeedList(seed, dataLen, data, seedList, &seedCount, nearBuffer, &nearCount, localClusterRes);
				if (nearCount >= MINPOINTS + 1)				//这个点是核点
				{
					//data[i].flags |= 0x00000004;			//标记是核点

					for (int m = 0; m < MINPOINTS + 1; m++)		//将nearBuffer中的点移动到seedList
						addIntoSeedList(nearBuffer[m], seedList, &seedCount, data, localClusterRes);
				}
				nearCount = 0;							//清空nearBuffer，与addNearIntoSeedList对应
			}
		}
		else
		{
			localClusterRes[i] = NOISE;			//暂时将簇号标记为noise，等别的簇来写它
		}

		nearCount = 0;							//清空nearBuffer，与addNearIntoSeedList对应

	}



	//本地聚类结束，往全局结果中写，同时填写冲突矩阵
	for (int i = 0; i < dataLen; i++)
	{
		if (localClusterRes[i] >= 0){		//不是noise，我要写！
			int res = clusterResult[data[i].pos];		//前面的结果
			int ifCore = clusterIfCore[data[i].pos];	//是否是核点
			if (res < 0)	//NOISE，肆无忌惮地填写
			{
				clusterResult[data[i].pos] = localClusterRes[i];
			}
			else{	//根据已经写入的内容判断类别			
				if (ifCore == 1)			//是核点，发生冲突，localClusterRes[i]代表的行中标注出每一个列是否冲突
				{	//否则就不是冲突，因为此时核点是之前写好的，是一定正确的
					int a, b;
					if (localClusterRes[i] < res)		//通过这种方法，形成上三角矩阵
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
