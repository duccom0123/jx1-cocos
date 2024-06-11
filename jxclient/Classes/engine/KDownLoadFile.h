//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KList.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef	KDDWNLOADFILE_H
#define	KDDWNLOADFILE_H
#pragma once
//---------------------------------------------------------------------------
#include "cocos2d.h"
#include "curl/curl.h"  
USING_NS_CC;

//---------------------------------------------------------------------------
enum DOWN_LOAD_STATE
{//游戏ui中的父节点
	STATE_OK_NET=0,//正常
	STATE_NO_NET,  //没有网络
	STATE_NO_FILE,    //文件不存在无法写入
	STATE_NO_URLFILE, //文件不存在无法写入
	STATE_NEXT_FILE,//下一个文件
	STATE_WANCHENG_FILE,
};
class downloadFile : public cocos2d::CCObject
{
public:
	static downloadFile *m_inst;
	static downloadFile *GetInst(std::string ndownloadUrl,std::string WritablePath);
	static void Replease();

	bool DownLoadFile(std::string nfullPath,std::string filename,bool reload=false);
	long getLocalFileLenth(const char* filename);
	static int progressFunc(void *ptr, double totalToDownload, double nowDownloaded, double totalToUpLoad, double nowUpLoaded);
	static size_t downLoadPackage(void *ptr, size_t size, size_t nmemb, void *userdata);
	double getDownloadFileLenth(const char *url);
	static size_t save_header(void *ptr, size_t size, size_t nmemb, void *data);
	static size_t writefunc(void *ptr, size_t size, size_t nmemb, void *userdata);
	FILE* createFile(const char *file,bool reload=false);
	void  setCurpercent(float i){curpercent=i;};
	float getCurpercent(){return curpercent; };

	void  setdownloadUrl(std::string strurl){m_downloadUrl=strurl;};
	std::string getdownloadUrl(){return m_downloadUrl; };

	int   getDownState(){return m_DownState;}
	
private:
	float  curpercent; //当前的百分比
	std::string m_downloadUrl;
	std::string m_WritablePath;
	int    m_DownState;
	//CURL *_curl;
};
//-------------------------------------------------------------------------
#endif
