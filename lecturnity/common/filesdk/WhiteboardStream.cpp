#include "StdAfx.h"

#include <algorithm>

#include "WhiteboardStream.h"
#include "InteractionArea.h"
#include "MiscFunctions.h"
#include "SgmlParser.h"

/*********************************************
 *********************************************/

CWhiteboardEvent::CWhiteboardEvent(CWhiteboardStream *pWb) 
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_pWb = pWb;

   Init();
}

CWhiteboardEvent::~CWhiteboardEvent() 
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();
}

void CWhiteboardEvent::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // initialize integer entries with -1
   m_iTimestamp  = -1;
   m_iBgColor    = -1;
   m_iPageNumber = -1;
   m_aSortedIds  = NULL;

}

void CWhiteboardEvent::InitEmpty()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   
   m_iTimestamp  = 0;
   m_iBgColor    = 7;
   m_iPageNumber = 0;
}

void CWhiteboardEvent::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iTimestamp  = -1;
   m_iBgColor    = -1;
   m_iPageNumber = -1;

   // clear data structure, the array contains only pointers 
   // to the objects stored in WhiteboardStream
   m_objects.RemoveAll();

   if (m_aSortedIds)
      delete[] m_aSortedIds;
   m_aSortedIds = NULL;
}

// Extracts information from a eventqueue line
// void CWhiteboardEvent::Read(CString &eventLine, CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> &objectArray)
/*
void CWhiteboardEvent::Read(LPCTSTR eventLine, CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> &objectArray)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // find first entry (time stamp)
   int position;
   CString line = eventLine;
   if ((position = line.Find(" ", 0)) == -1)
      return;
   CString szValue = line.Left(position);
   m_iTimestamp = atoi(szValue);
   line = line.Right(line.GetLength()-position-1);

   // find second entry (page number)
   if ((position = line.Find(" ", 0)) == -1)
      return;
   szValue = line.Left(position);
   m_iPageNumber = atoi(szValue);
   line = line.Right(line.GetLength()-position-1);

   // find third entry (page background)
   if ((position = line.Find(" ", 0)) == -1)
      return;
   szValue = line.Left(position);
   m_iBgColor = atoi(szValue);
   line = line.Right(line.GetLength()-position-1);

   // find fourth entry (number of objects)
   m_iObjectCount = 0;
   if ((position = line.Find(" ", 0)) == -1)
      return;
   szValue = line.Left(position);
   m_iObjectCount = atoi(szValue);
   line = line.Right(line.GetLength()-position-1);
   
   // read object number and find object belong to it
   for (int i = 0; 0 < m_iObjectCount; ++i)
   {
      if ((position = line.Find(" ", 0)) != -1)
      {
         szValue = line.Left(position);
         int arrayID = atoi(szValue);
         if (arrayID < objectArray.GetSize())
         {
            m_objects.Add(objectArray[arrayID]);
         }
         line = line.Right(line.GetLength()-position-1); 
      }
      else
         break;
   }

   // read last entry in line (not followed by blank)
   if (!line.IsEmpty() && (i < m_iObjectCount))
   {
      szValue = line;
      int arrayID = atoi(szValue);
      if (arrayID < objectArray.GetSize())
      {
         m_objects.Add(objectArray[arrayID]);
      }
      line = line.Right(line.GetLength()-position-1);
   }

   {
      int nSize = m_objects.GetSize();
      m_aSortedIds = new int[nSize];
      for (int i=0; i<nSize; ++i)
         m_aSortedIds[i] = ((DrawSdk::DrawObject *) m_objects.GetAt(i))->GetObjectNumber();
      std::sort(m_aSortedIds, m_aSortedIds + nSize);
   }
}
*/

void CWhiteboardEvent::AddObjects(CPtrArray &objects) 
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
 
   m_objects.Copy(objects);
}

// adds the specified element from the specified array to the end of this event's object array
void CWhiteboardEvent::AddObject(int objectNumber, CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> &objectArray)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
 
   if (objectNumber < objectArray.GetSize())
   {
      m_objects.Add(objectArray[objectNumber]);
   }
}

// sets the specified object at the specified postion of this event's object array; position must be valid
void CWhiteboardEvent::SetObject(int iObjectNumber, DrawSdk::DrawObject *pObject)
{
   m_objects.SetAt(iObjectNumber, pObject);
}

// PZI: adds the specified object to the end of this event's object array
void CWhiteboardEvent::AddObject(DrawSdk::DrawObject *pObject, BOOL bAddToWBObjects)
{
   m_objects.Add(pObject);
   // this is used just to free memory of 
   if( bAddToWBObjects == TRUE && m_pWb != NULL )
      m_pWb->m_arrObjects.Add(pObject);
}

// remove telepointer
void CWhiteboardEvent::RemovePointerObjects()
{
   for(int i=0; i<m_objects.GetCount(); i++) {
      DrawSdk::DrawObject *pObject = (DrawSdk::DrawObject *)m_objects.GetAt(i);
	  if (pObject->GetType() == DrawSdk::TELEPOINTER)
         m_objects.RemoveAt(i--);
   }
}
void CWhiteboardEvent::RemoveMousePointerObjects()
{
   for(int i=0; i<m_objects.GetCount(); i++) {
      DrawSdk::DrawObject *pObject = (DrawSdk::DrawObject *)m_objects.GetAt(i);
	  if (pObject->GetType() == DrawSdk::TELEPOINTER){
		  if(((DrawSdk::Pointer *)pObject)->IsMousePointer())
			  m_objects.RemoveAt(i--);
	  }
   }
}
void CWhiteboardEvent::RemoveTelePointerObjects()
{
   for(int i=0; i<m_objects.GetCount(); i++) {
      DrawSdk::DrawObject *pObject = (DrawSdk::DrawObject *)m_objects.GetAt(i);
	  if (pObject->GetType() == DrawSdk::TELEPOINTER){
		  if(!((DrawSdk::Pointer *)pObject)->IsMousePointer())
			  m_objects.RemoveAt(i--);
	  }
   }
}

// PZI: additional argument added to switch off image copy
void CWhiteboardEvent::Write(CFileOutput *pEventOutput, CFileOutput *pObjectOutput,
                   LPCTSTR tszRecordPath, LPCTSTR tszPrefix, int timeOffset,
                   CMap<CString, LPCTSTR, CString, CString&> &imageMap, bool bCopyImages, bool bRemovePointer)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (pEventOutput == NULL || pObjectOutput == NULL)
      return;

   int newTimestamp =  m_iTimestamp + timeOffset;
   if (newTimestamp < timeOffset)
      newTimestamp = timeOffset;

   // event file is raw byte data
   static char szEvent[500];
   szEvent[0] = 0;

   int iObjectCount = m_objects.GetSize();
   int iPointerRemoveCount = 0;

   if (bRemovePointer) {
       for (int i=0; i<iObjectCount; ++i) {
           DrawSdk::DrawObject *object = (DrawSdk::DrawObject *)m_objects[i];

           if (object != NULL && bRemovePointer && object->GetType() == DrawSdk::TELEPOINTER)
               ++iPointerRemoveCount; // Neither write event nor object
       }
   }


   sprintf(szEvent, "%d %d %d %d", newTimestamp, m_iExportPageNumber,
       m_iBgColor, iObjectCount - iPointerRemoveCount);
   pEventOutput->Append((BYTE *)szEvent, strlen(szEvent));

   DrawSdk::CharArray aOutput;

   for (int i = 0; i < iObjectCount; ++i)
   {
      DrawSdk::DrawObject *object = (DrawSdk::DrawObject *)m_objects[i];

      if (object != NULL && bRemovePointer && object->GetType() == DrawSdk::TELEPOINTER)
          continue; // Neither write event nor object

      int objectNumber = object->GetObjectNumber();
      if (objectNumber == -1) // never written
      {
          objectNumber = CWhiteboardStream::m_objectCounter;
          ++CWhiteboardStream::m_objectCounter;
          object->SetObjectNumber(objectNumber);
          if (object->GetType() == DrawSdk::IMAGE)
          {
              CString csExportPrefix = tszRecordPath;
              csExportPrefix += _T("\\");
              csExportPrefix += tszPrefix;

              // copy images to new names if requested
              if(bCopyImages)
                  HRESULT hr = CWhiteboardStream::CopyImage((DrawSdk::Image *)object, csExportPrefix, imageMap, NULL);
              // also changes the image prefix of the image object
          }
          object->Write(&aOutput);

          pObjectOutput->Append(aOutput.GetBuffer(), aOutput.GetLength());
      }

      sprintf(szEvent, " %d", objectNumber);
      pEventOutput->Append((BYTE *)szEvent, strlen(szEvent));

      
      aOutput.Reset();
   }

   sprintf(szEvent, "\n");
   pEventOutput->Append((BYTE *)szEvent, strlen(szEvent));
}

