// Encapsulate.cpp : implementation file
//

//#include "stdafx.h"
#include "DrawProg.h"
#include "Encapsulate.h"
#include "condat.h"
#include <wx/msgdlg.h>
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif*/

/////////////////////////////////////////////////////////////////////////////
// Encapsulate dialog
/*
This is the encapsulation dialog box.
*/


Encapsulate::Encapsulate(INXObjList* _encapsulated, wxWindow* pParent /*=NULL*/)
	: wxDialog(pParent, wxID_ANY, _T("New Project"), wxPoint(100,100), wxSize(470,400),  wxDEFAULT_DIALOG_STYLE)
{
	//{{AFX_DATA_INIT(Encapsulate)
	m_BlockName = _T("");
	m_LongDescription = _T("");
	m_csMenuName = _T("");
	//}}AFX_DATA_INIT
	encapsulated = _encapsulated;
	inNum = 0;
	outNum = 0;
	startNum = 0;
	finishNum = 0;
}

Encapsulate::~Encapsulate()
{
	for (UINT i=1; i<=inNum; i++) {
		delete inStatic[i];
		delete inEdit[i];
	}
	for (UINT i=1; i<=outNum; i++) {
		delete outStatic[i];
		delete outEdit[i];
	}
	for (UINT i=1; i<=startNum; i++) {
		delete startStatic[i];
		delete startEdit[i];
	}
	for (UINT i=1; i<=finishNum; i++) {
		delete finishStatic[i];
		delete finishEdit[i];
	}
}
/*
void Encapsulate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Encapsulate)
	DDX_Text(pDX, IDC_EDIT_BLOCK, (CString)m_BlockName);
	DDX_Text(pDX, IDC_EDIT_LONG, (CString)m_LongDescription);
	DDX_CBString(pDX, IDC_MENU_NAME_COMBO, m_csMenuName);
	DDX_Control(pDX, IDC_MENU_NAME_COMBO, m_ctlMenuName);
	//}}AFX_DATA_MAP
}*/


BEGIN_EVENT_TABLE(Encapsulate, wxDialog)
	EVT_BUTTON(IDOK, OnOk)
END_EVENT_TABLE()

// Methods
INXString Encapsulate::getMenuName()
{
	return m_csMenuName;
}

void Encapsulate::setMenuNamesDropList(set<INXString> sMenuNames)
{
	set<INXString>::iterator it = sMenuNames.begin();
	while( it != sMenuNames.end() ){
		m_ctlMenuName.SetValue(*it);
		it++;
	}
}

