#include "stdafx.h" //����ļ�һ��Ҫ��ǰ�� �������ˣ�������
#include "quat2Euler.h"



namespace zfun
{
	float numFormat(float num, int rz)
	{
		//����������
		float e = pow(10, rz);
		return floor(num*e) / e;
	}

	Eigen::Vector3d Quaterniond2Euler(const double x, const double y, const double z, const double w)
	{
		Eigen::Quaterniond q;
		q.x() = x;
		q.y() = y;
		q.z() = z;
		q.w() = w;
		Eigen::Vector3d euler = q.toRotationMatrix().eulerAngles(2, 1, 0);
		return euler;
	}
}