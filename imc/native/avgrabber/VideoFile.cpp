#include "global.h"
#include "resource.h"

//#include "VideoFile.h"
//#include "VideoCapturer.h"
#include "indeo_video.h"
//#include "rectkeeper.h"
#include "lutils.h"

using namespace std;

#ifdef _DEBUG
   static FILE *debugFile = _tfopen(_T("C:\\videofile.txt"), _T("w"));
#endif

VideoFile::VideoFile(_TCHAR* tszFileName) : RiffFile(tszFileName)
{
#ifdef DEBUG
	cout << "VideoFile::VideoFile(\"" << tszFileName << "\")" << endl;
#endif

   indexIndex_ = 0;

   // create the avi index:
   indexArray_ = NULL;
   indexArray_ = (AVIINDEXENTRY*) malloc(AVI_INDEX_SIZE * sizeof(AVIINDEXENTRY));
   m_nCurrentIndexLength = AVI_INDEX_SIZE;

   if (!indexArray_)
      throw VideoIOException(_T("Memory error!"));

	// set compBuffer_ to NULL before first malloc()
	compBuffer_ = NULL;
	lastFrameBuffer_ = NULL;
	cerr << "*** Constructor: set outputBitmapInfo_ to NULL" << endl;
	outputBitmapInfo_ = NULL;
	cerr << "*** Constructor: outputBitmapInfo_ == " << outputBitmapInfo_ << endl;
}

VideoFile::~VideoFile()
{
#ifdef DEBUG
	cout << "VideoFile::~VideoFile()" << endl;
#endif
   if (indexArray_)
      free(indexArray_);
   indexArray_ = NULL;

	// release compression buffer:
	if (compBuffer_ != NULL)
	{
		free(compBuffer_);
		compBuffer_ = NULL;
	}

	if (lastFrameBuffer_ != NULL)
	{
		free(lastFrameBuffer_);
		lastFrameBuffer_ = NULL;
	}

	if (outputBitmapInfo_ != NULL)
	{
		cerr << "*** Detructor: outputBitmapInfo_ == " << outputBitmapInfo_ << endl;
		free(outputBitmapInfo_);
		cerr << "*** Destructor: set outputBitmapInfo_ to NULL" << endl;
		outputBitmapInfo_ = NULL;
	}
}

void VideoFile::setMainAviHeader(MainAVIHeader* aviHeader)
{
	memcpy(&aviHeader_, aviHeader, sizeof MainAVIHeader);
}

