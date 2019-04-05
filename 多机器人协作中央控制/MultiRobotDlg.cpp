
// MultiRobotDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������Э���������.h"
#include "MultiRobotDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMultiRobotDlg �Ի���



CMultiRobotDlg::CMultiRobotDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MY_DIALOG, pParent)
	, m_voltage(0)
	, m_angular_velocity_x(0)
	, m_angular_velocity_y(0)
	, m_angular_velocity_z(0)
	, m_linear_acceleration_x(0)
	, m_linear_acceleration_y(0)
	, m_linear_acceleration_z(0)
	, m_roll(0)
	, m_pitch(0)
	, m_yaw(0)
	, m_movelin_display(0)
	, m_moveang_display(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMultiRobotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_MFCVSLISTBOX2, m_RobotList);
	DDX_Control(pDX, IDC_LIST4, m_RobotList);
	DDX_Text(pDX, IDC_EDIT1, m_voltage);
	DDX_Text(pDX, IDC_EDIT2, m_angular_velocity_x);
	DDX_Text(pDX, IDC_EDIT3, m_angular_velocity_y);
	DDX_Text(pDX, IDC_EDIT4, m_angular_velocity_z);
	DDX_Text(pDX, IDC_EDIT5, m_linear_acceleration_x);
	DDX_Text(pDX, IDC_EDIT6, m_linear_acceleration_y);
	DDX_Text(pDX, IDC_EDIT7, m_linear_acceleration_z);
	DDX_Text(pDX, IDC_EDIT8, m_roll);
	DDX_Text(pDX, IDC_EDIT9, m_pitch);
	DDX_Text(pDX, IDC_EDIT10, m_yaw);
	DDX_Control(pDX, IDC_CHECK1, m_moveEnable);
	DDX_Control(pDX, IDC_SLIDER1, m_movelin);
	DDX_Control(pDX, IDC_SLIDER2, m_moveang);
	DDX_Text(pDX, IDC_EDIT11, m_movelin_display);
	DDV_MinMaxInt(pDX, m_movelin_display, -200, 200);
	//  DDX_Control(pDX, IDC_EDIT12, m_moveang_display);
	DDX_Text(pDX, IDC_EDIT12, m_moveang_display);
	DDV_MinMaxInt(pDX, m_moveang_display, -180, 180);
	DDX_Control(pDX, IDC_CHECK2, m_wsadFlag);
}

