
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

	int robotlist_checkIndex;//�洢Ŀǰѡ����robot�б��еĵڼ��������ˡ�



//����logo
	ULONG_PTR m_gdiplusToken;
//����
	struct task
	{
		//1����to point����
		//2:�������ߵ����񣬵������ߵ�ָ���ص㡣
		//3:ת��ָ���Ƕ�
		int taskname; 
		float x;
		float y;
		int lf;//������̬��ʱ��������ת��������ת����תΪ1 ��Ϊ-1
	};
	vector<vector<task>> taskqueue;//������У��ƶ���4������˳���Ӧ�Ż������б��еĻ����ˡ�

	vector<task> aimiTaskQueue; //������У���ʱ�ǰ��׼һ����˵�������С�
						   
	//���׼һ����˵Ŀ�������
	HANDLE haimiTaskMutex;//���ڶ��߳� ������,�����ϱ������м�����
	HANDLE haimiTaskrunThread;//����ִ���߳��߳̾��
	DWORD aimiTaskrunThreadID;//����ִ���߳��߳�ID

	//С�����˿�������
	HANDLE hTaskMutex;//���ڶ��߳� ������,�ϱ������м�����
	HANDLE hTaskrunThread[4];//����ִ���߳��߳̾��
	DWORD TaskrunThreadID[4];//����ִ���߳��߳�ID

	//������ȴ�����
	int bigtask=0;//0:û������ 1������ּ����� 2�������˱������
	HANDLE MultiRobotControl_Mutex;//���ڶ��߳� ������,�ϱ������м�����
	HANDLE hMultiRobotControlThread;//����ִ���߳��߳̾��
	DWORD MultiRobotControlThreadID;//����ִ���߳��߳�ID

	//���ץȡ�ź�
	int finishGet_flag=0;

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CMultiRobotApp theApp;