void VideoFile::initCodec()
{
	if (codecInfo_.fccHandler == inputBitmapInfo_.bmiHeader.biCompression)
	{

		return;
	}

#ifdef _DEBUG
   fprintf(debugFile, "- codec handler: %s\n", codecInfo_.fccString);
   fprintf(debugFile, "- supports quality: %d\n", codecInfo_.bSupportsQuality);
   fprintf(debugFile, "- quality: %d\n", codecInfo_.iQuality);
   fprintf(debugFile, "- use keyframes: %d\n", codecInfo_.bUseKeyframes);
   fprintf(debugFile, "- keyframe rate: %d\n", codecInfo_.iKeyframeRate);
   fprintf(debugFile, "- default keyframe rate: %d\n", codecInfo_.iDefaultKeyframeRate);
   fprintf(debugFile, "- use datarate: %d\n", codecInfo_.bUseDatarate);
   fprintf(debugFile, "- datarate: %d\n", codecInfo_.bUseDatarate);
   fprintf(debugFile, "\n");
   fprintf(debugFile, "- frame rate: %d/%d\n", aviStreamHeader_.dwRate, aviStreamHeader_.dwScale);
   fflush(debugFile);
#endif
//   fclose(file);
//   file = NULL;

	cerr << "*** enabling recompression/compression" << endl;

	hic_ = ICOpen(ICTYPE_VIDEO, codecInfo_.fccHandler, ICMODE_COMPRESS);
	if (hic_ == 0)
		throw VideoIOException(Local::localizeString(VIDEO_CODEC_INITERR));

   // let's make sure again that the input format is supported:
   DWORD res = ICCompressQuery(hic_, &inputBitmapInfo_, NULL);

   if (res != ICERR_OK)
   {
      ICClose(hic_);

      VideoCapturer::throwVideoException(res);
   }

	// get output format from codec:
	outputBitmapInfoSize_ = ICCompressGetFormatSize(hic_, &inputBitmapInfo_);
	// reserve memory:
	if (outputBitmapInfo_ != NULL)
	{
		free(outputBitmapInfo_); // free it first eventually
		cerr << "*** initCodec: set outputBitmapInfo_ to NULL" << endl;
		outputBitmapInfo_ = NULL;
	}

	cerr << "*** initCodec: malloc " << outputBitmapInfoSize_ << " bytes for outputBitmapInfo_" << endl;
	outputBitmapInfo_ = (BITMAPINFO*) malloc(outputBitmapInfoSize_);
	// now get format
	res = ICCompressGetFormat(hic_, &inputBitmapInfo_, outputBitmapInfo_);

	if (res != ICERR_OK)
	{
		ICClose(hic_);
      VideoCapturer::throwVideoException(res);
	}

#ifdef DEBUG
	cout << "inputBitmapInfo_.bmiHeader.biSizeImage == " << inputBitmapInfo_.bmiHeader.biSizeImage << endl;
	cout << "outputBitmapInfo_.bmiHeader.biSizeImage == " << outputBitmapInfo_->bmiHeader.biSizeImage << endl;
	cout << "Query: " << ICCompressQuery(hic_, &inputBitmapInfo_, outputBitmapInfo_) << endl;
	cout << "  ICERR_OK == " << ICERR_OK << endl;
#endif

	// IV50 is very obstinate: let's try to set the quality
	// value by brute force?
	if (codecInfo_.dwFlags & VIDCF_QUALITY)
	{
		cerr << " * set quality to " << codecInfo_.iQuality << endl;
		DWORD setFuckingQualityNow = codecInfo_.iQuality;
		cerr << " * address of quality DWORD: " << (DWORD) &setFuckingQualityNow << endl;
		ICSendMessage(hic_, ICM_SETQUALITY, setFuckingQualityNow, 0);
	}

	// tickle the indeo codec:
	enableIndeoQualityControl();

	// This message has to be sent to different codecs,
   // amongst them is the very screwy IV50 codec.
	ICCOMPRESSFRAMES icf;

	memset(&icf, 0, sizeof icf);

	icf.dwFlags		= 0;
	icf.lpbiOutput	= &outputBitmapInfo_->bmiHeader; // (LPBITMAPINFOHEADER)pbiOutput;
	icf.lpbiInput	= &inputBitmapInfo_.bmiHeader; // (LPBITMAPINFOHEADER)pbiInput;
	icf.lStartFrame = 0;
	icf.lFrameCount = 0x0FFFFFFF;
	icf.lQuality	= codecInfo_.iQuality;
   if (codecInfo_.bSupportsDatarate && codecInfo_.bUseDatarate)
      icf.lDataRate = codecInfo_.iDatarate * 1024; // bytes/s
   else
      icf.lDataRate = 0; // 0x7fffffff; // no rate limit

   icf.lKeyRate	= codecInfo_.iKeyframeRate;
	icf.dwRate		= 1000000;
	icf.dwScale		= aviStreamHeader_.dwScale;

   // Ok, now send the message to the compressor:
	ICSendMessage(hic_, ICM_COMPRESS_FRAMES_INFO, (WPARAM)&icf, sizeof(ICCOMPRESSFRAMES));

	res = ICCompressBegin(hic_, &inputBitmapInfo_, outputBitmapInfo_);
	if (res != ICERR_OK)
	{
		cerr << "ICCompressBegin() failed!" << endl;
		ICClose(hic_);
		hic_ = 0;
		throw VideoIOException(Local::localizeString(VIDEO_CODEC_BEGINERR));
	}

	if (compBuffer_ != NULL)
		free(compBuffer_);
   compBuffer_ = NULL;

	compBufferSize_ = ICCompressGetSize(hic_, &inputBitmapInfo_, outputBitmapInfo_);
#ifdef DEBUG
	cout << "Compressed image buffer size: " << compBufferSize_ << endl;
#endif

	// note that in the avi header:
	aviStreamHeader_.dwSuggestedBufferSize = compBufferSize_;

	compBuffer_ = malloc(compBufferSize_);
	if (compBuffer_ == NULL)
	{
		cerr << "Could not malloc() compressed image buffer!" << endl;
		ICClose(hic_);
		hic_ = 0;
		throw MemoryVideoException(Local::localizeString(VIDEO_NOMEM_BITMAP));
	}

	temporalCompress_ = codecInfo_.bSupportsKeyframes;

	if (codecInfo_.bSupportsKeyframes && !(codecInfo_.dwFlags & VIDCF_FASTTEMPORALC))
	{
		// VIDCF_TEMPORAL flag is set, but not FASTTEMPORALC, 
		// then we need a backbuffer!

		fastTemporalCompress_ = false;

		if (lastFrameBuffer_ != NULL)
			free(lastFrameBuffer_);
		lastFrameBuffer_ = NULL;

		lastFrameBuffer_ = malloc(inputBitmapInfo_.bmiHeader.biSizeImage);
		if (lastFrameBuffer_ == NULL)
		{
			ICClose(hic_);
			hic_ = NULL;
			cerr << "Could not malloc() backbuffer for VIDCF_TEMPORAL!" << endl;
			throw MemoryVideoException(Local::localizeString(VIDEO_NOMEM_BITMAP));
		}
	}

	// if fast temporal compression is supported, use that!
	if (codecInfo_.dwFlags & VIDCF_FASTTEMPORALC)
	{
		temporalCompress_ = false;
		fastTemporalCompress_ = true;
	}
	else
		fastTemporalCompress_ = false;

	DWORD driverQ = 0;
	ICSendMessage(hic_, ICM_GETQUALITY, (DWORD) (LPVOID) &driverQ, 0);
	cerr << "*** DRIVER QUALITY SETTING: " << driverQ << endl;

	// reset keyframe counter:
	lastKeyFrame_ = 0;

   // PZI: set structing performance optimization for LSGC
   InitStructuringOptimization();
}

