#ifndef __METADATA_MERGER
#define __METADATA_MERGER

#include "la_document.h"

namespace ASSISTANT
{
   class MetadataMerger
   {
   public:
       MetadataMerger() {}
      ~MetadataMerger() {}

      HRESULT DoMerge(Document *pDocument, CString &csLmdFilename);
   protected:
   };
}

#endif
