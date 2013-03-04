#ifndef __DRAWSDK_TOOLS__
#define __DRAWSDK_TOOLS__

#ifdef DRAWSDK_DLL
   #define DRAWSDK_EXPORT   __declspec(dllexport)
#else
   #ifndef _LIB
      #ifndef DRAWSDK_EXPORT
         #define DRAWSDK_EXPORT   __declspec(dllimport)
      #endif
   #else
      #define DRAWSDK_EXPORT
   #endif
#endif

namespace DrawSdk
{
   
   enum ObjectType 
   {
      RECTANGLE,
      OVAL,
      LINE,
      TEXT,
      POLYGON,
      MARKER,
      FREEHAND,
      TELEPOINTER,
      IMAGE,
      TARGET
   };

   // texts with special purpose (sometimes changeable)
   enum TextTypeId
   {
      TEXT_TYPE_NOTHING,
         TEXT_TYPE_CHANGE_TIME,
         TEXT_TYPE_CHANGE_TRIES,
         TEXT_TYPE_CHANGE_NUMBER,
         TEXT_TYPE_EVAL_Q_CORRECT,
         TEXT_TYPE_EVAL_Q_POINTS,
         TEXT_TYPE_EVAL_QQ_PASSED,
         TEXT_TYPE_EVAL_QQ_FAILED,
         TEXT_TYPE_EVAL_QQ_ACHIEVED,
         TEXT_TYPE_EVAL_QQ_ACHIEVED_PERCENT,
         TEXT_TYPE_EVAL_QQ_TOTAL,
         TEXT_TYPE_EVAL_QQ_REQUIRED,
         TEXT_TYPE_EVAL_QQ_REQUIRED_PERCENT,
         TEXT_TYPE_CHANGE_LAST,
         TEXT_TYPE_BLANK_SUPPORT,
         TEXT_TYPE_RADIO_SUPPORT,
         TEXT_TYPE_QUESTION_TITLE,
         TEXT_TYPE_QUESTION_TEXT,
         TEXT_TYPE_NUMBER_SUPPORT,
         TEXT_TYPE_TRIES_SUPPORT,
         TEXT_TYPE_TIME_SUPPORT,
         TEXT_TYPE_TARGET_SUPPORT
   };
}

#endif