BEGIN_MESSAGE_MAP(CMultiRobotDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK1, &CMultiRobotDlg::OnBnClickedCheck1)
//	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER1, &CMultiRobotDlg::OnTRBNThumbPosChangingSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CMultiRobotDlg::OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, &CMultiRobotDlg::OnNMCustomdrawSlider2)
	ON_BN_CLICKED(IDC_BUTTON1, &CMultiRobotDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMultiRobotDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMultiRobotDlg ��Ϣ�������

BOOL CMultiRobotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��


	//***************************************** TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_Menu.LoadMenu(IDR_MENU1);//��ʼ���˵�
	SetMenu(&m_Menu);
	//zzq���Դ���
	theApp.robotServer.init(6000);
	if (theApp.robotServer.is_Open() < 0)
	{
		AfxMessageBox(_T("��������ʼ��ʧ��"));
	}
	theApp.robotServer.hMutex = CreateMutex(NULL, FALSE, NULL);
	//���������߳�
	theApp.robotServer.hListenThread = CreateThread(NULL, 0, ListenAcceptThreadFun, NULL, 0, &theApp.robotServer.ListenThreadID);
	//���öԻ���ˢ��ʱ��
	SetTimer(1, 500, NULL); m_voltage = 0;
	//��ʼ���˶����ƻ�����
	m_movelin.SetRange(-50, 50);
	m_movelin.SetTicFreq(5);
	m_movelin.SetPos(0);
	//m_movelin.SetLineSize(5);//һ�еĴ�С����Ӧ���̵ķ����
	m_moveang.SetRange(-180, 180);
	m_moveang.SetTicFreq(5);
	m_moveang.SetPos(0);
	





	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMultiRobotDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMultiRobotDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMultiRobotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





/*----------------------------------------------------------------------
						���̺߳�������
*/

DWORD WINAPI ListenAcceptThreadFun(LPVOID p)
{
	//���ڶ��߳� �����߳�
	vector<HANDLE> hUpdataRobotThread;
	vector<DWORD> UpdataRobotThreadID;
	//����
	if (theApp.robotServer.Listen(ROBOT_MAXCONNECT_NUM) == SOCKET_ERROR)//����
	{
		AfxMessageBox(_T("�����������"));
		return -1;
	}
	
	while (true)
	{
		int acptret = theApp.robotServer.Accept();

		//�������û�
		if (acptret == INVALID_SOCKET)
		{
			//printf("accept error\n");
			continue; //�����ȴ���һ������
		}
		else
		{
			WaitForSingleObject(theApp.robotServer.hMutex, INFINITE);//����
			HANDLE h1; DWORD id1;
			uint8_t therobotid = theApp.robotServer.robotlist.back().robotID;
			h1 = CreateThread(NULL, 0, updataRobotStatusThreadFun, &therobotid, 0, &id1);
			hUpdataRobotThread.push_back(h1);
			UpdataRobotThreadID.push_back(id1);
			//����
			ReleaseMutex(theApp.robotServer.hMutex);
		}
	}


}


//ˢ�»�����״̬ �������ֱ�Ӵ�robotServer.robotlist��pop�������ҹر��߳�
DWORD WINAPI updataRobotStatusThreadFun(LPVOID p)
{
	uint8_t robotid = *(uint8_t*)p;
	int robotindex = theApp.robotServer.findID(robotid);

	//�ж��������� �Ͽ�������ѭ��
	while (theApp.robotServer.robotlist[robotindex].connectStatus>0)
	{
		//updata ��ѹֵ
		WaitForSingleObject(theApp.robotServer.hMutex, INFINITE);//����
		robotindex = theApp.robotServer.findID(robotid);
		theApp.robotServer.robotlist[robotindex].Voltage = theApp.robotServer.robotlist[robotindex].getVoltage();
		//����
		ReleaseMutex(theApp.robotServer.hMutex);

		

		Sleep(1000);
	}
	
	//ɾ����id�Ľ��� ��robotServer.robotlist��pop��
	WaitForSingleObject(theApp.robotServer.hMutex, INFINITE);//����
	robotindex = theApp.robotServer.findID(robotid);
	vector<robot>::iterator it = theApp.robotServer.robotlist.begin() + robotindex;
	theApp.robotServer.robotlist.erase(it);
	//����
	ReleaseMutex(theApp.robotServer.hMutex);
	return 0;
}




void CMultiRobotDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	//����
	WaitForSingleObject(theApp.robotServer.hMutex, INFINITE);
	//ˢ�»������б�
	updataRobotListDisplay();
	
	//ˢ��ָ�������˵���Ϣ����ѹ IMU 
	int index = m_RobotList.GetCurSel();
	if (index >= 0)
	{
		//ˢ�µ�ѹ
		m_voltage = theApp.robotServer.robotlist[index].Voltage;
		//ˢ��IMU
		imu_msg imudata;
		imudata = theApp.robotServer.robotlist[index].getIMU();
		m_angular_velocity_x = zfun::numFormat(imudata.angular_velocity_x,2);
		m_angular_velocity_y = zfun::numFormat(imudata.angular_velocity_y,2);
		m_angular_velocity_z = zfun::numFormat(imudata.angular_velocity_z,2);
		m_linear_acceleration_x = zfun::numFormat(imudata.linear_acceleration_x,2);
		m_linear_acceleration_y = zfun::numFormat(imudata.linear_acceleration_y,2);
		m_linear_acceleration_z = zfun::numFormat(imudata.linear_acceleration_z,2);
		//��Ԫ��תRPY
		Eigen::Vector3d rpy;
		rpy = zfun::Quaterniond2Euler(imudata.orientation_x, imudata.orientation_y, imudata.orientation_z, imudata.orientation_w);
		m_roll = zfun::numFormat(rpy[0],3);
		m_pitch = zfun::numFormat(rpy[1], 3);
		m_yaw = zfun::numFormat(rpy[2], 3);
		//ˢ���˶�����ʹ�ܿؼ�
		if (theApp.robotServer.robotlist[index].getTorque() == 1)
		{
			m_moveEnable.SetCheck(true);
		}
		else if(theApp.robotServer.robotlist[index].getTorque() == 0)
		{
			m_moveEnable.SetCheck(false);
		}
	}

	//����
	ReleaseMutex(theApp.robotServer.hMutex);

	UpdateData(false);

	CDialogEx::OnTimer(nIDEvent);
}


//Listbox �� robotlist ���жԱȸ���
void CMultiRobotDlg::updataRobotListDisplay()
{
	//ˢ��robotlist�б�
	int flagone=1; //1��һ�� 0�ǲ�һ��
	//�ж��Ƿ�һ��
	if (theApp.robotServer.getRobotListNum() == m_RobotList.GetCount())
	{
		for (size_t i = 0; i < m_RobotList.GetCount(); i++)
		{
			CString robotname;
			robotname.Format(_T("%x"), theApp.robotServer.robotlist[i].robotID);
			CString m_RobotListstr;
			m_RobotList.GetText(i, m_RobotListstr);
			if (robotname != m_RobotListstr) flagone = 0;
		}
	}
	else
	{
		flagone = 0;
	}
	

	if (flagone == 0)
	{
		m_RobotList.ResetContent();
		for (size_t i = 0; i < theApp.robotServer.getRobotListNum(); i++)
		{
			CString robotname;
			robotname.Format(_T("%x"), theApp.robotServer.robotlist[i].robotID);


			m_RobotList.AddString(robotname);
		}
		UpdateData(false);
	}
}





