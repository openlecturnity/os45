#include "global.h"

void AvGrabberDebugMsg(char *szFormat, bool bPrefix);

//LONG WINAPI EHandler(struct _EXCEPTION_POINTERS* ExceptionInfo) {
//    return EXCEPTION_CONTINUE_EXECUTION;
//}

void RunDiagnosisInternal() {
    //::SetUnhandledExceptionFilter(EHandler);

    ::MessageBox(NULL, _T("Test started."), _T("LECTURNITY"), MB_OK);
    
    
    DWORD dwTime1 = timeGetTime();

    char *szMessage = new char[300];


    {
        AvGrabber *grabber = new AvGrabber;
        int iCount = grabber->getAudioDeviceCount();

        sprintf_s(szMessage, 300, "Number audio devices: %d\n", iCount);
        AvGrabberDebugMsg(szMessage, true);

        AudioDevice **paDevices = new AudioDevice*[iCount];
        for (int i=0; i<iCount; ++i)
            paDevices[i] = new AudioDevice;

        grabber->getAudioDevices(paDevices);

        for (int i=0; i<iCount; ++i) {
            sprintf_s(szMessage, 300, " - %S: #%d %d\n", paDevices[i]->tszName, paDevices[i]->nSampleRates, 
                paDevices[i]->nSampleRates > 0 ? paDevices[i]->sampleRates[0] : 0);
            AvGrabberDebugMsg(szMessage, false);
        }

        for (int i=0; i<iCount; ++i)
            delete paDevices[i];

        delete paDevices;

        delete grabber;
    }

    
    {
        AvGrabber *grabber1 = new AvGrabber;

        int iCount = grabber1->getAudioDeviceCount();
        if (iCount > 0) {
            AudioMixer *pMixer = grabber1->setAudioDevice(0, 22050, 16);

            sprintf_s(szMessage, 300, "Mixer created: enabled %d, volume %d\n", 
                pMixer->isEnabled(), pMixer->getVolume());
            AvGrabberDebugMsg(szMessage, true);
        }

        delete grabber1;
    }

   /*
   for (int i=0; i<10; ++i)
   {
      
      AvGrabber grabber;
      int iCount = grabber.getAudioDeviceCount();
      if (iCount > 0)
      {
         AudioDevice *aDevices = new AudioDevice[iCount];
         
         
         AudioMixer *pMixer = grabber.setAudioDevice(0, 22050, 16);
         
         grabber.start("c:\\avgrabber_audio");
         
         int iEnd = 40;
         if (i == 9)
            iEnd = 400;

         for (int j=0; j<iEnd; ++j)
         {
            grabber.getAudioLevel();
            grabber.getAudioState();
            ::Sleep(50);
            grabber.getRecordingTime();
            ::Sleep(50);
         }

         grabber.getAudioDataRate();
         
         grabber.stop();
         
         
         delete aDevices;
      }
   }
   */

   /*
   {
      AvGrabber grabber1;
      int iCount = grabber1.getAudioDeviceCount();
      if (iCount > 0)
      {
         AudioMixer *pMixer = grabber1.setAudioDevice(0, 22050, 16);
         grabber1.start("c:\\avgrabber_audio");

         Sleep(50);

         AvGrabber grabber2;
         grabber2.setAudioDevice(0, 22050, 16);
         grabber2.start("c:\\avgrabber_audio2");

         pMixer->getVolume();

         Sleep(3000);


         grabber1.getAudioLevel();
         grabber2.getAudioLevel();

         grabber1.stop();
         grabber2.stop();
      }
       
   }

   ::DeleteFile("c:\\avgrabber_audio.wav");
   ::DeleteFile("c:\\avgrabber_audio2.wav");
*/

  
   /*
   AvGrabber grabberSg;
   grabberSg.initScreenCapturer();

   int iCount = grabberSg.getSgCodecCount();
 
   CodecInfo **paCodecs = new CodecInfo*[iCount];
   ZeroMemory(paCodecs, iCount * sizeof (CodecInfo*));
   for (int i=0; i<iCount; ++i)
      paCodecs[i] = new CodecInfo();
   grabberSg.getSgCodecs(paCodecs);

   
   
   DWORD dwTime2 = timeGetTime();

   _TCHAR tszMessage[200];

   _stprintf(tszMessage, _T("Version 4: Test abgeschlossen; %d Sekunden"), (dwTime2-dwTime1)/1000);

   ::MessageBox(NULL, tszMessage, _T("Ergebnis"), MB_OK);
   */

   ::MessageBox(NULL, _T("Test finished."), _T("LECTURNITY"), MB_OK);

   delete[] szMessage;
}

void CALLBACK RunDiagnosis(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
    __try {
        RunDiagnosisInternal();
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        // Also catches crashes and the like
        ::MessageBoxA(NULL, "During the test an error occured.", "Error", MB_OK | MB_ICONERROR);
    }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow ) {

    RunDiagnosis(NULL, NULL, NULL, 0);

    return 0;
}