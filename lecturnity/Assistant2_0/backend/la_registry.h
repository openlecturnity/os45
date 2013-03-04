/*********************************************************************

  program  : la_registry.h
  authors  : Gabriela Maass
  date     : 08.01.2002
  revision : $Id: la_registry.h,v 1.2 2007-07-25 09:21:14 maass Exp $
  desc     : 
  
**********************************************************************/

#ifndef __LA_REGISTRY__
#define __LA_REGISTRY__

#pragma warning( disable: 4786)

#include <afxtempl.h>
#include "mlb_types.h"

namespace ASSISTANT
{
   class WindowsEntry
   {
   public:
      
      WindowsEntry() {}
      ~WindowsEntry() {}
      
      bool GetShellFolders(const _TCHAR *subFolder, CString &returnValue);
   };
   
   class AudioEntry
   {
   public:
      
      AudioEntry();
      ~AudioEntry() {}
      
      int Read();
   };
   
   class EntryAttributes
   {
   public:
      EntryAttributes() {}
      ~EntryAttributes() {}
      
      CString
         name,
         value;
   };
   
   class SuiteEntry
   {
   protected:
      enum type {
            MAX = 20,
            EXISTING = 0,
            NEW = 1,
            OLD = 2
      };
   public:
      SuiteEntry() {}
      ~SuiteEntry() {}
      
      static bool ReadProgramPath(CString &playerPath);
      
   };
}

#endif
