//��Ҫ����ʵ��һЩС�͵� �Զ��庯��
#pragma once
#include "Eigen\Dense"
#include <math.h>
#include <iostream>
#include <vector>
#include <string>

namespace zfun
{
	float numFormat(float num, int rz);//����С��������λ
	//��Ԫ��תŷ����
	Eigen::Vector3d Quaterniond2Euler(const double x, const double y, const double z, const double w);

	//����vectorԪ��
	int findVecterElm(std::vector<int> vec, int elm);
	int findVecterElm(std::vector<std::string> vec, std::string elm);
	

}
