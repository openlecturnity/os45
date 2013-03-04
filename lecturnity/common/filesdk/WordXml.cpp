#include "stdafx.h"
#include "WordXml.h"
#include "Resource.h"
#include "Base64.h"
#include "ManualConfiguration.h" // lsutl32
#include "MiscFunctions.h"

//////////////////////////////////////////////////////////////////////
// CWordSlideObject
//////////////////////////////////////////////////////////////////////

CWordSlideObject::CWordSlideObject(void) {
    m_pImage = NULL;
    m_aText.RemoveAll();
    m_rcClickBounds.SetRectEmpty();
    m_ptClick.SetPoint(0,0);
}

CWordSlideObject::~CWordSlideObject(void) {
    if (m_pImage != NULL)
        delete m_pImage;

    for (int i = 0; i < m_aText.GetCount(); ++i) {
        if (m_aText[i] != NULL)
            delete m_aText[i];
    }
    m_aText.RemoveAll();
}

void CWordSlideObject::AddImage(DrawSdk::Image *pImage) {
    if (m_pImage != NULL)
        delete m_pImage;

    m_pImage = (DrawSdk::Image *)pImage->Copy();
}

void CWordSlideObject::AddText(DrawSdk::Text *pText) {
    // NULL means end of group
    if (pText != NULL)
        m_aText.Add((DrawSdk::Text *)pText->Copy());
    else
        m_aText.Add(NULL);
}


//////////////////////////////////////////////////////////////////////
// CWordXml
//////////////////////////////////////////////////////////////////////

CWordXml::CWordXml(CManualConfiguration *pManualConfiguration, LPCTSTR tszDocumentTitle) {
    m_pManualConfiguration = pManualConfiguration;
    m_logFont.lfHeight         = 11;
    m_logFont.lfWidth          = 0;
    m_logFont.lfEscapement     = 0;
    m_logFont.lfOrientation    = 0;
    m_logFont.lfWeight         = FW_NORMAL;
    m_logFont.lfItalic         = FALSE;
    m_logFont.lfUnderline      = FALSE;
    m_logFont.lfStrikeOut      = FALSE;
    m_logFont.lfCharSet        = DEFAULT_CHARSET;
    m_logFont.lfOutPrecision   = OUT_TT_PRECIS;
    m_logFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
    m_logFont.lfQuality        = ANTIALIASED_QUALITY;
    m_logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    _stprintf((TCHAR *) &m_logFont.lfFaceName, _T("Arial"));

    m_csDocumentTitle.Empty();
    if (tszDocumentTitle != NULL)
        m_csDocumentTitle = tszDocumentTitle;

    ASSERT(m_pManualConfiguration);
}

CWordXml::~CWordXml(void) {
    m_pManualConfiguration = NULL;
}

HRESULT CWordXml::Write(CString csFilename, CMetaInformation &metainfo, CArray<CWordSlideObject *, CWordSlideObject *> &aPage) {
    HRESULT hr = S_OK;

    if (csFilename.IsEmpty())
        hr = E_FAIL;

    HANDLE hWordFile = NULL;
    if (SUCCEEDED(hr)) {
        hWordFile = CreateFile(csFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hWordFile == INVALID_HANDLE_VALUE)
            hr = E_FAIL;
    }

    CFileOutput wordOutput;
    if (SUCCEEDED(hr))
        hr = wordOutput.Init(hWordFile, 0, true);

    // Write Header
    if (SUCCEEDED(hr))
        hr = Write(wordOutput, IDR_WORD_XML_DOCUMENT_BEGIN);

    // Write Document Properties
    if (SUCCEEDED(hr))
        hr = WriteProperties(wordOutput, metainfo);

    // Write Document List
    if (SUCCEEDED(hr))
        hr = Write(wordOutput, IDR_WORD_XML_LIST);

    // Write Document Styles
    if (SUCCEEDED(hr))
        hr = Write(wordOutput, IDR_WORD_XML_STYLES);

    // Write Document Parameter
    if (SUCCEEDED(hr))
        hr = Write(wordOutput, IDR_WORD_XML_DOCUMENT_PARAMETER);

    //Generate Thumbnail image
    if (SUCCEEDED(hr))
        hr = GenerateThumbnail(csFilename, aPage, metainfo);
    // Write document body
    if (SUCCEEDED(hr)) 
        hr = WriteDocumentContent(wordOutput, aPage, metainfo);

    // Write Footer
    if (SUCCEEDED(hr))
        hr = Write(wordOutput, IDR_WORD_XML_DOCUMENT_END);


    wordOutput.Flush();
    CloseHandle(hWordFile);

    return hr;
}

///// Private

LPBYTE CWordXml::GetDataFromResource(int iTextResourceID, DWORD &dwSize)  {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HINSTANCE hInstance = ::AfxGetResourceHandle();
    HRSRC hRsrc = ::FindResource (hInstance, MAKEINTRESOURCE(iTextResourceID), _T("TXT")); 
    ASSERT(hRsrc != NULL);

    dwSize = ::SizeofResource(hInstance, hRsrc);
    LPBYTE lpRsrc = (LPBYTE)::LoadResource(hInstance, hRsrc);
    ASSERT(lpRsrc != NULL);

    return lpRsrc;
}