void __ExpandRect(RECT *pRect1, RECT *pRect2, bool &bFirst)
{
   if (bFirst)
   {
      *pRect1 = *pRect2;
      bFirst = false;
   }
   else
   {
      if (pRect1->top > pRect2->top)
         pRect1->top = pRect2->top;
      if (pRect1->left > pRect2->left)
         pRect1->left = pRect2->left;
      if (pRect1->right < pRect2->right)
         pRect1->right = pRect2->right;
      if (pRect1->bottom < pRect2->bottom)
         pRect1->bottom = pRect2->bottom;
   }
}

void CWhiteboardEvent::Draw(CDC *pDC, CRect &drawRect, DrawSdk::ZoomManager *zoom, CWhiteboardEvent *pLastEvent, bool bHidePointer)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // fill background with background color
   if (NULL == pLastEvent)
   {
      CBrush cBrush(AofColor::Get(m_iBgColor));
      pDC->FillRect(drawRect, &cBrush);
   }

   // save actual clip region
   CRect clipRect;
   pDC->GetClipBox(&clipRect);

   // Calculate the clipping region
   CRgn rgn;

   CRect rect;
   if (pLastEvent)
   {
      CRect newClipRect;
      int n1 = 0;
      int n2 = 0;
      int s1 = m_objects.GetSize();
      int s2 = pLastEvent->m_objects.GetSize();
      int *aOtherIds = pLastEvent->m_aSortedIds;

      CRgn tmpRgn;
      bool bFirst = true;

      while (n1 < s1 || n2 < s2)
      {
         if (m_aSortedIds[n1] == aOtherIds[n2])
         {
            n1++;
            n2++;
         }
         else if (n1 < s1 && n2 < s2)
         {
            if (m_aSortedIds[n1] < aOtherIds[n2])
            {
               // The (n1 < s1) check has to be left of the m_aSortedIds[n1] < aOtherIds[n2]
               // check, otherwise a buffer overflow may occur (when n1 == s1).
               while (n1 < s1 && m_aSortedIds[n1] < aOtherIds[n2])
               {
                  // Object at m_aSortedIds[n1] has to be added to clip region
                  
                  m_pWb->m_arrObjects[m_aSortedIds[n1]]->GetScreenBoundingBox(&rect, drawRect.left, drawRect.top, zoom);
                  __ExpandRect(&newClipRect, &rect, bFirst);
                  n1++;
               }
            }
            else if (aOtherIds[n2] < m_aSortedIds[n1])
            {
               // The (n2 < s2) check has to be left of the aOtherIds[n2] < m_aSortedIds[n1]
               // check, otherwise a buffer overflow may occur (when n2 == s2).
               while (n2 < s2 && aOtherIds[n2] < m_aSortedIds[n1])
               {
                  // Object at aOtherIds[n2] has to be added to clip region
                  m_pWb->m_arrObjects[aOtherIds[n2]]->GetScreenBoundingBox(&rect, drawRect.left, drawRect.top, zoom);
                  __ExpandRect(&newClipRect, &rect, bFirst);
                  n2++;
               }
            }
         }

         if (n1 < s1 && n2 >= s2)
         {
            // We have reached end of aOtherIds, add all remaining
            // objects from m_aSortedIds to clip region
            while (n1 < s1)
            {
               // Add object at m_aSortedIds[n1] to clip region
               m_pWb->m_arrObjects[m_aSortedIds[n1]]->GetScreenBoundingBox(&rect, drawRect.left, drawRect.top, zoom);
               __ExpandRect(&newClipRect, &rect, bFirst);
               n1++;
            }
         }
         else if (n2 < s2 && n1 >= s1)
         {
            // We have reached end of m_aSortedIds, add all remaining
            // objects from aOtherIds to clip region
            while (n2 < s2)
            {
               // Add object at aSortedIds[n2] to clip region
               m_pWb->m_arrObjects[aOtherIds[n2]]->GetScreenBoundingBox(&rect, drawRect.left, drawRect.top, zoom);
               __ExpandRect(&newClipRect, &rect, bFirst);
               n2++;
            }
         }
      }

      rgn.CreateRectRgn(newClipRect.left, newClipRect.top, newClipRect.right, newClipRect.bottom);
      pDC->SelectClipRgn(&rgn);
      pDC->GetClipBox(&rect);
      //rgn.DeleteObject();

      CBrush cBrush(AofColor::Get(m_iBgColor));
      pDC->FillRect(drawRect, &cBrush);
   }
   else
   {
      rgn.CreateRectRgn(drawRect.left, drawRect.top, drawRect.right, drawRect.bottom);
      pDC->SelectClipRgn(&rgn);
   }

   //Get HDC fom CDC
   HDC hDC = pDC->m_hDC;
   // draw all objects belonging to event entry
   for (int i = 0; i < m_objects.GetSize(); ++i)
   {
      DrawSdk::DrawObject *object = (DrawSdk::DrawObject *)m_objects.GetAt(i);
      if (object) {
          if (!bHidePointer || object->GetType() != DrawSdk::TELEPOINTER)
              object->Draw(hDC, drawRect.left, drawRect.top, zoom);
      }
   }
   rgn.DeleteObject();

   // Uncomment this code in order to draw the clipping region
   // --->
   /*
   if (pLastEvent)
   {
      CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
      CGdiObject hollow;
      hollow.CreateStockObject(HOLLOW_BRUSH);
      CBrush *pOldBrush = (CBrush *) pDC->SelectObject(&hollow);
      CPen   *pOldPen   = (CPen *)   pDC->SelectObject(&pen);
      pDC->Rectangle(&rect);
      pDC->SelectObject(&pen);
      pDC->SelectObject(pOldBrush);
      pen.DeleteObject();
      hollow.DeleteObject();
   }
   */
   // <---

   // reset clipping region
   rgn.CreateRectRgn(clipRect.left, clipRect.top, clipRect.right, clipRect.bottom);
   pDC->SelectClipRgn(&rgn);

   rgn.DeleteObject();
}

CWhiteboardEvent *CWhiteboardEvent::Copy()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CWhiteboardEvent *retEvent = new CWhiteboardEvent(m_pWb);

   retEvent->SetTimestamp(m_iTimestamp);
   retEvent->SetPageNumber(m_iPageNumber);
   retEvent->SetExportPageNumber(m_iExportPageNumber);
   retEvent->SetBgColor(m_iBgColor);
   retEvent->AddObjects(m_objects);

   return retEvent;
}

void CWhiteboardEvent::CreateSortedObjArray()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int nObjs = m_objects.GetSize();
   m_aSortedIds = new int[nObjs];
   for (int i=0; i<nObjs; ++i)
      m_aSortedIds[i] = ((DrawSdk::DrawObject *) m_objects[i])->GetObjectNumber();
   std::sort(m_aSortedIds, m_aSortedIds + nObjs);
}

/*********************************************
 *********************************************/


int CWhiteboardStream::m_objectCounter = 0;
int CWhiteboardStream::m_pageCounter = 0;

DrawSdk::CImageArray *CWhiteboardStream::s_pImageList = NULL;