void VideoFile::writeAviHeaders()
{
	openRiffChunk(_T("AVI ")); // open RIFF 'AVI '
	openListChunk(_T("hdrl")); // open LIST 'hdrl'
   openChunk(_T("avih")); // open 'avih'

	// we need this file position here:
	MMRESULT mmres = mmioGetInfo(hmmio_, &mmioInfo, 0);
	mainAviHeaderOffset_ = mmioInfo.lDiskOffset;

	writeMainAVIHeader();
	closeChunk(); // close 'avih'

	openListChunk(_T("strl")); // open LIST 'strh'
	// avi stream header, only one, no audio!
	openChunk(_T("strh")); // open 'strh'

	// we need this file offset, too:
	mmres = mmioGetInfo(hmmio_, &mmioInfo, 0);
	videoStreamHeaderOffset_ = mmioInfo.lDiskOffset;

	writeData(&aviStreamHeader_, sizeof LecturnityAVIStreamHeader);

	closeChunk(); // close 'strh'

	openChunk(_T("strf")); // open 'strf'
	if (codecInfo_.fccHandler != inputBitmapInfo_.bmiHeader.biCompression)
		writeData(outputBitmapInfo_, outputBitmapInfoSize_);
	else
		writeData(&inputBitmapInfo_, bitmapInfoSize_);

	// retrieve file position for the 'JUNK' chunk
	mmres = mmioGetInfo(hmmio_, &mmioInfo, 0);
	long filePos = mmioInfo.lDiskOffset;

	closeChunk(); // close 'strf'
	closeChunk(); // close LIST 'strl'
	closeChunk(); // close the 'hdrl' chunk

	// calculate size of JUNK chunk:
	long padLength = 2048 - filePos - 8;
	if (padLength < 0) padLength += 2048;

#ifdef DEBUG
	cout << "Current file position is " << filePos << ", padding " << padLength << endl;
#endif

	writeJunkChunk(padLength);

	// now we open the 'movi' chunk:
	openListChunk(_T("movi")); // open LIST 'movi'
}

