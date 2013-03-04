// FixObjDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "FixObj.h"
#include "FixObjDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFixObjDlg Dialogfeld

CFixObjDlg::CFixObjDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFixObjDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFixObjDlg)
		// HINWEIS: Der Klassenassistent fügt hier Member-Initialisierung ein
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFixObjDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFixObjDlg)
	DDX_Control(pDX, IDC_Drop, m_ctrlDrop);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFixObjDlg, CDialog)
	//{{AFX_MSG_MAP(CFixObjDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFixObjDlg Nachrichten-Handler

BOOL CFixObjDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden
	
   SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE);
	
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CFixObjDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CFixObjDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFixObjDlg::OnDropFiles(HDROP hDropInfo) 
{
   int nFiles = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
   for (int i=0; i<nFiles; ++i)
   {
      int nFileNameLen = ::DragQueryFile(hDropInfo, i, NULL, 0);
      if (nFileNameLen > 0)
      {
         _TCHAR *ptszFileName = new _TCHAR[nFileNameLen + 1];
         ::DragQueryFile(hDropInfo, i, ptszFileName, nFileNameLen + 1);

         CString csFilename = ptszFileName;

         HRESULT hr = FixIt(csFilename);
         if (SUCCEEDED(hr))
         {
            if (hr != S_FALSE)
               MessageBox(_T("File corrected."), _T("Result"), MB_OK | MB_ICONINFORMATION);
            else
               MessageBox(_T("No change to file made."), _T("Result"), MB_OK | MB_ICONINFORMATION);
         }
         // else error message already displayed
            
      }
   }

	
	CDialog::OnDropFiles(hDropInfo);
}


class FileLineInputStream
{
private:
   CFile *m_pfileInput;
   BYTE   m_abBuffer[1024];
   UINT   m_nFileLength;
   UINT   m_nBufferContent;
   UINT   m_nBufferPos;

public:
   FileLineInputStream(CFile *fileInput)
   {
      m_pfileInput = fileInput;
      m_nFileLength = m_pfileInput->GetLength();
      ZeroMemory(m_abBuffer, sizeof m_abBuffer);
      m_nBufferContent = 0;
      m_nBufferPos = 0;
   }

   /**
    * Fills the given CString with bytes from the file up to and excluding
    * the next '\n'. If there is no '\n' the remainder of the file is used.
    * The CString can remain empty if there is only a '\n' in the line and 
    * nothing else.
    * 
    * Returns normally S_OK and if the end of the file is reached S_FALSE.
    */
   HRESULT ReadLine(CString &cstrLine)
   {
      if (cstrLine.GetLength() > 0)
         cstrLine.Empty();

      UINT nBufferLength = sizeof m_abBuffer;

      if (m_pfileInput->GetPosition() >= m_nFileLength && m_nBufferPos >= m_nBufferContent)
         return S_FALSE;

      // else: file or buffer has some bytes left


      while (true)
      {
         if (m_pfileInput->GetPosition() >= m_nFileLength && m_nBufferPos >= m_nBufferContent) 
         {
            // after at least one loop: file exhausted 
            break; // while
         }

         if (m_nBufferPos >= m_nBufferContent) // buffer empty
         {
            m_nBufferContent = m_pfileInput->Read(m_abBuffer, sizeof m_abBuffer);
            m_nBufferPos = 0;
         }

         // look for a '\n'
         int nSlashPos = -1;
         for (UINT n=m_nBufferPos; n<m_nBufferContent; ++n)
         {
            if (m_abBuffer[n] == '\n')
            {
               nSlashPos = n;
               break; // for
            }
         }

         if (nSlashPos >= 0) // there is a slash in this buffer
         {
            CString cstrPart((char *)(m_abBuffer+m_nBufferPos), nSlashPos-m_nBufferPos);
            cstrLine += cstrPart;

            m_nBufferPos = nSlashPos+1; // mark as used up to and including the slash
            break; // while
         }
         else // this buffer does not contain a slash, copy all
         {
            CString cstrPart((char *)(m_abBuffer+m_nBufferPos), m_nBufferContent-m_nBufferPos);
            cstrLine += cstrPart;

            m_nBufferPos = m_nBufferContent; // the buffer is used
            // no slash, no break, continue with reading
         }
      }

      return S_OK; // cstrLine may be empty but file was not eof (see beginning of method)

   }
};