UINT CWhiteboardStream::s_nInstanceCount = 0;


//static 
HRESULT CWhiteboardStream::CopyImage(DrawSdk::Image *pImage, 
                                     CString &csTargetPathAndPrefix, 
                                     CMap<CString, LPCTSTR, CString, CString&> &mapImages, 
                                     LPCTSTR tszSourcePath)
{
   HRESULT hr = S_OK;

   if (pImage == NULL)
      return E_POINTER;

   if (pImage->GetObjectNumber() < 0)
      return E_INVALIDARG;

   CString csImagePath = pImage->GetImageName();
   CString csImageName = csImagePath;
   StringManipulation::GetFilename(csImageName);
   CString csSuffix = csImageName;
   StringManipulation::GetFileSuffix(csSuffix);
   
   bool bCopyImage = true;
   
   // Was this image already copied/written before?

   CString csExportFilename;
   bool bFound = (mapImages.Lookup(csImagePath, csExportFilename) != 0);
   if (bFound)
   {
      // An image with this name has been copied before.
      // Do they match?
      struct _stat statSrcFile;
      struct _stat statDestFile;
      
      if (_tstat(csImagePath, &statSrcFile) == 0 &&
         _tstat(csExportFilename, &statDestFile) == 0)
      {
         if (statSrcFile.st_size == statDestFile.st_size)
         {
            // Ok, files are equal, take the old file.
            bCopyImage = false;
         }
      }
   }
   
   if (bCopyImage)
   {
      // This image is unknown up to now, let's copy it
      // and enter it into the image map for later lookup
      // Note: We need not create a unique file name here
      // anymore, because the object number is unique a
      // number enough.
      
      int iObjectNumber = pImage->GetObjectNumber();
      csExportFilename.Format(_T("%s_%d.%s"), csTargetPathAndPrefix, iObjectNumber, csSuffix);
      mapImages.SetAt(csImagePath, csExportFilename);
      bool bSuccess = (CopyFile(csImagePath, csExportFilename, FALSE) != 0);
      // Bugfix 6294: May be there is a wrong path, try local path
      if (!bSuccess && tszSourcePath != NULL) {
          CString csLocalPath = tszSourcePath;
          CString csFilenameOnly = csImagePath;
          StringManipulation::GetFilename(csFilenameOnly);
          csImagePath.Format(_T("%s\\%s"), csLocalPath, csFilenameOnly);
          bSuccess = (CopyFile(csImagePath, csExportFilename, FALSE) != 0);
      }
      if (!bSuccess)
         hr = E_FAIL;
   }
   else
      hr = S_FALSE; // nothing copied, only image prefix changed (below)
   
   if (SUCCEEDED(hr))
      pImage->SetImagePrefix(csExportFilename);


   return hr;
}



CWhiteboardStream::CWhiteboardStream() : CMediaStream()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (s_nInstanceCount == 0)
      s_pImageList = new DrawSdk::CImageArray;

   ++s_nInstanceCount;

   Init();
}

CWhiteboardStream::~CWhiteboardStream()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   
   --s_nInstanceCount;
   if (s_nInstanceCount == 0)
      delete s_pImageList;

   Clear();
}

CWhiteboardStream *CWhiteboardStream::Copy()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CWhiteboardStream *pCopyStream = new CWhiteboardStream();

   pCopyStream->SetEvqFilename(m_csEvqFilename);
   pCopyStream->SetObjFilename(m_csObjFilename);
   pCopyStream->SetPageDimension(m_csPageDimension);
   
   pCopyStream->Open();

   return pCopyStream;
}


void CWhiteboardStream::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iID              = -1;
   m_pLastDrawnEvent  = NULL;
}

void CWhiteboardStream::InitEmpty()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CWhiteboardEvent *pEvent = new CWhiteboardEvent(this);
   pEvent->InitEmpty();
   
   m_lstEvents.Add(pEvent);
}           

void CWhiteboardStream::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int i = 0;
   // delete all objects
   for (i = 0; i < m_arrObjects.GetSize(); ++i)
   {
      DrawSdk::DrawObject *drawObject = m_arrObjects.GetAt(i);
      if (drawObject)
         delete drawObject;
   }

   // remove array entries
   m_arrObjects.RemoveAll();

   
   // delete all events
//   POSITION position = m_lstEvents.GetHeadPosition();
//   while (position != NULL)
//   {
//      CWhiteboardEvent *event = m_lstEvents.GetNext(position);
//      if (event)
//         delete event;
//   }
   for (i=0; i<m_lstEvents.GetSize(); ++i)
   {
      CWhiteboardEvent *event = m_lstEvents.GetAt(i);
      if (event)
         delete event;
   }
   // remove list entries
   m_lstEvents.RemoveAll();



   //if (m_imageList)
   //   delete m_imageList;
   //m_imageList = NULL;

   //m_lastUsedPosition = NULL;
   m_iID              = -1;
}

bool CWhiteboardStream::GetPageDimension(CSize &dimPage)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   LPCTSTR szPageDimension = GetPageDimension();
   if (szPageDimension != NULL)
   {
      int iLength = _tcslen(szPageDimension);
      if (iLength > 0)
      {
         const _TCHAR *szSeparator = _tcschr(szPageDimension, 'x');
         if (szSeparator != NULL)
         {
            int iWidth = _ttoi(szPageDimension);
            int iHeight = _ttoi(szSeparator + 1);

            dimPage.cx = iWidth;
            dimPage.cy = iHeight;

            return true;
         }
      }
   }

   return false;
}

HRESULT CWhiteboardStream::Open()
{
    return Open(true);
}

HRESULT CWhiteboardStream::Open(bool bSaveImageToList)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = ReadObjectFile(bSaveImageToList);

   bool bSuccess = SUCCEEDED(hr);
   if (!bSuccess)
   {
      if (hr == E_WB_INVALID_INTERACTION || hr == E_WB_INVALID_QUESTION || hr == E_WB_INVALID_SUPPORT)
      {
         // probably only a "partial" error
         bSuccess = true;
      }
   }
   if (bSuccess)
      bSuccess = ReadEventqueue();

   return hr; // might be the "partial" error from objects parsing
}  

// private
HRESULT CWhiteboardStream::ReadObjectFile(bool bSaveImageToList)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CString recordPath;
   recordPath = m_csObjFilename;
   StringManipulation::GetPath(recordPath);

   SgmlFile objFile;
   objFile.Read(m_csObjFilename);
   
   SgmlElement *sgmlRoot = objFile.GetRoot();
   if (!sgmlRoot)
      return E_WB_INVALID_DATA;

   HRESULT hr = ParseObjectSgml(sgmlRoot, recordPath, bSaveImageToList);

   return hr;
}

