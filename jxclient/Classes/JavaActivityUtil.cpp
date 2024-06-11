/**
    Android.mk 里面记得添加
*/
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include "cocos2d.h"
//#include "CrashHTTP/ExceptionHTTP.h"
using namespace cocos2d;

extern "C" {
    JNIEXPORT void Java_org_cocos2dx_lib_XbCrashHandler_nativeSaveCrash(JNIEnv* env, jobject thiz, jstring carshText)
    {
		if  (env)
		{
	    	//std::string  strValue = env->getStringUTFCharsJNI(env, carshText);
	        //const char* crashStr = strValue.c_str();
	        //todo 调用 http发送的
	        const char* chars = env->GetStringUTFChars(carshText, NULL);
	             // string ret(chars);
            CCLog("chedan   carsh log : %s", chars);
	        env->ReleaseStringUTFChars(carshText, chars);

		}
    }
}

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