HRESULT CWordXml::GetCStringFromResource(int iTextResourceID, CString &csResource)  {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;

    DWORD dwSize;
    LPBYTE lpRsrc = GetDataFromResource(iTextResourceID, dwSize);

#ifdef _UNICODE
    WCHAR *wszBuffer = (WCHAR *)malloc((dwSize+1)*sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpRsrc, dwSize, wszBuffer, dwSize);
    wszBuffer[dwSize] = _T('\0');
    csResource = wszBuffer;
    delete wszBuffer;
#else
    csResource = lpRsrc;
#endif

    ::FreeResource(lpRsrc);

    return hr;
}

HRESULT CWordXml::Replace(CString &csText,LPCTSTR tszTemplateVar, LPCTSTR tszReplaceText) {
    HRESULT hr = S_OK;

    int iPos = csText.Find(tszTemplateVar);
    if (iPos >= 0)
        csText.Replace(tszTemplateVar, tszReplaceText);
    else
        hr = E_FAIL;

    return hr;
}

HRESULT CWordXml::Write(CFileOutput &wordOutput, int iTextResourceID) {
    HRESULT hr = S_OK;

    CString csResource;
    GetCStringFromResource(iTextResourceID, csResource);
    wordOutput.Append(csResource, csResource.GetLength());

    return hr;
}

HRESULT CWordXml::WriteProperties(CFileOutput &wordOutput, CMetaInformation &metainfo) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HRESULT hr = S_OK;

    CString csProperty;
    GetCStringFromResource(IDR_WORD_XML_DOCUMENT_PROPERTIES, csProperty);

    CTime time = CTime::GetCurrentTime();
    CString csTime = time.Format("%Y-%m-%dT%H:%M:%SZ");

    CString csAuthor(metainfo.GetAuthor());
    StringManipulation::EncodeXmlSpecialCharacters(csAuthor);
    Replace(csProperty, _T("%AUTHOR%"), csAuthor);


    CString csTitle = metainfo.GetTitle();
    if (!m_csDocumentTitle.IsEmpty() && (csTitle.IsEmpty() || csTitle == _T("recording")))
        csTitle = m_csDocumentTitle;
   
    StringManipulation::EncodeXmlSpecialCharacters(csTitle);

    Replace(csProperty, _T("%TITLE%"), csTitle);
    Replace(csProperty, _T("%DATE%"), csTime);

    wordOutput.Append(csProperty, csProperty.GetLength());

    return hr;
}

HRESULT CWordXml::GenerateImageData(Gdiplus::Bitmap *bmpImage, CFileOutput &wordOutput, bool bFromFile, CString csImageName) {
    HRESULT hr = S_OK;

    if (bFromFile) {
        CFile file;
        if (file.Open(csImageName, CFile::modeRead | CFile::shareDenyNone)) {
            UINT nSize = (UINT)file.GetLength();
            LPBYTE lpBuffer = (LPBYTE)malloc(nSize);
            file.Read((void *)lpBuffer, nSize);
            file.Close();

            int iNewLength;
            LPSTR lpNewBuffer = CBase64::Encode(nSize, lpBuffer, iNewLength);
            wordOutput.Append((BYTE *)lpNewBuffer, iNewLength);

            delete lpBuffer;
            delete lpNewBuffer;
        }
    } else {
        if (!bmpImage) {
            ASSERT(bmpImage);
            return S_FALSE;
        }
        IStream *pStream;
        ::CreateStreamOnHGlobal(NULL, true, &pStream);

        CLSID pngClsid;
        GetEncoderClsid(L"image/png", &pngClsid);
        Gdiplus::Status status = bmpImage->Save(pStream, &pngClsid, NULL);

        if (!pStream) {
            ASSERT(bmpImage);
            return S_FALSE;
        }
        STATSTG statstg;
        pStream->Stat(&statstg, STATFLAG_DEFAULT);
        UINT nSize = (UINT)statstg.cbSize.LowPart;
        LPBYTE lpBuffer = (LPBYTE)malloc(nSize);
        LARGE_INTEGER li = {0};
        hr = pStream->Seek(li, STREAM_SEEK_SET, NULL);

        if (hr == S_OK) {
            ULONG nRead;
            hr = pStream->Read((void *)lpBuffer, nSize, &nRead);

            if (hr == S_OK) {
                int iNewLength;
                LPSTR lpNewBuffer = CBase64::Encode(nSize, lpBuffer, iNewLength);
                wordOutput.Append((BYTE *)lpNewBuffer, iNewLength);

                delete lpNewBuffer;       
            }
        }
        delete lpBuffer;
        pStream->Release();     
    }

    return hr;
}