HRESULT CWhiteboardStream::ParseObjectSgml(SgmlElement *pObjectElement, CString &csRecordPath, bool bSaveImageToList) {
    HRESULT hrFirstError = S_OK;

    if (pObjectElement == NULL)
        return E_POINTER;

    CArray<SgmlElement *, SgmlElement *> aElements;
    pObjectElement->GetElements(aElements);
    for (int i = 0; i < aElements.GetSize(); ++i) {
        SgmlElement *pObjTag = aElements[i];
        if (pObjTag != NULL) {
            CString csName = pObjTag->GetName();

            if (csName == _T("MARK"))
                m_MarkReader.Parse(pObjTag);
            else if (m_InteractionReader.IsInteractionObject(csName)) {
                // at the moment (Oct 2010) these are: AREA, BUTTON, RADIO, CHECK, FIELD, CALLOUT, CLICK

                HRESULT hr = m_InteractionReader.Parse(pObjTag, csRecordPath);

                if (FAILED(hr) && SUCCEEDED(hrFirstError)) {
                    hrFirstError = E_WB_INVALID_INTERACTION; // does not lead to an abort; but will be returned
                    _ASSERT(false);
                }
            } else if (csName == _T("QUESTIONNAIRE")) {
                HRESULT hr = m_QuestionReader.Parse(pObjTag, csRecordPath);

                if (FAILED(hr) && SUCCEEDED(hrFirstError)) {
                    hrFirstError = E_WB_INVALID_QUESTION; // does not lead to an abort; but will be returned
                    _ASSERT(false);
                }
            } else {
                CString csErrorFont = _T("");
                DrawSdk::DrawObject *pObject = CreateObject(pObjTag, csRecordPath, bSaveImageToList, &csErrorFont);
                if (pObject) {
                    pObject->SetObjectNumber(m_arrObjects.GetSize());

                    bool bIsTypedText = 
                        pObject->GetType() == DrawSdk::TEXT && ((DrawSdk::Text *)pObject)->HasTextType();
                    if (bIsTypedText || pObject->GetType() == DrawSdk::TARGET || pObject->IsDndMoveable()) {
                        // new object belonging to a question

                        HRESULT hr = m_InteractionReader.AddSupportObject(pObject);
                        delete pObject; // a copy was made

                        if (FAILED(hr) && SUCCEEDED(hrFirstError)) {
                            hrFirstError = E_WB_INVALID_SUPPORT; // does not lead to an abort; but will be returned
                            _ASSERT(false);
                        }

                        // Note: So you have 2 locations for storing things having to to with questions:
                        // - m_QuestionReader: CQuestionnaire and CQuestions
                        // - m_InteractionReader: for buttons (with special actions) and other interaction objects
                        // - m_InteractionReader: Support objects (type DrawSdk::DrawObject)
                    } else {
                        // old case: normal object used in the event queue

                        m_arrObjects.Add(pObject);
                    }
                    if(!csErrorFont.IsEmpty()) {
                        AddUnsupportedFont(csErrorFont);
                    }
                }
            }
        }
    }

    return hrFirstError;
}


int CWhiteboardStream::GetLength()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

//   if (m_lstEvents.IsEmpty())
//      return 0;

//   CWhiteboardEvent *lastEvent = m_lstEvents.GetTail();
//   if (lastEvent)
//      return lastEvent->GetTimestamp();
//   else
//      return 0;

   if (m_lstEvents.GetSize() == 0)
      return 0;

   CWhiteboardEvent *lastEvent = m_lstEvents.GetAt(m_lstEvents.GetSize() - 1);
   if (lastEvent)
      return lastEvent->GetTimestamp();
   else return 0;
}

int GetNumber(char *szToken, int iValidCharacters)
{
   szToken[iValidCharacters] = 0;
   return atoi(szToken);
}

bool CWhiteboardStream::ReadEventqueue()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   
   CString csRecordPath;
   csRecordPath = m_csEvqFilename;
   StringManipulation::GetPath(csRecordPath);

   HANDLE evqFile = CreateFile(m_csEvqFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

   if (evqFile == 0)
   {
      MessageBox (NULL, _T("Fehler ReadEventqueue Open"), NULL, MB_OK);
      return false;
   }

   unsigned long read;
   CWhiteboardEvent *event = NULL;
   char charBufi[16*1024];
   int blankCount = 0;

   char aOneToken[120]; // no number should be longer
   ZeroMemory(aOneToken, 120);
   int iTokenCharacters = 0;

   while (ReadFile(evqFile, charBufi, 16*1024, &read, NULL) == TRUE && read != 0)
   {
      for (unsigned int i = 0; i < read; ++i)
      {
         char charBuf = charBufi[i];
         if (charBuf == ' ' || charBuf == '\n')
         {
            // it is a separator

            if (blankCount == 0)
            { 
               event = new CWhiteboardEvent(this);
               if (event)
               {
                  int iTimestamp = GetNumber(aOneToken, iTokenCharacters);
                  event->SetTimestamp(iTimestamp);
                  m_lstEvents.Add(event);
               }
            }
            else if (blankCount == 1)
            {
               if (event)
               {
                  int iPageNumber = GetNumber(aOneToken, iTokenCharacters);
                  event->SetPageNumber(iPageNumber);
               }
            }
            else if (blankCount == 2)
            {
               if (event)
               {
                  int iBgColor = GetNumber(aOneToken, iTokenCharacters);
                  event->SetBgColor(iBgColor);
               }
            }
            else if (blankCount == 3)
            {
               if (event)
               {
                  int iItemCount = GetNumber(aOneToken, iTokenCharacters);
                  event->SetObjectCount(iItemCount);
               }
            }
            else
            {
               if (event)
               {
                  int iItemId = GetNumber(aOneToken, iTokenCharacters);
                  event->AddObject(iItemId, m_arrObjects);
               }
            }
            
            if (charBuf == ' ')
               ++blankCount;
            else
               blankCount = 0;

            iTokenCharacters = 0;
         }
         else
         {
            // it is part of a number

            aOneToken[iTokenCharacters++] = charBuf;
         }
      }
   }

   if (m_lstEvents.GetSize() > 0 && m_lstEvents[0] != NULL)
   {
      if (m_lstEvents[0]->GetTimestamp() > 0)
      {
         m_lstEvents[0]->SetTimestamp(0);

         // Bugfix #3086:
         // the first event of a document should be 0 ms (page start time is also corrected)
      }
   }

   // BUGFIX 5622: no content at beginning in Demo Mode
   if (m_lstEvents.GetSize() > 1 && m_lstEvents[0] != NULL && m_lstEvents[1] != NULL) {
      if (m_lstEvents[0]->GetTimestamp() == 0 
         && m_lstEvents[0]->GetObjectCount() == 0
         && m_lstEvents[0]->GetPageNumber() == m_lstEvents[1]->GetPageNumber() ) {
            // overwrite dummy event at 0 msec of structured screen recording
            // Note: clip starts with offset
            m_lstEvents[0] = m_lstEvents[1]->Copy();
      }
   }

   {
      int nEvents = m_lstEvents.GetSize();
      for (int i=0; i<nEvents; ++i)
         m_lstEvents[i]->CreateSortedObjArray();
   }

   CloseHandle(evqFile);

   return true;
}

CPageStream *CWhiteboardStream::CreatePageStreamFromEvents()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CPageStream *pPageStream = new CPageStream;
   
   int nLastPageNumber = -1;
   int nExportPageNumber = -1;
   CPage *pLastPage = NULL;
   CWhiteboardEvent *pEvent = NULL;
   for (int i = 0; i < m_lstEvents.GetSize(); ++i)
   {
      pEvent = m_lstEvents[i];
      if (pEvent)
      {
         if (pEvent->GetPageNumber() != nLastPageNumber)
         {
            if (pLastPage)
               pLastPage->SetEnd(pEvent->GetTimestamp());
            CPage *page = new CPage();
            page->SetPageNumber(pEvent->GetPageNumber());
            page->SetBegin(pEvent->GetTimestamp());
            CPage *findPage = pPageStream->FindPageWithNumber(pEvent->GetPageNumber());
            if (findPage) {
               nExportPageNumber = findPage->GetJoinId();
            } else {
               ++m_pageCounter;
               nExportPageNumber = m_pageCounter;
            }
            
            page->SetJoinId(nExportPageNumber);
            pPageStream->Add(page);
            nLastPageNumber = pEvent->GetPageNumber();
            pLastPage = page;
         }
         pEvent->SetExportPageNumber(nExportPageNumber);
      }
   }

   if (pLastPage && pEvent)
      pLastPage->SetEnd(pEvent->GetTimestamp());

   return pPageStream;
}

// static
Gdiplus::ARGB CWhiteboardStream::GetColor(SgmlElement *pObjTag) {
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Gdiplus::ARGB retColor = RGB(255, 255, 255);

   if (pObjTag->GetValue(_T("rgb")) != NULL) {
       retColor = pObjTag->GetARGBValue(_T("rgb"));
   } else if (pObjTag->GetValue(_T("fcolor")) != NULL) {
       int color = pObjTag->GetIntValue(_T("fcolor"));
       retColor = AofColor::GetARGB(color);
   } else if (pObjTag->GetValue(_T("ccolor")) != NULL) {
       int color = pObjTag->GetIntValue(_T("ccolor"));
       retColor = AofColor::GetARGB(color);
   }

   return retColor;
}

