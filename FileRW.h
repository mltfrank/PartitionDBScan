#ifndef _FILE_R_W_H_
#define _FILE_R_W_H_

#include <stdio.h>
#include "stdlib.h"
#include "assert.h"
#include "DefineMain.h"


const char file_in[] = "Normalization";

void load_data(double dataMatrix[N][3]) {
	double temp;//配合文件格式，无用     
	int point_count = 0;//记录读取的数据个数     
	FILE * fin;
	fopen_s(&fin, file_in, "r");
	while (!feof(fin) && point_count < N)
	{
		fscanf_s(fin, "%lf\t%lf\t%lf\t%lf\n", &temp, &dataMatrix[point_count][0], &dataMatrix[point_count][1], &dataMatrix[point_count][2]);
		point_count++;
	}
	fclose(fin);

}

#endif