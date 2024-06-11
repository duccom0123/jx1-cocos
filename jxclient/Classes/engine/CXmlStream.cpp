
#include "CXmlStream.h"

CXmlStream::CXmlStream(const char* dicFilename)
{
	this->m_dictionary = CCDictionary::createWithContentsOfFile(dicFilename);
}
 
CXmlStream::~CXmlStream()
{
 
}
 
const char* CXmlStream::GetStringByKey(const char* keyword)
{
	if (!keyword || !m_dictionary) return NULL;

	CCString* pStr1 = (CCString*)this->m_dictionary->objectForKey(keyword);

	return pStr1->getCString();
}
 
const char* CXmlStream::GetStringByKeyFromFile(const char* dicFilename, const char* keyword)
{
	if (!dicFilename) return NULL;

	CCDictionary *dictionary = CCDictionary::createWithContentsOfFile(dicFilename);

	if  (!dictionary) return NULL;

	CCString* pStr1 = (CCString*)dictionary->objectForKey(keyword);
	return pStr1->getCString();
}

/*
// Insert objects to dictionary
*  CCString* pValue1 = CCString::create("100");
*  CCString* pValue2 = CCString::create("120");
*  CCInteger* pValue3 = CCInteger::create(200);
*  pDict->setObject(pValue1, "key1");
*  pDict->setObject(pValue2, "key2");
*  pDict->setObject(pValue3, "key3");

*/