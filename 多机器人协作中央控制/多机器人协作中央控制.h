
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
	int seleteimshow = -2;//ѡ����һ����ʾimg��-1����ȫ��ʾ,-2����ʾ
	int ThreadOn = 1; //=0�����˳��߳�

	//config
	FileStorage config;

	//��־λ����
	bool show2Dflag = false;//��ʾ2D��ͼ
	bool movecompFlag = true;//�˶�����

	Size showsize;//�����ʾ���ڵĴ�С


//����
	struct task
	{
		int taskname; //1����to point����
		float x;
		float y;
	};
	vector<task> taskqueue;//������У���ʱ��1�Ż����˵�������С�
						   
	HANDLE hTaskMutex;//���ڶ��߳� ������,�����ϱ������м�����
	HANDLE hTaskrunThread;//����ִ���߳��߳̾��
	DWORD TaskrunThreadID;//����ִ���߳��߳�ID
	DECLARE_MESSAGE_MAP()
};

extern CMultiRobotApp theApp;
