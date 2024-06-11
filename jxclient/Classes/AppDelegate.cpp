#include "AppDelegate.h"
#include "gamescene/KgameWorld.h"
#include "gamescene/KUpdateLayer.h"
USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    CCDirector* pDirector = CCDirector::sharedDirector();
    CCEGLView* pEGLView   = CCEGLView::sharedOpenGLView();
    //CCEGLView::sharedOpenGLView()->setDesignResolutionSize(800, 480, kResolutionNoBorder);
	//pEGLView->setFrameSize(960,540); //800,480
	pDirector->setDepthTest(true);//关闭深度测试
	pDirector->setProjection(kCCDirectorProjection2D);  
    pDirector->setOpenGLView(pEGLView);
    pEGLView->setDesignResolutionSize(1024,768,kResolutionExactFit);//kResolutionNoBorder
    // turn on display FPS  设置调试信息
    pDirector->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0/60);

#ifndef WIN32
    /*struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE,&sa,0);*/
        struct sigaction sa;
    	sa.sa_handler = SIG_IGN;//设定接受到指定信号后的动作为忽略
    	sa.sa_flags = 0;
    	if (sigemptyset(&sa.sa_mask) == -1 ||     //初始化信号集为空
    		sigaction(SIGPIPE, &sa, 0) == -1) {   //屏蔽SIGPIPE信号
    			CCMessageBox("failed to Init","Init Error");
    			exit(EXIT_FAILURE);
    	}
    	//我的网络服务器程序使用了libevent库，这个库使用了pthread线程库。
    	sigset_t signal_mask;
        sigemptyset(&signal_mask);
        sigaddset(&signal_mask,SIGPIPE);
        int rc = pthread_sigmask(SIG_BLOCK,&signal_mask,NULL);
        if (rc != 0) {
            CCMessageBox("failed to Init","Init Error");
        }
#endif

    // create a scene. it's an autorelease object 创建 场景
    //CCScene *pScene = HelloWorld::scene();
	CCScene *pScene = KUpdateLayer::scene();
    // run 运行这个场景
    pDirector->runWithScene(pScene);
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    CCDirector::sharedDirector()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    CCDirector::sharedDirector()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}

