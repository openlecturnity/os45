   /* Das geht auf jeden Fall gar nicht (unzulässiger Cast):
      IBaseFilter *pInstance = (IBaseFilter *)CWavDestFilter::CreateInstance(NULL, &hr);
      CComPtr< IBaseFilter > pTransform(pInstance);
      pInstance = NULL;
      */
      
      /* Warum geht das nicht?????????
      CWavDestFilter transform(NULL, &hr);
      CComQIPtr< IBaseFilter, &IID_IBaseFilter > pTransform(&transform);
      */

      
      /*
      CWavDestFilter *pTransformDst = new CWavDestFilter(NULL, &hr);
      if (SUCCEEDED(hr))
      hr = pTransformDst->QueryInterface(IID_IBaseFilter, (void **) &pTransform);
      //pTransformDst->Release();
      pTransformDst = NULL;
      //*/
      /*
      CComPtr< IBaseFilter > pTransform;
      CWavDestFilter transform(NULL, &hr);
      if (SUCCEEDED(hr))
         hr = transform.QueryInterface(IID_IBaseFilter, (void **) &pTransform);
      //*/
      /*
      CWavDestFilter *pInstance = new CWavDestFilter(NULL, &hr);
      CComQIPtr< IBaseFilter, &IID_IBaseFilter > pTransform(pInstance);
      pInstance->Release();
      */

      /*
      else
      {
         
         CComPtr< IGraphBuilder > pGraph;
         if (SUCCEEDED(hr))
         {
            hr = pBuilder->GetFiltergraph(&pGraph);
         }
         
         if (SUCCEEDED(hr))
         {
            hr = pGraph->AddFilter(pTransform, L"Wav Dest");
         }
         
         if (SUCCEEDED(hr))
         {
            hr = pGraph->AddFilter(pMux, L"File Writer");
         }
 
                  
         //MessageBox(NULL, "Wait", "for me", MB_OK);

         
         // TODO: propper error checking ??
       
         CComPtr< IPin > pIn = GetPin(pTransform, PINDIR_INPUT);
         if (!pIn)
            hr = E_FAIL;

         if (SUCCEEDED(hr))
            hr = pGraph->ConnectDirect(pPin, pIn, NULL);

           
         CComPtr< IPin > pOut = GetPin(pTransform, PINDIR_OUTPUT); 
         if (!pOut) 
            hr = E_FAIL;

         CComPtr< IPin > pIn2 = GetPin(pMux, PINDIR_INPUT);
         if (!pIn2)
            hr = E_FAIL;
         
         if (SUCCEEDED(hr))
            hr = pGraph->ConnectDirect(pOut, pIn2, NULL);
         
         
 
      } // isAudio
      //*/
 


   
      /*
      if (SUCCEEDED(hr))
      {
         hr = ::CoCreateInstance(CLSID_FileWriter, NULL, CLSCTX_INPROC,
            IID_IBaseFilter, (void **) &pMux);
      } 
      //*/
  


      
   /*   
      //MessageBox(NULL, "ist es", "AUDIO", MB_OK);
   }
   else
   {
      
      if (SUCCEEDED(hr))
      {
         hr = pBuilder->SetOutputFileName(
            &MEDIASUBTYPE_Avi, _bstr_t(szOutputPath), &pMux, NULL);
      }

      if (SUCCEEDED(hr) && pMux)
      {
         CComQIPtr< IFileSinkFilter2 > pSink(pMux);
         if (pSink)
         {
            pSink->SetMode(AM_FILE_OVERWRITE); // delete file before overwriting it
         }
      }
   }
   //*/




IPin *GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir)
{
    BOOL       bFound = FALSE;
    IEnumPins  *pEnum;
    IPin       *pPin;

    pFilter->EnumPins(&pEnum);
    while(pEnum->Next(1, &pPin, 0) == S_OK)
    {
        PIN_DIRECTION PinDirThis;
        pPin->QueryDirection(&PinDirThis);
        if (bFound = (PinDir == PinDirThis))
            break;
        pPin->Release();
    }
    pEnum->Release();
    return (bFound ? pPin : 0);  
}





   // Test:
   // Was kann der AviSplitter oder andere
   CComPtr< IBaseFilter > pSplitter;
   // CLSID_AsyncReader, CLSID_AviSplitter
   ::CoCreateInstance(CLSID_AsyncReader, NULL, CLSCTX_INPROC_SERVER,
      IID_IBaseFilter, (void **) &pSplitter);

   CComQIPtr< IFileSourceFilter, &IID_IFileSourceFilter > pSource(pSplitter);
   pSource->Load(L"c:\\Sumpf\\BodyDivX.avi", NULL);

   CComPtr< IEnumPins > pEnum;
   pSplitter->EnumPins(&pEnum);

   CComPtr< IPin > pPin;
   for (i=0; (pEnum->Next(1, &pPin, NULL) == S_OK); ++i)
   {
      printf("Pin %d:\n", i);

      PIN_INFO info;
      pPin->QueryPinInfo(&info);

      wprintf(L"   Name: %s\n", info.achName);

      if (info.dir == PINDIR_OUTPUT)
         printf("   Dir: Output\n");
      else
         printf("   Dir: Input\n");



      CComPtr< IEnumMediaTypes  > pEnum2;
      pPin->EnumMediaTypes(&pEnum2);

      AM_MEDIA_TYPE *type = new AM_MEDIA_TYPE;
      ZeroMemory(type, sizeof AM_MEDIA_TYPE);
      bool oneFound = false;

      for (int j=0; (pEnum2->Next(1, &type, NULL) == S_OK); ++j)
      {
         oneFound = true;

         printf("   MediaType %d:\n", j);

         if (type->majortype == MEDIATYPE_Stream)
         {
            printf("      is stream\n");
         }
         else
         {
            printf("      is not stream\n");
         }

      }

      if (oneFound)
         DeleteMediaType(type);
      else
         delete type;
     
      pPin.Attach(NULL);
      
   }


   
/*

void writeMD(IMediaDet *det, bool del)
{
   BSTR s = NULL;
   det->get_Filename(&s);
   int l = wcslen(s);
   char *ns = (char *)malloc(l+1);
   wcstombs(ns, s, l+1);
   printf("%s %8s\n", del ? "Deleting " : "Setting ", ns);
   delete ns;
   SysFreeString(s);
}



IMediaDet* AVInfo::sm_pMediaDet = NULL;
char* AVInfo::sm_szSourceFile = NULL;

void AVInfo::StaticSet(char *path, IMediaDet *det) // static
{
   if (sm_szSourceFile)
      StaticDelete();

   writeMD(det, false);
      

   sm_szSourceFile = path;
   sm_pMediaDet = det;
}

void AVInfo::StaticDelete() // static
{
   if (sm_szSourceFile)
   {
      delete sm_szSourceFile;
      sm_szSourceFile = NULL;
      writeMD(sm_pMediaDet, true);
      sm_pMediaDet->Release();
      sm_pMediaDet = NULL;
      //delete sm_pMediaDet; 
      // ruft einen "benutzerdefinierten Haltepunkt" auf
   }
}
*/

