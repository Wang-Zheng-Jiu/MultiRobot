//��Ҫ����ʵ��һЩС�͵� �Զ��庯��
#pragma once

#include "opencv2/opencv.hpp"
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
	int findVecterElm(std::vector<uint8_t> vec, uint8_t elm);
	

	//����֮��ľ���
	double distancePoint(cv::Vec3d p1, cv::Vec3d p2);
	

}