HRESULT CWordXml::GenerateImages(CFileOutput &wordOutput, int &iImageId, int &iShapeId, DrawSdk::Image *pImage, int iImageType, CRect rcClickBounds, CString csClickNumber, CString csCalloutText , CPoint ptClick) {
    HRESULT hr = S_OK;

    CString csImageBlock1;
    GetCStringFromResource(IDR_WORD_XML_IMAGEBLOCK1, csImageBlock1);
    if (!rcClickBounds.IsRectEmpty() && iImageType != IMAGE_CLICK_AREA_SNAPSHOT) {
        Replace(csImageBlock1, _T("%KEEPNEXT%"), _T("<w:keepNext/>"));
    } else {
        Replace(csImageBlock1, _T("%KEEPNEXT%"), _T(""));
    }
    CString csImageBlock2;
    GetCStringFromResource(IDR_WORD_XML_IMAGEBLOCK2, csImageBlock2);
    if (iImageType == IMAGE_CLICK_AREA_SNAPSHOT) {
        Replace(csImageBlock2, _T("%BORDERTYPE%"), _T("none"));
        
        Replace(csImageBlock1, _T("<w:p>"), _T(""));
        Replace(csImageBlock2, _T("</w:p>"), _T(""));
    } else {
        Replace(csImageBlock2, _T("%BORDERTYPE%"), _T("single"));
    }

    CString csShapeType;
    GetCStringFromResource(IDR_WORD_XML_SHAPETYPE, csShapeType);

    CString csCurrentImage = csImageBlock1;

    Replace(csCurrentImage, _T("%SHAPETYPE%"), csShapeType);

    CString csImageType;
    csImageType.Format(_T("03%06d.png"), iImageId);
    Replace(csCurrentImage, _T("%IMAGETYPE%"), csImageType);

    wordOutput.Append(csCurrentImage);

    // Write binary image data
    CString csImageName = pImage->GetImageName();

    Gdiplus::Bitmap *bmpClick = NULL;
    Gdiplus::Bitmap *bmpSmallSnapshot = NULL;

    // Compute the area that will be cut from original image. May be subject to change.
    Gdiplus::Rect rect(rcClickBounds.left < 0 ? 0 : rcClickBounds.left
        , rcClickBounds.top < 0 ? 0 : rcClickBounds.top
        , min(rcClickBounds.Width() + (rcClickBounds.left < 0 ? rcClickBounds.left: 0)
                , pImage->GetWidth() - (rcClickBounds.left < 0 ? 0 : rcClickBounds.left) -1)
        , min(rcClickBounds.Height() + (rcClickBounds.top < 0 ? rcClickBounds.top : 0)
                , pImage->GetHeight() - (rcClickBounds.top < 0 ? 0 : rcClickBounds.top) -1) 
        );

    //Click visualization
    switch (iImageType) {
        case IMAGE_SCREENSHOT:
            GenerateImageData(NULL, wordOutput, true, csImageName);
            break;
        case IMAGE_CLICK_VISUALIZATION: {
            bmpClick = Gdiplus::Bitmap::FromFile(csImageName);
            Gdiplus::Graphics graphics(bmpClick);
            graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
            Gdiplus::Pen redPen(Gdiplus::Color(255, 255, 0, 0), 2.0f);
            Gdiplus::Color clrYellow(255, 253, 243, 141);
            Gdiplus::SolidBrush yellowBrush(clrYellow);
            Gdiplus::SolidBrush blackBrush(Gdiplus::Color(0, 0, 0));

            // draw red rectangle
            graphics.DrawRectangle(&redPen, rect);

            // calculate the width of the ellipse
            Gdiplus::Font fontText(_T("Arial"), 14.0, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
            LOGFONT lf;
            fontText.GetLogFontW(&graphics, &lf);

            double dTextWidth = DrawSdk::Text::GetTextWidth(csClickNumber, csClickNumber.GetLength(), &lf);
            Gdiplus::RectF rectEllipse(0.0, 0.0, 0.0, 0.0);
            rectEllipse.Width = dTextWidth + 12;
            rectEllipse.Height = 20;

            CRect rcImage;
            rcImage.SetRect(0, 0, bmpClick->GetWidth(), bmpClick->GetHeight());
            CRect rcClick;
            rcClick.SetRect(rect.X, rect.Y, rect.X + rect.Width, rect.Y + rect.Height);
            Gdiplus::PointF p1, p2;
            CalculateClickNumberPosition(rcImage, rcClick, rectEllipse, p1, p2);

            // draw connection red line
            graphics.DrawLine(&redPen, p1, p2);
            // draw yellow ellipse
            Gdiplus::GraphicsPath path;

            Gdiplus::RectF rectLeft;
            Gdiplus::RectF rectRight;

            rectLeft.X = rectEllipse.X;
            rectLeft.Y = rectEllipse.Y;
            rectLeft.Width = rectEllipse.Height;
            rectLeft.Height = rectEllipse.Height;

            rectRight.X = rectEllipse.X + rectEllipse.Width - rectEllipse.Height;
            rectRight.Y = rectEllipse.Y;
            rectRight.Width = rectEllipse.Height;
            rectRight.Height = rectEllipse.Height;

            path.AddArc(rectLeft, 90, 180);
            //if (rectEllipse.Height < rectEllipse.Width)
            path.AddLine(rectLeft.X + rectLeft.Width / 2, rectLeft.Y, rectRight.X + rectRight.Width / 2, rectRight.Y);
            path.AddArc(rectRight, 90, -180);
            path.AddLine(rectRight.X + rectRight.Width / 2, rectRight.Y + rectRight.Height, rectLeft.X + rectLeft.Width / 2, rectLeft.Y + rectLeft.Height);
            path.CloseFigure();

            /*graphics.FillEllipse(&yellowBrush, rectEllipse);
            graphics.DrawEllipse(&redPen, rectEllipse);*/
            graphics.DrawPath(&redPen, &path);
            graphics.FillPath(&yellowBrush, &path);
            
            Gdiplus::PointF ptCenterPoint;
            ptCenterPoint.X = (Gdiplus::REAL) rectEllipse.X + (rectEllipse.Width - dTextWidth) / 2 - 1;
            ptCenterPoint.Y = (Gdiplus::REAL) rectEllipse.Y + (rectEllipse.Height - fontText.GetHeight(&graphics))/2 + 1;
            graphics.DrawString(csClickNumber, csClickNumber.GetLength(), &fontText, ptCenterPoint, &blackBrush);
            
            // draw click visualization final image
            hr = GenerateImageData(bmpClick, wordOutput, false, csImageName);
            break;
        }
        case IMAGE_CLICK_AREA_SNAPSHOT: {
            bmpClick = Gdiplus::Bitmap::FromFile(csImageName);

            CFont* pFont = new CFont();
            pFont->CreateFontIndirect(&m_logFont);
            CWindowDC dc(NULL);
            int iTextWidth = dc.GetTextExtent(csCalloutText, csCalloutText.GetLength()).cx + 40;

            while (iTextWidth > 643 - 16) {
                iTextWidth = iTextWidth - 643 - 16;
            }
            int iImageWidth = 643 - 16 - iTextWidth;
            if (rect.Width > iImageWidth) {
                if (iImageWidth >= 75) {
                    int width = iImageWidth;
                    int x = ptClick.x - width/2;
                    if (rect.X <= x && rect.GetRight() >= (x + width)) {
                        rect.X = x;
                    } else if (rect.GetRight() < (x + width)) {
                        rect.X = rect.GetRight() - width;
                    }
                    rect.Width = width;
                } else {
                    if (rect.Width > 643 - 16) {
                        rect.Width = 643 - 16;
                    }
                }
            }
            if (rect.Height > 75) {
                int height = 75;
                int y = ptClick.y - height/2;
                if (rect.Y <= y && rect.GetBottom() >= (y + height)) {
                    rect.Y = y;
                } else if (rect.GetBottom() < (y + height)) {
                    rect.Y = rect.GetBottom() - height;
                }
                rect.Height = height;
            }
            bmpSmallSnapshot = bmpClick->Clone(rect, bmpClick->GetPixelFormat());
            if (bmpSmallSnapshot == NULL) {
                ASSERT(false); // This should not occur. If this occur, identify the problem and fix computing algorithm.
                // As back-up use full image without any cuts.
                bmpSmallSnapshot = bmpClick;
            }

            Gdiplus::Graphics gr(bmpSmallSnapshot);
            gr.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
            Gdiplus::Pen blackPen(Gdiplus::Color(0, 0, 0), 1.0);
            gr.DrawRectangle(&blackPen, Gdiplus::RectF(0, 0, bmpSmallSnapshot->GetWidth() - 1, bmpSmallSnapshot->GetHeight() -1 ) );
            
            hr = GenerateImageData(bmpSmallSnapshot, wordOutput, false, csImageName);
            break;
        }
        default:
            ASSERT(false); // no other mode defined other that those 3.
            break;
    }

    csCurrentImage = csImageBlock2;

    CString csShapeId;
    csShapeId.Format(_T("_x0000_i%d"), iShapeId);
    Replace(csCurrentImage, _T("%SHAPEID%"), csShapeId);

    // calculate maximal width and height
    // standard word zoom factor of images
    double dMaxSlideWidth = SLIDE_WIDTH;
    double dMaxSlideHeight = SLIDE_HEIGHT;
    bool bZoomChanged = false;
    double dZoomX = 0.7;
    double dZoomY = 0.7;
    double dImageWidth;
    double dImageHeight;
    if (iImageType == IMAGE_CLICK_AREA_SNAPSHOT && bmpSmallSnapshot != NULL) {
        dImageWidth =  bmpSmallSnapshot->GetWidth() * dZoomX;
        dImageHeight =  bmpSmallSnapshot->GetHeight() * dZoomY;
    } else {
        dImageWidth =  pImage->GetWidth() * dZoomX;
        dImageHeight =  pImage->GetHeight() * dZoomY;
    }

    if (iImageType != IMAGE_CLICK_AREA_SNAPSHOT) {
        if (dImageWidth > dMaxSlideWidth) {
            dZoomX = dMaxSlideWidth / pImage->GetWidth();
            bZoomChanged = true;
        }
        if (dImageHeight > dMaxSlideHeight) {
            dZoomY = dMaxSlideHeight / pImage->GetHeight();
            bZoomChanged = true;
        }

        if (bZoomChanged) {
            if (dZoomX < dZoomY) {
                dImageWidth =  pImage->GetWidth() * dZoomX;
                dImageHeight =  pImage->GetHeight() * dZoomX;
            } else {
                dImageWidth =  pImage->GetWidth() * dZoomY;
                dImageHeight =  pImage->GetHeight() * dZoomY;
            }
        } 
        if (m_pManualConfiguration->SlidesNumberPerPage > 1) {
            double dImageHeightScaled = ((SLIDE_HEIGHT - HEADER_HEIGHT - FOOTER_HEIGHT)/m_pManualConfiguration->SlidesNumberPerPage) - (CHAPTER_TEXT_HEIGHT + CALLOUT_TEXT_HEIGHT + SNAPSHOT_HEIGHT);
            if (dImageHeight > dImageHeightScaled) {
                dZoomY = dImageHeightScaled / dImageHeight;
                dImageHeightScaled = dImageHeight * dZoomY;
            }
            dImageWidth = (dImageHeightScaled * dImageWidth)/dImageHeight;
            dImageHeight = dImageHeightScaled;
        }
    }

    CString csImageWidth;
    csImageWidth.Format(_T("%2.2f"), dImageWidth);
    Replace(csCurrentImage, _T("%WIDTH%"), csImageWidth);

    CString csImageHeight;
    csImageHeight.Format(_T("%2.2f"), dImageHeight);
    Replace(csCurrentImage, _T("%HEIGHT%"), csImageHeight);

    // image type used above
    Replace(csCurrentImage, _T("%IMAGETYPE%"), csImageType);

    CString csImagePrefix = pImage->GetImageName();
    StringManipulation::GetFilePrefix(csImagePrefix);
    Replace(csCurrentImage, _T("%IMAGENAME%"), csImagePrefix);

    wordOutput.Append(csCurrentImage);

    ++iShapeId;
    ++iImageId;

    return hr;
}

void CWordXml::CalculateClickNumberPosition(CRect rcScreen, CRect rcClick, Gdiplus::RectF& rcEllipse, Gdiplus::PointF &p1, Gdiplus::PointF &p2) {
    int iLineLength = 20;
    int iEllipseH = rcEllipse.Height;
    int iEllipseW = rcEllipse.Width;

    if (rcScreen.right > rcClick.right + iLineLength + iEllipseW) {
        p1.X = rcClick.right;
        p1.Y = rcClick.top + rcClick.Height() / 2;
        p2.X = p1.X + iLineLength;
        p2.Y = p1.Y;
        rcEllipse.X = p2.X;
        rcEllipse.Y = p2.Y - iEllipseH /2;
        rcEllipse.Width = iEllipseW;
        rcEllipse.Height = iEllipseH;
    } else if (rcScreen.left < rcClick.left - iLineLength - iEllipseW) {
        p1.X = rcClick.left - iLineLength;
        p1.Y = rcClick.top + rcClick.Height() / 2;
        p2.X = rcClick.left;
        p2.Y = p1.Y;
        rcEllipse.X = p1.X - iEllipseW;
        rcEllipse.Y = p1.Y - iEllipseH /2;
        rcEllipse.Width = iEllipseW;
        rcEllipse.Height = iEllipseH;
    } else if (rcScreen.top < rcClick.top - iLineLength - iEllipseH) {
        p1.X = rcClick.left + rcClick.Width() / 2;
        p1.Y = rcClick.top - iLineLength;
        p2.X = p1.X;
        p2.Y = rcClick.top;
        rcEllipse.X = p1.X - iEllipseW / 2;
        rcEllipse.Y = p1.Y - iEllipseH;
        rcEllipse.Width = iEllipseW;
        rcEllipse.Height = iEllipseH;
    } else if (rcScreen.bottom > rcClick.bottom + iLineLength + iEllipseH) {
        p1.X = rcClick.left + rcClick.Width() / 2;
        p1.Y = rcClick.bottom;
        p2.X = p1.X;
        p2.Y = p1.Y + iLineLength;
        rcEllipse.X = p1.X - iEllipseW / 2;
        rcEllipse.Y = p2.Y;
        rcEllipse.Width = iEllipseW;
        rcEllipse.Height = iEllipseH;
    }
}

HRESULT CWordXml::GenerateThumbnail(CString csFilename, CArray<CWordSlideObject *, CWordSlideObject *> &aSlide, CMetaInformation &metainfo) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;    
    if (aSlide.GetCount() <= 0)
        return S_OK;

    CSize aSizes [] = {CSize(395, 295), CSize(285, 215), CSize(157, 118)}; 
    CArray<Gdiplus::Image*, Gdiplus::Image*>aScreenImages;
    CArray<CString, CString>aCalloutTexts;
    CArray<CString, CString>aChapterTitles;


    int iNumOfSlidesOnPage = m_pManualConfiguration->SlidesNumberPerPage;
    //Get informations from slides
    for (int i = 0; i < aSlide.GetCount() && i < iNumOfSlidesOnPage; i++) {
        aChapterTitles.Add(aSlide.GetAt(i)->GetChapterTitle());

        CString csImageName = aSlide.GetAt(i)->GetImage()->GetImageName();
        Gdiplus::Image* pOrigImage = Gdiplus::Image::FromFile(csImageName);
        aScreenImages.Add(pOrigImage);

        CString csText = _T("");
        CArray<DrawSdk::Text*, DrawSdk::Text*>aTexts;
        aSlide.GetAt(i)->GetTexts(aTexts);
        for (int j = 0; j < aTexts.GetCount(); j ++) {
            DrawSdk::Text *pText = aTexts[j];
            if (pText != NULL) {
                csText.AppendFormat(_T("%s"), pText->GetString());
            }
        }
        aCalloutTexts.Add(csText);
    }

    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);

    Gdiplus::Image* pBlankPage = ImageProcessor::LoadImage(IDB_WORD_BLANK_PAGE, _T("PNG"), ::AfxGetResourceHandle());

    Gdiplus::Graphics *graphics = Gdiplus::Graphics::FromImage(pBlankPage);

    Gdiplus::SolidBrush brushChapter(Gdiplus::Color(0,0,0));
    Gdiplus::Font fontChapter(_T("Arial"), 10.0, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush brushText(Gdiplus::Color(0,0,0));
    Gdiplus::Font fontText(_T("Arial"), 8.0, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

    int iVOffset = aSizes[iNumOfSlidesOnPage - 1].cy + 50;
    int iHOffset = 62;
    int iChapterVPos = 77;
    int iPictVPos = 101;
    int iTextVPos = iPictVPos + aSizes[iNumOfSlidesOnPage - 1].cy + 5;
  

    for (int i = 0; i < aScreenImages.GetCount(); i++) {
        //Draw Chapter Title
        Gdiplus::PointF ptChapter((Gdiplus::REAL) iHOffset, (Gdiplus::REAL) iChapterVPos);
        graphics->DrawString(aChapterTitles.GetAt(i), aChapterTitles.GetAt(i).GetLength(), &fontChapter, ptChapter, &brushChapter);
        
        //Draw recorded page
        Gdiplus::PointF ptImage((Gdiplus::REAL) iHOffset, (Gdiplus::REAL) iPictVPos);
        Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0), 1.0f);
        
        int iNw, iNh;
        Calculator::FitRectInRect(aSizes[iNumOfSlidesOnPage - 1].cx, aSizes[iNumOfSlidesOnPage - 1].cy, aScreenImages.GetAt(i)->GetWidth() , aScreenImages.GetAt(i)->GetHeight(), &iNw, &iNh);
        Gdiplus::Image* pScaledImage = aScreenImages.GetAt(i)->GetThumbnailImage(/*aSizes[iNumOfSlidesOnPage - 1].cx, aSizes[iNumOfSlidesOnPage - 1].cy*/iNw, iNh);
   
        Gdiplus::RectF rectImage(ptImage.X, ptImage.Y, iNw, iNh);
        graphics->DrawImage(pScaledImage, rectImage);
        graphics->DrawRectangle(&pen, rectImage);//iHOffset,iPictVPos, aSizes[iNumOfSlidesOnPage - 1].cx, aSizes[iNumOfSlidesOnPage -1].cy);

        //Draw callout text
        Gdiplus::PointF ptText((Gdiplus::REAL) iHOffset, (Gdiplus::REAL) iTextVPos);
        graphics->DrawString(aCalloutTexts.GetAt(i), aCalloutTexts.GetAt(i).GetLength(), &fontText, ptText, &brushText);

        iChapterVPos += iVOffset;
        iPictVPos += iVOffset;
        iTextVPos += iVOffset;
    }
    if (m_pManualConfiguration->ShowHeader) {
        Gdiplus::Image* pHeader = ImageProcessor::LoadImage(IDB_WORD_HEADER, _T("PNG"), ::AfxGetResourceHandle());
        graphics->DrawImage(pHeader, Gdiplus::PointF(0.0, 0.0));
        if (aChapterTitles.GetCount() > 0) {
            Gdiplus::PointF ptHeaderText(62.0, 27.0);
            Gdiplus::SolidBrush brushHeader(Gdiplus::Color(70,70,70));
            Gdiplus::Font fontHeader(_T("Arial"), 8.0, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
            graphics->DrawString(aChapterTitles.GetAt(0), aChapterTitles.GetAt(0).GetLength(), &fontHeader, ptHeaderText, &brushHeader);
            delete pHeader;
        }
    }
    
    if (m_pManualConfiguration->ShowFooter) {
        Gdiplus::Image* pFooter = ImageProcessor::LoadImage(IDB_WORD_FOOTER, _T("PNG"), ::AfxGetResourceHandle());
        UINT iFooterHeight = pFooter->GetHeight();
        UINT iPageHeight = pBlankPage->GetHeight();
        graphics->DrawImage(pFooter, Gdiplus::PointF(0.0, (Gdiplus::REAL)(iPageHeight - iFooterHeight)));
        
        Gdiplus::PointF ptAuthorText(62.0, 709.0);
        Gdiplus::PointF ptDateText(184.0, 709.0);
        Gdiplus::SolidBrush brushFooter(Gdiplus::Color(70,70,70));
        Gdiplus::Font fontFooter(_T("Arial"), 6.0, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        CString csAuthorTitle;
        csAuthorTitle.LoadString(IDS_AUTHOR);
        CString csAuthorValue(metainfo.GetAuthor());
        CString csAuthor = csAuthorTitle + _T(": ") + csAuthorValue;
        graphics->DrawString(csAuthor, csAuthor.GetLength(), &fontFooter, ptAuthorText, &brushFooter);

        CString csDateTitle;
        csDateTitle.LoadString(IDS_DATE);
        CString csDateFormat;
        csDateFormat.LoadString(IDS_DATE_FORMAT);
        CTime time = CTime::GetCurrentTime();
        CString csDateValue = time.Format(csDateFormat);
        CString csDate = csDateTitle + _T(": ") + csDateValue;
        graphics->DrawString(csDate, csDate.GetLength(), &fontFooter, ptDateText, &brushFooter);
        delete pFooter;
    }
    Gdiplus::Pen pen(Gdiplus::Color(255,0,0), 1.0);

    CString csFilePrefix = csFilename;
    StringManipulation::GetPathFilePrefix(csFilePrefix);
    csFilePrefix += _T(".png");
    
    
    IStream *pStream;
    ::CreateStreamOnHGlobal(NULL, true, &pStream);
    pBlankPage->Save(pStream, &pngClsid, NULL);
    delete graphics;
    Gdiplus::Image* pNewImage = Gdiplus::Image::FromStream(pStream);
    int iNewWidth, iNewHeight;
    
    Calculator::FitRectInRect(150, 150, pBlankPage->GetWidth(), pBlankPage->GetHeight(), &iNewWidth, &iNewHeight);
    Gdiplus::Image* pThumb = pNewImage->GetThumbnailImage(iNewWidth, iNewHeight);
    pThumb->Save(csFilePrefix, &pngClsid, NULL);

    pStream->Release();
    delete pBlankPage;
    
    return hr;
}

HRESULT CWordXml::WriteDocumentContent(CFileOutput &wordOutput, CArray<CWordSlideObject *, CWordSlideObject *> &aSlide, CMetaInformation &metainfo) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HRESULT hr = S_OK;

    int iClickNumber = 0;
    bool bFirstPage = true;
    int iShapeId = 1025;
    int iImageId = 1;
    CString csLine;

    csLine = _T("	<w:body>\n");
    wordOutput.Append(csLine, csLine.GetLength());

    if (m_pManualConfiguration->ShowCoverSheet && SUCCEEDED(hr)) {
        hr = WriteCoverSheet(wordOutput, metainfo);
    }

    csLine = _T("\t<wx:sect>\n\t<w:titlePg/>\n");
    wordOutput.Append(csLine, csLine.GetLength());

    // Write Table Of Content
    if (m_pManualConfiguration->ShowTableOfContent && SUCCEEDED(hr)) {
        hr = WriteTableOfContent(wordOutput, aSlide, metainfo);
    }

    for (int i = 0; i < aSlide.GetCount(); ++i) {
        if (aSlide[i] != NULL) {
            DrawSdk::Image *pImage = aSlide[i]->GetImage();
            if (pImage != NULL) {
                CString csChapterTitle;
                GetCStringFromResource(IDR_WORD_XML_CHAPTER, csChapterTitle);

                // TOCID and AMLID Are required for table of content. This information is replaced and exists in chapter, no matter if table of content is made visible by user.
                CString csTocId = _T("_TOC");
                csTocId.AppendFormat(_T("%d"), i+1);
                Replace(csChapterTitle, _T("%TOCID%"), csTocId);
                CString csAmlId;
                csAmlId.Format(_T("%d"), i);
                Replace(csChapterTitle, _T("%AMLID%"), csAmlId);

                if (i == 0) { // for first chapter add page break (if table of content is visible)
                    Replace(csChapterTitle, _T("%PAGEBREAK%"), m_pManualConfiguration->ShowTableOfContent ? _T("<w:br w:type=\"page\"/>") : _T(""));
                }

                CString csPage = _T("");
                csPage.LoadString(IDS_DEFAULT_PAGE_TITLE);
                StringManipulation::EncodeXmlSpecialCharacters(csPage);
                csPage.AppendFormat(_T(" %d"), i+1);
                Replace(csChapterTitle, _T("%TITLE%"), csPage);
                wordOutput.Append(csChapterTitle, csChapterTitle.GetLength());

                CRect rcBounds = aSlide[i]->GetClickObjectBounds();
                if (!rcBounds.IsRectEmpty()) {
                    iClickNumber ++;
                    CString csClickNumber;
                    csClickNumber.Format(_T("%d"), iClickNumber);
                    GenerateImages(wordOutput, iImageId, iShapeId, pImage, IMAGE_CLICK_VISUALIZATION, rcBounds, csClickNumber);
                    
                    CArray<DrawSdk::Text *, DrawSdk::Text *> aTexts;
                    aSlide[i]->GetTexts(aTexts);
                    CString csCalloutText = _T("(") + csClickNumber + _T(") ");
                    for (int j = 0; j < aTexts.GetCount(); ++j) {
                        DrawSdk::Text *pText = aTexts[j];
                        if (pText != NULL) {
                            csCalloutText.AppendFormat(_T("%s"), pText->GetString());
                        }
                    }
                    csCalloutText.Append(_T("   "));
                    CString csOriginalText = csCalloutText;
                    StringManipulation::EncodeXmlSpecialCharacters(csCalloutText);
                    CString csText;
                    GetCStringFromResource(IDR_WORD_XML_CALLOUTTEXT, csText);
                    Replace(csText, _T("%TEXT%"), csCalloutText);
                    wordOutput.Append(csText, csText.GetLength());

                    GenerateImages(wordOutput, iImageId, iShapeId, pImage, IMAGE_CLICK_AREA_SNAPSHOT, rcBounds, _T(""), csOriginalText, aSlide[i]->GetClickPoint());
                    
                    csLine = _T("</w:p>");
                    wordOutput.Append(csLine, csLine.GetLength());
                } else {
                    GenerateImages(wordOutput, iImageId, iShapeId, pImage, IMAGE_SCREENSHOT, rcBounds);
                }      
            }
        }   
    } 

    //Begin doc section
    csLine = _T("		<w:sectPr>\n");
    wordOutput.Append(csLine, csLine.GetLength());
    
    if (m_pManualConfiguration->ShowHeader) {
        CString csHeader;
        GetCStringFromResource(IDR_WORD_XML_HEADER, csHeader);
        wordOutput.Append(csHeader, csHeader.GetLength());
    }

    if (m_pManualConfiguration->ShowFooter) {
        CString csFooter;
        GetCStringFromResource(IDR_WORD_XML_FOOTER, csFooter);
        ReplaceAuthorAndDate(csFooter, metainfo);
        wordOutput.Append(csFooter, csFooter.GetLength());
    }

    //doc section content
    CString csSectContent;
    GetCStringFromResource(IDR_WORD_XML_DOCUMENT_SECTION, csSectContent);
    wordOutput.Append(csSectContent, csSectContent.GetLength());

    csLine = _T("		</w:sectPr>\n");
    wordOutput.Append(csLine, csLine.GetLength());
    //End doc section

    csLine = _T("		</wx:sect>\n");
    wordOutput.Append(csLine, csLine.GetLength());
    csLine = _T("	</w:body>\n");
    wordOutput.Append(csLine, csLine.GetLength());

    return hr;
}

