// GrabSequenceDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "GrabSequenceDlg.h"
#include "afxdialogex.h"


// CGrabSequenceDlg 대화 상자

IMPLEMENT_DYNAMIC(CGrabSequenceDlg, CDialog)

CGrabSequenceDlg::CGrabSequenceDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_GRAB_SEQUENCE_DLG, pParent)
{
	for (int i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
	{
		m_iEditAddrCount[i] = LIGHTCTRL_GRAB_ADDR_PAGE_COUNT;
		m_sEditDesc[i] = _T("");

		for (int j = 0; j < LIGHTCTRL_GRAB_ADDR_PAGE_COUNT; j++)
			m_iEditAddrSeq[i][j] = j + 1;
	}
}

CGrabSequenceDlg::~CGrabSequenceDlg()
{
}

void CGrabSequenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ0, m_iEditAddrSeq[0][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ1, m_iEditAddrSeq[0][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ2, m_iEditAddrSeq[0][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ3, m_iEditAddrSeq[0][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ4, m_iEditAddrSeq[0][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ5, m_iEditAddrSeq[0][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ6, m_iEditAddrSeq[0][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ7, m_iEditAddrSeq[0][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ8, m_iEditAddrSeq[0][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ9, m_iEditAddrSeq[0][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ10, m_iEditAddrSeq[0][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ11, m_iEditAddrSeq[0][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ12, m_iEditAddrSeq[0][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ13, m_iEditAddrSeq[0][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ14, m_iEditAddrSeq[0][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR0_SEQ15, m_iEditAddrSeq[0][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ0, m_iEditAddrSeq[1][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ1, m_iEditAddrSeq[1][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ2, m_iEditAddrSeq[1][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ3, m_iEditAddrSeq[1][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ4, m_iEditAddrSeq[1][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ5, m_iEditAddrSeq[1][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ6, m_iEditAddrSeq[1][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ7, m_iEditAddrSeq[1][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ8, m_iEditAddrSeq[1][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ9, m_iEditAddrSeq[1][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ10, m_iEditAddrSeq[1][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ11, m_iEditAddrSeq[1][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ12, m_iEditAddrSeq[1][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ13, m_iEditAddrSeq[1][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ14, m_iEditAddrSeq[1][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_SEQ15, m_iEditAddrSeq[1][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ0, m_iEditAddrSeq[2][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ1, m_iEditAddrSeq[2][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ2, m_iEditAddrSeq[2][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ3, m_iEditAddrSeq[2][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ4, m_iEditAddrSeq[2][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ5, m_iEditAddrSeq[2][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ6, m_iEditAddrSeq[2][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ7, m_iEditAddrSeq[2][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ8, m_iEditAddrSeq[2][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ9, m_iEditAddrSeq[2][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ10, m_iEditAddrSeq[2][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ11, m_iEditAddrSeq[2][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ12, m_iEditAddrSeq[2][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ13, m_iEditAddrSeq[2][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ14, m_iEditAddrSeq[2][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_SEQ15, m_iEditAddrSeq[2][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ0, m_iEditAddrSeq[3][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ1, m_iEditAddrSeq[3][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ2, m_iEditAddrSeq[3][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ3, m_iEditAddrSeq[3][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ4, m_iEditAddrSeq[3][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ5, m_iEditAddrSeq[3][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ6, m_iEditAddrSeq[3][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ7, m_iEditAddrSeq[3][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ8, m_iEditAddrSeq[3][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ9, m_iEditAddrSeq[3][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ10, m_iEditAddrSeq[3][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ11, m_iEditAddrSeq[3][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ12, m_iEditAddrSeq[3][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ13, m_iEditAddrSeq[3][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ14, m_iEditAddrSeq[3][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_SEQ15, m_iEditAddrSeq[3][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ0, m_iEditAddrSeq[4][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ1, m_iEditAddrSeq[4][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ2, m_iEditAddrSeq[4][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ3, m_iEditAddrSeq[4][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ4, m_iEditAddrSeq[4][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ5, m_iEditAddrSeq[4][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ6, m_iEditAddrSeq[4][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ7, m_iEditAddrSeq[4][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ8, m_iEditAddrSeq[4][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ9, m_iEditAddrSeq[4][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ10, m_iEditAddrSeq[4][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ11, m_iEditAddrSeq[4][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ12, m_iEditAddrSeq[4][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ13, m_iEditAddrSeq[4][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ14, m_iEditAddrSeq[4][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_SEQ15, m_iEditAddrSeq[4][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ0, m_iEditAddrSeq[5][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ1, m_iEditAddrSeq[5][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ2, m_iEditAddrSeq[5][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ3, m_iEditAddrSeq[5][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ4, m_iEditAddrSeq[5][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ5, m_iEditAddrSeq[5][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ6, m_iEditAddrSeq[5][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ7, m_iEditAddrSeq[5][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ8, m_iEditAddrSeq[5][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ9, m_iEditAddrSeq[5][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ10, m_iEditAddrSeq[5][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ11, m_iEditAddrSeq[5][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ12, m_iEditAddrSeq[5][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ13, m_iEditAddrSeq[5][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ14, m_iEditAddrSeq[5][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_SEQ15, m_iEditAddrSeq[5][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ0, m_iEditAddrSeq[6][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ1, m_iEditAddrSeq[6][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ2, m_iEditAddrSeq[6][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ3, m_iEditAddrSeq[6][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ4, m_iEditAddrSeq[6][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ5, m_iEditAddrSeq[6][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ6, m_iEditAddrSeq[6][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ7, m_iEditAddrSeq[6][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ8, m_iEditAddrSeq[6][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ9, m_iEditAddrSeq[6][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ10, m_iEditAddrSeq[6][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ11, m_iEditAddrSeq[6][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ12, m_iEditAddrSeq[6][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ13, m_iEditAddrSeq[6][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ14, m_iEditAddrSeq[6][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_SEQ15, m_iEditAddrSeq[6][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ0, m_iEditAddrSeq[7][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ1, m_iEditAddrSeq[7][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ2, m_iEditAddrSeq[7][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ3, m_iEditAddrSeq[7][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ4, m_iEditAddrSeq[7][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ5, m_iEditAddrSeq[7][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ6, m_iEditAddrSeq[7][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ7, m_iEditAddrSeq[7][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ8, m_iEditAddrSeq[7][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ9, m_iEditAddrSeq[7][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ10, m_iEditAddrSeq[7][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ11, m_iEditAddrSeq[7][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ12, m_iEditAddrSeq[7][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ13, m_iEditAddrSeq[7][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ14, m_iEditAddrSeq[7][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_SEQ15, m_iEditAddrSeq[7][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ0, m_iEditAddrSeq[8][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ1, m_iEditAddrSeq[8][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ2, m_iEditAddrSeq[8][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ3, m_iEditAddrSeq[8][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ4, m_iEditAddrSeq[8][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ5, m_iEditAddrSeq[8][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ6, m_iEditAddrSeq[8][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ7, m_iEditAddrSeq[8][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ8, m_iEditAddrSeq[8][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ9, m_iEditAddrSeq[8][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ10, m_iEditAddrSeq[8][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ11, m_iEditAddrSeq[8][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ12, m_iEditAddrSeq[8][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ13, m_iEditAddrSeq[8][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ14, m_iEditAddrSeq[8][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_SEQ15, m_iEditAddrSeq[8][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ0, m_iEditAddrSeq[9][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ1, m_iEditAddrSeq[9][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ2, m_iEditAddrSeq[9][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ3, m_iEditAddrSeq[9][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ4, m_iEditAddrSeq[9][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ5, m_iEditAddrSeq[9][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ6, m_iEditAddrSeq[9][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ7, m_iEditAddrSeq[9][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ8, m_iEditAddrSeq[9][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ9, m_iEditAddrSeq[9][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ10, m_iEditAddrSeq[9][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ11, m_iEditAddrSeq[9][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ12, m_iEditAddrSeq[9][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ13, m_iEditAddrSeq[9][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ14, m_iEditAddrSeq[9][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_SEQ15, m_iEditAddrSeq[9][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ0, m_iEditAddrSeq[10][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ1, m_iEditAddrSeq[10][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ2, m_iEditAddrSeq[10][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ3, m_iEditAddrSeq[10][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ4, m_iEditAddrSeq[10][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ5, m_iEditAddrSeq[10][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ6, m_iEditAddrSeq[10][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ7, m_iEditAddrSeq[10][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ8, m_iEditAddrSeq[10][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ9, m_iEditAddrSeq[10][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ10, m_iEditAddrSeq[10][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ11, m_iEditAddrSeq[10][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ12, m_iEditAddrSeq[10][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ13, m_iEditAddrSeq[10][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ14, m_iEditAddrSeq[10][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_SEQ15, m_iEditAddrSeq[10][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ0, m_iEditAddrSeq[11][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ1, m_iEditAddrSeq[11][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ2, m_iEditAddrSeq[11][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ3, m_iEditAddrSeq[11][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ4, m_iEditAddrSeq[11][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ5, m_iEditAddrSeq[11][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ6, m_iEditAddrSeq[11][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ7, m_iEditAddrSeq[11][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ8, m_iEditAddrSeq[11][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ9, m_iEditAddrSeq[11][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ10, m_iEditAddrSeq[11][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ11, m_iEditAddrSeq[11][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ12, m_iEditAddrSeq[11][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ13, m_iEditAddrSeq[11][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ14, m_iEditAddrSeq[11][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_SEQ15, m_iEditAddrSeq[11][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ0, m_iEditAddrSeq[12][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ1, m_iEditAddrSeq[12][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ2, m_iEditAddrSeq[12][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ3, m_iEditAddrSeq[12][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ4, m_iEditAddrSeq[12][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ5, m_iEditAddrSeq[12][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ6, m_iEditAddrSeq[12][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ7, m_iEditAddrSeq[12][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ8, m_iEditAddrSeq[12][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ9, m_iEditAddrSeq[12][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ10, m_iEditAddrSeq[12][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ11, m_iEditAddrSeq[12][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ12, m_iEditAddrSeq[12][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ13, m_iEditAddrSeq[12][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ14, m_iEditAddrSeq[12][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_SEQ15, m_iEditAddrSeq[12][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ0, m_iEditAddrSeq[13][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ1, m_iEditAddrSeq[13][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ2, m_iEditAddrSeq[13][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ3, m_iEditAddrSeq[13][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ4, m_iEditAddrSeq[13][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ5, m_iEditAddrSeq[13][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ6, m_iEditAddrSeq[13][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ7, m_iEditAddrSeq[13][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ8, m_iEditAddrSeq[13][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ9, m_iEditAddrSeq[13][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ10, m_iEditAddrSeq[13][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ11, m_iEditAddrSeq[13][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ12, m_iEditAddrSeq[13][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ13, m_iEditAddrSeq[13][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ14, m_iEditAddrSeq[13][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_SEQ15, m_iEditAddrSeq[13][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ0, m_iEditAddrSeq[14][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ1, m_iEditAddrSeq[14][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ2, m_iEditAddrSeq[14][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ3, m_iEditAddrSeq[14][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ4, m_iEditAddrSeq[14][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ5, m_iEditAddrSeq[14][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ6, m_iEditAddrSeq[14][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ7, m_iEditAddrSeq[14][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ8, m_iEditAddrSeq[14][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ9, m_iEditAddrSeq[14][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ10, m_iEditAddrSeq[14][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ11, m_iEditAddrSeq[14][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ12, m_iEditAddrSeq[14][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ13, m_iEditAddrSeq[14][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ14, m_iEditAddrSeq[14][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_SEQ15, m_iEditAddrSeq[14][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ0, m_iEditAddrSeq[15][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ1, m_iEditAddrSeq[15][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ2, m_iEditAddrSeq[15][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ3, m_iEditAddrSeq[15][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ4, m_iEditAddrSeq[15][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ5, m_iEditAddrSeq[15][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ6, m_iEditAddrSeq[15][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ7, m_iEditAddrSeq[15][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ8, m_iEditAddrSeq[15][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ9, m_iEditAddrSeq[15][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ10, m_iEditAddrSeq[15][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ11, m_iEditAddrSeq[15][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ12, m_iEditAddrSeq[15][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ13, m_iEditAddrSeq[15][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ14, m_iEditAddrSeq[15][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_SEQ15, m_iEditAddrSeq[15][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ0, m_iEditAddrSeq[16][0]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ1, m_iEditAddrSeq[16][1]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ2, m_iEditAddrSeq[16][2]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ3, m_iEditAddrSeq[16][3]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ4, m_iEditAddrSeq[16][4]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ5, m_iEditAddrSeq[16][5]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ6, m_iEditAddrSeq[16][6]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ7, m_iEditAddrSeq[16][7]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ8, m_iEditAddrSeq[16][8]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ9, m_iEditAddrSeq[16][9]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ10, m_iEditAddrSeq[16][10]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ11, m_iEditAddrSeq[16][11]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ12, m_iEditAddrSeq[16][12]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ13, m_iEditAddrSeq[16][13]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ14, m_iEditAddrSeq[16][14]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_SEQ15, m_iEditAddrSeq[16][15]);

	DDX_Text(pDX, IDC_EDIT_ADDR0_COUNT, m_iEditAddrCount[0]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_COUNT, m_iEditAddrCount[1]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_COUNT, m_iEditAddrCount[2]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_COUNT, m_iEditAddrCount[3]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_COUNT, m_iEditAddrCount[4]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_COUNT, m_iEditAddrCount[5]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_COUNT, m_iEditAddrCount[6]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_COUNT, m_iEditAddrCount[7]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_COUNT, m_iEditAddrCount[8]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_COUNT, m_iEditAddrCount[9]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_COUNT, m_iEditAddrCount[10]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_COUNT, m_iEditAddrCount[11]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_COUNT, m_iEditAddrCount[12]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_COUNT, m_iEditAddrCount[13]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_COUNT, m_iEditAddrCount[14]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_COUNT, m_iEditAddrCount[15]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_COUNT, m_iEditAddrCount[16]);

	DDX_Text(pDX, IDC_EDIT_DESC_ADDR0, m_sEditDesc[0]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR1, m_sEditDesc[1]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR2, m_sEditDesc[2]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR3, m_sEditDesc[3]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR4, m_sEditDesc[4]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR5, m_sEditDesc[5]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR6, m_sEditDesc[6]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR7, m_sEditDesc[7]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR8, m_sEditDesc[8]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR9, m_sEditDesc[9]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR10, m_sEditDesc[10]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR11, m_sEditDesc[11]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR12, m_sEditDesc[12]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR13, m_sEditDesc[13]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR14, m_sEditDesc[14]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR15, m_sEditDesc[15]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR16, m_sEditDesc[16]);

	DDX_Control(pDX, IDC_BUTTON_OK, m_bnSave);
	DDX_Control(pDX, IDC_BUTTON_CANCEL, m_bnExit);
}


BEGIN_MESSAGE_MAP(CGrabSequenceDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CGrabSequenceDlg::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CGrabSequenceDlg::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CGrabSequenceDlg 메시지 처리기

BOOL CGrabSequenceDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CGrabSequenceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i, j;
	for (i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
	{
		m_iEditAddrCount[i] = THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[i];
		m_sEditDesc[i] = THEAPP.m_pModelDataManager->m_PageControlData.m_sAddrDesc[i];

		for (j = 0; j < LIGHTCTRL_GRAB_ADDR_PAGE_COUNT; j++)
			m_iEditAddrSeq[i][j] = THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrSeq[i][j] + 1;
	}

	//////////////////////////////////////////////////////////////////////////

	m_bnSave.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	m_bnExit.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	ChangeLanguage();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CGrabSequenceDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("그랩 시퀀스 설정"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_1, _T("페이지 1"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_2, _T("페이지 2"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_3, _T("페이지 3"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_4, _T("페이지 4"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_5, _T("페이지 5"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_6, _T("페이지 6"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_7, _T("페이지 7"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_8, _T("페이지 8"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_9, _T("페이지 9"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_10, _T("페이지 10"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_11, _T("페이지 11"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_12, _T("페이지 12"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_13, _T("페이지 13"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_14, _T("페이지 14"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_15, _T("페이지 15"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_16, _T("페이지 16"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_17, _T("그랩 수"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_18, _T("용도 설명"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_19, _T("주소 1"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_20, _T("주소 2"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_21, _T("주소 3"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_22, _T("주소 4"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_23, _T("주소 5"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_24, _T("주소 6"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_25, _T("주소 7"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_26, _T("주소 8"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_27, _T("주소 9"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_28, _T("주소 10"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_29, _T("주소 11"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_30, _T("주소 12"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_31, _T("주소 13"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_32, _T("주소 14"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_33, _T("주소 15"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_34, _T("주소 16"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_35, _T("주소 17"));
		SetDlgItemText(IDC_BUTTON_OK, _T("저장"));
		SetDlgItemText(IDC_BUTTON_CANCEL, _T("닫기"));
	}
	else
	{
		this->SetWindowText(_T("Set the grab sequence"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_1, _T("Page 1"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_2, _T("Page 2"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_3, _T("Page 3"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_4, _T("Page 4"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_5, _T("Page 5"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_6, _T("Page 6"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_7, _T("Page 7"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_8, _T("Page 8"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_9, _T("Page 9"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_10, _T("Page 10"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_11, _T("Page 11"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_12, _T("Page 12"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_13, _T("Page 13"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_14, _T("Page 14"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_15, _T("Page 15"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_16, _T("Page 16"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_17, _T("Count"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_18, _T("Description"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_19, _T("Addr 1"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_20, _T("Addr 2"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_21, _T("Addr 3"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_22, _T("Addr 4"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_23, _T("Addr 5"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_24, _T("Addr 6"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_25, _T("Addr 7"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_26, _T("Addr 8"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_27, _T("Addr 9"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_28, _T("Addr 10"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_29, _T("Addr 11"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_30, _T("Addr 12"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_31, _T("Addr 13"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_32, _T("Addr 14"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_33, _T("Addr 15"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_34, _T("Addr 16"));
		SetDlgItemText(IDC_STATIC_GRABSEQUENCEDLG_35, _T("Addr 17"));
		SetDlgItemText(IDC_BUTTON_OK, _T("Save"));
		SetDlgItemText(IDC_BUTTON_CANCEL, _T("Close"));
	}
}

void CGrabSequenceDlg::OnBnClickedButtonOk()
{
	UpdateData(TRUE);

	int i, j;

	for (i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
	{
		if (m_iEditAddrCount[i] < 1 || m_iEditAddrCount[i] > LIGHTCTRL_GRAB_ADDR_PAGE_COUNT)
		{
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("그랩 수 값은 1 ~ %d 사이의 값으로 설정할 수 있습니다.", LIGHTCTRL_GRAB_ADDR_PAGE_COUNT);
			else
				strMessageBox.Format("The number of grabs can be set between 1 and %d.", LIGHTCTRL_GRAB_ADDR_PAGE_COUNT);
			AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
			return;
		}

		for (j = 0; j < LIGHTCTRL_GRAB_ADDR_PAGE_COUNT; j++)
		{
			if (m_iEditAddrSeq[i][j] < 1 || m_iEditAddrSeq[i][j] > LIGHTCTRL_PAGE_COUNT)
			{
				if (THEAPP.m_iPGMLanguageSelect == 0)
					strMessageBox.Format("페이지 값은 1 ~ %d 사이의 값으로 설정할 수 있습니다.", LIGHTCTRL_PAGE_COUNT);
				else
					strMessageBox.Format("The page value can be set between 1 and %d.", LIGHTCTRL_PAGE_COUNT);
				AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
				return;
			}
		}
	}

	for (i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
	{
		THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[i] = m_iEditAddrCount[i];
		THEAPP.m_pModelDataManager->m_PageControlData.m_sAddrDesc[i] = m_sEditDesc[i];

		for (j = 0; j < LIGHTCTRL_GRAB_ADDR_PAGE_COUNT; j++)
			THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrSeq[i][j] = m_iEditAddrSeq[i][j] - 1;
	}

#ifdef INLINE_MODE

	for (i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
	{
		THEAPP.m_pModelDataManager->m_PageControlData.SetGrabSequence(THEAPP.m_iCurVisionCamType, i, THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrSeq[i], THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[i]);
	}

#endif

	//**********  Grab Sequence 값을 파일에 써줌

	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
	if (GetFileAttributes(strOpticModelFolder) == -1)
		return;

	CString strSequenceInfo;
	strSequenceInfo.Format("%s\\HW\\Vision_N%d\\SequenceInfo.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	THEAPP.m_pModelDataManager->SaveSequenceInfo(strSequenceInfo);

	THEAPP.m_pTabControlDlg->m_pCameraLightManagerDlg->LoadViewParam();
	THEAPP.m_pTabControlDlg->m_pJogSetDlg->LoadViewParam();

	CDialog::OnOK();
}

void CGrabSequenceDlg::OnBnClickedButtonCancel()
{
	CDialog::OnCancel();
}
