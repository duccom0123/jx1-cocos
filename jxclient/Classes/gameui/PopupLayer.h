//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __PopupDemo__PopupLayer__
#define __PopupDemo__PopupLayer__

#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;
using namespace std;

class PopupLayer:public CCLayer
{
  
public:
    PopupLayer();
    ~PopupLayer();
    
    virtual bool init();
    CREATE_FUNC(PopupLayer);
    
     // 需要重写触摸注册函数，重新给定触摸级别
    virtual void registerWithTouchDispatcher(void);
     // 重写触摸函数，永远返回 true ，屏蔽其它层，达到 “模态” 效果
    bool ccTouchBegan(cocos2d::CCTouch *pTouch,cocos2d::CCEvent *pEvent);
    // 构架，并设置对话框背景图片
    static PopupLayer* create(const char* backgroundImage,int nKind=0);
	//static PopupLayer* createWith();
    
     // 它可以显示标题，并且设定显示文字大小
    void setTitle(const char*title,int fontsize = 20);
    // 文本内容，padding 为文字到对话框两边预留的距离，这是可控的，距上方的距离亦是如此
    void setContentText(const char *text, int fontsize = 20, int padding = 50, int paddintTop = 100);
    // 回调函数，当点击按钮后，我们关闭弹出层的同事，需要一个回调函数，以通知我们点击了哪个按钮（如果有多个）
    void setCallbackFunc(CCObject* target, SEL_CallFuncN callfun);
     // 为了添加按钮方面，封装了一个函数，传入些必要的参数
    bool addButton(const char* normalImage, const char* selectedImage, const char* title, int tag = 0,int isSpr=0);
    
    // 为了在显示层时之前的属性生效，选择在 onEnter 里动态展示
    virtual void onEnter();
    virtual void onExit();
	virtual void update(float delta); 
	virtual void draw();
    
private:
    
    void buttonCallback(CCObject* pSender);
    
    // 文字内容两边的空白区
    int m_contentPadding;
    int m_contentPaddingTop;
    
    CCObject* m_callbackListener;
    SEL_CallFuncN m_callback;
    
    CC_SYNTHESIZE_RETAIN(CCMenu*, m__pMenu, MenuButton);
    CC_SYNTHESIZE_RETAIN(CCSprite*, m__sfBackGround, SpriteBackGround);
    CC_SYNTHESIZE_RETAIN(CCScale9Sprite*, m__s9BackGround, Sprite9BackGround);
    CC_SYNTHESIZE_RETAIN(CCLabelTTF*, m__ltTitle, LabelTitle);
    CC_SYNTHESIZE_RETAIN(CCLabelTTF*, m__ltContentText, LabelContentText);

};
#endif /* defined(__PopupDemo__PopupLayer__) */