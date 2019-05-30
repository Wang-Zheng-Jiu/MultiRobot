// directionDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������Э���������.h"
#include "directionDlg.h"
#include "afxdialogex.h"


// directionDlg �Ի���

IMPLEMENT_DYNAMIC(directionDlg, CDialogEx)

directionDlg::directionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

directionDlg::~directionDlg()
{
}

void directionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_contents);
	DDX_Control(pDX, IDC_EDIT1, m_readmedisplay);
}


BEGIN_MESSAGE_MAP(directionDlg, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &directionDlg::OnNMClickTree1)
//	ON_NOTIFY(NM_THEMECHANGED, IDC_TREE1, &directionDlg::OnNMThemeChangedTree1)
END_MESSAGE_MAP()


// directionDlg ��Ϣ�������


BOOL directionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	HTREEITEM hRobotMFC= m_contents.InsertItem(_T("�˻���������"));
	m_contents.InsertItem(_T("�������б�"), hRobotMFC);
	m_contents.InsertItem(_T("�����˴���������"), hRobotMFC);
	m_contents.InsertItem(_T("�������˶�����"), hRobotMFC);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void directionDlg::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	DWORD dwpos = GetMessagePos();
	TVHITTESTINFO ht = { 0 };
	ht.pt.x = GET_X_LPARAM(dwpos);
	ht.pt.y = GET_Y_LPARAM(dwpos);
	::MapWindowPoints(HWND_DESKTOP, pNMHDR->hwndFrom, &ht.pt, 1); //����Ļ����ת���ɿؼ�����
	TreeView_HitTest(pNMHDR->hwndFrom, &ht);   //ȷ�����������һ��
	CString str = m_contents.GetItemText(ht.hItem);

	if (str == "�������б�")
	{
		//���
		m_readmedisplay.SetSel(0, -1);
		m_readmedisplay.ReplaceSel(_T(""));
		//�������
		CString neirongstr = _T("�������б�\r\n����ʾ�Ѿ����ϵ�turbot�����ˣ�ѡ�Ͽ��Եõ������Լ����п���");
		m_readmedisplay.ReplaceSel(neirongstr); 
	}
	else if(str=="�����˴���������")
	{
		//���
		m_readmedisplay.SetSel(0, -1);
		m_readmedisplay.ReplaceSel(_T(""));
		//�������
		CString neirongstr = _T("�����˴���������\r\nδ��������");
		m_readmedisplay.ReplaceSel(neirongstr);
	}
	else if(str== "�������˶�����")
	{
		//���
		m_readmedisplay.SetSel(0, -1);
		m_readmedisplay.ReplaceSel(_T(""));
		//�������
		CString neirongstr = _T("�������˶�����\r\nWSAD����turbot�����ˣ�ѡ�л����˽��п��ơ�\r\n�������ҿ��ư��׼һ����ˡ�");
		m_readmedisplay.ReplaceSel(neirongstr);
	}
}


//void directionDlg::OnNMThemeChangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// �ù���Ҫ��ʹ�� Windows XP ����߰汾��
//	// ���� _WIN32_WINNT ���� >= 0x0501��
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	*pResult = 0;
//}
