
// MultiRobotDlg.h : ͷ�ļ�
//

#pragma once
#include "afxvslistbox.h"
#include "afxwin.h"
#include "afxcmn.h"


// CMultiRobotDlg �Ի���
class CMultiRobotDlg : public CDialogEx
{
// ����
public:
	CMultiRobotDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MY_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CMenu m_Menu;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// �������б�,��ӦrobotAPI�л����˷��������robotlist��ѡ����۽���������ˡ�
	CListBox m_RobotList;
	// ѡ�л����˵ĵ�ѹ
	float m_voltage;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//�Աȸ���robotlist����ʾ
	void updataRobotListDisplay();


	int keyflag = 0;//�����жϰ���״̬���ǰ���״̬����û��״̬��0��û��
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	afx_msg void OnEnChangeEdit2();
	//����������ʾIMUλ����Ϣ
	float m_angular_velocity_x;
	float m_angular_velocity_y;
	float m_angular_velocity_z;
	float m_linear_acceleration_x;
	float m_linear_acceleration_y;
	float m_linear_acceleration_z;
	float m_roll;
	float m_pitch;
	float m_yaw;
	// �˶�ʹ��
	CButton m_moveEnable;
	afx_msg void OnBnClickedCheck1();
	// �ƶ����ٶ�
	CSliderCtrl m_movelin;
	// �ƶ��ĽǶ�
	CSliderCtrl m_moveang;
	float m_movelin_display;
	float m_moveang_display;

	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	//��ʼ�˶���ť����
	afx_msg void OnBnClickedButton1();
	//ֹͣ�˶���ť����
	afx_msg void OnBnClickedButton2();
	// ���ڱ�ʾwsad�����Ƿ�ʹ�û�����Ĳ�������
	CButton m_wsadFlag;
	afx_msg void On32775();
	// ��ʾIP��ַ�Ͷ˿ں�
	CEdit m_disIPaddr;
	// չʾIPC���б�
	CListBox m_IPClist;
	// ��ʾ��ѡIPC��rtsp��ַ
	CString m_rtsp;
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton3();
	// ��ʾˮƽ�����ϵķ���Ƕ�
	float m_direction;

	//������λ��ʱ�ĺ���
	afx_msg void On32777();
	// ��ʱʱ��
	float m_delaytime;
	afx_msg void On32778();
	// ���������
	CEdit m_printout;

	//�Զ��庯�������ڵ������
	void printd(CString cout);//����ַ�����Ϣ
	void printd(string cout);//����ַ�����Ϣ
	void printd(int cout);
	void printd(float cout);

	//������ʾ2D��ͼ
	afx_msg void Onshow2Donoff();
	
	afx_msg void Onvision();
	afx_msg void OnAddIPC();
	afx_msg void OnLbnDblclkList1();
	// ��ʾ�벻��ʾ���
	BOOL m_showimg;
	afx_msg void OnBnClickedCheck3();
};

//���ģ���������
//����ļ����߳�

//socket�����߳�
DWORD WINAPI ListenAcceptThreadFun(LPVOID p);
//-socket���߳�
DWORD WINAPI updataRobotStatusThreadFun(LPVOID p);

//IPC�Ӿ�����λ�߳�
DWORD WINAPI IPCvisionLocationSystemThreadFun(LPVOID p);
//-IPC�Ӵ����߳�
DWORD WINAPI IPCvisionLocationSonThreadFun(LPVOID p);
//��ʾ�߳�
DWORD WINAPI IPCvisionLocationSon_ShowThreadFun(LPVOID p);
void map_mouse_callback(int event, int x, int y, int flags, void* param); //��ʾ�߳��е����ص�����