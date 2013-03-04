#pragma warning(disable:4786)

#include <windows.h>
#include <vfw.h>
#include <map>
#include "jni.h"
#include "jni-util.h"
#include "imc_epresenter_media_codec_video_LsgcDecoder.h"
#include "twlc.h"

/********* HELPER METHODS ***************************************************/

static unsigned int
jfourCCToInt(JNIEnv *env, jstring jfourcc)
{
   char *szTemp;
   int fourcc;
   
   
   szTemp = (char *) env->GetStringUTFChars(jfourcc, 0);
 
   if (strcmp(szTemp, "RGB") == 0 || strcmp(szTemp, "rgb") == 0)
      fourcc = BI_RGB; /* BI_RGB */
   else if (strcmp(szTemp, "RLE8") == 0)
      fourcc = BI_RLE8;
   else if (strcmp(szTemp, "RLE4") == 0)
      fourcc = BI_RLE4;
   else if (strcmp(szTemp, "BITF") == 0)
      fourcc = BI_BITFIELDS;
   else
      fourcc = *((int*)szTemp);
   env->ReleaseStringUTFChars(jfourcc, szTemp);

   return fourcc;
}

static BITMAPINFOHEADER *
JBitMapInfo_To_BITMAPINFO(JNIEnv *env, jobject jbi, BITMAPINFOHEADER *bi)
{
   jstring jfourCC;
   BITMAPINFOHEADER *temp;
   int biSize;
   
   if (jbi == NULL) {
      return NULL;
   }
   
   biSize = sizeof(BITMAPINFOHEADER) + (int) GetIntField(env, jbi, "extraSize");
   
   if (bi == NULL) {
      temp = (BITMAPINFOHEADER *) malloc(biSize);
   } else {
      temp = bi;
   }
   
   temp->biWidth = (int) GetIntField(env, jbi, "biWidth");
   temp->biHeight = (int) GetIntField(env, jbi, "biHeight");
   temp->biPlanes = (int) GetIntField(env, jbi, "biPlanes");
   temp->biBitCount = (int) GetIntField(env, jbi, "biBitCount");
   temp->biSizeImage = (int) GetIntField(env, jbi, "biSizeImage");
   temp->biXPelsPerMeter = (int) GetIntField(env, jbi, "biXPelsPerMeter");
   temp->biYPelsPerMeter = (int) GetIntField(env, jbi, "biYPelsPerMeter");
   temp->biClrUsed = (int) GetIntField(env, jbi, "biClrUsed");
   temp->biClrImportant = (int) GetIntField(env, jbi, "biClrImportant");
   temp->biSize = biSize;
   
   jfourCC = (jstring) GetObjectField(env, jbi, "fourcc", "Ljava/lang/String;");
   temp->biCompression = jfourCCToInt(env, jfourCC);

   //
   // extra bytes
   //
   if (temp->biSize > sizeof(BITMAPINFOHEADER)) {
      jbyteArray jba = (jbyteArray) GetObjectField(env, jbi, "extraBytes", "[B");
      char * bits = (char *) env->GetByteArrayElements(jba, 0);
      memcpy((char*) bi + sizeof(BITMAPINFOHEADER),
         bits,
         temp->biSize - sizeof(BITMAPINFOHEADER));
      env->ReleaseByteArrayElements(jba, (signed char *) bits, 0);
   }

   return temp;
}


/************** MAIN SECTION ************************************************/

static std::map<jlong, CodecInst*> instances_;

/*
 * Class:     imc_epresenter_media_codec_video_LsgcDecoder
 * Method:    nDecompressBegin
 * Signature: (Lcom/sun/media/vfw/BitMapInfo;Lcom/sun/media/vfw/BitMapInfo;)Z
 */
