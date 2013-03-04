#include "StdAfx.h"
#include "changeurlcmd.h"
#include "resource.h"

int __cdecl _tmain(int argc, _TCHAR **argv, _TCHAR **envp)
{
   ::AfxSetResourceHandle((HINSTANCE) ::GetModuleHandle(NULL));

   if (argc < 3)
   {
      CString csParms1;
      csParms1.LoadString(IDS_MSG_CMDPARAMS1);
      _tprintf(csParms1);
      CString csParms2;
      csParms2.LoadString(IDS_MSG_CMDPARAMS2);
      _tprintf(csParms2);

      return 1;
   }

   CChangeUrlCmd changeUrlCmd;
   // "Step 1"
   changeUrlCmd.SetDocumentName(argv[1]);

   bool success = true;

   // Parse the command line
   int nTargetType = REPLAY_TYPE_LOCAL;
   CString csUrlHttp(_T(""));
   CString csUrlStreaming(_T(""));
   CString csPathSmil(_T(""));
   changeUrlCmd.SetSmilFileName(_T(""));

   int nCurrentArg = 2;
   while (nCurrentArg < argc)
   {
      CString csArg = argv[nCurrentArg];
      if (_tcsicmp(csArg, _T("-local")) == 0)
      {
         nTargetType = REPLAY_TYPE_LOCAL;
         nCurrentArg++;
      }
      else if (_tcsicmp(csArg, _T("-http")) == 0)
      {
         nTargetType = REPLAY_TYPE_HTTP;
         nCurrentArg++;
         if (nCurrentArg < argc)
         {
            csUrlHttp = argv[nCurrentArg];
            nCurrentArg++;
         }
         else
         {
            CString csMsg;
            csMsg.LoadString(IDS_ERR_NOHTTPGIVEN);
            _tprintf(csMsg);
            success = false;
         }
      }
      else if (_tcsicmp(csArg, _T("-streaming")) == 0)
      {
         nTargetType = REPLAY_TYPE_STREAMING;
         nCurrentArg++;
         if (nCurrentArg < argc)
         {
            csUrlStreaming = argv[nCurrentArg];
            nCurrentArg++;
            if (nCurrentArg < argc)
            {
               CString csTmp = argv[nCurrentArg];
               if (csTmp.GetAt(0) != _T('-'))
               {
                  csUrlHttp = csTmp;
                  nCurrentArg++;
               }
            }
         }
         else
         {
            CString csMsg;
            csMsg.LoadString(IDS_ERR_NOSTREAMINGGIVEN);
            _tprintf(csMsg);
            success = false;
         }
      }
      else if (_tcsicmp(csArg, _T("-smil")) == 0)
      {
         nCurrentArg++;
         if (nCurrentArg < argc)
         {
            csPathSmil = argv[nCurrentArg];
            //_tprintf(_T("SMIL File: %s\n"), csPathSmil);
            nCurrentArg++;
         }
         else
         {
            CString csMsg;
            csMsg.LoadString(IDS_ERR_NOSMILGIVEN);
            _tprintf(csMsg);
            success = false;
         }
      }
      else
      {
         CString csMsg;
         csMsg.Format(IDS_ERR_UNKNOWNPARAMETER, csArg);
         _tprintf(csMsg);
         success = false;
      }
   }

   if (success)
   {
      changeUrlCmd.SetSmilFileName(csPathSmil);
   }


   if (success)
      success = changeUrlCmd.ResolveHtmlFileNames();
   if (success)
      success = changeUrlCmd.RetrieveCurrentType();

   // "Step 2"
   if (success)
   {
      changeUrlCmd.SetTargetType(nTargetType);

      switch (nTargetType)
      {
      case REPLAY_TYPE_LOCAL:
         changeUrlCmd.SetUrlHttp(_T(""));
         changeUrlCmd.SetUrlStreaming(_T(""));
         break;

      case REPLAY_TYPE_HTTP:
         changeUrlCmd.SetUrlHttp(csUrlHttp);
         changeUrlCmd.SetUrlStreaming(_T(""));
         break;

      case REPLAY_TYPE_STREAMING:
         changeUrlCmd.SetUrlHttp(csUrlHttp);
         changeUrlCmd.SetUrlStreaming(csUrlStreaming);
         break;
      }
   }

   /*
   CString csTargetType = argv[2];
   if (success)
   {
      if (csTargetType == _T("-local"))
      {
         changeUrlCmd.SetTargetType(REPLAY_TYPE_LOCAL);

         changeUrlCmd.SetUrlHttp(_T(""));
         changeUrlCmd.SetUrlStreaming(_T(""));
         nSmilOffset = 3;
      }
      else if (csTargetType == _T("-http"))
      {
         changeUrlCmd.SetTargetType(REPLAY_TYPE_HTTP);

         if (argc >= 4)
         {
            changeUrlCmd.SetUrlHttp(argv[3]);
            changeUrlCmd.SetUrlStreaming(_T(""));
            nSmilOffset = 4;
         }
         else
         {
            CString csMsg;
            csMsg.LoadString(IDS_ERR_NOHTTPGIVEN);
            _tprintf(csMsg);
            success = false;
         }
      }
      else if (csTargetType == _T("-streaming"))
      {
         changeUrlCmd.SetTargetType(REPLAY_TYPE_STREAMING);

         if (argc >= 4)
         {
            changeUrlCmd.SetUrlStreaming(argv[3]);
            if (argc >= 5)
            {
               CString csTmp(argv[4]);
               if (csTmp != _T("-smil"))
               {
                  changeUrlCmd.SetUrlHttp(argv[4]);
                  nSmilOffset = 5;
               }
               else
               {
                  nSmilOffset = 4;
               }
            }
            else
            {
               changeUrlCmd.SetUrlHttp(_T(""));
               nSmilOffset = 4;
            }
         }
         else
         {
            CString csMsg;
            csMsg.LoadString(IDS_ERR_NOSTREAMINGGIVEN);
            _tprintf(csMsg);
            success = false;
         }
      }
      else
      {
         CString csMsg;
         csMsg.Format(IDS_ERR_UNKNOWNPARAMETER, csTargetType);
         _tprintf(csMsg);
         success = false;
      }
   }
   */

   // "Step 3"
   if (success)
      success = changeUrlCmd.UpdateUrls();

   return success ? 0 : 1;
}

