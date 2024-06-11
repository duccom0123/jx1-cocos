#include "KCheckbox.h"
#include "cocos2d.h"
#include "gamecore/KCore.h"
using namespace cocos2d;

//创建,参数：未选中图片名，选中图片名，文字，状态（0-未选中，1-选中）
CCSprite * KCheckbox::create(const char * picNoSelect, const char * picSelect, char * content, int status,int nKind)
{
	CCSprite * sp = CCSprite::create();

	//checkbox
	CCSprite * sp_checkbox_select = CCSprite::create(picSelect);
	CCSprite * sp_checkbox_noSelect = CCSprite::create(picNoSelect);
	CCSize size_checkbox = sp_checkbox_noSelect->getContentSize();

	//文字
	CCLabelTTF * label_content = CCLabelTTF::create(UTEXT(content,1).c_str(),UI_GAME_FONT_DEFAULT,20);
	label_content->setColor(ccYELLOW);
	label_content->setHorizontalAlignment(kCCTextAlignmentLeft); //左对齐
	CCSize size_content = label_content->getContentSize();

	//缩放比例
	float scale_h = size_checkbox.height / size_content.height;

	//缩放文字
	label_content->setScale(scale_h);

	//最终大小
	CCSize size = CCSize(size_checkbox.width * 2 + size_content.width * scale_h , size_checkbox.height * 1.5);
	sp->setContentSize(size);

	//设置checkbox状态
	sp->addChild(sp_checkbox_noSelect);
	sp->addChild(sp_checkbox_select);
	sp_checkbox_noSelect->setPosition(CCPointMake(size_checkbox.width,size.height/2));
	sp_checkbox_select->setPosition(CCPointMake(size_checkbox.width,size.height/2));
	sp_checkbox_noSelect->setTag(0); //不选中tag＝0
	sp_checkbox_select->setTag(1);   //选中tag＝1

	if (status == 0)
	{
		//设置未选中状态
		sp_checkbox_noSelect->setVisible(true);
		sp_checkbox_select->setVisible(false);
	}
	else
	{
		//设置选中状态
		sp_checkbox_noSelect->setVisible(false);
		sp_checkbox_select->setVisible(true);
	}

	//设置文字位置
	sp->addChild(label_content);
	label_content->setAnchorPoint(CCPoint(0,0.5));
	label_content->setPosition(CCPointMake(size_checkbox.width * 2, size.height/2));

	return sp;
}

//点击操作
int KCheckbox::click()
{
	int status = getStatus();
	CCSprite * sp_checkbox_noSelect = (CCSprite *)(this->getChildByTag(0));
	CCSprite * sp_checkbox_select   = (CCSprite *)(this->getChildByTag(1));

	if (status == 0)
	{
		status = 1;//状态设置为选中
		sp_checkbox_noSelect->setVisible(false);
		sp_checkbox_select->setVisible(true);
	}
	else
	{
		status = 0;//状态设置为未选中
		sp_checkbox_noSelect->setVisible(true);
		sp_checkbox_select->setVisible(false);
	}

	return status;
}

//获取当前状态,0-未选中，1-选中
int KCheckbox::getStatus()
{
	CCSprite * sp_checkbox_noSelect = (CCSprite *)(this->getChildByTag(0));
	int status = 0; //未选中
	if (!sp_checkbox_noSelect->isVisible())
	{
		status = 1;//选中
	}

	return status;
}
