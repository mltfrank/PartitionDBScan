#include "DefineMain.h"
#include "stdio.h"

/*
��ȱ���
*/
void dfs(int clusterNo, int lineNo, int localClusterCount)
{
	for (int i = lineNo + 1; i < localClusterCount; i++)
	{
		if (conflictMatrix[lineNo][i] && clusterGlobalMap[i] < 0)	//������ͻ���һ�û�б�ǹ�
		{
			clusterGlobalMap[i] = clusterNo;	//���ȫ�ִغ�
			dfs(clusterNo, i, localClusterCount);		//������ȱ���
		}
	}
}

/*
��������ȵı����������ͻ
*/
void calConflict(int localClusterCount)
{
	for (int i = 0; i < localClusterCount; i++)	//����ÿһ��
		if (clusterGlobalMap[i] < 0)	//��û�д������һ��
		{
			clusterGlobalMap[i] = i;	//���ȫ�ִغ�
			dfs(i, i, localClusterCount);
		}
}

/*
���µ�map���������
*/
void refillCluster()
{
	for (int i = 0; i < N; i++)
		if (clusterResult[i] >= 0)
			clusterResult[i] = clusterGlobalMap[clusterResult[i]];
}

