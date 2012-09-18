// IconParams.cpp : implementation file
//

#include "stdafx.h"
#include "DrawProg.h"
#include "../common/LucidEnums.h"
#include "IconParams.h"
#include "ScreenTagDialog.h"
#include "GlobalFuncs_2.h"
#include "FileOperations.h"
#include <wx/msgdlg.h>

#define ID_SCREEN_TAG_BUTT		100
#define ID_WIDGET_GROUP_COMBO	101

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 


// global variable declared in CDrawProgApp
extern char workDir[WORK_DIR_SIZE];

/////////////////////////////////////////////////////////////////////////////
// IconParams dialog


void IconParams::SetHTML() {
	
	INXString helpPath;
	CFileOperation fo;

	// don't assume help file is in a particular directory, check CDF dir first then IDF
	helpPath = workDir + CDFDIR + pOrigBlob->m_csIconType + HTML_HELP_FILE;
	if (!fo.CheckPath(helpPath)) {
		helpPath = workDir + IDFDIR + pOrigBlob->m_csIconType + HTML_HELP_FILE;
		// if help not found then use default
		if (!fo.CheckPath(helpPath)) {
//			wxMessageBox("Unable to open " + helpPath);
//			return;
			helpPath = workDir + DEFAULT_HELPFILE;
		}
	}
	
	//COleVariant sLoc("http://www.google.com");
//	COleVariant sLoc("file://C:\\Users\\bob\\Documents\\how-to-load-mshtml-with-data.html");
	COleVariant sLoc((CString)helpPath);
	html_control.Navigate2(sLoc, NULL, NULL, NULL, NULL);
	
}


IconParams::IconParams(
					   ConData* pBlob, 
					   Project* _pProject, 
					   CWnd* pParent /*=NULL*/)
	: CDialog(IconParams::IDD, pParent)
{
	//{{AFX_DATA_INIT(IconParams)
	//}}AFX_DATA_INIT
	//setArray();
	mBlobTempStore.loadFromProject(*pBlob);
	pOrigBlob = pBlob;
	pProject = _pProject;
}

IconParams::~IconParams()
{
	delete nameLabel;
	delete nameVal;
	delete instLabel;
	delete instVal;
	delete descLabel;
	delete descVal;

	for (UINT i=1; i<pOrigBlob->iParamNum; i++) {
		delete paramLabel[i];
		if (pOrigBlob->iconParam[i]->dataType == 4) {
			delete enumVal[i];
			delete screenTagButton;
		}
		else if (pOrigBlob->iconParam[i]->IsTextbox()) {
			delete paramVal[i];
		}
		else {
			delete enumVal[i];
		}
	}
}

bool IconParams::isDigit(char c) {
	if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' ||
		c == '6' || c == '7' || c == '8' || c == '9') {
		return TRUE;
	}
	return FALSE;
}

void IconParams::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(IconParams)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDOK, m_ctlOK);
	DDX_Control(pDX, IDC_EXPLORER1, html_control);
}


BEGIN_MESSAGE_MAP(IconParams, CDialog)
	ON_BN_CLICKED ( ID_SCREEN_TAG_BUTT, OnScreenTagManager)
	ON_CBN_SELCHANGE( ID_WIDGET_GROUP_COMBO, OnSelChangeWidgetGroup)
	//}}AFX_MSG_MAP
	ON_WM_TIMER(TIMER_IDS_LOADHTMLHELP,OnTimer)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// IconParams message handlers

void IconParams::OnSelChangeWidgetGroup()
{
	if(!errorsInData() ){
		// Must call updateWidgetInProject b4 transferDialogDataToCache
		// as otherwise required data gets overwritten
		updateWidgetInProject();
		transferDialogDataToCache();
	}
}


void IconParams::OnOK() 
{	
	
	if(!errorsInData()){
		
		
		
		// Must call updateWidgetInProject b4 transferDialogDataToCache
		// as otherwise required data gets overwritten
		
		if(m_bIsWidget)
			updateWidgetInProject();

		transferDialogDataToCache();
		mBlobTempStore.commitToProject( pOrigBlob );

		CDialog::OnOK();
		
	}
}


