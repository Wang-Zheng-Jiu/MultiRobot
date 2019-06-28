
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
	uArmSocket uArm;//��е�ۿ���
	ROBOTServer robotServer;
	IPClocation visionLSys;//�Ӿ���λϵͳ
	vector<IPCobj> obj;//�����е�����
	vector<vector<IPCobj>> everyIPCobj;

	//�����һЩ������Ϣ
	HANDLE IPCshowImgMutex;//ר��ΪIPCshowImg���ƵĻ�������
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
		//1:����to point����
		//2:�������ߵ����񣬵������ߵ�ָ���ص㡣
		//3:ת��ָ���Ƕ�
		//4:��е�۶���ָ��û�е���˶���ĳ��λ�ã�ץȡ�ٷŵ�ָ��λ�á�x,y,z  to  x1,y1,z1�����ǲ���������
		//5:��е�۶���ָ��û�е�۷������塣
		int taskname; 
		float x;
		float y;
		int lf;//������̬��ʱ��������ת��������ת����תΪ1 ��Ϊ-1 ,����̾���תȦΪ0
		float z;
		float x1;
		float y1;
		float z1;
	};
	
	vector<vector<task>> taskqueue;//������У��ƶ���4������˳���Ӧ�Ż������б��еĻ����ˡ�
	void taskqueue_push(int index,int taskn,Point2f toP,int lf=1);

	vector<task> aimiTaskQueue; //������У���ʱ�ǰ��׼һ����˵�������С�

	vector<task> uArmTaskQueue; //������У���е�۵��������
	void uArmTaskQueue_push(int taskn, Vec3f p1, Vec3f p2);

						   
	//���׼һ����˵Ŀ�������
	HANDLE haimiTaskMutex;//���ڶ��߳� ������,��������н��м�����
	HANDLE haimiTaskrunThread;//����ִ���߳��߳̾��
	DWORD aimiTaskrunThreadID;//����ִ���߳��߳�ID

	//С�����˿�������
	HANDLE hTaskMutex;//���ڶ��߳� ������,������н��м�����
	HANDLE hTaskrunThread[4];//����ִ���߳��߳̾��
	DWORD TaskrunThreadID[4];//����ִ���߳��߳�ID

	//��е�۵Ŀ�������
	HANDLE huArmTaskMutex;//���ڶ��߳� ������,��������н��м�����
	HANDLE huArmTaskrunThread;//����ִ���߳��߳̾��
	DWORD uArmTaskrunThreadID;//����ִ���߳��߳�ID

	//������ȴ�����
	int bigtask=0;//0:û������ 1������ּ����� 2�������˱������
	HANDLE MultiRobotControl_Mutex;//���ڶ��߳� ������,�ϱ������м�����
	HANDLE hMultiRobotControlThread;//����ִ���߳��߳̾��
	DWORD MultiRobotControlThreadID;//����ִ���߳��߳�ID

	//���ڴ�ӡ��mfc���������򣬿��̲߳���
	CString printd_str;
	HANDLE printd_str_Mutex;//printd_str�����Ļ�������
	void printd(CString cout);//����ַ�����Ϣ
	void printd(string cout);//����ַ�����Ϣ
	void printd(int cout);
	void printd(float cout);

	//ʶ�𵽵�����λ��
	vector<IPCobj> rgbobj;
	HANDLE rgbobj_Mutex;//rgbobj�����Ļ�������


	//���ץȡ�ź�
	int finishGet_flag=0;

	//��λ�켣��¼:�ռ������ʱ�����꣨x,y,z,t��
	vector<Vec4f> track_data;
	bool track_data_flag;

	//�ж�ʶ�𵽵������Ƿ��ڻ�е�ۿ�ץȡ��Χ��
	INT8 judgeGrabRange(vector<IPCobj> inputobj);//��������ץ������0����ץ�õ���

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CMultiRobotApp theApp;