// static
DrawSdk::DrawObject *CWhiteboardStream::CreateObject(SgmlElement *pObjTag, CString &csRecordPath, bool bSaveImageToList, CString *pcsErrorFont) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    DrawSdk::DrawObject *object = NULL;

    double x = pObjTag->GetDoubleFallback(_T("xf"));
    double y = pObjTag->GetDoubleFallback(_T("yf"));
    double width = pObjTag->GetDoubleFallback(_T("widthf"));
    double height = pObjTag->GetDoubleFallback(_T("heightf"));

    Gdiplus::ARGB penColor; 
    if (_tcscmp(pObjTag->GetName(), _T("IMAGE")) != 0)
        penColor = GetColor(pObjTag); // image has no color definition

    if (_tcscmp(pObjTag->GetName(), _T("IMAGE")) == 0) {
        LPCTSTR tszImageName = pObjTag->GetValue(_T("fname"));
        CString csFileName = tszImageName;
        // TODO what about relative paths (relative to the LEP file)?
        if (csFileName.Find(_T("\\")) < 0)
            csFileName.Format(_T("%s\\%s"), csRecordPath, tszImageName);
        else {
            // else it is an absolute path
            // The file can not be found. May be the path was wrong, try the local path.
            if (_taccess(csFileName, 00) != 0) {
                CString csTmpFilenameWithoutPath = csFileName;
                StringManipulation::GetFilename(csTmpFilenameWithoutPath);
                CString csTmpFilename;
                csTmpFilename.Format(_T("%s\\%s"), csRecordPath, csTmpFilenameWithoutPath);
                if (_taccess(csTmpFilename, 00) == 0)
                    csFileName = csTmpFilename;
            }

        }

        bool bDndMoveable = false;
        LPCTSTR szMoveType = pObjTag->GetValue(_T("move"));
        if (szMoveType != NULL) {
            if (_tcscmp(szMoveType, _T("true")) == 0)
                bDndMoveable= true;
        }

        int iBelongsKey = 0;
        LPCTSTR szBelongs = pObjTag->GetValue(_T("belongs"));
        if (szBelongs != NULL) {
            iBelongsKey = pObjTag->GetIntValue(_T("belongs"));
        }

        DrawSdk::CImageInformation *pImageInformation = s_pImageList->GetImage((LPCTSTR)csFileName);
        Gdiplus::Image *pImage = NULL;
        // For Bugfix 5301: 
        // If 'bSaveImageToList' is true (default) then the image data is kept im memory. 
        // If 'bSaveImageToList' is false then 'pImage' is NULL and then the image file 
        // is opened, the data is read in and then the image file closed. 
        // This avoids having too much image data in the memory.
        if (bSaveImageToList && pImageInformation)
            pImage = pImageInformation->GetImageData();
        object = new DrawSdk::Image(x, y, width, height, (LPCTSTR)csFileName, pImage, bDndMoveable, iBelongsKey);
    } else if (_tcscmp(pObjTag->GetName(), _T("LINE")) == 0) {
        double lineWidth = pObjTag->GetDoubleFallback(_T("linewidthf"));
        int ori        = pObjTag->GetIntValue(_T("ori"));
        int dir        = pObjTag->GetIntValue(_T("dir"));
        int lineStyle  = pObjTag->GetIntValue(_T("linestyle"));
        int arrowStyle = pObjTag->GetIntValue(_T("arrowstyle"));
        int arrowConfig = pObjTag->GetIntValue(_T("arrowconfig"));

        object = new DrawSdk::Line((double)x, (double)y, (double)width, (double)height, penColor, lineWidth, lineStyle, arrowStyle, arrowConfig, ori, dir);
    } else if (_tcscmp(pObjTag->GetName(), _T("FILLEDRECT")) == 0) {
        Gdiplus::ARGB fillColor = GetColor(pObjTag);

        object = new DrawSdk::Rectangle((double)x, (double)y, (double)width, (double)height, fillColor, fillColor, 0.0, 0);
    } else if (_tcscmp(pObjTag->GetName(), _T("OUTLINERECT")) == 0) {
        Gdiplus::ARGB penColor = GetColor(pObjTag);

        double lineWidth  = pObjTag->GetDoubleFallback(_T("linewidthf"));
        int lineStyle     = pObjTag->GetIntValue(_T("linestyle"));

        object = new DrawSdk::Rectangle((double)x, (double)y, (double)width, (double)height, penColor, lineWidth, lineStyle);
    } else if (_tcscmp(pObjTag->GetName(), _T("FILLEDCIRC")) == 0) {
        Gdiplus::ARGB fillColor = GetColor(pObjTag);

        Gdiplus::RectF rect((Gdiplus::REAL)x, (Gdiplus::REAL)y, (Gdiplus::REAL)width, (Gdiplus::REAL)height);
        object = new DrawSdk::Oval(&rect, fillColor, fillColor, 1.0, 0);
    } else if (_tcscmp(pObjTag->GetName(), _T("OUTLINECIRC")) == 0) {
        Gdiplus::ARGB lineColor = GetColor(pObjTag);

        Gdiplus::RectF rect((Gdiplus::REAL)x, (Gdiplus::REAL)y, (Gdiplus::REAL)width, (Gdiplus::REAL)height);

        double lineWidth  = pObjTag->GetDoubleFallback(_T("linewidthf"));
        int lineStyle     = pObjTag->GetIntValue(_T("linestyle"));

        object = new DrawSdk::Oval(&rect, lineColor, lineWidth, lineStyle);
    } else if (_tcscmp(pObjTag->GetName(), _T("POLYLINES")) == 0 || 
               _tcscmp(pObjTag->GetName(), _T("FILLEDPOLY")) == 0 ||
               _tcscmp(pObjTag->GetName(), _T("FREEHAND")) == 0 ) {
        object = CreatePolyobject(pObjTag);
    }
    else if (_tcscmp(pObjTag->GetName(), _T("TEXT")) == 0) {
        object = CreateText(pObjTag, pcsErrorFont);
    } else if (_tcscmp(pObjTag->GetName(), _T("POINTER")) == 0) {
        bool bMousePointer = false;
        LPCTSTR tszType = pObjTag->GetValue(_T("type"));
        if (tszType != NULL)
            bMousePointer = _tcscmp(tszType, _T("mouse")) == 0;

        object = new DrawSdk::Pointer((double)x, (double)y, bMousePointer);
    } else if (_tcscmp(pObjTag->GetName(), _T("TARGET")) == 0) {
        // TODO this is a TCHAR (should be)
        LPCTSTR tszName = pObjTag->GetValue(_T("name"));

        int iBelongsKey = 0;
        LPCTSTR szBelongs = pObjTag->GetValue(_T("belongs"));
        if (szBelongs != NULL) {
            iBelongsKey = pObjTag->GetIntValue(_T("belongs"));
        }

        object = new DrawSdk::TargetArea((double)x, (double)y, (double)width, (double)height, tszName, iBelongsKey);

        DrawSdk::TargetArea *pTarget = (DrawSdk::TargetArea *)object;

        // Next lines are commented out because TargetArea is now derived from DrawObject (not Rectangle)
        //      // not every target area has these values (at least not the ones produced with 2.0.0.p0)
        //      if (objTag->GetValue(_T("rgb")) != NULL)
        //         pTarget->SetLineColor(GetColor(objTag));
        //
        //      if (objTag->GetValue(_T("linewidth")) != NULL)
        //         pTarget->SetLineWidth(objTag->GetIntValue(_T("linewidth")));
        //      if (objTag->GetValue(_T("linestyle")) != NULL)
        //         pTarget->SetLineStyle(objTag->GetIntValue(_T("linestyle")));
    }

    LPCTSTR tszVisible = pObjTag->GetValue(_T("visible"));
    if (object != NULL && tszVisible != NULL)
        object->SetVisibility(tszVisible);

    return object;
}

