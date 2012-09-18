#include "DrawProgView.h"
#include <wx/dcclient.h>
#include "Porting_Classes/INXGLFont.h"

BEGIN_EVENT_TABLE(DrawProgView, wxGLCanvas)
    EVT_SIZE(DrawProgView::OnSize)
    EVT_PAINT(DrawProgView::OnPaint)
END_EVENT_TABLE()

DrawProgView::DrawProgView(wxWindow *parent, const wxPoint& pos,
        const wxSize& size, long style):wxGLCanvas(parent, (wxGLCanvas*) NULL, wxID_ANY,
		pos, size, style/*|wxFULL_REPAINT_ON_RESIZE|wxVSCROLL|wxHSCROLL*/)
{
	pDEP = NULL;
}

DrawProgView::~DrawProgView(void)
{
	delete pDEP;
}

void DrawProgView::OnPaint(wxPaintEvent& WXUNUSED(event)){
	Render();
}
void DrawProgView::OnSize(wxSizeEvent& event){
	  // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);

    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
 /*   int w, h;
    GetClientSize(&w, &h);

    SetCurrent();
	glOrtho(0,w,h,0,0.0f,100.0f);*/
}
void DrawProgView::OnEraseBackground(wxEraseEvent& WXUNUSED(event)){

}
void DrawProgView::Render(){

	SetCurrent();
 //   wxPaintDC(this);
	InitGL();
	//set background
	glClearColor(1.0, 1.0, 1.0, 0.0);
    /* clear color and depth buffers */
    glClear(GL_COLOR_BUFFER_BIT);
	
	// gl drawing code
    if(pDEP != NULL){
    		pDEP->DrawGL();
    	}else{
    		INXGLFont newViewText;
    		newViewText.setFontSize(18);
    		newViewText.textOut(25,100,"New INX OpenGL Window");
    		newViewText.setFontSize(14);
    		newViewText.textOut(80,100 + 25,"still in progress");
			
	}
    glFlush();
    SwapBuffers();
}
void DrawProgView::InitGL(){
	int w, h;
    GetClientSize(&w, &h);
	glOrtho(0,w,h,0,0.0f,100.0f);
	glClearDepth(1);
	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