void CWordXml::ReplaceAuthorAndDate(CString& csDest, CMetaInformation &metainfo) {
    CString csFooterInfo;
    csFooterInfo.LoadString(IDS_AUTHOR);
    StringManipulation::EncodeXmlSpecialCharacters(csFooterInfo);
    Replace(csDest, _T("%AUTHOR%"), csFooterInfo);

    CString csAuthor(metainfo.GetAuthor());
    StringManipulation::EncodeXmlSpecialCharacters(csAuthor);
    Replace(csDest, _T("%AUTHOR_NAME%"), csAuthor);

    CTime time = CTime::GetCurrentTime();
    CString csDateFormat;
    csDateFormat.LoadString(IDS_DATE_FORMAT);
    csFooterInfo = time.Format(csDateFormat);// ("%d.%m.%Y");
    Replace(csDest, _T("%DATE_VALUE%"), csFooterInfo);
    csFooterInfo.LoadString(IDS_DATE); 
    Replace(csDest, _T("%DATE%"), csFooterInfo);
}

int CWordXml::GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if(size == 0) {
        return -1;  // Failure
    }
    pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if(pImageCodecInfo == NULL) {
        return -1;  // Failure
    }
    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for(UINT j = 0; j < num; ++j) {
        if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 ) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }    
    }
    free(pImageCodecInfo);
    return -1;  // Failure
}