// static
DrawSdk::DrawObject *CWhiteboardStream::CreatePolyobject(SgmlElement *objTag)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   DrawSdk::Polygon *polyObject = NULL;

   
   if (_tcscmp(objTag->GetName(), _T("POLYLINES")) == 0)
   {
      Gdiplus::ARGB penColor = GetColor(objTag);

      double lineWidth = objTag->GetDoubleFallback(_T("linewidthf"));
      int lineStyle = objTag->GetIntValue(_T("linestyle"));
      int arrowStyle = objTag->GetIntValue(_T("arrowstyle"));
      int arrowConfig = objTag->GetIntValue(_T("arrowconfig"));
      polyObject = new DrawSdk::Polygon(NULL, 0, penColor, lineWidth, lineStyle, arrowStyle, arrowConfig, false);
   }
   else if (_tcscmp(objTag->GetName(), _T("FILLEDPOLY")) == 0)
   {
      Gdiplus::ARGB fillColor = GetColor(objTag);
      polyObject = new DrawSdk::Polygon(NULL, 0, fillColor, fillColor, 0.0, 0);
   }
   else if (_tcscmp(objTag->GetName(), _T("FREEHAND")) == 0)
   {
      Gdiplus::ARGB penColor = GetColor(objTag);
      double lineWidth  = objTag->GetDoubleFallback(_T("linewidthf"));
      int lineStyle     = objTag->GetIntValue(_T("linestyle"));

      polyObject = new DrawSdk::Polygon(NULL, 0, penColor, lineWidth, lineStyle);
      polyObject->SetIsFreehand(true);
   }

   CArray<SgmlElement *, SgmlElement *> aElements;
   objTag->GetElements(aElements);
   for  (int i = 0; i < aElements.GetCount(); ++i) {
       SgmlElement *pPtTag = aElements[i];
       if (pPtTag != NULL) {
           DrawSdk::DPoint pt;
           pt.x = pPtTag->GetDoubleFallback(_T("xf"));
           pt.y = pPtTag->GetDoubleFallback(_T("yf"));
           polyObject->AddPoint(pt);
       }
   }

   return polyObject;
}

// static
DrawSdk::DrawObject *CWhiteboardStream::CreateText(SgmlElement *objTag, CString* pcsErrorFont)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   DrawSdk::Text *textObject = NULL;

   double width = objTag->GetDoubleFallback(_T("widthf"));
   double height = objTag->GetDoubleFallback(_T("heightf"));

   CArray<SgmlElement *, SgmlElement *> aElements;
   objTag->GetElements(aElements);
   SgmlElement *pItemTag = aElements[0];
   if (pItemTag) {
       double x = pItemTag->GetDoubleFallback(_T("xf"));
       double y = pItemTag->GetDoubleFallback(_T("yf"));
       Gdiplus::ARGB penColor  = GetColor(pItemTag);
       CString fontFamily = pItemTag->GetValue(_T("font"));
       int size           = pItemTag->GetIntValue(_T("size"));
       CString weight     = pItemTag->GetValue(_T("weight"));
       CString slant      = pItemTag->GetValue(_T("slant"));
       CString ttfName    = pItemTag->GetValue(_T("ttf"));
       CString text       = pItemTag->GetParameter();
       int textStyle      = pItemTag->GetIntValue(_T("style"));
       bool bIsUnderlined = (textStyle & FLAG_UNDERLINE) != 0;

	   StringManipulation::DecodeXmlSpecialCharacters(text);

       // Note: objTag (TEXT) is used here instead of itemTag (TEXTITEM).
       bool bSpecialDraw = false;
       const _TCHAR *tszSpecialDraw = objTag->GetValue(_T("specialDraw"));
       if (tszSpecialDraw != NULL)
           bSpecialDraw = objTag->GetBoolValue(_T("specialDraw"));

       // Note: objTag (TEXT) is used here instead of itemTag (TEXTITEM).
       LPCTSTR szTextType = objTag->GetValue(_T("type"));
       DrawSdk::TextTypeId idTextType = DrawSdk::TEXT_TYPE_NOTHING;

       if (szTextType != NULL)
           idTextType = DrawSdk::Text::GetTextTypeForString(szTextType);

       LOGFONT logFont;
       FontManipulation::Create(fontFamily, size, weight, slant, bIsUnderlined, logFont);
       Gdiplus::FontFamily currfontFamily(logFont.lfFaceName);
       Gdiplus::Font gdipFontCurrent(&currfontFamily, abs(logFont.lfHeight), 0, Gdiplus::UnitPixel);
       BOOL bIsFontAvailable = gdipFontCurrent.IsAvailable();

       // NOTE: size==0 is used by structured clips to handle text for full text search which are not displayed
       if (!bIsFontAvailable && size != 0) {
           _tcscpy(logFont.lfFaceName, _T("Arial"));

           //AddUnsupportedFont(fontFamily);
           if(pcsErrorFont != NULL)
               *pcsErrorFont = fontFamily;
           ttfName.Empty();
       }
       if (ttfName.IsEmpty())
           textObject = new DrawSdk::Text((double)x, (double)y, (double)width, (double)height, penColor, text, text.GetLength(), &logFont, idTextType);
       else {
           CString sgmlTTFName;
           StringManipulation::TransformForSgml(ttfName, sgmlTTFName);
           textObject = new DrawSdk::Text((double)x, (double)y, (double)width, (double)height, penColor, text, text.GetLength(), &logFont, sgmlTTFName, idTextType);
       }
       textObject->DrawSpecial(bSpecialDraw);
       text.ReleaseBuffer();
   }
   

   return textObject;
}

void CWhiteboardStream::AddUnsupportedFont(CString csFont)
{
    bool bFound = false;
    for(int i = 0; i < m_acsUnsupportedFonts.GetCount(); i++)
    {
        if(csFont.CompareNoCase(m_acsUnsupportedFonts.GetAt(i)) == 0)
        {
            bFound = true;
            break;
        }
    }
    if(!bFound)
        m_acsUnsupportedFonts.Add(csFont);
}

void CWhiteboardStream::GetUnsupportedFonts(CArray<CString, CString> &acsUnsupportedFonts)
{
    acsUnsupportedFonts.RemoveAll();
    for(int i = 0; i < m_acsUnsupportedFonts.GetCount(); i++)
    {
        acsUnsupportedFonts.Add(m_acsUnsupportedFonts.GetAt(i));
    }
}

void CWhiteboardStream::ClearUnsupportedFonts()
{
    m_acsUnsupportedFonts.RemoveAll();
}

bool CheckRemovePointers(CPageStream *pPageStream, CWhiteboardEvent *pEvent, int iOffset) {
    bool bRemovePointersHere = false;

    if (pPageStream != NULL && pEvent != NULL) {
        int iGlobalEventTime = pEvent->GetTimestamp() + iOffset;

        CPage *pPage = pPageStream->FindPageAtTimestamp(iGlobalEventTime);

        if (pPage != NULL) {
            if (pPage->IsHidePointers())
                bRemovePointersHere = true;
        }
    }

    return bRemovePointersHere;
}