void VideoFile::writeMainAVIHeader()
{
	writeData(&aviHeader_, sizeof(MainAVIHeader));
}

void VideoFile::setAVIStreamHeader(LecturnityAVIStreamHeader* aviStreamHeader,
								   int aviStreamHeaderSize)
{
	memcpy(&aviStreamHeader_, aviStreamHeader, aviStreamHeaderSize);
	aviStreamHeaderSize_ = aviStreamHeaderSize;

	// set the compressor's FCC
	aviStreamHeader_.fccHandler = codecInfo_.fccHandler;
}

void VideoFile::setBitmapInfoHeader(BITMAPINFO* bitmapInfo, int bitmapInfoSize)
{
	memcpy(&inputBitmapInfo_, bitmapInfo, bitmapInfoSize);
	bitmapInfoSize_ = bitmapInfoSize;

	// copy it to the output format:
	// memcpy(&outputBitmapInfo_, &inputBitmapInfo_, bitmapInfoSize); // hell no!
	// set the fccHandler of the outputBitmapInfo_ structure
	// outputBitmapInfo_.bmiHeader.biCompression = codecInfo_.fccHandler; // no no no!
}

DWORD VideoFile::writeFrame(void* data, int dataSize, DWORD frameQuota, DWORD dwFlags,
                            DWORD dwUser, QCDATA *pQcData, SrMouseData *pMouseData, 
                            SrClickData *pClickData, DWORD dwTimeCapturedMs)
{
#ifdef DEBUG
    cout << "writeFrame()" << endl;
#endif

    static MMIOINFO mmioInfo;

    if (indexIndex_ == m_nCurrentIndexLength)
    {
        // enlarge index array, should normally not be necessary (initial size for 3 hours)

        AVIINDEXENTRY* aIndexArrayNew = (AVIINDEXENTRY*) malloc(2 * m_nCurrentIndexLength * sizeof(AVIINDEXENTRY));
        memcpy(aIndexArrayNew, indexArray_, m_nCurrentIndexLength * sizeof(AVIINDEXENTRY));
        m_nCurrentIndexLength *= 2;
        delete indexArray_;
        indexArray_ = aIndexArrayNew;
    }

    AVIINDEXENTRY* indexEntry = &indexArray_[indexIndex_++];
    if (indexEntry == NULL)
        return false;

    aviStreamHeader_.dwLength++;
    aviHeader_.dwTotalFrames++;


    if (data == NULL && dataSize == 0)
    {
        // Special case for a null frame.

        openChunk(_T("00db"));

        mmioGetInfo(hmmio_, &mmioInfo, 0);
        indexEntry->ckid = mmioStringToFOURCC(_T("00db"), 0);
        indexEntry->dwChunkOffset = mmioInfo.lDiskOffset - 8; // 8 bytes == RIFF header
        indexEntry->dwChunkLength = 0;
        indexEntry->dwFlags       = 0;

        closeChunk();

        return 0;
    }


    bool res = true;
    DWORD returnFlags = 0;
    DWORD ckid = 0;

    DWORD compressResult = ICERR_OK;


#ifdef _DEBUG
    int actTime = (int) ((aviStreamHeader_.dwLength - 1) * aviStreamHeader_.dwScale / 1000);
    fprintf(debugFile, "Writing frame at %d ms, dwTimeCaptured == %d, delta == %d\n", actTime, dwTimeCapturedMs, actTime - dwTimeCapturedMs);
    fflush(debugFile);

    if (abs((int)(actTime - dwTimeCapturedMs)) > aviStreamHeader_.dwScale / 2000 + 2)
        printf("~~~~~~~~~~~~ DEVIATION frame/written TOO LARGE ~~~~~~~~~~~~~~\n");
#endif


    if (codecInfo_.fccHandler != inputBitmapInfo_.bmiHeader.biCompression)
    {
        // compress the video stream with a codec

        bool makeKeyFrame = false;
        if (codecInfo_.bUseKeyframes)
            makeKeyFrame = (aviStreamHeader_.dwLength - lastKeyFrame_ >= codecInfo_.iKeyframeRate);
        if (aviStreamHeader_.dwLength == 1) // first frame
            makeKeyFrame = true;

#ifdef _DEBUG
        if (makeKeyFrame)
            cout << "*-*-*-*-*-* FORCE KEYFRAME *-*-*-*-*-*-*" << endl;
        // cout << "frameQuota == " << frameQuota << endl;
#endif

        bool supplyLastBuffer = temporalCompress_ && !makeKeyFrame;

        if ((dwUser & VHDR_QC_DATA) == VHDR_QC_DATA && pQcData != NULL)
        {
            // Quick compress mode, read out some RECT data
            int nByteSize = pQcData->cbBufferSize;
            char *pTarget = ((char *) compBuffer_) + compBufferSize_ - nByteSize;
            void *pSource = pQcData->pData + 1024 - nByteSize;
            // Quickcapture data is added to the very end of all used buffers (QCData or lpData).
            // The compress buffer of LSGC should be large enough so as not to 
            // collide with other data in that buffer. What about other codecs?

            memcpy(pTarget, pSource, nByteSize);
        }
        else if ((dwUser & VHDR_QC_NOCHANGE) == VHDR_QC_NOCHANGE)
        {
            CRectKeeper::OutputNoChange((char *) compBuffer_, compBufferSize_);
        }
        else
        {
            // No QC mode
            char *szCompBuffer = ((char *)compBuffer_) + compBufferSize_ - 3;
            szCompBuffer[0] = 0;
            szCompBuffer[1] = 0;
            szCompBuffer[2] = 0;

            BYTE *pDataEnd = ((BYTE *)compBuffer_) + compBufferSize_;

            if ((dwUser & VHDR_CONTAINS_MOUSE) == VHDR_CONTAINS_MOUSE && pMouseData != NULL) {
                int iDataLength = pMouseData->WriteData(pDataEnd);
                pDataEnd -= iDataLength;
            }

            if ((dwUser & VHDR_CONTAINS_CLICK) == VHDR_CONTAINS_CLICK && pClickData != NULL) {
                pClickData->WriteData(pDataEnd);
            }
        }

#ifdef _DEBUG
        fprintf(debugFile, "Frame #%d, quality==%d\n", aviStreamHeader_.dwLength, codecInfo_.iQuality);
        fflush(debugFile);
#endif

        compressResult = ICCompress(
            hic_,  // hic
            makeKeyFrame ? ICCOMPRESS_KEYFRAME : 0,  // dwFlags
            &outputBitmapInfo_->bmiHeader,  // lpbiOutput
            compBuffer_,  // lpData (output buffer)
            &inputBitmapInfo_.bmiHeader,  // lpbiInput
            data, // lpInput
            &ckid, // lpckid
            &returnFlags, // lpdwFlags
            aviStreamHeader_.dwLength, // lFrameNum
            frameQuota, // dwFrameSize, data rate/byte quota, may be 0x7fffffff
            codecInfo_.bSupportsQuality ? codecInfo_.iQuality : 0,  // dwQuality
            supplyLastBuffer ? &lastFrameBitmapInfo_.bmiHeader : NULL,
            supplyLastBuffer ? lastFrameBuffer_ : NULL);

        if (temporalCompress_)
        {
#ifdef DEBUG
            cout << "... copying buffers for temporal compression..." << endl;
#endif
            // copy input buffer to lastBuffer_
            memcpy(lastFrameBuffer_, data, dataSize);
            memcpy(&lastFrameBitmapInfo_, &inputBitmapInfo_, sizeof BITMAPINFO);
        }

#ifdef _DEBUG
        if (aviStreamHeader_.dwLength == 1)
        {
            if ((returnFlags & AVIIF_KEYFRAME) == 0)
            {
                cout << "+-+-+-+-+-+-+-+-+-+- Kein Keyframe beim ersten Frame erzeugt!" << endl;
                if (makeKeyFrame)
                    cout << "   (hab's ihm aber gesagt)" << endl;
            }
        }
#endif

        res = (compressResult == ICERR_OK);
    }

    if (res) // always true if "codec" is RGB
    {
        if (codecInfo_.fccHandler == inputBitmapInfo_.bmiHeader.biCompression)
            returnFlags = dwFlags & VHDR_KEYFRAME ? AVIIF_KEYFRAME : 0;

        if (returnFlags & AVIIF_KEYFRAME)
        {
            lastKeyFrame_ = aviStreamHeader_.dwLength;
#ifdef _DEBUG
            cout << "  * K *" << endl;
#endif
        }

        if (codecInfo_.fccHandler == inputBitmapInfo_.bmiHeader.biCompression)
            openChunk(_T("00db"));
        else
            openChunk(_T("00dc"));

        mmioGetInfo(hmmio_, &mmioInfo, 0);
        indexEntry->dwChunkOffset = mmioInfo.lDiskOffset - 8; // 8 bytes == RIFF header


        if (codecInfo_.fccHandler == inputBitmapInfo_.bmiHeader.biCompression)
        {
            indexEntry->ckid = mmioStringToFOURCC(_T("00db"), 0);
            // NOOOO:
            // indexEntry->dwChunkLength = inputBitmapInfo_.bmiHeader.biSizeImage;
            indexEntry->dwChunkLength = dataSize; // may differ!!!!
        }
        else
        {
            indexEntry->ckid = mmioStringToFOURCC(_T("00dc"), 0);
            indexEntry->dwChunkLength = outputBitmapInfo_->bmiHeader.biSizeImage;
        }
        indexEntry->dwFlags       = returnFlags;

        if (codecInfo_.fccHandler != inputBitmapInfo_.bmiHeader.biCompression)
        {
            if (!writeData(compBuffer_, outputBitmapInfo_->bmiHeader.biSizeImage))
                throw VideoIOException(Local::localizeString(VIDEO_IO_NOWRITE));
        }
        else
        {
            if (!writeData(data, dataSize))
                throw VideoIOException(Local::localizeString(VIDEO_IO_NOWRITE));
        }

        closeChunk();

        if (codecInfo_.fccHandler != inputBitmapInfo_.bmiHeader.biCompression)
            return outputBitmapInfo_->bmiHeader.biSizeImage + 24; // RIFF header + Index
        else
            return dataSize + 24; // RIFF header + Index
    }
    else
    {
#ifdef _DEBUG
        cerr << "*-*-*-*-*-*-* ICCompress failed!" << endl;
#endif
        // Write a JUNK chunk with the error code:
        openChunk(_T("JUNK"));
        char* error = "***ICCompress failed";
        writeData((void *) error, 20);
        writeData((void *) &compressResult, sizeof DWORD);
        closeChunk();

        // We have to decrement these three variables here,
        // as they already have been incremented and would
        // be incremented twice otherwise (oder so ähnlich).
        --indexIndex_;
        --aviStreamHeader_.dwLength;
        --aviHeader_.dwTotalFrames;

        writeNullFrame();
    }

    return 0;

    //return res;
}