void CChangeUrlCmd::LogMessage(CString csMessage, UINT nBeep)
{
   ReplaceSpecialCharactersInString(csMessage);

   if (nBeep == MB_ICONERROR)
   {
      CString csError;
      csError.LoadString(IDS_ERROR);
      _tprintf(csError);
      _tprintf(_T(": "));
   }

   _tprintf(csMessage);
   _tprintf(_T("\n"));
}

bool CChangeUrlCmd::QuestionMessage(CString csMessage)
{
   bool res = false;

   ReplaceSpecialCharactersInString(csMessage);

   CString csQuestion;
   csQuestion.LoadString(IDS_QUESTION);
   _tprintf(csQuestion);
   _tprintf(_T(": "));

   _tprintf(csMessage);
   _tprintf(_T("\n"));
   CString csYesNo;
   csYesNo.LoadString(IDS_YESNO);
   _tprintf(csYesNo);

   char c;
   int result = scanf_s("%c", &c);
   _tprintf(_T("\n"));
   if (result > 0)
   {
      if (csYesNo.Compare(_T("(J/N)?")) == 0)
      {
         if ((c == 'j') || (c == 'J'))
            res = true;
      }
      if (csYesNo.Compare(_T("(Y/N)?")) == 0)
      {
         if ((c == 'y') || (c == 'Y'))
            res = true;
      }
   }

   return res;
}

void CChangeUrlCmd::ReplaceSpecialCharactersInString(CString &csMsg)
{
   csMsg.Replace(_T("Ä"), _T("Ae"));
   csMsg.Replace(_T("Ö"), _T("Oe"));
   csMsg.Replace(_T("Ü"), _T("Ue"));
   csMsg.Replace(_T("ä"), _T("ae"));
   csMsg.Replace(_T("ö"), _T("oe"));
   csMsg.Replace(_T("ü"), _T("ue"));
   csMsg.Replace(_T("ß"), _T("ss"));
}

bool CChangeUrlCmd::RequestSmilFileName(CString &csSmilFileName)
{
   if (m_csSmilFileName != _T(""))
   {
      csSmilFileName = m_csSmilFileName;
      return true;
   }

   csSmilFileName = _T("");
   return false;
}

void CChangeUrlCmd::SetSmilFileName(LPCTSTR tszSmilFileName) 
{
   CChangeUrlEngine::SetSmilFileName(tszSmilFileName);
   m_csSmilFileName = tszSmilFileName; 
}
