// PPT2Flash.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"

#include "batconvcore.tlh"
#include "FlashEngine.h"
#include "SgmlParser.h"


// file sdk
#include "FileOutput.h"
#include "MiscFunctions.h"

#define FINISH_SUCCESS    0
#define ERROR_PARAMETERS  -1
#define ERROR_PATHNAME    -2
#define ERROR_OUTDIR      -3
#define ERROR_POWERPOINT  -4
#define ERROR_LSD         -5
#define ERROR_SWF         -6
#define ERROR_XML         -7

HRESULT ImportPPT(LPCTSTR lpszPowerPointName, LPCTSTR lpszLsdname)
{
   ::CoInitialize(NULL);

   CString csLsdPath = lpszLsdname;

   HRESULT hresult = S_OK;
   CLSID clsid;

   // get component CLSID
   if (SUCCEEDED(hresult))
      hresult = CLSIDFromProgID(OLESTR("batch_ppt2lsd.CBatchConvCore"), &clsid);

   // the class we defined in dll
   batch_ppt2lsd::_CBatchConvCore *t;

   // create a class instantce
   if (SUCCEEDED(hresult))
      hresult = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER,
      __uuidof(batch_ppt2lsd::_CBatchConvCore), (LPVOID *) &t);

   if (SUCCEEDED(hresult))
   {
      // ppt filename should be in unicode
      WCHAR *wszText = NULL;
      size_t nLen = _tcslen(lpszPowerPointName);
      wszText = new WCHAR[nLen + 1];
#ifndef _UNICODE
      ::MultiByteToWideChar(CP_ACP, 0, lpszPowerPointName, -1, wszText, nLen + 1);
#else
      wcscpy(wszText, lpszPowerPointName);
#endif

      int errID = t->Launch(wszText, (LPCTSTR)csLsdPath);

      // Release
      t->Release();

      if (errID != 0)
         hresult = E_FAIL;

      if (wszText)
         delete[] wszText;

   }

   ::CoUninitialize();

   return hresult;
}


void GetPath(CString &csPath)
{
   int iSlashPos = csPath.ReverseFind(_T('\\'));
   if (iSlashPos != -1)
      csPath = csPath.Left(iSlashPos);
   else
      csPath.Empty();
}

void GetName(CString &csName)
{
   int iSlashPos = csName.ReverseFind(_T('\\'));
   if (iSlashPos != -1)
      csName = csName.Right(csName.GetLength() - (iSlashPos+1));
}

void GetPrefix(CString &csPrefix)
{
   int iSlashPos = csPrefix.ReverseFind(_T('.'));
   if (iSlashPos != -1)
      csPrefix = csPrefix.Left(iSlashPos);

   return;
}

void MaskIfNecessary(CString &csXmlLine)
{
   CString csResult;

   for (int i = 0; i < csXmlLine.GetLength(); ++i)
   {
      if (csXmlLine[i] == _T('<'))
         csResult += "&lt;";
      else if (csXmlLine[i] == _T('>'))
         csResult += "&gt;";
      else if (csXmlLine[i] == _T('&'))
         csResult += "&amp;";
      else if (csXmlLine[i] == _T('\"'))
         csResult += "&quot;";
      else if (csXmlLine[i] == _T('\''))
         csResult += "&apos;";
      else
         csResult += csXmlLine[i];
   }

   csXmlLine = csResult;
}

