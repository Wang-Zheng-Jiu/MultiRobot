#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// directionDlg �Ի���

class directionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(directionDlg)

public:
	directionDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~directionDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// Ŀ¼
	CTreeCtrl m_contents;
	virtual BOOL OnInitDialog();
	// //����˵��
	CEdit m_readmedisplay;
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
};
