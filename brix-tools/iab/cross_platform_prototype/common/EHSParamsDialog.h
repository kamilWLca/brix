#pragma once

#include "resource1.h"
#include "afxwin.h"
#include "EHSInitParams.h"


// EHSParamsDialog dialog

class EHSParamsDialog : public CDialog
{
	DECLARE_DYNAMIC(EHSParamsDialog)

public:
	EHSParamsDialog(EHSInitParams* pEHSInitParams, CWnd* pParent = NULL);   // standard constructor
	virtual ~EHSParamsDialog();
	BOOL EHSParamsDialog::OnInitDialog();

private:
	EHSInitParams* _pEHSInitParams;

// Dialog Data
	enum { IDD = IDD_EHS_PARAMETERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_hasFrame;
public:
	afx_msg void OnBnClickedOk();
public:
	CButton m_zOnTop;
public:
	CButton m_zNormal;
public:
	CButton m_zBottom;
public:
	afx_msg void OnBnClickedRadioOnTop2();
public:
	afx_msg void OnBnClickedRadioNormal2();
public:
	afx_msg void OnBnClickedRadioBottom2();
public:
	int nXCoord;
public:
	int nYCoord;
public:
	int nWidth;
public:
	int nHeight;
public:
	int nRed;
public:
	int nGreen;
public:
	int nBlue;
public:
	int nAlpha;
public:
	int nHasFrame;
public:
	int nZOrder;
};