HRESULT WriteXmlFile(ASSISTANT::Page *pPage, CString &csDocumentTitle, CString &csXmlFilename, CString &csSwfName)
{
   HANDLE hXmlFile = CreateFile(csXmlFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
                                FILE_ATTRIBUTE_NORMAL, NULL);

   if (hXmlFile == INVALID_HANDLE_VALUE)
      return E_FAIL;

   CFileOutput xmlFile;
   xmlFile.Init(hXmlFile, 0, true);

   CString csLine;
   CString csSgmlLine;

   csLine = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
   xmlFile.Append(csLine);
   
   csLine = _T("<document>\n");
   xmlFile.Append(csLine);
   
   //StringManipulation::TransformForSgml(csDocumentTitle, csSgmlLine);
   csSgmlLine = csDocumentTitle;
   MaskIfNecessary(csSgmlLine);
   csLine.Format(_T("    <title>%s</title>\n"), csSgmlLine);
   xmlFile.Append(csLine);
   if (pPage->GetWidth() > pPage->GetHeight())
   {
      csLine = _T("    <orientation>landscape</orientation>\n");
      xmlFile.Append(csLine);
   }
   else
   {
      csLine = _T("    <orientation>portrait</orientation>\n");
      xmlFile.Append(csLine);
   }

   CString csName = csSwfName;
   GetName(csName);
   MaskIfNecessary(csName);
   csLine.Format(_T("    <img>%s</img>\n"), csName);
   xmlFile.Append(csLine);

   //StringManipulation::TransformForSgml(pPage->GetName(), csSgmlLine);
   csSgmlLine = pPage->GetName();
   MaskIfNecessary(csSgmlLine);
   csLine.Format(_T("    <slidetitle>%s</slidetitle>\n"), csSgmlLine);
   xmlFile.Append(csLine);

   CString csAllTexts;
   CArray<ASSISTANT::DrawObject *, ASSISTANT::DrawObject *> arObjects;
   pPage->GetObjects(arObjects);
   for (int i = 0; i < arObjects.GetSize(); ++i)
   {
      if (arObjects[i]->GetType() == ASSISTANT::TEXT)
      {
         if (!csAllTexts.IsEmpty())
            csAllTexts += " ";
         csAllTexts += ((ASSISTANT::Text *)arObjects[i])->GetString();
      }
   }
   
   //StringManipulation::TransformForSgml(csAllTexts, csSgmlLine);
   csSgmlLine = csAllTexts;
   MaskIfNecessary(csSgmlLine);
   csLine.Format(_T("    <text>%s</text>\n"), csSgmlLine);
   xmlFile.Append(csLine);

   csLine = _T("</document>\n");
   xmlFile.Append(csLine);
   
   xmlFile.Flush();
   CloseHandle(hXmlFile);

   return S_OK;
}
int _tmain(int argc, _TCHAR* argv[])
{
   if (argc < 3)
   {
      _tprintf(_T("usage: PPT2Flash.exe <input file> <target directory>\n"));
      _tprintf(_T("<input file>: PowerPoint filename without .ppt extension\n"));
      _tprintf(_T("<target directory>: Directory for output files\n"));
      return ERROR_PARAMETERS;
   }

   ULONG_PTR gdiplusToken;
   Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

   int iReturnValue = FINISH_SUCCESS;

   CString csProgramPath = argv[0];
   
   TCHAR tszPathBuffer[MAX_PATH];
   TCHAR *tszFileName;
   DWORD dwResult = GetFullPathName(csProgramPath, MAX_PATH, tszPathBuffer, &tszFileName);

   if (dwResult == 0)
      return ERROR_PATHNAME;

   csProgramPath = tszPathBuffer;
   GetPath(csProgramPath);

   CString csPPTPrefix = argv[1];
   GetPrefix(csPPTPrefix);

   CString csPPTName = csPPTPrefix + _T(".ppt");

   CString csOutputDirectory = argv[2];
   if (_taccess(csOutputDirectory, 00) != 0)
   {
#ifdef _DEBUG
      _tprintf(_T("The output directory does not exist\n"));
#endif
      return ERROR_OUTDIR;
   }

   CString csPrefix = csPPTPrefix;
   GetName(csPrefix);

   CString csLsdName;
   csLsdName.Format(_T("%s\\%s.lsd"), csOutputDirectory, csPrefix);

   HRESULT hr = ImportPPT(csPPTName, csLsdName);

   if (FAILED(hr))
   {
#ifdef _DEBUG
      _tprintf(_T("PowerPoint conversion failed\n"));
#endif
      return ERROR_POWERPOINT;
   }

   CArray<ASSISTANT::Page *, ASSISTANT::Page *> arPages;
   CStringArray arLsdTmpFileNames;
   CString csDocumentTitle;
   // Load Lsd and append pages
   if (SUCCEEDED(hr))
   {

      SGMLTree *pSgmlTree = new SGMLTree(csLsdName);
      arLsdTmpFileNames.Add(csLsdName);

      int iMaxWidth = 0;
      int iMaxHeight = 0;
      if (pSgmlTree)
      {
         int pages;
         pSgmlTree->Buildtree(pages);
         SGMLElement *pRoot = pSgmlTree->root;
         if (pRoot)
         {
            csDocumentTitle = pRoot->GetAttribute("name");  
            SGMLElement *pElement = pRoot->son;

            while (pElement)
            {
               if (pElement->Name == _T("PAGE"))
               {
                  ASSISTANT::Page *pPage = new ASSISTANT::Page(pElement, csOutputDirectory, arLsdTmpFileNames);
                  arPages.Add(pPage);
                  if (pPage->GetWidth() > iMaxWidth)
                     iMaxWidth = pPage->GetWidth();
                  if (pPage->GetHeight() > iMaxHeight)
                     iMaxHeight = pPage->GetHeight();
               }
               pElement = pElement->next;
            }
         }

         delete pSgmlTree;
      }
      else
      {
         hr = E_FAIL;
         iReturnValue = ERROR_LSD;
      }
      // TODO höhe breite bestimmen
   }

   if (SUCCEEDED(hr))
   {
      for (int i = 0; i < arPages.GetSize(); ++i)
      {
         ASSISTANT::Page *pPage = arPages[i];

         if (pPage != NULL)
         {
            CString csOutputFilename;
            csOutputFilename.Format(_T("%s\\%s_slide%d.swf"), csOutputDirectory, csPrefix, i);
            CFlashEngine *pFlashEngine = new CFlashEngine();
            pFlashEngine->SetOutputFile(csOutputFilename);

            int iPageWidth = pPage->GetWidth();
            int iPageHeight = pPage->GetHeight();
            pFlashEngine->SetInputDimensions(iPageWidth, iPageHeight);
            pFlashEngine->SetOutputDimensions(iPageWidth, iPageHeight);

            pFlashEngine->ReadAndProcessPages(pPage);
            pFlashEngine->ConvertImages(csProgramPath);
            pFlashEngine->ConvertFonts(csProgramPath);

            Gdiplus::ARGB refBackColor = Gdiplus::Color::MakeARGB(255, 255, 255, 255);

            hr = pFlashEngine->BuildMovie(pPage, refBackColor, csProgramPath);
            //pFlashEngine->Cleanup();

            if (FAILED(hr))
               iReturnValue = ERROR_SWF;

            delete pFlashEngine;

            if (SUCCEEDED(hr))
            {
               CString csXmlFilename;
               csXmlFilename.Format(_T("%s\\%s_slide%d.xml"), csOutputDirectory, csPrefix, i);

               hr = WriteXmlFile(pPage, csDocumentTitle, csXmlFilename, csOutputFilename);
               if (FAILED(hr))
                  iReturnValue = ERROR_XML;
            }
         }
      }

   }


   for (int i = 0; i < arPages.GetSize(); ++i)
   {
      delete arPages[i];
   }
   arPages.RemoveAll();


   Gdiplus::GdiplusShutdown(gdiplusToken);

   BOOL bResult;
   for (int i = 0; i < arLsdTmpFileNames.GetSize(); ++i)
   {
      if (!arLsdTmpFileNames[i].IsEmpty())
         bResult = DeleteFile(arLsdTmpFileNames[i]);
      if (bResult == 0)
      {
         DWORD dwError = GetLastError();
      }
   }

   return iReturnValue;
}


