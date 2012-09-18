// ProjectDialog.cpp : implementation file
//

#include "ProjectDialog.h"
//#include "PathDialog.h"
//#include "FolderDlg.h"
#include "LucidConstants.h"

#include <fstream>
using namespace std;
//#include "direct.h" @todo

#include <wx/dirdlg.h>
#include <wx/msgdlg.h>
#include <wx/filename.h>

#define IDOK wxID_OK
// CProjectDialog dialog

//DECLARE_CLASS(CProjectDialog, wxDialog)

BEGIN_EVENT_TABLE(CProjectDialog, wxDialog)
	EVT_BUTTON(IDOK, CProjectDialog::OnOk)
	EVT_BUTTON(IDC_PROJ_DIR_BUTT, CProjectDialog::OnFindProjDir)
END_EVENT_TABLE()

CProjectDialog::CProjectDialog(wxWindow* pParent /*=NULL*/)
	: wxDialog(pParent, wxID_ANY, _T("New Project"), wxPoint(100,100), wxSize(470,400),  wxDEFAULT_DIALOG_STYLE)
	, m_ProjectName(_T(""))
	//, m_ProjectDir(_T("C:\\Lucid Projects"))
	// Get the project directory from the registry
	, m_ProjectDir(_T("C://"))
	, m_ProjectCommercialName(_T(""))
	, m_ProjectVersion(_T(""))
	, m_ProjectDescription(_T(""))
{
	bIsWindowsEnv = true;

	wxRect texBoxRect(wxPoint(130,22), wxSize(270,22)); // set the frame for text box
	wxSize textSize(70, 20); // set initial size of text
	// set the text boxes for Project Dialog
	// set name of the project
	projectNameTxtBox = new wxTextCtrl(this, -1, wxT(""), texBoxRect.GetPosition(),texBoxRect.GetSize());
	wxStaticText *projectNameTxt = new wxStaticText(this, -1, wxT("Project Name"), 
		wxPoint(texBoxRect.x - textSize.GetWidth() - 10 , texBoxRect.y), textSize);
	texBoxRect.y += (texBoxRect.GetHeight() + 10);
	textSize.x += 37;
	projectRtDirTxtBox = new wxTextCtrl(this, -1, m_ProjectDir, texBoxRect.GetPosition(),texBoxRect.GetSize());
	wxStaticText *projectRtDirTxt = new wxStaticText(this, -1, wxT("Project Root Directory"), 
		wxPoint(texBoxRect.x - textSize.GetWidth() - 10 , texBoxRect.y), textSize);
	wxButton * projectRtDirButton = new wxButton(this, IDC_PROJ_DIR_BUTT, _("..."), wxPoint(texBoxRect.x + texBoxRect.width + 10, texBoxRect.y), wxSize(22,22));

	texBoxRect.y += (texBoxRect.GetHeight() + 50);
	textSize.x -= 20;
	commercialNameBox = new wxTextCtrl(this, -1, wxT(""), texBoxRect.GetPosition(),texBoxRect.GetSize());
	wxStaticText *commercialName = new wxStaticText(this, -1, wxT("Commercial Name"), 
		wxPoint(texBoxRect.x - textSize.GetWidth() - 10 , texBoxRect.y), textSize);
	
	texBoxRect.y += (texBoxRect.GetHeight() + 10);
	texBoxRect.width -= 140;
	versionTxtBox = new wxTextCtrl(this, -1, wxT(""), texBoxRect.GetPosition(),texBoxRect.GetSize());
	wxStaticText *versionTxt = new wxStaticText(this, -1, wxT("Version"), 
		wxPoint(texBoxRect.x - textSize.GetWidth() - 10 , texBoxRect.y), textSize);

	texBoxRect.y += (texBoxRect.GetHeight() + 10);
	texBoxRect.width += 140;
	texBoxRect.height += 100;
	descriptionBox = new wxTextCtrl(this, -1, wxT(""), texBoxRect.GetPosition(),texBoxRect.GetSize());
	wxStaticText *description = new wxStaticText(this, -1, wxT("Description"), 
		wxPoint(texBoxRect.x - textSize.GetWidth() - 10 , texBoxRect.y), textSize);

	wxPoint buttonPos(texBoxRect.x, texBoxRect.y + texBoxRect.height + 20);
	wxButton * okButton = new wxButton(this, IDOK, _("OK"), buttonPos, wxSize(75,25));
	buttonPos.x += 95;
	wxButton * cancelButton = new wxButton(this, wxID_CANCEL, _("Cancel"), buttonPos, wxSize(75,25));
}

