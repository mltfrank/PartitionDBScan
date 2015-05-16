#ifndef _LOCAL_DBSCAN_H_
#define _LOCAL_DBSCAN_H_

#include "DefineMain.h"

/*
将冲突区的点判断是否是核点，写入全局内存
*/
void writeConflictPointIfCore(PartitionData * data, int dataLen);

/*
DBSCAN算法
*/
void localDBSCAN(PartitionData * data, int dataLen);


#endif