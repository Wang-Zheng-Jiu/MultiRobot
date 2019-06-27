
// �������Э���������.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "�������Э���������.h"
#include "MultiRobotDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMultiRobotApp

BEGIN_MESSAGE_MAP(CMultiRobotApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMultiRobotApp ����

CMultiRobotApp::CMultiRobotApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	//robotServer(6000);
}


// Ψһ��һ�� CMultiRobotApp ����

CMultiRobotApp theApp;


// CMultiRobotApp ��ʼ��

BOOL CMultiRobotApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));


	//���ӳ�ʼ��zzq
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	CMultiRobotDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}





int CMultiRobotApp::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	return CWinApp::ExitInstance();
}

void CMultiRobotApp::taskqueue_push(int index, int taskn, Point2f toP, int lf)
{
	CMultiRobotApp::task ntask;

	if (taskn == 1)
	{
		ntask.taskname = taskn; ntask.x = toP.x; ntask.y = toP.y;
		WaitForSingleObject(theApp.robotServer.hMutex, INFINITE);
		theApp.taskqueue[index].push_back(ntask);
		ReleaseMutex(theApp.robotServer.hMutex);//����
	}
	else if(taskn==3)
	{
		ntask.taskname = taskn; ntask.x = toP.x; ntask.y = toP.y; ntask.lf = lf;
		WaitForSingleObject(theApp.robotServer.hMutex, INFINITE);
		theApp.taskqueue[index].push_back(ntask);
		ReleaseMutex(theApp.robotServer.hMutex);//����
	}
	
}

void CMultiRobotApp::uArmTaskQueue_push(int taskn, Vec3f p1, Vec3f p2)
{
	CMultiRobotApp::task foo;
	float dx = 293, dy = 386, dz = 0;

	WaitForSingleObject(theApp.huArmTaskMutex, INFINITE);

	foo.taskname = taskn;
	foo.x = (p1[0] * 1000 + dx);
	foo.y = (p1[1] * 1000 + dy);
	foo.z = (p1[2] * 1000 + dz);
	foo.x1 = p2[0] * 1000 + dx;
	foo.y1 = p2[1] * 1000 + dy;
	foo.z1 = p2[2] * 1000 + dz;

	if (taskn == 4)
	{
		//�����޷�
		float dd = sqrt(foo.x*foo.x + foo.y*foo.y);
		float dd1 = sqrt(foo.x1*foo.x1 + foo.y1*foo.y1);
		if (dd > 350 || dd1 > 350)
		{
			ReleaseMutex(theApp.huArmTaskMutex);//����
			return;
		}
			
	}
	theApp.uArmTaskQueue.push_back(foo);


	ReleaseMutex(theApp.huArmTaskMutex);//����
}