#include "DefineMain.h"
#include "stdio.h"

/*
深度遍历
*/
void dfs(int clusterNo, int lineNo, int localClusterCount)
{
	for (int i = lineNo + 1; i < localClusterCount; i++)
	{
		if (conflictMatrix[lineNo][i] && clusterGlobalMap[i] < 0)	//发生冲突而且还没有标记过
		{
			clusterGlobalMap[i] = clusterNo;	//标记全局簇号
			dfs(clusterNo, i, localClusterCount);		//深度优先遍历
		}
	}
}

/*
用深度优先的遍历来处理冲突
*/
void calConflict(int localClusterCount)
{
	for (int i = 0; i < localClusterCount; i++)	//遍历每一行
		if (clusterGlobalMap[i] < 0)	//还没有处理过这一行
		{
			clusterGlobalMap[i] = i;	//标记全局簇号
			dfs(i, i, localClusterCount);
		}
}

/*
用新的map重填聚类结果
*/
void refillCluster()
{
	for (int i = 0; i < N; i++)
		if (clusterResult[i] >= 0)
			clusterResult[i] = clusterGlobalMap[clusterResult[i]];
}