void VideoFile::writeNullFrame() {
   writeFrame(NULL, 0);
}

void VideoFile::setFrameRate(long usecsPerFrame) {
	aviStreamHeader_.dwScale      = usecsPerFrame;
	aviHeader_.dwMicroSecPerFrame = usecsPerFrame;
}

void VideoFile::finalizeAviFile() {
	cerr << "*** finalizeAviFile: outputBitmapInfo_ == " << outputBitmapInfo_ << endl;

	// close compression session
	ICCompressEnd(hic_);
	ICClose(hic_);
	hic_ = NULL;

	closeChunk();

   // PZI: terminate structing performance optimization for LSGC
   CloseStructuringOptimization();

	// write the index:
	openChunk(_T("idx1"));

   bool allOk = true;

   if (!writeData(indexArray_, indexIndex_*sizeof(AVIINDEXENTRY)))
      allOk = false;

	if (!allOk)
		throw VideoIOException(Local::localizeString(VIDEO_INDEX_WRITEERR));

	closeChunk(); // close 'idx1' chunk

	closeChunk(); // close 'AVI ' riff chunk

	// now update the headers:
	LONG mmres = mmioSeek(hmmio_, mainAviHeaderOffset_, SEEK_SET);
	if (mmres == -1)
		throw VideoIOException(Local::localizeString(VIDEO_HDR_UPDATEERR));
		
	writeMainAVIHeader();

	mmres = mmioSeek(hmmio_, videoStreamHeaderOffset_, SEEK_SET);
	if (mmres == -1)
		throw VideoIOException(Local::localizeString(VIDEO_HDR_UPDATEERR));

	if (!writeData(&aviStreamHeader_, aviStreamHeaderSize_))
		throw VideoIOException(Local::localizeString(VIDEO_HDR_UPDATEERR));

	// now reset position in file
	mmres = mmioSeek(hmmio_, 0, SEEK_END);
	if (mmres == -1)
		throw VideoIOException(Local::localizeString(VIDEO_HDR_UPDATEERR));
}

