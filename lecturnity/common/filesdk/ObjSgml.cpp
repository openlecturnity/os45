#include "stdafx.h"
#include "ObjSgml.h"

CObjSgml::CObjSgml(void)
{
}

CObjSgml::~CObjSgml(void)
{
}

HRESULT CObjSgml::GetImageFileNames(CStringArray &aImageFilenames) {
    HRESULT hr = S_OK;

    if (m_pRoot == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aElements;
        m_pRoot->GetElements(aElements);
        for (int i = 0; i < aElements.GetSize(); ++i) {
            SgmlElement *pElement = aElements[i];
            if (pElement != NULL) {
                if (_tcscmp(pElement->GetName(), _T("IMAGE")) == 0) {
                    bool bAlwaysInserted = false;
                    CString csImageName = pElement->GetValue(_T("fname"));
                    for (int j = 0; j < aImageFilenames.GetSize() && !bAlwaysInserted; ++j) {
                        if (aImageFilenames[j] == csImageName) 
                            bAlwaysInserted = true;
                    }
                    if (!bAlwaysInserted)
                        aImageFilenames.Add(csImageName);
                }
            }
        }
    }

    return hr;
}