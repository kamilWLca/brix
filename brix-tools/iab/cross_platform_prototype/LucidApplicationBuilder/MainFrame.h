/*
 * MainFrame.h
 *
 *  Created on: 5 Sep 2012
 *      Author: kamil
 */

#ifndef MAINFRAME_H_
#define MAINFRAME_H_


#include <wx/mdi.h>
#include <wx/menu.h>
#include <wx/event.h>
#include <wx/toolbar.h>
#include <wx/combobox.h>
#include "FunctionBlockTree.h"


class MainFrame: public wxMDIParentFrame
{
public:
	MainFrame(wxWindow *parent);
public:
	~MainFrame(void);
public:
	FunctionBlockTree *m_treeCtrl;
protected:
	wxMenuBar *m_menu_bar;

protected:
	void CreateTree(long style, wxWindow* parent);
	void InitToolBar(wxToolBar* toolBar);
    void OnQuit(wxCommandEvent& event);
	void OnSize(wxSizeEvent& event);
    DECLARE_EVENT_TABLE()
};

#endif /* MAINFRAME_H_ */