HRESULT CFixObjDlg::FixIt(CString &csFilename)
{
   HRESULT hr = S_OK;

   CWaitCursor wait;

   CFile inputFile;
   CFileException fileException;
   
   if (!inputFile.Open(csFilename, CFile::modeRead, &fileException))
   {
      MessageBox(_T("File could not be opened."), _T("Error"), MB_OK | MB_ICONEXCLAMATION);

      hr = E_INVALIDARG;
   }
   else
      inputFile.Close();


   if (SUCCEEDED(hr))
   {
      int nLastSlashPosition = csFilename.ReverseFind(_T('\\'));
      int nLastDotPos = csFilename.ReverseFind(_T('.'));
      CString csSuffix;
      if (nLastDotPos > 0)
         csSuffix = csFilename.Right(csFilename.GetLength() - nLastDotPos - 1);

      if (csSuffix.GetLength() > 0 && (csSuffix.CollateNoCase(_T("obj")) == 0 || csSuffix.CollateNoCase(_T("evq")) == 0))
      {
         // nothing to be done, everything alright
      }
      else
      {
         MessageBox(_T("Wrong file format!\nFile suffix must be \".evq\" or \".obj\"."), _T("Error"), MB_OK | MB_ICONEXCLAMATION);

         hr = E_INVALIDARG;
      }


      CString cstrOrigFileName;
      if (SUCCEEDED(hr))
      {
         cstrOrigFileName = csFilename.Left(nLastDotPos);
         cstrOrigFileName += _T("_orig.");
         cstrOrigFileName += csSuffix;
      
         if (!::CopyFile(csFilename, cstrOrigFileName, FALSE))
         {
            MessageBox(_T("Backup copy of the file could not be created."), _T("Error"), MB_OK | MB_ICONEXCLAMATION);

            hr = E_FAIL;
         }
         
      }

      bool bInputFileOpened = false;
      if (SUCCEEDED(hr))
      {
         if (!inputFile.Open(cstrOrigFileName, CFile::modeRead, &fileException))
         {
            MessageBox(_T("File could not be opened."), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
            
            hr = E_INVALIDARG;
         }
         else
            bInputFileOpened = true;
      }
      
      CFile outputFile;
      bool bOutputFileOpened = false;
      if (SUCCEEDED(hr))
      {
         if (!outputFile.Open(csFilename, CFile::modeWrite | CFile::modeCreate, &fileException))
         {
            MessageBox(_T("File could not be overwritten."), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
            
            hr = E_INVALIDARG;
         }
         else
            bOutputFileOpened = true;
      }

      if (SUCCEEDED(hr))
      {
         if (csSuffix.CollateNoCase(_T("obj")) == 0)
            hr = ProcessObjFile(inputFile, outputFile);
         else if (csSuffix.CollateNoCase(_T("evq")) == 0)
            hr = ProcessEvqFile(inputFile, outputFile);
      }


      if (bInputFileOpened)
         inputFile.Close();
      if (bOutputFileOpened)
         outputFile.Close();
      
   }


   return hr;
}

// private
HRESULT CFixObjDlg::ProcessObjFile(CFile &fileInput, CFile &fileOutput)
{
   HRESULT hr = S_OK;

   bool bStartFound = false;
   BYTE buffer[1024];
   
   
   UINT nTotalRead = 0;
   CByteArray lineArray;

   bool bSomeChangeDone = false;
   
   
   int nFileLength = fileInput.GetLength();
   
   
   UINT nRead = fileInput.Read(buffer, sizeof buffer);
   while(nRead > 0)
   {
      nTotalRead += nRead;
      
      if (!bStartFound)
      {
         if (strncmp((char *)buffer, "<WB ", 4) == 0)
            bStartFound = true;
         else
         {
            MessageBox(_T("File contains no object data."), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
            
            hr = E_INVALIDARG;
            break;
         }
      }
      
      int nFilePos = fileInput.GetPosition();
      
      
      UINT nBufferPos = 0;
      for (; nBufferPos<nRead; ++nBufferPos)
      {
         lineArray.Add(buffer[nBufferPos]);
         
         // second parameter means: last line in file is not ended with a '\n'
         if (buffer[nBufferPos] == '\n' || (nFilePos == nFileLength && nBufferPos+1 == nRead))
         {
            // line complete
            
            char *pszLine = new char[lineArray.GetSize() + 1];
            memcpy(pszLine, lineArray.GetData(), lineArray.GetSize());
            pszLine[lineArray.GetSize()] = 0;
            
            // maybe change the line
            bool bWriteFixLinePt = false;
            if (strncmp(pszLine, "<POLYLINES ", 11) == 0)
            {
               char *pszCount = strstr(pszLine, "count=0");
               if (NULL != pszCount)
               {
                  *(pszCount+6) = '1';
                  bWriteFixLinePt = true;
               }
            }
            
            
            fileOutput.Write(pszLine, lineArray.GetSize());
            
            if (bWriteFixLinePt)
            {
               char *fixLine = "<pt x=-1000 y=-1000></pt>\r\n";
               fileOutput.Write(fixLine, strlen(fixLine));

               bSomeChangeDone = true;
            }
            
            delete[] pszLine;
            
            
            lineArray.RemoveAll();
         }
      }
      
      
      nRead = fileInput.Read(buffer, sizeof buffer);
   }

   if (SUCCEEDED(hr) && !bSomeChangeDone)
      return S_FALSE;

   return hr;
}

// private
HRESULT CFixObjDlg::ProcessEvqFile(CFile &fileInput, CFile &fileOutput)
{
   HRESULT hr = S_OK;

   FileLineInputStream stream(&fileInput);

   CString csLine;
   CString csPreviousLine;

   bool bSomeChangeDone = false;

   hr = stream.ReadLine(csLine);
   while (SUCCEEDED(hr))
   {
      if (!csPreviousLine.IsEmpty())
      {
         bool bTimesIdentical = false;

         int nSpacePosOne = csPreviousLine.Find(_T(' '), 0);
         int nSpacePosTwo = csLine.Find(_T(' '), 0);
         if (nSpacePosOne > 0 && nSpacePosTwo > 0 && nSpacePosOne == nSpacePosTwo)
         {
            CString csTimeOne = csPreviousLine.Left(nSpacePosOne);
            CString csTimeTwo = csLine.Left(nSpacePosTwo);

            if (csTimeOne.Compare(csTimeTwo) == 0)
            {
               // times equal

               /*
               nSpacePosOne = csPreviousLine.Find(_T(' '), nSpacePosOne + 1);
               nSpacePosTwo = csLine.Find(_T(' '), nSpacePosTwo + 1);

               if (nSpacePosOne > 0 && nSpacePosTwo > 0)
               {
                  CString csObjectsOne = csPreviousLine.Right(csPreviousLine.GetLength() - nSpacePosOne - 1);
                  CString csObjectsTwo = csLine.Right(csLine.GetLength() - nSpacePosTwo - 1);

                  if (csObjectsOne.Compare(csObjectsTwo) == 0)
                  {
                     bLinesIdentical = true;
                  }
               }
               */
               bTimesIdentical = true;
            }
         }

         if (!bTimesIdentical)
         {
            // only works with non-UNICODE
            fileOutput.Write(csPreviousLine, csPreviousLine.GetLength());
            fileOutput.Write("\n", 1);
         }
         else
         {
            // csPreviousLine is simply discarded
            bSomeChangeDone = true;
         }
      }
 

      csPreviousLine = csLine;
      hr = stream.ReadLine(csLine);

      if (SUCCEEDED(hr) && hr == S_FALSE)
         break; // no more data
      
   }

   if (SUCCEEDED(hr) && csPreviousLine.GetLength() > 0)
   {
      // write last line without trailing \n
      fileOutput.Write(csPreviousLine, csPreviousLine.GetLength());
   }

   if (SUCCEEDED(hr))
   {
      if (bSomeChangeDone)
         hr = S_OK;
      else
         hr = S_FALSE;
   }

   return hr;
}
