#include "KDownLoadFile.h"
    
//#include "..\deprecated\CCDeprecated.h"
#include <string>
#ifdef WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

//using namespace cocos2d;

downloadFile* downloadFile::m_inst = NULL;

downloadFile* downloadFile::GetInst(std::string ndownloadUrl,std::string WritablePath)
{
	if (!m_inst)
	{
		m_inst = new downloadFile();
		if (m_inst)
		{
			//m_inst->_curl      = NULL;
			m_inst->curpercent = 0;
			m_inst->setdownloadUrl(ndownloadUrl);
			m_inst->m_WritablePath= WritablePath;
			m_inst->m_DownState =STATE_OK_NET;
		}
		return m_inst;
	}
	return m_inst;
}

void downloadFile::Replease()
{

	if (m_inst)
	{
		/*if (m_inst->_curl)
		{
			curl_easy_cleanup(m_inst->_curl);
			curl_global_cleanup();
		}*/

		delete m_inst;
		m_inst = NULL;
	}
}
//std::string fullPath,std::filename,
bool downloadFile::DownLoadFile(std::string nfullPath,std::string filename,bool reload)
{
	std::string fullpath = nfullPath + filename;//CCFileUtils::sharedFileUtils()->getWritablePath() + filename;
	//CCMessageBox(fullpath.c_str(),"3333");
	FILE *fp = NULL;
	fp = createFile(fullpath.c_str(),reload);
	if (fp == NULL)
	{
		m_DownState =STATE_NO_FILE;
		return false;
	}

	long localFileLenth = 0;

	if (!reload)
	   localFileLenth = getLocalFileLenth(filename.c_str());          //已经下载的大小  
	
	long timeout = 20;
	//downloadUrl = "http://123.456.789/folder/";
	CURLcode res;
	std::string packageUrl = m_downloadUrl + filename; //下载地址+下载文件名 foldername + "/" 

	double filesize = getDownloadFileLenth(packageUrl.c_str());//获取远程文件的大小

	if (filesize == -1 || filesize == 0)
	{
		if  (fp)
		{
			fclose(fp);
			fp = NULL;
		}
		m_DownState =STATE_NO_URLFILE;
		return false;
	}
	//如果不是重载的
	if (!reload && localFileLenth>0 && localFileLenth >=filesize)
	{//已经下载完成了 //可以进行下一个文件的下载
		m_DownState =STATE_WANCHENG_FILE;
		if (fp)
		{
		 fclose(fp);
		 fp =NULL;
		}
		return true;
	}


	CURL *_curl;
	   _curl = curl_easy_init();
	if (!_curl)
	{//无网络
		if  (fp)
		{
			fclose(fp);
			fp = NULL;
		}
		curl_easy_cleanup(_curl);
		curl_global_cleanup();
		m_DownState =STATE_NO_NET;
		return false;
	}

	m_DownState =STATE_OK_NET;
	curl_easy_setopt(_curl, CURLOPT_URL,packageUrl.c_str());
	curl_easy_setopt(_curl, CURLOPT_TIMEOUT,timeout);                  //timeout秒下载时间  
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION,downLoadPackage);   //写文件回调方法  
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA,fp);
	curl_easy_setopt(_curl, CURLOPT_RESUME_FROM,localFileLenth);       //断点续传位置  
	curl_easy_setopt(_curl, CURLOPT_NOPROGRESS,0);                     //是否关闭进度显示
	curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION,progressFunc);    //下载进度回调方法
	res = curl_easy_perform(_curl);
	curl_easy_cleanup(_curl);
	curl_global_cleanup();

	if (fp)
	{
		fclose(fp);
		fp =NULL;
	}

	if (res != 0) {

		return false;
	}
	else {
		m_DownState =STATE_NEXT_FILE;
		return true;
	}

}

FILE* downloadFile::createFile(const char *file,bool reload)
{
	if (!file)
		return NULL;

	/*char buf[260];
	const char *pStart = file + 1;
	const char *pEnd = NULL;

	do
	{
		pEnd = strchr(pStart, '/');
		if (pEnd)
		{
			memcpy(buf, file, pEnd - file);
			buf[pEnd - file] = 0;
			pStart = ++pEnd;

			if (chdir(buf) == -1)
			{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
				//if (mkdir(buf))
				if ((GetFileAttributesA(buf)) == INVALID_FILE_ATTRIBUTES)
				{
					if  (!CreateDirectoryA(buf, 0))
						return NULL;
				}
				else
					return NULL;
#else
				//mkdir(m_downloadDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
				if (mkdir(buf, S_IRWXU))
					return NULL;
#endif
				
			}

		}
		else*/
		//{//wb+
			FILE *fp = NULL;
			if (reload)
			{//清空重新写
				if ((fp = fopen(file,"wb+")) == NULL)
					return NULL;
			}
			else
			{//未部最加写
				if ((fp = fopen(file,"ab+")) == NULL)
					return NULL;
			}
		
			return fp;
		//}
	//} while (1);

	//return NULL;
}

long downloadFile::getLocalFileLenth(const char* filename)
{
	std::string fullPath = m_WritablePath+filename;//CCFileUtils::sharedFileUtils()->getWritablePath() + filename;

	FILE *fp = fopen(fullPath.c_str(),"r");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		long length = ftell(fp);
		fclose(fp);
		fp =NULL;
		return length;
	}
	return 0;
}
int downloadFile::progressFunc(void *ptr, double totalToDownload, double nowDownloaded, double totalToUpLoad, double nowUpLoaded)
{
	//当前下载文件的总体大小totalToDownload  
	//当前下载的大小nowDownloaded  
	//当前下载的文件进度 
	if (m_inst)
	    m_inst->curpercent = nowDownloaded/totalToDownload * 100;
	return 0;
}

size_t downloadFile::downLoadPackage(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	FILE *fp = (FILE*)userdata;
	size_t written = fwrite(ptr, size, nmemb, fp);
	return written;
}

double downloadFile::getDownloadFileLenth(const char *url){

	double len = 0.0;
	CURL *handle = curl_easy_init();
	if (!handle)
	{
		curl_easy_cleanup(handle);
		curl_global_cleanup();
		return 0;
	}

	curl_easy_setopt(handle, CURLOPT_URL, url);
//设置超时 CURLOPT_TIMEOUT_MS

	curl_easy_setopt(handle, CURLOPT_TIMEOUT,5); 

	curl_easy_setopt(handle, CURLOPT_HEADER, 1);    //只要求header头

	curl_easy_setopt(handle, CURLOPT_NOBODY, 1);    //不需求body

	curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, save_header);

	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writefunc);

	if (curl_easy_perform(handle) == CURLE_OK) {

		if (CURLE_OK == curl_easy_getinfo(handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &len)){


		}
		else {

			//curl_easy_getinfo failed!
			len = -1;
		}

	}
	else {

		len = -1;

	}
	curl_easy_cleanup(handle);
	curl_global_cleanup();
	return len;

}
size_t downloadFile::save_header(void *ptr, size_t size, size_t nmemb, void *data)
{
	return (size_t)(size * nmemb);
}

size_t downloadFile::writefunc(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	return (size_t)(size * nmemb);
}