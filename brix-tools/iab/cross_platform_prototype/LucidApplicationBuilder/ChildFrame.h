#ifndef _CHILDFRAME_H
#define _CHILDFRAME_H

#include "wx/list.h"
#include "wx/mdi.h"
#include "DrawProgView.h"
class ChildFrame: public wxMDIChildFrame
{
public:
	ChildFrame(wxMDIParentFrame *parent,wxPoint pos, wxSize size,const wxString& title);
	DrawProgView *canvas;
public:
	~ChildFrame(void);

protected:
	wxList m_children;
};

#endif
