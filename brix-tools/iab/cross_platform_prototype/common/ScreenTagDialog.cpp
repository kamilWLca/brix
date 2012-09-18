// ScreenTagDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../common/ProjectMetaData.h"
#include "ScreenTagDialog.h"
#include "ScreenTagDialogRes.h"
#include "./ScreenTagDescripEditor.h"
#include "../common/GlobalFuncs_2.h"
#include <cassert>
#include <utility>

// CScreenTagDialog dialog

IMPLEMENT_DYNAMIC(CScreenTagDialog, CDialog)


CScreenTagDialog::CScreenTagDialog(  ProjectMetaData  *pPMD, CWnd* pParent /*=NULL*/)
: CDialog(_T("IDD_SCREEN_TAGS"), pParent), m_pProjectMetaData(pPMD)
{
#ifdef DEBUG
	int i=0;
#endif
}

CScreenTagDialog::~CScreenTagDialog()
{
}

void CScreenTagDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCRNTAG_TAG_EDIT, m_control_addTag);
	DDX_Control(pDX, IDC_SCRNTAG_DESCR_EDIT, m_control_addDescription);
	//DDX_Control(pDX, IDC_MAIN_LIST, m_MainListBox);
	DDX_Control(pDX, IDC_DESCR_LIST, m_ctlDescrList);
	DDX_Control(pDX, IDC_TAG_LIST, m_ctlTagList);
	DDX_Control(pDX, IDC_USAGE_LIST, m_ctlTimesUsed);
	DDX_Control(pDX, IDC_SCRNTAG_BUTTON_DELETE, m_ctlDeleteBtn);
	DDX_Control(pDX, IDC_SCRNTAG_EDITDESCRIP_BT, m_ctlEditDescripBtn);
}


BEGIN_MESSAGE_MAP(CScreenTagDialog, CDialog)
	ON_STN_CLICKED(IDC_SCRNTAG_STATIC_TAGS, &CScreenTagDialog::OnStnClickedScrntagStaticTags)
	ON_BN_CLICKED(IDC_SCRNTAG_BUTTON_ADD, &CScreenTagDialog::OnBnClickedScrntagButtonAdd)
	ON_BN_CLICKED(IDC_SCRNTAG_BUTTON_DELETE, &CScreenTagDialog::OnBnClickedScrntagButtonDelete)
	ON_LBN_SELCHANGE(IDC_TAG_LIST, &CScreenTagDialog::OnLbnSelchangeTagList)
	ON_BN_CLICKED(IDC_SCRNTAG_EDITDESCRIP_BT, &CScreenTagDialog::OnBnClickedScrntagEditdescripBt)
END_MESSAGE_MAP()


// CScreenTagDialog message handlers

BOOL CScreenTagDialog::OnInitDialog()
{
//	      UpdateData();
	CDialog::OnInitDialog();

	m_ctlDeleteBtn.EnableWindow(false);
	m_ctlEditDescripBtn.EnableWindow(false);
	m_vTagsToBeProcessed.clear();

	m_control_addTag.SetWindowText(_T(""));

	m_pProjectMetaData->getScreenTagMgrData(m_TagDescrMap);

	updateDisplayWithUnderlyingData();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CScreenTagDialog::PostNcDestroy()
{
	CDialog::PostNcDestroy();
}

void CScreenTagDialog::OnOK()
{

	// Process the project meta data changes ...

	vector<TagAndDescriptionProcessable_t>::iterator it;
	TagAndDescriptionProcessable_t *dumPtr = NULL;

	if( m_vTagsToBeProcessed.size() > 0 ){

		it = m_vTagsToBeProcessed.begin();

		while(it != m_vTagsToBeProcessed.end()){

			dumPtr = &(*it);

			if(dumPtr->processOption==kAdd){

				m_pProjectMetaData->createNewScreenTag(dumPtr->tag, dumPtr->tagDescr);

			}else if(dumPtr->processOption==kRemove){

				m_pProjectMetaData->removeScreenTag(dumPtr->tag);
				m_pProjectMetaData->removeGuiFiles(dumPtr->tag);

			}else if(dumPtr->processOption==kEditDescrip){

				m_pProjectMetaData->editScreenTagDescription(dumPtr->tag, dumPtr->tagDescr);
			}

			it++;

		} // while(it != m_vTagsToBeProcessed.end())

	} // if(m_vTagsToBeProcessed.size() >0)

	//m_pProjectMetaData->writeProjectFile();

	CDialog::OnOK();

}



void CScreenTagDialog::OnStnClickedScrntagStaticTags()
{
	// TODO: Add your control notification handler code here
}


void CScreenTagDialog::OnBnClickedScrntagButtonDelete()
{	

	CString tag;

	// Load dialog varbls into support ones
	UpdateData();

	// get selected row pos
	int selPos = LB_ERR;
	selPos = m_ctlTagList.GetCurSel();

	if(selPos != LB_ERR){ // something is selected

		// NB. The key is the screen tag itself.
		m_ctlTagList.GetText( selPos, tag );

		// Add tag details to the list which might be committed to ProjectMetaData
		// (depending on OK/Cancel button press).

		TagAndDescriptionProcessable_t dummyTagAndDescr;
		dummyTagAndDescr.tag = tag;
		//dummyTagAndDescr.tagDescr = dumDescr;
		dummyTagAndDescr.processOption = kRemove;

		m_vTagsToBeProcessed.push_back(dummyTagAndDescr);

	// Now add to local display-support variable.

		m_TagDescrMap.erase(m_TagDescrMap.find(tag));

		m_ctlDeleteBtn.EnableWindow(false);

		updateDisplayWithUnderlyingData();

	}
}


void CScreenTagDialog::OnBnClickedScrntagButtonAdd()
{
	CString dumDescr, dumTag, cStr3;
	
	//Get the text from the dialog
	m_control_addTag.GetWindowText(dumTag);
	m_control_addDescription.GetWindowText(dumDescr);

	if( m_TagDescrMap.find(dumTag) != m_TagDescrMap.end() ){

		AfxMessageBox( "This tag already exists, and therefore cannot be added!");

	}else if( dumTag.GetLength() == 0 ){

		AfxMessageBox( "The tag has zero length, and therefore cannot be added!");

	}else{

		// Add tag details to the list which might be committed to ProjectMetaData
		// (depending on OK/Cancel button press).

		TagAndDescriptionProcessable_t dummyTagAndDescr;
		dummyTagAndDescr.tag = dumTag;
		dummyTagAndDescr.tagDescr = dumDescr;
		dummyTagAndDescr.processOption = kAdd;

		m_vTagsToBeProcessed.push_back(dummyTagAndDescr);

	// Now add to local display-support variable.

		ScreenMgrSupportData_t dummyScrnSuppDat;
		dummyScrnSuppDat.tagDescr = dumDescr;
		dummyScrnSuppDat.useCount = intToString(0);
		m_TagDescrMap[dumTag] = dummyScrnSuppDat;

		updateDisplayWithUnderlyingData();

	}

}

CString CScreenTagDialog::formShowableLine(CString tag, CString descr)
{
	return CString( "[" + tag + "]" + "  ( " + descr + " )" );
}

void CScreenTagDialog::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
	//m_TagDescrMap = m_TempStore;
}

