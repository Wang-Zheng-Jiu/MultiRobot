
// �������Э���������.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "robotAPI.h"
#include "zfun.h"
#include "IPClocation.h"

using namespace std;
// CMultiRobotApp: 
// �йش����ʵ�֣������ �������Э���������.cpp
//

class CMultiRobotApp : public CWinApp
{
public:
	CMultiRobotApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	ROBOTServer robotServer;
	IPClocation visionLSys;//�Ӿ���λϵͳ
	vector<IPCobj> obj;//�����е�����
	vector<vector<IPCobj>> everyIPCobj;

	//�����һЩ������Ϣ
	vector<Mat> IPCshowImg;//������ͷ����ȡ�����ľ�������Ļ��棬��ʵʱ���¡�
	int seleteimshow = -1;//ѡ����һ����ʾimg��-1����ȫ��ʾ
	int ThreadOn = 1; //=0�����˳��߳�

	//config
	FileStorage config;

	//��־λ����
	bool show2Dflag = false;//��ʾ2D��ͼ
	bool movecompFlag = false;//�˶�����

	DECLARE_MESSAGE_MAP()
};

extern CMultiRobotApp theApp;