//�Ըû����˽��а�������
BOOL CMultiRobotDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	
	if (pMsg->message == WM_KEYDOWN)
	{
		if (keyflag == 0)
		{
			////�鿴��ǰѡ������ˡ�
			int index= m_RobotList.GetCurSel();
			if (index < 0)
			{
				return CDialogEx::PreTranslateMessage(pMsg);
			}
			//���WSADʱ��ʲô��������
			float ilin, iang;
			if (m_wsadFlag.GetCheck() == BST_CHECKED)
			{
				ilin = (float)m_movelin.GetPos() / 100.0;
				iang = (float)m_moveang.GetPos() / 100.0;
			}
			else
			{
				ilin = 0.2;
				iang = 1.8;
			}

			switch (pMsg->wParam)
			{
			case 'W'://ǰ��
				
				WaitForSingleObject(theApp.robotServer.hMutex, INFINITE);//����
				theApp.robotServer.robotlist[index].move(ilin, 0);
				//����
				ReleaseMutex(theApp.robotServer.hMutex);	
				break;
			case 'S'://����
				WaitForSingleObject(theApp.robotServer.hMutex, INFINITE);//����
				theApp.robotServer.robotlist[index].move(-ilin, 0);
				//����
				ReleaseMutex(theApp.robotServer.hMutex);
				break;
			case 'A'://��
				WaitForSingleObject(theApp.robotServer.hMutex, INFINITE);//����
				theApp.robotServer.robotlist[index].move(0, iang);
				//����
				ReleaseMutex(theApp.robotServer.hMutex);
				break;
			case 'D'://��
				WaitForSingleObject(theApp.robotServer.hMutex, INFINITE);//����
				theApp.robotServer.robotlist[index].move(0, -iang);
				//����
				ReleaseMutex(theApp.robotServer.hMutex);
				break;
			default:
				break;
			}
		}
		keyflag = 1;
		return true;
	}
	else if (pMsg->message == WM_KEYUP)
	{
		////�鿴��ǰѡ������ˡ�
		int index = m_RobotList.GetCurSel();
		if (index < 0)
		{
			return CDialogEx::PreTranslateMessage(pMsg);
		}
		//����
		WaitForSingleObject(theApp.robotServer.hMutex, INFINITE);
		theApp.robotServer.robotlist[index].move(0, 0);
		//����
		ReleaseMutex(theApp.robotServer.hMutex);

		keyflag = 0;

	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


//void CMultiRobotDlg::OnEnChangeEdit2()
//{
//	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
//	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
//	// ���������� CRichEditCtrl().SetEventMask()��
//	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
//
//	// TODO:  �ڴ���ӿؼ�֪ͨ����������
//}

//checkbox  ʹ�ܵ������
void CMultiRobotDlg::OnBnClickedCheck1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//�鿴��ǰѡ������ˡ�
	int index = m_RobotList.GetCurSel();
	if (index < 0)
	{
		return;
	}
	//����
	WaitForSingleObject(theApp.robotServer.hMutex, INFINITE);
	if (m_moveEnable.GetCheck() == BST_CHECKED)//enabel
	{
		theApp.robotServer.robotlist[index].setTorque(1);
	}
	else
	{
		theApp.robotServer.robotlist[index].setTorque(0);
	}
	//����
	ReleaseMutex(theApp.robotServer.hMutex);
}



void CMultiRobotDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_movelin_display = m_movelin.GetPos();
	UpdateData(false);
	*pResult = 0;
}



void CMultiRobotDlg::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_moveang_display = m_moveang.GetPos();
	UpdateData(false);
	*pResult = 0;
}

//��ʼ�˶�
void CMultiRobotDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int index = m_RobotList.GetCurSel();
	if (index < 0)
	{
		return;
	}
	float ilin, iang;
	ilin = (float)m_movelin.GetPos() / 100.0;
	iang = (float)m_moveang.GetPos()/100;
	//����
	WaitForSingleObject(theApp.robotServer.hMutex, INFINITE);
	theApp.robotServer.robotlist[index].move(ilin, iang);
	//����
	ReleaseMutex(theApp.robotServer.hMutex);
}


//ֹͣ�˶�
void CMultiRobotDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int index = m_RobotList.GetCurSel();
	if (index < 0)
	{
		return;
	}

	//����
	WaitForSingleObject(theApp.robotServer.hMutex, INFINITE);
	theApp.robotServer.robotlist[index].move(0, 0);
	//����
	ReleaseMutex(theApp.robotServer.hMutex);
}