bool IconParams::errorsInData() 
{

	// TODO: Add extra validation here
	INXString strText, enumLabel, csScreenTag, csOldScreenTag, csWidgetTag;
	INXString csNewFuncName, csOldFuncName, csInstNum, csNewDescription;
	long tmp;
	int dotCnt;
	double dtmp;
	bool errorFound = false;
	UINT index = 0, paramID;

	for (UINT i=1; i<mBlobTempStore.m_iParamNum; i++) {

		paramID = ICONPARAMS_ID_FIRST_PARAMETER + i - 1;	//@todo - very fragile, relies on param input boxes having a contiguous set of IDs, better to construct list of IDs at init time

		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// TEXTBOXES.
		// ( Only do error checking if it is a textbox.)

		if (mBlobTempStore.m_pParamStoreArr[i]->IsTextbox()) {
			// get the text in the textbox
			this->GetDlgItemTextA(paramID, values[i]);

			// Check textbox text is valid
			// check for empty string
			//paramVal[i]->DisplayToolTip(true);
			if (mBlobTempStore.m_pParamStoreArr[i]->m_iDataType != 3 && values[i] == "") {
				wxMessageBox("ERROR: " + labels[i] + " must have a value.");
				errorFound = true;
			}
			// if the parameter datatype is bool then check it is a 0 or 1
			else if (mBlobTempStore.m_pParamStoreArr[i]->m_iDataType == 0) {
				//tmp = atoi(dialog.values[i]);
				if (values[i] != "0" && values[i] != "1") {
					wxMessageBox("ERROR: " + labels[i] + " is of type bool and must be a 0 or 1");
					errorFound = true;
				}
			}
			// if the parameter datatype is integer then check it is in range
			else if (mBlobTempStore.m_pParamStoreArr[i]->m_iDataType == 1) {
				errno = 0;
				tmp = atoi(values[i]);
				if (errno == ERANGE || (tmp > atoi(mBlobTempStore.m_pParamStoreArr[i]->m_csMaxRange)) || (tmp < atoi(mBlobTempStore.m_pParamStoreArr[i]->m_csMinRange))) {
					wxMessageBox("ERROR: " + labels[i] + " must be between " +
						mBlobTempStore.m_pParamStoreArr[i]->m_csMinRange + " and " + mBlobTempStore.m_pParamStoreArr[i]->m_csMaxRange);
					errorFound = true;
				}		
				// check it is an integer
				for (int j=0; j < values[i].GetLength(); j++) {
					if (j==0 && values[i].GetAt(0) == '-' && errorFound) {
						if (values[i].GetLength() < 2) {
							wxMessageBox("ERROR: " + labels[i] + " must be of type integer.");
							errorFound = true;
						}
					}
					else if (!isDigit(values[i].GetAt(j)) && errorFound) {
						wxMessageBox("ERROR: " + labels[i] + " must be of type integer.");
						errorFound = true;
					}
				}
			}
			// if the parameter datatype is real then check it is in range
			else if (mBlobTempStore.m_pParamStoreArr[i]->m_iDataType == 2) {
				dtmp = atof(values[i]);
				if ((dtmp > atof(mBlobTempStore.m_pParamStoreArr[i]->m_csMaxRange)) || (dtmp < atof(mBlobTempStore.m_pParamStoreArr[i]->m_csMinRange))) {
					wxMessageBox("ERROR: " + labels[i] + " must be between " +
						mBlobTempStore.m_pParamStoreArr[i]->m_csMinRange + " and " + mBlobTempStore.m_pParamStoreArr[i]->m_csMaxRange);
					errorFound = true;
				}
				// check it is a real
				dotCnt = 0;
				for (int j=0; j < values[i].GetLength(); j++) {
					if (values[i].GetAt(j) == '.') {
						dotCnt++;
					}
					if (j==0 && values[i].GetAt(0) == '-' && errorFound) {
						if (values[i].GetLength() < 2) {
							wxMessageBox("ERROR: " + labels[i] + " must be of type real.");
							errorFound = true;
						}
					}
					else if (((!isDigit(values[i].GetAt(j)) && values[i].GetAt(j) != '.') || dotCnt > 1) && errorFound)  {
						wxMessageBox("ERROR: " + labels[i] + " must be of type real.");
						errorFound = true;
					}
				}
			}
			// if the parameter datatype is widget tag then check it doesn't already exist
			else if (mBlobTempStore.m_pParamStoreArr[i]->m_iDataType == 5) {

				// check for spaces in widget tag
				for (int k=0; k<values[i].GetLength(); k++) {
					if (values[i].GetAt(k) == ' ') {
						errorFound = true;
					}
				}

				if (!errorFound) {

					m_bIsWidget = true;
					// get the screen tag in the dialog box
					for (UINT j=1; j<mBlobTempStore.m_iParamNum; j++) {
						if (mBlobTempStore.m_pParamStoreArr[j]->m_iDataType == 4) {
							enumVal[j]->GetWindowText(csScreenTag);
						}
					}
					// check if the widget tag has been modified
					if (values[i] != mBlobTempStore.m_pParamStoreArr[i]->m_csValue) {
						// Check widget
						GuiWidget widget(values[i], csScreenTag);
						if (pProject->pProjMData->guiWidgetInVec(widget)) {
							wxMessageBox("ERROR: Tag already belongs to this Widget Group.");
							errorFound = true;
						}
					}

				} else {

					wxMessageBox("ERROR: Tag must not contain spaces.");
				}
			}
			// Check file names are 8.3 format
			/* What a **** idea this was - no directories allowed!!! 
			else if (mBlobTempStore.m_csType.Find("file_") != -1) {
				if (mBlobTempStore.m_pParamStoreArr[i]->m_csName == "File name") {
					if (!IsFileName8Dot3Format(values[i])) {
						wxMessageBox("ERROR: The file name must be in 8.3 format.");
						errorFound = true;
					}
				}
			}
			*/

		} // if (mBlobTempStore.iconParam[i]->IsTextbox())


		// drop-down lists
		// screen tags
		else if (mBlobTempStore.m_pParamStoreArr[i]->m_iDataType == 4) {
			enumVal[i]->GetWindowText(csScreenTag);

			// check if the screen tag has been modified
			if (csScreenTag != mBlobTempStore.m_pParamStoreArr[i]->m_csValue && !errorFound) {
				// get the widget tag in the dialog box
				for (UINT j=1; j<mBlobTempStore.m_iParamNum; j++) {
					if (mBlobTempStore.m_pParamStoreArr[j]->m_iDataType == 5) {
						csWidgetTag = values[j];
					}
				}

				// Check widget 
				GuiWidget widget(csWidgetTag, csScreenTag);
				if (pProject->pProjMData->guiWidgetInVec(widget)) {
					wxMessageBox("ERROR: Group already has this tag.");
					errorFound = false;
				}
				else {
					values[i] = csScreenTag;
				}
			}
		}
		// enumerated values
		else {
			enumVal[i]->GetWindowText(enumLabel);
			values[i] = mBlobTempStore.m_pParamStoreArr[i]->GetEnumVal(enumLabel);
		}

	}  // for (UINT i=1; i<mBlobTempStore.iParamNum; i++)
	
	return errorFound;
}