void CScreenTagDialog::addMainString(CString &cStr)
{
	m_ctlTagList.AddString(_T(cStr));
}

void CScreenTagDialog::updateDisplayWithUnderlyingData()
{
	CString cStr1;

	// Iterate through map to load the tags themselves into
	// the tagVector ref...

	//m_pProjectMetaData->getScreenTagMgrData( m_TagDescrMap );

	int sz = -1;

	m_ctlTagList.ResetContent(); // clear-out the outgoing vector anyway
	m_ctlDescrList.ResetContent();
	m_ctlTimesUsed.ResetContent();

	if(!(sz = m_TagDescrMap.size())){ // there are no tags to load ....

		return;
	}else{

		ScreenMgrSupportData_t dummySSD;
		dummySSD.tagDescr = "";
		dummySSD.useCount = intToString(-1);

		pair<CString, ScreenMgrSupportData_t> pr("", dummySSD);
		map<INXString, ScreenMgrSupportData_t>::const_iterator it;

		it = m_TagDescrMap.begin();

		while(it != m_TagDescrMap.end()){
			pr = *it;
			m_ctlTagList.AddString(pr.first);
			m_ctlDescrList.AddString(pr.second.tagDescr);
			m_ctlTimesUsed.AddString((pr.second.useCount));
			it++;
		} // while(it != m_TagDescrMap.end())

		UpdateData(false);

		return;
	}

}



void CScreenTagDialog::OnLbnSelchangeTagList()
{
	UpdateData();

	CString tag;
	// get selected row pos
	int selPos = LB_ERR;
	selPos = m_ctlTagList.GetCurSel();

	if(selPos != LB_ERR){ // something is selected

		m_ctlEditDescripBtn.EnableWindow(true);

		// NB. The key is the screen tag itself.
		m_ctlTagList.GetText( selPos, tag );

		if( m_pProjectMetaData->getScreenTagUsageInWidgets(tag)){
			m_ctlDeleteBtn.EnableWindow(false);
		}else{
			m_ctlDeleteBtn.EnableWindow(true);
		}

	}

	UpdateData(false);
}

void CScreenTagDialog::OnBnClickedScrntagEditdescripBt()
{
	TagAndDescription_t tagAndDescr;

	CString tag;

	// Load dialog variables into support ones
	UpdateData();

	// get selected row pos
	int selPos = LB_ERR;
	selPos = m_ctlTagList.GetCurSel();

	if(selPos != LB_ERR){ // something is selected

		// get current tag and descrip from dialog.
		// NB. The key is the screen tag itself.
		m_ctlTagList.GetText( selPos, (CString)tagAndDescr.tag );
		m_ctlDescrList.GetText( selPos, (CString)tagAndDescr.tagDescr );

		// Start new dlog with these values loaded.
		CScreenTagDescripEditor dlog( tagAndDescr );

		if(dlog.DoModal() == IDOK){

			ScreenMgrSupportData_t dummy = m_TagDescrMap[tagAndDescr.tag];
			dummy.tagDescr = dlog.getTagDescription();

			// Set the local variables to dlog's values, and set to this dialog itself.
			m_TagDescrMap[tagAndDescr.tag] = dummy;
			updateDisplayWithUnderlyingData();

			TagAndDescriptionProcessable_t dummyProcessable;
			dummyProcessable.tag = tagAndDescr.tag;
			dummyProcessable.tagDescr = dummy.tagDescr;
			dummyProcessable.processOption = kEditDescrip;

			m_vTagsToBeProcessed.push_back(dummyProcessable);

		}
	}
}
