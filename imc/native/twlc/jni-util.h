/*
 * @(#)jni-util.h	1.4 01/03/15
 *
 * Copyright (c) 1996-2001 Sun Microsystems, Inc. All Rights Reserved.
 *
 * Sun grants you ("Licensee") a non-exclusive, royalty free, license to use,
 * modify and redistribute this software in source and binary code form,
 * provided that i) this copyright notice and license appear on all copies of
 * the software; and ii) Licensee does not utilize the software in a manner
 * which is disparaging to Sun.
 *
 * This software is provided "AS IS," without a warranty of any kind. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NON-INFRINGEMENT, ARE HEREBY EXCLUDED. SUN AND ITS LICENSORS SHALL NOT BE
 * LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING
 * OR DISTRIBUTING THE SOFTWARE OR ITS DERIVATIVES. IN NO EVENT WILL SUN OR ITS
 * LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR DIRECT,
 * INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES, HOWEVER
 * CAUSED AND REGARDLESS OF THE THEORY OF LIABILITY, ARISING OUT OF THE USE OF
 * OR INABILITY TO USE SOFTWARE, EVEN IF SUN HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 *
 * This software is not designed or intended for use in on-line control of
 * aircraft, air traffic, aircraft navigation or aircraft communications; or in
 * the design, construction, operation or maintenance of any nuclear
 * facility. Licensee represents and warrants that it will not use or
 * redistribute the Software for such purposes.
 */

#ifndef _JNI_UTIL_H_
#define _JNI_UTIL_H_

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

    JNIEXPORT jint GetIntField(JNIEnv *env, jobject obj, char *field);
    JNIEXPORT void SetIntField(JNIEnv *env, jobject obj, char *field, int val);
    JNIEXPORT jlong GetLongField(JNIEnv *env, jobject obj, char *field);
    JNIEXPORT void SetLongField(JNIEnv *env, jobject obj, char *field, jlong val);
    JNIEXPORT jfloat GetFloatField(JNIEnv *env, jobject obj, char *field);
    JNIEXPORT void SetFloatField(JNIEnv *env, jobject obj, char *field, jfloat val);
    JNIEXPORT jobject GetObjectField(JNIEnv *env, jobject obj, char *field, char *type);
    JNIEXPORT void SetObjectField(JNIEnv *env, jobject obj, char *field, char *type, jobject val);
    JNIEXPORT jmethodID GetMethodID(JNIEnv *env, jobject obj, char *name, char *sig);
    JNIEXPORT jobject CallObjectMethod(JNIEnv *env, jobject obj, char *name, char *sig, ...);
    JNIEXPORT jint CallIntMethod(JNIEnv *env, jobject obj, char *name, char *sig, ...);
    JNIEXPORT jlong CallLongMethod(JNIEnv *env, jobject obj, char *name, char *sig, ...);
    JNIEXPORT jdouble CallDoubleMethod(JNIEnv *env, jobject obj, char *name, char *sig, ...);
    JNIEXPORT void CallVoidMethod(JNIEnv *env, jobject obj, char *name, char *sig, ...);
    JNIEXPORT void CallStaticVoidMethod(JNIEnv *env, jclass cls, char *name, char *sig, ...);
    JNIEXPORT char *GetObjectClassName(JNIEnv *env, jobject obj);
    JNIEXPORT jboolean IsInstanceOf(JNIEnv *env, jobject obj, char *cls);

#ifdef __cplusplus
}
#endif

#endif /* _JNI_UTIL_H_ */