// PZI: additional argument added to switch off image copy
// note: "to" is NOT included - TODO: is this correct??
void CWhiteboardStream::Write(CFileOutput *pEventOutput, CFileOutput *pObjectOutput,
                              LPCTSTR tszRecordPath, LPCTSTR tszPrefix, int from, int to, 
                              int timeOffset, bool copyImages, CPageStream *pPageStream)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   
   CWhiteboardEvent *lastEvent = NULL;
   bool isBoundingEvent = true;

   if (m_lstEvents.GetSize() == 0)
      return;

   CMap<CString, LPCTSTR, CString, CString&> imageMap;

   for (int i=0; i<m_lstEvents.GetSize(); ++i)
   {
      CWhiteboardEvent *event = m_lstEvents.GetAt(i);

      bool bRemovePointersHere = CheckRemovePointers(pPageStream, event, timeOffset);

    

      if (event && event->GetTimestamp() >= to)
      {
         if (lastEvent) 
         {
            // if there is no event in this part, 
            // but one before part
            // write this event with the timestamp of the begin and the end of part
            int offset = timeOffset + (from - lastEvent->GetTimestamp());
            lastEvent->Write(pEventOutput, pObjectOutput, tszRecordPath, tszPrefix, offset, imageMap, copyImages, bRemovePointersHere);

            offset = timeOffset + (to - lastEvent->GetTimestamp());
            lastEvent->Write(pEventOutput, pObjectOutput, tszRecordPath, tszPrefix, offset, imageMap, copyImages, bRemovePointersHere);
            lastEvent = NULL;
         }
         else 
         {
            if (isBoundingEvent) 
            {
               // there is no event before part, 
               // but one after part
               // write this event with the timestamp of the begin and the end of part
               int offset = timeOffset + (from - event->GetTimestamp());
               event->Write(pEventOutput, pObjectOutput, tszRecordPath, tszPrefix, offset, imageMap, copyImages, bRemovePointersHere);

               offset = timeOffset + (to - event->GetTimestamp());
               event->Write(pEventOutput, pObjectOutput, tszRecordPath, tszPrefix, offset, imageMap, copyImages, bRemovePointersHere);
            }
            else 
            {
               // rewrite the last event in part with the timestamp of the end of part
               if (i > 0)
               {
                  lastEvent = m_lstEvents.GetAt(i-1);
                  int offset = timeOffset + (to - lastEvent->GetTimestamp());
                  lastEvent->Write(pEventOutput, pObjectOutput, tszRecordPath, tszPrefix, offset, imageMap, copyImages, bRemovePointersHere);
               }
            }
         }
         return;
      }

      if (event && event->GetTimestamp() > from)
      {
         if (isBoundingEvent)
         {
            if (lastEvent) 
            {
               // write event at beginning of segment
               int offset = timeOffset - (lastEvent->GetTimestamp() - from);
               lastEvent->Write(pEventOutput, pObjectOutput, tszRecordPath, tszPrefix, offset, imageMap, copyImages, bRemovePointersHere);
               lastEvent = NULL;
            }
            // write actual event
            event->Write(pEventOutput, pObjectOutput, tszRecordPath, tszPrefix, timeOffset, imageMap, copyImages, bRemovePointersHere);
            isBoundingEvent = false;
         }
         else
            event->Write(pEventOutput, pObjectOutput, tszRecordPath, tszPrefix, timeOffset, imageMap, copyImages, bRemovePointersHere);
      }
      else
         lastEvent = event;
   }

   // if there is no event written write last event
   if (lastEvent) 
   {
       bool bRemovePointersHere = CheckRemovePointers(pPageStream, lastEvent, timeOffset);

      // if there is no event in this part, 
      // write last event before part if exists
      int offset = timeOffset - lastEvent->GetTimestamp() + from;
      lastEvent->Write(pEventOutput, pObjectOutput, tszRecordPath, tszPrefix, offset, imageMap, copyImages, false);
      lastEvent = NULL;
   }
}

void CWhiteboardStream::Write(CString csRecordPath, CPageStream *pPageStream) {
   CString csEvqFilename;
   csEvqFilename.Format(_T("%ls\\%ls"), csRecordPath, m_csEvqFilename);
   CString csObjFilename;
   csObjFilename.Format(_T("%ls\\%ls"), csRecordPath, m_csObjFilename);

   TRACE("  write EVQ: %ls\n", csEvqFilename);
   TRACE("  write OBJ: %ls\n", csObjFilename);

   LPCTSTR tszRecordPath = csRecordPath;
   CString filePrefix = m_csEvqFilename;
   StringManipulation::GetFilePrefix(filePrefix);

   CFileOutput *objFp;
   HANDLE hObjFile = CreateFile(csObjFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hObjFile == INVALID_HANDLE_VALUE)
   {
      TRACE("ERROR WRITING EMPTY OBJ FILE: %ls\n",csObjFilename);
      return;
   } 
   else 
   {
      objFp = new CFileOutput;
      objFp->Init(hObjFile, 0, true);
   }
   HANDLE hEvqFile = CreateFile(csEvqFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   CFileOutput *evqFp;
   if (hEvqFile == INVALID_HANDLE_VALUE)
   {
      TRACE("ERROR WRITING EMPTY EVQ FILE: %ls\n",csEvqFilename);
      return;
   } 
   else 
   {
      evqFp = new CFileOutput;
      evqFp->Init(hEvqFile, 0, false); // EVQ files are not written in UTF-8 format
   }

   // write OBJ/EVQ files

   // write OBJ header
   objFp->Append(_T("<WB orient=bottomleft>\n"));

   // reset object counter and object numbers to ensure sequential numbering
   ResetObjects();
   CWhiteboardStream::m_objectCounter = 0;
   m_pageCounter = 0;

   // write events and objects
   Write(evqFp, objFp, tszRecordPath, filePrefix, 0, GetLength()+1, 0, false, pPageStream);

   // write OBJ tail
   objFp->Append(_T("</WB>"));
   objFp->Flush();

   // close file handles
   CloseHandle(hObjFile);
   delete objFp;
   objFp = NULL;

   evqFp->Flush();
   CloseHandle(hEvqFile);
   delete evqFp;
   evqFp = NULL;
}

void CWhiteboardStream::ResetObjects()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   for (int i = 0; i < m_arrObjects.GetSize(); ++i)
      m_arrObjects[i]->ResetObjectNumber();
}

int CEventCompare(const void *elem1, const void *elem2)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CWhiteboardEvent *e1 = (CWhiteboardEvent *) elem1;
   CWhiteboardEvent *e2 = (CWhiteboardEvent *) elem2;

   if (e1->GetTimestamp() < e2->GetTimestamp())
      return -1;
   else if (e1->GetTimestamp() == e2->GetTimestamp())
      return 0;
   return 1;
}

//POSITION CWhiteboardStream::FindEventBefore(int timestamp)
int CWhiteboardStream::FindEventBefore(int timestamp)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   //POSITION position = m_lstEvents.GetTailPosition();
   //while (position != NULL)
   //{
   //   CWhiteboardEvent *event = m_lstEvents.GetAt(position);
   //   if (event && (event->GetTimestamp() <= timestamp))
   //      break;
   //   m_lstEvents.GetPrev(position);
   //}

   //if (position == NULL)
   //   position = m_lstEvents.GetHeadPosition();

   if (m_lstEvents.GetSize() == 0)
      return 0;

   int pos = 0;

   for (int i=0; i<m_lstEvents.GetSize(); ++i)
   {
      CWhiteboardEvent *pEvent = m_lstEvents.GetAt(i);
      if (pEvent && pEvent->GetTimestamp() <= timestamp)
         pos = i;
      else
         break;
   }

   return pos;
}

//POSITION CWhiteboardStream::FindEventAfter(int timestamp)
int CWhiteboardStream::FindEventAfter(int timestamp)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

//   POSITION position = m_lstEvents.GetHeadPosition();
//   while (position != NULL)
//   {
//      CWhiteboardEvent *event = m_lstEvents.GetAt(position);
//      if (event && (event->GetTimestamp() > timestamp))
//         break;
//      m_lstEvents.GetNext(position);
//   }
//
//   if (position == NULL)
//      position = m_lstEvents.GetTailPosition();
//   return position;

   if (m_lstEvents.GetSize() == 0)
      return 0;

   int pos = 0;

   for (int i=0; i<m_lstEvents.GetSize(); ++i)
   {
      CWhiteboardEvent *pEvent = m_lstEvents.GetAt(i);
      if (pEvent && pEvent->GetTimestamp() > timestamp)
      {
         pos = i;
         break;
      }
   }

   return pos;
}