void IconParams::transferDialogDataToCache()
{
	INXString strText, enumLabel, csScreenTag, csOldScreenTag, csWidgetTag;
	INXString csNewFuncName, csOldFuncName, csInstNum, csNewDescription;

	// save function name for subsystems
	if (mBlobTempStore.m_iUserDefined) {

		csOldFuncName = mBlobTempStore.m_csClassName;
		GetDlgItemTextA(ICONPARAMS_ID_FUNCTIONNAME_TEXT, csNewFuncName);
		mBlobTempStore.m_csClassName = csNewFuncName;
		// update instance name if function name has changed
		if (csNewFuncName != csOldFuncName) {
			csInstNum = intToString(mBlobTempStore.m_iInstNum);
			mBlobTempStore.m_csDescription = csNewFuncName + csInstNum;
		}
		GetDlgItemTextA(ICONPARAMS_ID_DESCRIPTION_TEXT, csNewDescription);
		mBlobTempStore.m_csLongDesc = csNewDescription;

	} else {

		// store instance name
		int len = instVal->LineLength(instVal->LineIndex(0));
		if (len) {
			instVal->GetLine(0, strText.GetBuffer(len), len);
			strText.ReleaseBuffer(len);
		}
		mBlobTempStore.m_csDescription = strText;
	}

	for (UINT i=1; i<mBlobTempStore.m_iParamNum; i++) {
		mBlobTempStore.m_pParamStoreArr[i]->m_csValue = values[i];
	}
}