CProjectDialog::~CProjectDialog()
{
}
/*
void CProjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PROJ_NAME_EDIT, (CString&)m_ProjectName);
	DDX_Text(pDX, IDC_PROJ_DIR_EDIT, (CString&)m_ProjectDir);
	DDX_Control(pDX, IDC_PROJ_DIR_EDIT, m_ProjectDirEdit);
	DDX_Text(pDX, IDC_PROJECT_COMMERCIALNAME, (CString&)m_ProjectCommercialName);
	DDX_Text(pDX, IDC_PROJECT_VERSION, (CString&)m_ProjectVersion);
	DDX_Text(pDX, IDC_PROJECT_DESCRIPTION, (CString&)m_ProjectDescription);
}
*/

// CProjectDialog message handlers
/*
BOOL CProjectDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_PROJ_NAME_EDIT);
	if(pEdit != NULL) {
		pEdit->SetFocus();
		return 0; //since we are explicity setting focus
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}*/

void CProjectDialog::OnOk(wxCommandEvent & event)
{
#ifdef TRY_EXCLUDE 
	CPathDialog dlg;
	m_ProjectName = projectNameTxtBox->GetLabel();
	m_ProjectDir = projectRtDirTxtBox->GetLabel();
	m_ProjectCommercialName = commercialNameBox->GetLabel();
	m_ProjectVersion = versionTxtBox->GetLabel();
	m_ProjectDescription = descriptionBox->GetLabel();
	// TODO: Add your control notification handler code here
//	UpdateData(TRUE);
	if (m_ProjectName == "") {
		wxMessageBox("You must enter a project name.");
	}
	else if (m_ProjectDir == "") {
		wxMessageBox("You must enter a project root directory.");
	}
	else if (dlg.isValidDir(m_ProjectDir)) {
		wxMessageBox("The project root path is invalid.");
	}
	else if (dlg.isValidDir(m_ProjectDir + "\\" + m_ProjectName)) {
		wxMessageBox("The project name is invalid.");
	}
	else if (ProjectExists()) {
		if (wxMessageBox("The project already exists.Do you want to replace it?",MB_YESNO|MB_ICONEXCLAMATION)==IDYES) {
			// write the project dir to the registry
			AfxGetApp()->WriteProfileString(_T("Project"), _T("ProjectRootDir"), m_ProjectDir);
			AfxGetApp()->WriteProfileString(_T("Project"), _T("ProjectDir"), m_ProjectDir + "\\" + m_ProjectName);
			EndModal(IDOK);
		}
	}
	else {
		// write the project dir to the registry
		AfxGetApp()->WriteProfileString(_T("Project"), _T("ProjectRootDir"), m_ProjectDir);
		AfxGetApp()->WriteProfileString(_T("Project"), _T("ProjectDir"), m_ProjectDir + "\\" + m_ProjectName);
		EndModal(IDOK);
	}
#endif //dont use until creting new project
}

void CProjectDialog::OnFindProjDir(wxCommandEvent & event)
{
	// TODO: Add your control notification handler code here
	INXString projDirPath = "";
	INXString projDirCaption(_T("Project Root Directory"));
	INXString projDirTitle(_T("Select a new project root directory."));
	/*
	unsigned int uFlags	= BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	//NOTE - new folder button in CFolderDialog doesn't work in Wine, so have disabled it
	if (!bIsWindowsEnv) {
		uFlags = uFlags | BIF_NONEWFOLDERBUTTON;
	}*/
	projectRtDirTxtBox->Clear();
	
	//m_ProjectDirEdit.GetWindowText(projDirPath);
	/*
	//CPathDialog dlg(projDirCaption, projDirTitle, projDirPath);
	CFolderDialog dlg(projDirTitle, m_ProjectDir, this, uFlags);
	dlg.SetSelectedFolder(projDirPath); //set initial tree selection node
	if(dlg.DoModal()==IDOK) {
		m_ProjectDirEdit.SetWindowText(dlg.GetFolderPath());
	}*/
	// wxWidgets choose a directory
	wxDirDialog dialog(this, _T("Select a new project root directory."), m_ProjectDir, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);	
    if (dialog.ShowModal() == wxID_OK){
        //wxLogMessage(_T("Selected path: %s"), dialog.GetPath().c_str());
		projectRtDirTxtBox->AppendText(dialog.GetPath().c_str());
	}else{
		projectRtDirTxtBox->AppendText(m_ProjectDir);
	}
}



bool CProjectDialog::IsValidDir()
{
	wxFileName dlg(m_ProjectDir);
	if (!dlg.DirExists()) {
		return false;
	}
	else {
		return true;
	}
}

bool CProjectDialog::IsValidName()
{
	int len = m_ProjectName.GetLength();
	if (m_ProjectName.GetAt(len - 1) == ' ') {
		return false;
	}
	else {
		return true;
	}
}

bool CProjectDialog::ProjectExists()
{
	INXString projectFilename = m_ProjectDir + wxT("\\") + m_ProjectName + wxT("\\") + m_ProjectName + PROJECTEXT;

	ifstream projectfile(projectFilename);

	if (projectfile.fail()) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}

void CProjectDialog::SetIsWinEnv(bool isWinEnv)
{
	bIsWindowsEnv = isWinEnv;
}
