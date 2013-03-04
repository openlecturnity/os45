#include "imc_epresenter_player_Manager.h"
#include "windows.h"

JNIEXPORT jint JNICALL Java_imc_epresenter_player_Manager_launchFileNative
  (JNIEnv *env, jclass jc, jstring fileName)
{  
	const char* cFileName;
	cFileName = env->GetStringUTFChars(fileName, NULL);
	HINSTANCE handle = ShellExecute(NULL, "open", cFileName, "", "", SW_SHOWNORMAL);
	// free the memory for the string
	env->ReleaseStringUTFChars(fileName, cFileName);
  
	// it's ok to make this cast, since the HINSTANCE is not a true
	// window handle, but rather an error message holder.
	return (jint)handle;
}


