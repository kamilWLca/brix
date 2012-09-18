#include "ChildFrame.h"

ChildFrame::ChildFrame(wxMDIParentFrame *parent, wxPoint pos, wxSize size, const wxString& title):
wxMDIChildFrame(parent, wxID_ANY, title, pos, size,wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
{
	canvas = (DrawProgView *) NULL;
	//SetScrollbar(wxVERTICAL, 0, 16, 50);
	//SetScrollbar(wxHORIZONTAL, 0, 16, 50);
	m_children.Append(this);
}

ChildFrame::~ChildFrame(void){
	m_children.DeleteObject(this);
}