void VideoFile::setCodec(CodecInfo* codecInfo)
{
	memcpy(&codecInfo_, codecInfo, sizeof CodecInfo);
	aviStreamHeader_.fccHandler = codecInfo_.fccHandler;

   // in: iQuality in range [0, 100]
   // out: iQuality in range [0, 10000]
   int newQuality = codecInfo_.iQuality;
   newQuality = newQuality < 0 ? 0 : newQuality;
   newQuality = newQuality > 100 ? 100 : newQuality;

   newQuality *= 100;

   codecInfo_.iQuality = newQuality;
}

void VideoFile::enableIndeoQualityControl()
{
	// check if we're into the Indeo codecs...
	if (!((codecInfo_.fccString[0] == 'I' &&
		   codecInfo_.fccString[1] == 'V') ||
		  (codecInfo_.fccString[0] == 'i' &&
		   codecInfo_.fccString[1] == 'v')))
	{
      // no, it's not an Indeo codec, just return
		return;
	}

	// cout << "Using an Indeo codec. Tickeling..." << endl;

   // Ok, we know it's an indeo codec. Now we send it some
   // messages so that it knows we want something of it.
   // It's quite funny that it is enough to just send these
   // messages to the codec, and after that, the quality
   // settings are accepted in the normal ICCOMPRESSFRAMES
   // message. But, even that is not normal: Normally it
   // should be possible to pass a quality value with
   // the ICCompress() call, but that won't work with
   // the IV50 codec. Funny, strange and odd.
	R4_ENC_SEQ_DATA r4enc;

	r4enc.dwSize			= sizeof(R4_ENC_SEQ_DATA);
	r4enc.dwFourCC			= codecInfo_.fccHandler;
	r4enc.dwVersion			= SPECIFIC_INTERFACE_VERSION;

	ICGetState(hic_, &r4enc, sizeof r4enc);

   /*
	cout << "R4_ENC_SEQ_DATA:" << endl;
	cout << "  dwSize: " << r4enc.dwSize << endl;
	cout << "  dwFourCC: " << r4enc.dwFourCC << endl;
	cout << "  dwVersion: " << r4enc.dwVersion << endl;
	cout << "  mtType: " << r4enc.mtType << endl;
	cout << "  oeEnvironment: " << r4enc.oeEnvironment << endl;
	cout << "  dwFlags: " << r4enc.dwFlags << endl;
	cout << "  dwKey: " << r4enc.dwKey << endl;
	cout << "  dwMinViewportWidth: " << r4enc.dwMinViewportWidth << endl;
	cout << "  dwMinViewportHeight: " << r4enc.dwMinViewportHeight << endl;
	cout << "  fQuickCompress: " << r4enc.fQuickCompress << endl;
	cout << "-----" << endl;
   */

	r4enc.dwSize			= sizeof(R4_ENC_SEQ_DATA);
	r4enc.dwFourCC			= codecInfo_.fccHandler;
	r4enc.dwVersion			= SPECIFIC_INTERFACE_VERSION;
	r4enc.mtType			= MT_ENCODE_SEQ_VALUE;
	r4enc.oeEnvironment		= OE_32;
	r4enc.dwFlags			= ENCSEQ_VALID | ENCSEQ_QUICK_COMPRESS;
	//r4enc.fQuickCompress	= !!arglist[0].asInt(); // don't change

	ICSetState(hic_, &r4enc, sizeof r4enc);

	ICGetState(hic_, &r4enc, sizeof r4enc);

   /*
	cout << "R4_ENC_SEQ_DATA:" << endl;
	cout << "  dwSize: " << r4enc.dwSize << endl;
	cout << "  dwFourCC: " << r4enc.dwFourCC << endl;
	cout << "  dwVersion: " << r4enc.dwVersion << endl;
	cout << "  mtType: " << r4enc.mtType << endl;
	cout << "  oeEnvironment: " << r4enc.oeEnvironment << endl;
	cout << "  dwFlags: " << r4enc.dwFlags << endl;
	cout << "  dwKey: " << r4enc.dwKey << endl;
	cout << "  dwMinViewportWidth: " << r4enc.dwMinViewportWidth << endl;
	cout << "  dwMinViewportHeight: " << r4enc.dwMinViewportHeight << endl;
	cout << "  fQuickCompress: " << r4enc.fQuickCompress << endl;
	cout << "-----" << endl;
   */
}