// Event Handlers
#define INX_PORT_LABEL_YALUE 300
BOOL Encapsulate::OnInitDialog() 
{
	//CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int x1=65, y1=INX_PORT_LABEL_YALUE, x2=135, y2=INX_PORT_LABEL_YALUE+25;
	int lx1=10, lx2=60;
	UINT id=100;
	ConData* blob;
	INXPOSITION pos;
	inNum = 0, outNum = 0, startNum = 0, finishNum = 0;
	UINT i;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	set<INXString> sL2LibMenuNames;

	// populate the menu name drop-list with the library level 2 menu names
	pFrame->m_cFuncBlockBar.m_cFuncBlckTree.getL2LibMenuNames(sL2LibMenuNames);
	setMenuNamesDropList(sL2LibMenuNames);

	// Use same font as Menu Name Combo for port labels and textboxes
	CFont *m_Font = NULL:/*//@todo m_ctlMenuName.GetFont(); */

	// Get the number of xports and port names for top down. Top down should have the xports defined.
	pos = encapsulated->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (encapsulated->GetNext(pos));
		if (blob->m_csIconType.Find("XINPUT") != -1) {
			inNum++;
			if (blob->description != "") {
				inNames[inNum] = blob->description;
			}
		}
		else if (blob->m_csIconType.Find("XOUTPUT") != -1) {
			outNum++;
			if (blob->description != "") {
				outNames[outNum] = blob->description;
			}
		}
		else if (blob->m_csIconType == "XSTART") {
			startNum++;
			if (blob->description != "") {
				startNames[startNum] = blob->description;
			}
		}
		else if (blob->m_csIconType == "XFINISH") {
			finishNum++;
			if (blob->description != "") {
				finishNames[finishNum] = blob->description;
			}
		}
	}

	// create textboxes and statics
	for (i=1; i<=inNum; i++) {
		inStatic[i] = new CStatic;
		inStatic[i]->Create(inLabels[i],WS_VISIBLE|WS_CHILD|SS_RIGHT,INXRect(lx1,y1,lx2,y2),this);
		inStatic[i]->SetFont(m_Font);
		inEdit[i] = new CEdit;
		inEdit[i]->Create(WS_CHILD | WS_VISIBLE | WS_DLGFRAME | ES_AUTOHSCROLL | WS_TABSTOP, INXRect(x1, y1, x2, y2), this, id);
		//inEdit[i]->SetLimitText(PORT_LABEL);
		inEdit[i]->SetFont(m_Font);
		inEdit[i]->ReplaceSel((LPCTSTR)inNames[i]);
		y1 = y2 + 10;
		y2 = y2 + 35;
		id++;
	}
	y1=INX_PORT_LABEL_YALUE; y2=INX_PORT_LABEL_YALUE+25;
	lx1 = x2 + 10;
	lx2 = x2 + 70;
	x1 = x2 + 75;
	x2 = x2 + 145;
	for (i=1; i<=outNum; i++) {
		outStatic[i] = new CStatic;
		outStatic[i]->Create(outLabels[i],WS_VISIBLE|WS_CHILD|SS_RIGHT,INXRect(lx1,y1,lx2,y2),this);
		outStatic[i]->SetFont(m_Font);
		outEdit[i] = new CEdit;
		outEdit[i]->Create(WS_CHILD | WS_VISIBLE | WS_DLGFRAME | ES_AUTOHSCROLL | WS_TABSTOP, INXRect(x1, y1, x2, y2), this, id);
		//outEdit[i]->SetLimitText(PORT_LABEL);
		outEdit[i]->SetFont(m_Font);
		outEdit[i]->ReplaceSel((LPCTSTR)outNames[i]);
		y1 = y2 + 10;
		y2 = y2 + 35;
		id++;
	}
	y1=INX_PORT_LABEL_YALUE; y2=INX_PORT_LABEL_YALUE+25;
	lx1 = x2 + 10;
	lx2 = x2 + 55;
	x1 = x2 + 60;
	x2 = x2 + 130;
	for (i=1; i<=startNum; i++) {
		startStatic[i] = new CStatic;
		startStatic[i]->Create(startLabels[i],WS_VISIBLE|WS_CHILD|SS_RIGHT,INXRect(lx1,y1,lx2,y2),this);
		startStatic[i]->SetFont(m_Font);
		startEdit[i] = new CEdit;
		startEdit[i]->Create(WS_CHILD | WS_VISIBLE | WS_DLGFRAME | ES_AUTOHSCROLL | WS_TABSTOP, INXRect(x1, y1, x2, y2), this, id);
		startEdit[i]->SetFont(m_Font);
		//startEdit[i]->SetLimitText(PORT_LABEL);
		startEdit[i]->ReplaceSel((LPCTSTR)startNames[i]);
		y1 = y2 + 10;
		y2 = y2 + 35;
		id++;
	}
	y1=INX_PORT_LABEL_YALUE; y2=INX_PORT_LABEL_YALUE+25;
	lx1 = x2 + 10;
	lx2 = x2 + 65;
	x1 = x2 + 70;
	x2 = x2 + 140;
	for (i=1; i<=finishNum; i++) {
		finishStatic[i] = new CStatic;
		finishStatic[i]->Create(finishLabels[i],WS_VISIBLE|WS_CHILD|SS_RIGHT,INXRect(lx1,y1,lx2,y2),this);
		finishStatic[i]->SetFont(m_Font);
		finishEdit[i] = new CEdit;
		finishEdit[i]->Create(WS_CHILD | WS_VISIBLE | WS_DLGFRAME | ES_AUTOHSCROLL | WS_TABSTOP, INXRect(x1, y1, x2, y2), this, id);
		finishEdit[i]->SetFont(m_Font);
		//finishEdit[i]->SetLimitText(PORT_LABEL);
		finishEdit[i]->ReplaceSel((LPCTSTR)finishNames[i]);
		y1 = y2 + 10;
		y2 = y2 + 35;
		id++;
	}

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BLOCK);
	if(pEdit != NULL) {
		pEdit->SetFocus();
		return 0; //since we are explicity setting focus
	}	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Encapsulate::OnOK(wxCommandEvent & event)
{
	bool errorFlag = FALSE;
	INXString strText = "";
	UINT i;
	
	// update attributes such as m_BlockName
	UpdateData(TRUE);
	//get the text from the textboxes
	for (i=1; i<=inNum; i++) {
		strText = "";
		int len = inEdit[i]->LineLength(inEdit[i]->LineIndex(0));
		if (len) {
			inEdit[i]->GetLine(0, strText.GetBuffer(len), len);
			strText.ReleaseBuffer(len);
		}
		INXString x(strText.MakeLower());
		inNames[i] = strText.MakeLower();
	}
	for (i=1; i<=outNum; i++) {
		strText = "";
		int len = outEdit[i]->LineLength(outEdit[i]->LineIndex(0));
		if (len) {
			outEdit[i]->GetLine(0, strText.GetBuffer(len), len);
			strText.ReleaseBuffer(len);
		}
		outNames[i] = (INXString)strText.MakeLower();
	}
	for (i=1; i<=startNum; i++) {
		strText = "";
		int len = startEdit[i]->LineLength(startEdit[i]->LineIndex(0));
		if (len) {
			startEdit[i]->GetLine(0, strText.GetBuffer(len), len);
			strText.ReleaseBuffer(len);
		}
		startNames[i] = strText.MakeLower();
	}
	for (i=1; i<=finishNum; i++) {
		strText = "";
		int len = finishEdit[i]->LineLength(finishEdit[i]->LineIndex(0));
		if (len) {
			finishEdit[i]->GetLine(0, strText.GetBuffer(len), len);
			strText.ReleaseBuffer(len);
		}
		finishNames[i] = strText.MakeLower();
	}

	// Check port names are not empty strings and are unique
	for (i=1; i<=inNum; i++) {
		// check port names
		if (inNames[i] == "") {
			wxMessageBox("WARNING: Enter a port name for " + inLabels[i]);
			errorFlag = TRUE;
		}
		for (UINT j=1; j<=inNum; j++) {
			if (i!=j && inNames[i] == inNames[j]) {
				wxMessageBox("WARNING: " + inLabels[i] + " is not unique.");
				errorFlag = TRUE;
			}
		}
	}
			
	if (!errorFlag) {
		for (i=1; i<=outNum; i++) {
			// check port names
			if (outNames[i] == "") {
				wxMessageBox("WARNING: Enter a port name for " + outLabels[i]);
				errorFlag = TRUE;
			}
			for (UINT j=1; j<=outNum; j++) {
				if (i!=j && outNames[i] == outNames[j]) {
					wxMessageBox("WARNING: " + outLabels[i] + " is not unique.");
					errorFlag = TRUE;					
				}
			}
		}
	}
			
	if (!errorFlag) {
		for (i=1; i<=startNum; i++) {
			// check port names
			if (startNames[i] == "") {
				wxMessageBox("WARNING: Enter a port name for " + startLabels[i]);					
				errorFlag = TRUE;
			}
			for (UINT j=1; j<=startNum; j++) {
				if (i!=j && startNames[i] == startNames[j]) {
					wxMessageBox("WARNING: " + startLabels[i] + " is not unique.");
					errorFlag = TRUE;
				}
			}
		}
	}
			
	if (!errorFlag) {
		for (i=1; i<=finishNum; i++) {
			// check port names
			if (finishNames[i] == "") {
				wxMessageBox("WARNING: Enter a port name for " + finishLabels[i]);
				errorFlag = TRUE;
			}
			for (UINT j=1; j<=finishNum; j++) {
				if (i!=j && finishNames[i] == finishNames[j]) {
					wxMessageBox("WARNING: " + finishLabels[i] + " is not unique.");
					errorFlag = TRUE;
				}
			}
		}
	}

	
	if (!errorFlag) {
		//CDialog::OnOK();
		EndModal(IDOK);
	}
}
