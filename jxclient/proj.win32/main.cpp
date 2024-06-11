#include "main.h"
#include "AppDelegate.h"
#include "CCEGLView.h"
#include "engine/KGbktoUtf8.h"
USING_NS_CC;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    //create the application instance
    AppDelegate app;
    CCEGLView* eglView = CCEGLView::sharedOpenGLView();
	eglView->setViewName(UTEXT("jx1 duccom0123",1).c_str());
    eglView->setFrameSize(1280,720);//(800,480);
    return CCApplication::sharedApplication()->run();
}
