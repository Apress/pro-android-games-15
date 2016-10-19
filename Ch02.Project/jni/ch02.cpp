#include "ch02_project_ProjectActivity.h"
JNIEXPORT jstring Java_ch02_project_ProjectActivity_getMyData
(JNIEnv* pEnv, jobject pThis)
{
return pEnv->NewStringUTF("Hello Chapter2! My first Android NDK/JNI Project");
}