void IconParams::OnScreenTagManager()
{

	// MSR:
	// Call this in case the user has edited the widget tag name.
	// this is just a text box, so it is hard to know when to check that the widget
	// tag name is OK.  So we just check on specific events rather than say letter-by;letter 
	// edits of the field.
	// This is OK, but we must commit now to project so that data in screentag mgr gets
	// updated OK.

	if(!errorsInData() ){
		// Must call updateWidgetInProject b4 transferDialogDataToCache
		// as otherwise required data gets overwritten
		updateWidgetInProject();
		transferDialogDataToCache();
	}


	vector<INXString> vScreenTags;
	INXString csTmpWG;

	CScreenTagDialog dlg(pProject->pProjMData);

	if (dlg.DoModal()==IDOK) {
		// Update drop-down with screen tags
		for (UINT i=1; i<mBlobTempStore.m_iParamNum; i++) {
			// Check if parameter is a screen tag
			if (mBlobTempStore.m_pParamStoreArr[i]->m_iDataType == 4) {

				enumVal[i]->GetWindowText(csTmpWG);
				enumVal[i]->ResetContent();
				enumVal[i]->SetWindowText(values[i]);

				pProject->pProjMData->getScreenTags(vScreenTags);
				int iCurSel = 0;
				for (UINT j=0; j<vScreenTags.size(); j++) {
					enumVal[i]->AddString(vScreenTags.at(j));
					if (vScreenTags.at(j) == csTmpWG) {
						iCurSel = j;
					}
				}
				enumVal[i]->SetCurSel(iCurSel);
			}
		}
	}
}

void IconParams::updateWidgetInProject()
{
	INXString csOldScreenTag, csScreenTag, csOldWidgetTag, csWidgetTag;

	for (UINT i=1; i<mBlobTempStore.m_iParamNum; i++) {
		// get old and new screen tags
		if (mBlobTempStore.m_pParamStoreArr[i]->m_iDataType == 4) {
			csOldScreenTag = mBlobTempStore.m_pParamStoreArr[i]->m_csValue;
			csScreenTag = values[i];
		}
		// get old and new widget tags
		else if (mBlobTempStore.m_pParamStoreArr[i]->m_iDataType == 5) {
			csOldWidgetTag = mBlobTempStore.m_pParamStoreArr[i]->m_csValue;
			csWidgetTag = values[i];
		}
	}

	GuiWidget oldWidget(csOldWidgetTag, csOldScreenTag);
	GuiWidget widget(csWidgetTag, csScreenTag);

	pProject->pProjMData->updateWidget(oldWidget, widget);

}


