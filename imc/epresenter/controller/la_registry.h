/*********************************************************************

 program  : la_registry.h
 authors  : Gabriela Maass
 date     : 08.01.2002
 revision : $Id: la_registry.h,v 1.1 2002-06-11 13:36:58 maass Exp $
 desc     : 

 **********************************************************************/

#ifndef __LA_REGISTRY__
#define __LA_REGISTRY__

namespace ASSISTANT
{
   typedef struct {
     char
         day[5],
         month[5],
         year[5];
     char
         major[5],
         minor[5],
         micro[5];
      char
         typ[5];
      char
         period[5];
      char
         language[5];
   } regEntry;


   class RegistryEntry 
   {
   protected:
      char 
         passWord[20],
         checkPassWord[20]; 
      int 
         passWordLength;

   public:
      RegistryEntry();
      ~RegistryEntry() {}

      int ReadSecurityEntry(ASSISTANT::regEntry *entry);
      int ReadChecksumEntry(ASSISTANT::regEntry *entry);
      int WriteChecksumEntry(ASSISTANT::regEntry *entry);
      int ComputeTimeDiff(ASSISTANT::regEntry *entry);
      int GetActualDate(ASSISTANT::regEntry *entry);
   };

   class AudioEntry
   {
   public:

      AudioEntry();
      ~AudioEntry() {}

      int Read();
   };

}

#endif