JNIEXPORT jlong JNICALL Java_imc_epresenter_media_codec_video_LsgcDecoder_nDecompressBegin
  (JNIEnv *env, jobject caller, jobject jbiIn, jobject jbiOut)
{
#ifdef _DEBUG
   printf("nDecompressBegin caller=%d\n", caller);
#endif

   jlong handle = (jlong)caller;

   CodecInst *ci = instances_[handle];
   if (ci == NULL)
   {
      ci = new CodecInst();
      instances_[handle] = ci;
   }

   /* Input Format */
   int size = sizeof(BITMAPINFOHEADER) + GetIntField(env, jbiIn, "extraSize");
   BITMAPINFOHEADER *biIn = (BITMAPINFOHEADER *) malloc(size);
   
   /* Output Format */
   size = sizeof(BITMAPINFOHEADER) + GetIntField(env, jbiOut, "extraSize");
   BITMAPINFOHEADER *biOut = (BITMAPINFOHEADER *) malloc(size);
   
   /* Convert them */
   JBitMapInfo_To_BITMAPINFO(env, jbiIn, biIn);
   JBitMapInfo_To_BITMAPINFO(env, jbiOut, biOut);
   
   int result = ci->DecompressBegin(biIn, biOut);
   
   if (biIn) free(biIn);
   if (biOut) free(biOut);
   
   if (result == ICERR_OK)
      return handle; // this the handle for further calls to nDecompress and nDecompressEnd
   else
   {
      if (instances_[handle] != NULL)
      {
         delete instances_[handle];
         instances_[handle] = NULL;
      }
      return result;
   }
 
}

/*
 * Class:     imc_epresenter_media_codec_video_LsgcDecoder
 * Method:    nDecompress
 * Signature: (JLcom/sun/media/vfw/BitMapInfo;Ljava/lang/Object;JLcom/sun/media/vfw/BitMapInfo;Ljava/lang/Object;J)I
 */
JNIEXPORT jint JNICALL Java_imc_epresenter_media_codec_video_LsgcDecoder_nDecompress
  (JNIEnv *env, jobject caller, jlong handle, jlong flags,
  jobject jbiIn, jobject jbufIn, jlong bytesIn, 
  jobject jbiOut, jobject jbufOut, jlong bytesOut)
{
   CodecInst *ci = instances_[handle];

#ifdef _DEBUG
   printf("nDecompress handle=%d\n", handle);
#endif

   if (ci == NULL)
      return -1;

   void *inData = (void *) bytesIn, *outData = (void*) bytesOut;

   /* Input Format */
   int size = sizeof(BITMAPINFOHEADER) + GetIntField(env, jbiIn, "extraSize");
   BITMAPINFOHEADER *biIn = (BITMAPINFOHEADER *) malloc(size);
   
   /* Output Format */
   size = sizeof(BITMAPINFOHEADER) + GetIntField(env, jbiIn, "extraSize");
   BITMAPINFOHEADER *biOut = (BITMAPINFOHEADER *) malloc(size);
   
   /* Convert them */
   JBitMapInfo_To_BITMAPINFO(env, jbiIn, biIn);
   JBitMapInfo_To_BITMAPINFO(env, jbiOut, biOut);
   
   if (bytesIn == 0)
      inData = (void *) env->GetByteArrayElements((jbyteArray) jbufIn, 0);
   if (bytesOut == 0)
      outData = (void *) env->GetByteArrayElements((jbyteArray) jbufOut, 0);
  

   ICDECOMPRESS icDecompress;
   icDecompress.lpbiInput = biIn;
   icDecompress.lpbiOutput = biOut;
   icDecompress.lpInput = inData;
   icDecompress.lpOutput = outData;
   if ((flags & 0x20000000L) != 0)
      icDecompress.dwFlags = ICDECOMPRESS_HURRYUP; // doesn't help anything here?

   int result = ci->Decompress(&icDecompress, 0);
   
   if (bytesIn == 0)
      env->ReleaseByteArrayElements(
         (jbyteArray) jbufIn, (signed char *)inData, 0);
   
   if (bytesOut == 0)
      env->ReleaseByteArrayElements(
         (jbyteArray) jbufOut, (signed char *) outData, 0);

   free(biIn);
   free(biOut);
   
   return result;
}


/*
 * Class:     imc_epresenter_media_codec_video_LsgcDecoder
 * Method:    nDecompressEnd
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_imc_epresenter_media_codec_video_LsgcDecoder_nDecompressEnd
  (JNIEnv *env, jobject caller, jlong handle)
{
#ifdef _DEBUG
   printf("nDecompressEnd handle=%d\n", handle);
#endif

   CodecInst *ci = instances_[handle];
    if (ci != NULL)
    {
      instances_[handle] = NULL;
      delete ci;
    }
}