long VideoFile::getCurrentFrame()
{
   return aviStreamHeader_.dwLength;
}



////////////////////////////////////////////////////////////////////////////
// performance optimization of structuring clips (SG extended)
// receiving data via callback function from lsgc.dll
////////////////////////////////////////////////////////////////////////////

// file to write data
FILE *sgExtendeFile = NULL;
#define SGEXT(x)  if(sgExtendeFile) { fprintf(sgExtendeFile, x); fflush(sgExtendeFile); }

// callback function for lsgc.dll to receive structuring values
LONG CALLBACK StructuringCallback(LPARAM lParam, UINT message, LONG l) 
{ 
   // write data for later sttructuring
   LMisc::StructuringValues* values = (LMisc::StructuringValues*)l;
   char buffer[50];
   sprintf(buffer, "%i %i %i\n", values->iFrameNr, values->iPixelCount, values->iAreaCount);
   SGEXT(buffer);

   return 0; 
} 

// set structing performance optimization for LSGC
// create file and init callback function
void VideoFile::InitStructuringOptimization() {
   if (_tcsnicmp(codecInfo_.fccString, _T("lsgc"), 4) == 0) {
      // using LSGC

      // create file for structuring data
      char filename[512];
      sprintf(filename, "%ls", szFileName_);
      int len = _tcslen(szFileName_);
      filename[len-3] = 's';
      filename[len-2] = 'g';
      filename[len-1] = 'e';
      sgExtendeFile = fopen(filename, "w");

      // write: version number
      SGEXT("VERSION 1\n");

      // set callback function for lsgc.dll
      ICSetStatusProc(hic_, 0, (LPARAM) (UINT) 0, &StructuringCallback);
   }
}

// close file for performance optimization of structuring clips (SG extended)
void VideoFile::CloseStructuringOptimization() {
   // close file
   if(sgExtendeFile) {
      fflush(sgExtendeFile);
      fclose(sgExtendeFile);
      sgExtendeFile = NULL;
   }

   // unset callback function for lsgc.dll
   ICSetStatusProc(hic_, 0, (LPARAM) (UINT) 0, NULL);
}