HRESULT CWordXml::WriteTableOfContent(CFileOutput &wordOutput, CArray<CWordSlideObject *, CWordSlideObject *> &aSlide, CMetaInformation &metainfo) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HRESULT hr = S_OK;

    CString csTableOfContent;
    GetCStringFromResource(IDR_WORD_XML_TABLE_OF_CONTENT_TITLE, csTableOfContent);
    CString csTableOfContentsStr;
    csTableOfContentsStr.LoadString(IDS_TABLE_OF_CONTENTS);
    Replace(csTableOfContent, _T("%TABLEOFCONTENT%"), csTableOfContentsStr);

    CString csTOCItemTemplate;
    GetCStringFromResource(IDR_WORD_XML_TABLE_OF_CONTENT, csTOCItemTemplate);

    for (int i = 0; i < aSlide.GetCount(); ++i) {
        if (aSlide[i] != NULL) {
            CString csTOCItemStyle;
            GetCStringFromResource(IDR_WORD_XML_TABLE_OF_CONTENT_ITEM_STYLE, csTOCItemStyle);
            csTableOfContent.Append(csTOCItemStyle, csTOCItemStyle.GetLength());

            if (i == 0) {// for first item
                CString csTOCBegin;
                GetCStringFromResource(IDR_WORD_XML_TABLE_OF_CONTENT_BEGIN, csTOCBegin);
                csTableOfContent.Append(csTOCBegin, csTOCBegin.GetLength());
            }

            CString csTOCItem = csTOCItemTemplate;

            CString csTocId = _T("_TOC");
            csTocId.AppendFormat(_T("%d"), i+1);
            Replace(csTOCItem, _T("%TOCID%"), csTocId);

            CString csPage = _T("");
            csPage.LoadString(IDS_DEFAULT_PAGE_TITLE);
            StringManipulation::EncodeXmlSpecialCharacters(csPage);
            csPage.AppendFormat(_T(" %d"), i+1);
            Replace(csTOCItem, _T("%TITLE%"), csPage);

            CString csPageNo;
            csPageNo.Format(_T("%d"), ((int)i / m_pManualConfiguration->SlidesNumberPerPage) + 2 + (m_pManualConfiguration->ShowCoverSheet? 1 : 0));
            Replace(csTOCItem, _T("%PAGENO%"), csPageNo);

            csTableOfContent.Append(csTOCItem, csTOCItem.GetLength());
        }
    }

    wordOutput.Append(csTableOfContent, csTableOfContent.GetLength());
    return hr;
}

HRESULT CWordXml::WriteCoverSheet(CFileOutput &wordOutput, CMetaInformation &metainfo) {
    HRESULT hr = S_OK;
    CString csCoverSheet;
    hr = GetCStringFromResource(IDR_WORD_XML_COVERSHEET, csCoverSheet);

    if (m_pManualConfiguration->ShowHeader) {
        CString csCoverSheetHeader;
        hr = GetCStringFromResource(IDR_WORD_XML_COVERSHEET_HEADER, csCoverSheetHeader);
        Replace(csCoverSheet, _T("%COVERSHEET_HEADER%"), csCoverSheetHeader);
    }

    CString csCoverTitle = metainfo.GetTitle();
    if (!m_csDocumentTitle.IsEmpty() && (csCoverTitle.IsEmpty() || csCoverTitle == _T("recording")))
        csCoverTitle = m_csDocumentTitle;

    StringManipulation::EncodeXmlSpecialCharacters(csCoverTitle);
    Replace(csCoverSheet, _T("%TITLE%"), csCoverTitle);

    ReplaceAuthorAndDate(csCoverSheet, metainfo);

    hr = wordOutput.Append(csCoverSheet, csCoverSheet.GetLength());
    return hr;
}