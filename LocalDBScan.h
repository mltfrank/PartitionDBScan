#ifndef _LOCAL_DBSCAN_H_
#define _LOCAL_DBSCAN_H_

#include "DefineMain.h"

/*
����ͻ���ĵ��ж��Ƿ��Ǻ˵㣬д��ȫ���ڴ�
*/
void writeConflictPointIfCore(PartitionData * data, int dataLen);

/*
DBSCAN�㷨
*/
void localDBSCAN(PartitionData * data, int dataLen);


#endif