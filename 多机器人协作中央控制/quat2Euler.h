//��Ҫ����ʵ��һЩС�͵� �Զ��庯��
#pragma once
#include <Eigen\Dense>
#include <math.h>
#include <iostream>

namespace zfun
{
	float numFormat(float num, int rz);//����С��������λ
	Eigen::Vector3d Quaterniond2Euler(const double x, const double y, const double z, const double w);

}
