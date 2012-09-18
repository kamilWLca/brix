
#ifndef DRAWPROGVIEW_H_
#define DRAWPROGVIEW_H_

#include "wx/glcanvas.h"
#include <GL/gl.h>
#include "DEP.h"

class DrawProgView: public wxGLCanvas{
public:
	DrawProgView(wxWindow *parent , const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0);
public:
	~DrawProgView(void);
	void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);

    void Render();
    void InitGL();
public:
	DEP* pDEP;
private:
	wxSize windowSize;
    bool   m_init;
    unsigned int m_gllist;

//
DECLARE_EVENT_TABLE()
};

#endif /* DRAWPROGVIEW_H_ */