BOOL IconParams::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetWindowRect(m_rect);
	m_nScrollPos = 0;
	// TODO: Add extra initialization here
	int x1=140, y1=10, x2=340, y2=32;
	int lx1=10, lx2=130;
	vector<INXString> vScreenTags;
	// Use same font as OK button for labels and textboxes
	CFont *m_Font = m_ctlOK.GetFont();

	// Added function name for Ess
	nameLabel = new CStatic;
	//nameLabel->Create("Function Name",WS_VISIBLE|WS_CHILD|SS_LEFT,INXRect(20,y1,150,y2),this);
	nameLabel->CreateEx(WS_EX_CLIENTEDGE,_T("STATIC"),_T("Function Name"),(WS_CHILD | WS_VISIBLE |  ES_AUTOHSCROLL | ES_READONLY),INXRect( 10, y1, 100, y2), this, ICONPARAMS_ID_FUNCTIONNAME_LABEL);
	nameLabel->SetFont(m_Font);

	nameVal = new CEdit;
	// Allow user to edit subsystem block names
	if (mBlobTempStore.m_iUserDefined) {
		//nameVal->Create(WS_CHILD | WS_VISIBLE | WS_DLGFRAME | ES_AUTOHSCROLL | WS_TABSTOP, INXRect(20, y1, 270, y2), this, ICONPARAMS_ID_FUNCTIONNAME_TEXT);
		nameVal->CreateEx(WS_EX_CLIENTEDGE,_T("EDIT"),_T(""),(WS_CHILD | WS_VISIBLE |  ES_AUTOHSCROLL ),INXRect( 100, y1, 300, y2), this, ICONPARAMS_ID_FUNCTIONNAME_TEXT);
	}
	else {
		//nameVal->Create(WS_CHILD | WS_VISIBLE | WS_DLGFRAME | ES_AUTOHSCROLL | ES_READONLY, INXRect(20, y1, 270, y2), this, ICONPARAMS_ID_FUNCTIONNAME_TEXT);
		nameVal->CreateEx(/*WS_EX_STATICEDGE*/WS_EX_CLIENTEDGE,_T("EDIT"),_T(""),(WS_CHILD | WS_VISIBLE |  ES_AUTOHSCROLL | ES_READONLY),INXRect( 100, y1, 300, y2), this, ICONPARAMS_ID_FUNCTIONNAME_TEXT);
	}
	nameVal->SetFont(m_Font);
	nameVal->ReplaceSel((LPCTSTR)mBlobTempStore.m_csClassName);
	/****************************************************************************************************/
	// Added instance name for Ess
	instLabel = new CStatic;
	//instLabel->Create("Instance Name",WS_VISIBLE|WS_CHILD|SS_LEFT,INXRect(300,y1,450,y2),this);
	instLabel->CreateEx(WS_EX_CLIENTEDGE,_T("STATIC"),_T("Instance Name"),(WS_CHILD | WS_VISIBLE |  ES_AUTOHSCROLL | ES_READONLY),INXRect( 310, y1, 400, y2), this, ICONPARAMS_ID_INSTANCENAME_LABEL);
	instLabel->SetFont(m_Font);

	instVal = new CEdit;
	if (mBlobTempStore.m_csType.Find("XINPUT") != -1 || mBlobTempStore.m_csType.Find("XOUTPUT") != -1 || 
		mBlobTempStore.m_csType == "XSTART" || mBlobTempStore.m_csType == "XFINISH" || mBlobTempStore.m_iUserDefined) {
		//instVal->Create(WS_CHILD | WS_VISIBLE | WS_DLGFRAME | ES_AUTOHSCROLL | ES_READONLY, INXRect(300, y1, 550, y2), this, ICONPARAMS_ID_INSTANCENAME_TEXT);
	    instVal->CreateEx(WS_EX_CLIENTEDGE,_T("EDIT"),_T(""),(WS_CHILD | WS_VISIBLE |  ES_AUTOHSCROLL | ES_READONLY ),INXRect( 400, y1, 580, y2), this, ICONPARAMS_ID_INSTANCENAME_TEXT);
	}
	else {
		//instVal->Create(WS_CHILD | WS_VISIBLE | WS_DLGFRAME | ES_AUTOHSCROLL | WS_TABSTOP, INXRect(300, y1, 550, y2), this, ICONPARAMS_ID_INSTANCENAME_TEXT);
		instVal->CreateEx(WS_EX_CLIENTEDGE,_T("EDIT"),_T(""),(WS_CHILD | WS_VISIBLE |  ES_AUTOHSCROLL ),INXRect( 400, y1, 580, y2), this, ICONPARAMS_ID_INSTANCENAME_TEXT);
	}
	instVal->SetFont(m_Font);
	instVal->ReplaceSel((LPCTSTR)mBlobTempStore.m_csDescription);
	
	y1 = y2+2;
	y2 = y2+24; //Moving to the right now..
	
	// Added Short description for Ess
	descLabel = new CStatic;
	descLabel->CreateEx(WS_EX_CLIENTEDGE,_T("STATIC"),_T("Description"),(WS_CHILD | WS_VISIBLE |  ES_AUTOHSCROLL | ES_READONLY),INXRect(10,y1,100,y2), this, ICONPARAMS_ID_DESCRIPTION_LABEL);
	//->Create("Description",WS_VISIBLE|WS_CHILD|SS_RIGHT,INXRect(lx1-40,y1,lx2-40,y2),this);
	descLabel->SetFont(m_Font);
	descVal = new CEdit;
	// Allow user to edit subsystem descriptions
	if (mBlobTempStore.m_iUserDefined) {
		descVal->CreateEx(WS_EX_CLIENTEDGE,_T("EDIT"),_T(""),(WS_CHILD | WS_VISIBLE |  ES_AUTOHSCROLL | ES_READONLY ),INXRect(100,y1,580,y2), this, ICONPARAMS_ID_DESCRIPTION_TEXT);
	//->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL | WS_TABSTOP, INXRect(10, y1, 680, y2), this, ICONPARAMS_ID_DESCRIPTION_TEXT);
	}
	else {
		descVal->CreateEx(WS_EX_CLIENTEDGE,_T("EDIT"),_T(""),(WS_CHILD | WS_VISIBLE |  ES_AUTOHSCROLL| ES_READONLY ),INXRect(100,y1,580,y2), this, ICONPARAMS_ID_DESCRIPTION_TEXT);
	//->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_READONLY, INXRect(10, y1, 680, y2), this, ICONPARAMS_ID_DESCRIPTION_TEXT);
	}
	descVal->SetLayeredWindowAttributes(RGB(255,244,244),255,LWA_ALPHA );
	descVal->EnableWindow(1);
	descVal->SetFont(m_Font);
	if (strlen((LPCTSTR)mBlobTempStore.m_csDescription) > 0) {
		descVal->ReplaceSel((LPCTSTR)mBlobTempStore.m_csDescription); // Render the short description if there is one
	}else {
		descVal->ReplaceSel((LPCTSTR)mBlobTempStore.m_csLongDesc); // Render the short description otherwise (which might be a url).
	}

