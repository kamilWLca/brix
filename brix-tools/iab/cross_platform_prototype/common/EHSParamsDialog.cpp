// EHSParamsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "EHSParamsDialog.h"


// EHSParamsDialog dialog

IMPLEMENT_DYNAMIC(EHSParamsDialog, CDialog)

EHSParamsDialog::EHSParamsDialog(EHSInitParams* pEHSInitParams, CWnd* pParent /*=NULL*/)
	: CDialog(EHSParamsDialog::IDD, pParent)
	, nHasFrame(0)
	, nZOrder(0)
	, nXCoord(0)
	, nYCoord(0)
	, nWidth(0)
	, nHeight(0)
	, nRed(0)
	, nGreen(0)
	, nBlue(0)
	, nAlpha(0)
{
	_pEHSInitParams = pEHSInitParams;
}

EHSParamsDialog::~EHSParamsDialog()
{
}

void EHSParamsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECK_FRAME2, m_hasFrame);
	DDX_Control(pDX, IDC_RADIO_ON_TOP2, m_zOnTop);
	DDX_Control(pDX, IDC_RADIO_NORMAL2, m_zNormal);
	DDX_Control(pDX, IDC_RADIO_BOTTOM2, m_zBottom);

	DDX_Text(pDX, IDC_EDIT_X3, nXCoord);
	DDX_Text(pDX, IDC_EDIT_Y3, nYCoord);
	DDX_Text(pDX, IDC_EDIT_WIDTH3, nWidth);
	DDX_Text(pDX, IDC_EDIT_HEIGHT3, nHeight);
	DDX_Text(pDX, IDC_EDIT_RED3, nRed);
	DDX_Text(pDX, IDC_EDIT_GREEN3, nGreen);
	DDX_Text(pDX, IDC_EDIT_BLUE3, nBlue);
	DDX_Text(pDX, IDC_EDIT_ALPHA3, nAlpha);
}


BEGIN_MESSAGE_MAP(EHSParamsDialog, CDialog)
	ON_BN_CLICKED(IDOK, &EHSParamsDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_ON_TOP2, &EHSParamsDialog::OnBnClickedRadioOnTop2)
	ON_BN_CLICKED(IDC_RADIO_NORMAL2, &EHSParamsDialog::OnBnClickedRadioNormal2)
	ON_BN_CLICKED(IDC_RADIO_BOTTOM2, &EHSParamsDialog::OnBnClickedRadioBottom2)
END_MESSAGE_MAP()


BOOL EHSParamsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	char cXCoord[32];
	char cYCoord[32];
	char cWidth[32];
	char cHeight[32];
	char cRed[32];
	char cGreen[32];
	char cBlue[32];
	char cAlpha[32];

	_itoa_s(_pEHSInitParams->nXCoord, cXCoord, 10);
	SetDlgItemTextA(IDC_EDIT_X3,cXCoord);
	_itoa_s(_pEHSInitParams->nYCoord, cYCoord, 10);
	SetDlgItemTextA(IDC_EDIT_Y3,cYCoord);
	_itoa_s(_pEHSInitParams->nWidth, cWidth, 10);
	SetDlgItemTextA(IDC_EDIT_WIDTH3,cWidth);
	_itoa_s(_pEHSInitParams->nHeight, cHeight, 10);
	SetDlgItemTextA(IDC_EDIT_HEIGHT3,cHeight);
	_itoa_s(_pEHSInitParams->nColourRed, cRed, 10);
	SetDlgItemTextA(IDC_EDIT_RED3,cRed);
	_itoa_s(_pEHSInitParams->nColourGreen, cGreen, 10);
	SetDlgItemTextA(IDC_EDIT_GREEN3,cGreen);
	_itoa_s(_pEHSInitParams->nColourBlue, cBlue, 10);
	SetDlgItemTextA(IDC_EDIT_BLUE3,cBlue);
	_itoa_s(_pEHSInitParams->nAlpha, cAlpha, 10);
	SetDlgItemTextA(IDC_EDIT_ALPHA3,cAlpha);

	if (_pEHSInitParams->nHasFrame) m_hasFrame.SetCheck(1);
	if (_pEHSInitParams->nZOrder == 2) {
		m_zOnTop.SetCheck(1);
	} else if (_pEHSInitParams->nZOrder == 1) {
		m_zNormal.SetCheck(1);
	} else {
		m_zBottom.SetCheck(1);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void EHSParamsDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (nWidth <= 0) {
		AfxMessageBox("Width must be greater than 0");
	}
	else if (nHeight <= 0) {
		AfxMessageBox("Height must be greater than 0");
	}
	else if (nRed < 0 || nRed > 255) {
		AfxMessageBox("Red Value must be within 0 and 255");
	}
	else if (nGreen < 0 || nGreen > 255) {
		AfxMessageBox("Green Value must be within 0 and 255");
	}
	else if (nBlue < 0 || nBlue > 255) {
		AfxMessageBox("Blue Value must be within 0 and 255");
	}
	else if (nAlpha < 0 || nAlpha > 255) {
		AfxMessageBox("Alpha Value must be within 0 and 255");
	}
	else {
		nHasFrame = m_hasFrame.GetCheck();
		if (m_zOnTop.GetCheck()) {
			nZOrder = 2;
		} else if (m_zNormal.GetCheck()) {
			nZOrder = 1;
		} else {
			nZOrder = 0;
		}

		OnOK();
	}

}

void EHSParamsDialog::OnBnClickedRadioOnTop2()
{
	// TODO: Add your control notification handler code here
	m_zNormal.SetCheck(0);
	m_zBottom.SetCheck(0);
}

void EHSParamsDialog::OnBnClickedRadioNormal2()
{
	// TODO: Add your control notification handler code here
	m_zOnTop.SetCheck(0);
	m_zBottom.SetCheck(0);
}

void EHSParamsDialog::OnBnClickedRadioBottom2()
{
	// TODO: Add your control notification handler code here
	m_zOnTop.SetCheck(0);
	m_zNormal.SetCheck(0);
}