void CWhiteboardStream::UpdateExportPageNumbers(CPageStream *pPageStream)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (pPageStream == NULL || pPageStream->IsEmpty())
        return;

    if (m_lstEvents.GetCount() == 0)
        return;

    int iPageIndex = 0;
    CPage *pPage = pPageStream->GetPage(iPageIndex);
    int iLastPageNumber = m_lstEvents[0]->GetPageNumber();
    for (int i = 0; i < m_lstEvents.GetSize() && pPage; ++i)
    {
        CWhiteboardEvent *pEvent = m_lstEvents.GetAt(i);
        if (pEvent)
        {
            bool bPageEventOnOldPage = pEvent->GetPageNumber() == iLastPageNumber;
            
            if ((pEvent->GetTimestamp() == pPage->GetEnd() && !bPageEventOnOldPage) ||
                (pEvent->GetTimestamp() > pPage->GetEnd()))
            {
                ++iPageIndex;
                pPage = pPageStream->GetPage(iPageIndex);
                if (!pPage)
                    break;
            }

            pEvent->SetExportPageNumber(pPage->GetJoinId());
            iLastPageNumber = pEvent->GetPageNumber();
        }
    }
}

bool CWhiteboardStream::ContainsMarks()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return m_MarkReader.GetCount() > 0;
}
   

HRESULT CWhiteboardStream::ExtractMarks(CStopJumpMark *aMarks, UINT *pnElementCount)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   if (pnElementCount == NULL)
      return E_POINTER;

   if (aMarks != NULL && *pnElementCount == 0)
      return E_INVALIDARG;


   if (aMarks == NULL)
   {
      *pnElementCount = m_MarkReader.GetCount();
      if (*pnElementCount == 0)
         return S_FALSE;
   }
   else
   {
      UINT nToCopy = min(*pnElementCount, m_MarkReader.GetCount());
      // TODO: Regard return value?
      // TODO: Copy only "nToCopy" elements?
      hr = m_MarkReader.Extract(aMarks);
      *pnElementCount = nToCopy;
   }

   return hr;
}

bool CWhiteboardStream::ContainsInteractions()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return m_InteractionReader.GetCount() > 0;
}

HRESULT CWhiteboardStream::ExtractInteractions(CInteractionArea **paInteractions, UINT *pnElementCount)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   if (pnElementCount == NULL)
      return E_POINTER;

   if (paInteractions != NULL && *pnElementCount == 0)
      return E_INVALIDARG;


   if (paInteractions == NULL)
   {
      *pnElementCount = m_InteractionReader.GetCount();
      if (*pnElementCount == 0)
         return S_FALSE;
   }
   else
   {
      UINT nToCopy = min(*pnElementCount, m_InteractionReader.GetCount());
      // TODO: Regard return value?
      // TODO: Copy only "nToCopy" elements?
      hr = m_InteractionReader.Extract(paInteractions);
      *pnElementCount = nToCopy;
   }

   return hr;
}

HRESULT CWhiteboardStream::ExtractInteractions(CArray<CInteractionArea *, CInteractionArea *> *paInteractives)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return m_InteractionReader.Extract(paInteractives);
}

bool CWhiteboardStream::ContainsQuestions()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return m_QuestionReader.GetCount() > 0;
}

HRESULT CWhiteboardStream::ExtractQuestionnaires(CQuestionnaire **paQuestionnaires, UINT *pnElementCount)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   if (pnElementCount == NULL)
      return E_POINTER;

   if (paQuestionnaires != NULL && *pnElementCount == 0)
      return E_INVALIDARG;


   if (paQuestionnaires == NULL)
   {
      *pnElementCount = m_QuestionReader.GetCount();
      if (*pnElementCount == 0)
         return S_FALSE;
   }
   else
   {
      UINT nToCopy = min(*pnElementCount, m_QuestionReader.GetCount());
      // TODO: Regard return value?
      // TODO: Copy only "nToCopy" elements?
      hr = m_QuestionReader.Extract(paQuestionnaires);
      *pnElementCount = nToCopy;
   }

   return hr;
}


HRESULT CWhiteboardStream::ExtractQuestionInteractives(CArray<CInteractionArea *, CInteractionArea *> *paQuestionInteractives)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return m_InteractionReader.ExtractQuestionInteractives(paQuestionInteractives);
}
 


bool CWhiteboardStream::Draw(CDC *pDC, int iTimestampMs, CRect &rcDrawRect, 
                             DrawSdk::ZoomManager *zoom, bool bForce, bool bIncludeInteractives, bool bHidePointers)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_lstEvents.GetSize() == 0)
      return false;

   bool bSomethingDrawn = false;

   CWhiteboardEvent *pEvent = GetEvent(iTimestampMs);

   if (pEvent && (bForce || pEvent != m_pLastDrawnEvent))
   {
      pEvent->Draw(pDC, rcDrawRect, zoom, bForce ? NULL : m_pLastDrawnEvent, bHidePointers);
      m_pLastDrawnEvent = pEvent;
      bSomethingDrawn = true;
   }

   if (bIncludeInteractives && m_InteractionReader.GetCount() > 0)
   {
      UINT nCount = m_InteractionReader.GetCount();
      for (int i=0; i<(signed)nCount; ++i)
      {
         CInteractionArea *pArea = m_InteractionReader.GetObject(i);
            
         if (pArea->IsVisibleAt(iTimestampMs))
         {
            pArea->Draw(pDC->m_hDC, (double)rcDrawRect.left, (double)rcDrawRect.top, zoom);
            bSomethingDrawn = true;
         }
      }
   }
   
   return bSomethingDrawn;
}

CWhiteboardEvent *CWhiteboardStream::GetLastDrawnEvent()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return m_pLastDrawnEvent;
}

CWhiteboardEvent *CWhiteboardStream::GetEvent(int timestamp)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   //if (m_lstEvents.IsEmpty())
   if (m_lstEvents.GetSize() == 0)
      return NULL;

   //POSITION position = FindEventBefore(timestamp);
   //CWhiteboardEvent *event = m_lstEvents.GetAt(position);
   int position = FindEventBefore(timestamp);
   CWhiteboardEvent *event = m_lstEvents.GetAt(position);

   return event;
}

// PZI: remove an event
// currently just removes the event without checks
// does not remove object
void CWhiteboardStream::RemoveEvent(int timestamp)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   
   /*
   if (m_lstEvents.GetSize() > 0) {
      int position = FindEventBefore(timestamp);
      m_lstEvents.RemoveAt(position);
   }
   */

   if (m_lstEvents.GetSize() > 0) {
      for (int i=0; i<m_lstEvents.GetSize(); ++i)
      {
         CWhiteboardEvent *pEvent = m_lstEvents.GetAt(i);
         if (pEvent && pEvent->GetTimestamp() == timestamp) 
            m_lstEvents.RemoveAt(i);
      }
   }
}

void CWhiteboardStream::GetObjectArray(CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> &drawObjects)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   for (int i = 0; i < m_arrObjects.GetSize(); ++i)
   {
      drawObjects.Add(m_arrObjects.ElementAt(i));
   }
}

void CWhiteboardStream::GetEventArray(CArray<CWhiteboardEvent *, CWhiteboardEvent *> &events)
{
   events.SetSize(0, m_lstEvents.GetSize());

   for (int i=0; i<m_lstEvents.GetSize(); ++i)
   {
      CWhiteboardEvent *pEvent = m_lstEvents.GetAt(i);
      if (pEvent)
         events.Add(pEvent);
   }
}

// PZI: add an event - inserts event according to its timestamp
void CWhiteboardStream::AddEvent(CWhiteboardEvent *event) {
   if(event) 
   {
      // try to insert event at correct time position
      for (int i=0; i<m_lstEvents.GetSize(); ++i)
      {
         CWhiteboardEvent *pEvent = m_lstEvents.GetAt(i);
         if (pEvent && pEvent->GetTimestamp() > event->GetTimestamp()) {
            m_lstEvents.InsertAt(i, event);
            return;
         }
      }
      // not found: attach to end
      m_lstEvents.Add(event);
   }
}
