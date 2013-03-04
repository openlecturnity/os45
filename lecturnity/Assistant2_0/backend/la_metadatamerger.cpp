#include "StdAfx.h"
#include <io.h>

#include "la_metadatamerger.h"
#include "sgml.h"
#include "LmdFile.h"

// merge the page information from PowerPoint (lsd) with the page information
// created if the lsd was changed by Studio (saved in lmd)
HRESULT ASSISTANT::MetadataMerger::DoMerge(Document *pDocument, CString &csLmdFilename) {
    HRESULT hr = S_OK;

    // check if fileName exists and is readable?
    if (_taccess(csLmdFilename, 04) != 0)
        hr = E_FAIL;

    // Read lmd file
    CLmdSgml lmdFile;
    if (SUCCEEDED(hr)) {
        bool bResult = lmdFile.Read(csLmdFilename, _T("<doc"));
        if (!bResult)
            hr = E_FAIL;
    }
    
    // assemble a list of all Page objects in the document
    CArray<Page *, Page *> aLsdPages;
    if (SUCCEEDED(hr)) 
        pDocument->CollectPages(aLsdPages);

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aLmdPages;
        lmdFile.GetAllPages(aLmdPages);
        for (int i = 0; i < aLmdPages.GetCount(); ++i) {
            SgmlElement *pPageTag = aLmdPages[i];
            if (pPageTag != NULL && _tcscmp(pPageTag->GetName(), _T("page")) == 0) {
                CArray<SgmlElement *, SgmlElement *> aPageProperties;
                pPageTag->GetElements(aPageProperties);
                // we need the following properties of
                // <page>:
                //   - <title>
                //   - <id>  ( == SlideID in PowerPoint)
                //   - <keyword>  (may be more than one)
                CString csPageID;
                CString csPageTitle;
                CString csPageKeywords;
                bool bFirstKeyword = true;
                for (int j = 0; j < aPageProperties.GetCount(); ++j) {
                    SgmlElement *pProperty = aPageProperties[j];
                    if (pProperty != NULL) {
                        if (_tcscmp(pProperty->GetName(), _T("title")) == 0)
                            csPageTitle = pProperty->GetParameter();
                        else if (_tcscmp(pProperty->GetName(), _T("id")) == 0)
                            csPageID = pProperty->GetParameter();
                        else if (_tcscmp(pProperty->GetName(), _T("keyword")) == 0) {
                            if (!bFirstKeyword)
                                csPageKeywords += _T("; ");
                            else
                                bFirstKeyword = false;
                            csPageKeywords += pProperty->GetParameter();
                        }
                    } 
                }

                // now, check for the metadata on/in the page
                bool bPageFound = false;
                for (int j = 0; j < aLsdPages.GetSize() && !bPageFound; ++j) {
                    // is it the correct slide?
                    Page *pPage = aLsdPages[j];
                    if (pPage != NULL && csPageID == pPage->GetSlideId()) {
                        // yes; set the keywords for this page
                        pPage->GetScorm()->SetKeywords(csPageKeywords);

                        // do we want to change the title?
                        CString csOldPageTitle = pPage->GetName();
                        // only change title if it has a default title, otherwise don't
                        if (csOldPageTitle == _T("Seite") || csOldPageTitle == _T("Page"))
                            pPage->SetName(csPageTitle);

                        // we needn't look at the rest of the Page objects
                        bPageFound = true;
                    }
                }
            }
        }
        aLmdPages.RemoveAll();
    }

    aLsdPages.RemoveAll();

    return hr;
}
