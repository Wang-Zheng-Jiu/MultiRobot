#pragma once


// addipcDlg �Ի���

class addipcDlg : public CDialogEx
{
	DECLARE_DYNAMIC(addipcDlg)

public:
	addipcDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~addipcDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// ͼƬ·��
	CString m_path;
	afx_msg void OnBnClickedButton1();
	CString rtsp;
	// ���ӿ�� mm
	float m_qsize;
};