// And now skipt the statically placd HTML window...	
	y1 = y2+380;//+200;
	y2 = y2+402;//+200;
    
	UINT paramID = ICONPARAMS_ID_FIRST_PARAMETER;	
	for (UINT i=1; i<mBlobTempStore.m_iParamNum; i++) {
		paramLabel[i] = new CStatic;
		paramLabel[i]->Create(labels[i],WS_VISIBLE|WS_CHILD|SS_RIGHT,INXRect(lx1,y1,lx2,y2),this);
		paramLabel[i]->SetFont(m_Font);

		// Check if parameter is a screen tag
		// dataType should be replaced with an enumerated type
		//if (mBlobTempStore->iconParam[i]->dataType == 4) {
		if ((mBlobTempStore.m_pParamStoreArr[i])->m_iDataType == 4) {//@tpdp need to use an enum type here.. Will go wrong soon!
			enumVal[i] = new CComboBox;
			enumVal[i]->Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | WS_TABSTOP, 
				INXRect(x1, y1, x2, y2+100), this, ID_WIDGET_GROUP_COMBO );
			enumVal[i]->SetFont(m_Font);
			// set the items in the drop-down. values[i] is an integer. The enumLabel needs to be displayed
			pProject->pProjMData->getScreenTags(vScreenTags);
			int iCurSel = 0;
			for (UINT j=0; j<vScreenTags.size(); j++) {
				enumVal[i]->AddString(vScreenTags.at(j));
				if (vScreenTags.at(j) == values[i]) {
					iCurSel = j;
				}
			}
			enumVal[i]->SetCurSel(iCurSel);
			screenTagButton = new CButton;
			screenTagButton->Create("Widget- Group Manager", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_MULTILINE|WS_TABSTOP, INXRect(x1+210, y1, x2+200, y2), this, ID_SCREEN_TAG_BUTT);
			screenTagButton->SetFont(m_Font);
		}
		// Check if parameter is a textbox or drop down
		else if (  pOrigBlob->iconParam[i]->IsTextbox()) {

			paramVal[i] = new CEdit;
			//paramVal[i]->Create(WS_CHILD | WS_VISIBLE | WS_DLGFRAME | ES_AUTOHSCROLL | WS_TABSTOP, INXRect(x1, y1, x2, y2), this, paramID);
			paramVal[i]->CreateEx(WS_EX_CLIENTEDGE,_T("EDIT"),_T(""),(WS_CHILD | WS_VISIBLE |  ES_AUTOHSCROLL ),INXRect(x1, y1, x2, y2), this, paramID);
			paramVal[i]->SetFont(m_Font);
			if (values[i] == "_") {
				paramVal[i]->ReplaceSel("");
			}
			else {
				paramVal[i]->ReplaceSel((LPCTSTR)values[i]);
			}
			// SDG: Started experimenting with tooltips
			//paramVal[i]->CreateToolTip(this, _T("Tooltip text"));
			//paramVal[i]->DisplayToolTip(TRUE);

		} else {
			enumVal[i] = new CComboBox;
			enumVal[i]->Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN | WS_TABSTOP, INXRect(x1, y1, x2, y2+100), this, paramID);
			enumVal[i]->SetFont(m_Font);
			// set the items in the drop-down. values[i] is an integer. The enumLabel needs to be displayed
			enumVal[i]->SetWindowText( (mBlobTempStore.m_pParamStoreArr[i])->GetEnumLabel( values[i] ) );
			for (int j=0; j<(mBlobTempStore.m_pParamStoreArr[i])->m_csaEnumLabelArr.GetSize(); j++) {
				enumVal[i]->AddString( (mBlobTempStore.m_pParamStoreArr[i])->m_csaEnumLabelArr.GetAt(j));
			}
		}
		y1 = y2+10;
		y2 = y2+35;
		paramID++;
	}
	
	SetTimer(TIMER_IDS_LOADHTMLHELP,TIMER_WAITMS_LOADHTMLHELP,NULL); //fire a timer to load th HTML page when we're up -we can't do it 'till the active-x is instantiated..

	if (!(mBlobTempStore.m_csType.Find("XINPUT") != -1 || mBlobTempStore.m_csType.Find("XOUTPUT") != -1 || 
		mBlobTempStore.m_csType == "XSTART" || mBlobTempStore.m_csType == "XFINISH" || mBlobTempStore.m_iUserDefined)) {
		if(instVal != NULL) {
			instVal->SetFocus();
			return 0; //since we are explicity setting focus
		}
	}
	else if (mBlobTempStore.m_iUserDefined) {
		if (nameVal != NULL) {
			nameVal->SetFocus();
			return 0;
		}
	}
	
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool IconParams::IsFileName8Dot3Format(INXString csFileName)
{
	bool bRet = true;
	int iLen;
	
	iLen = csFileName.GetLength();
	if (iLen > 12 || csFileName.GetAt(iLen - 4) != '.') {
		bRet = false;
	}
		
	return bRet;
}
BOOL IconParams::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

void IconParams::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
    KillTimer(TIMER_IDS_LOADHTMLHELP);
	SetHTML();
	CDialog::OnTimer(nIDEvent);
	
}

void IconParams::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{


	// TODO: Add your message handler code here and/or call default.
	int nDelta;
	//mBlobTempStore.m_iParamNum
	int nMaxPos = m_rect.Height()*2;// todo  the following aint set? - m_nCurHeight;


	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(nMaxPos/10,nMaxPos-m_nScrollPos);
		break;

	case SB_LINEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(nMaxPos/10,m_nScrollPos);
		break;

         case SB_PAGEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(nMaxPos/10,nMaxPos-m_nScrollPos);
		break;

	case SB_THUMBPOSITION:
		nDelta = (int)nPos - m_nScrollPos;
		break;

	case SB_PAGEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(nMaxPos/10,m_nScrollPos);
		break;
	
         default:
		return;
	}
	//nDelta=nDelta*2; // allow scrolling of twice th original size. //todo we should set this depnding on the number of parameters? 
	m_nScrollPos += nDelta;
	SetScrollPos(SB_VERT,m_nScrollPos,TRUE);
	ScrollWindow(0,-nDelta*5); /* this is the speed of the actual scroll */
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

