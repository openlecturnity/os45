#ifndef __VIDEOFILE
#define __VIDEOFILE

#pragma warning(disable:4786)

#define AVI_INDEX_SIZE  25*60*60*3  // in frames, 3h

//using namespace std;

struct QCDATA; // avoid including rectkeeper.h or global.h
class SrMouseData; // avoid including screencapturer.h or global.h
class SrClickData;

class VideoFile : public RiffFile
{
private:
	MainAVIHeader aviHeader_;

	LecturnityAVIStreamHeader aviStreamHeader_;
	int aviStreamHeaderSize_;
	
	BITMAPINFO* outputBitmapInfo_; // this is not fixed, so pointer!
	DWORD outputBitmapInfoSize_;
	BITMAPINFO inputBitmapInfo_;
	BITMAPINFO lastFrameBitmapInfo_;
	int bitmapInfoSize_;

	// info struct for getting file positions etc.
	MMIOINFO mmioInfo;

	// chunk offsets:
	long mainAviHeaderOffset_;
	long videoStreamHeaderOffset_;

	// for the index:
	// vector<AVIINDEXENTRY*> index_;
   AVIINDEXENTRY* indexArray_;
   unsigned int   indexIndex_;
   unsigned int   m_nCurrentIndexLength;

	// for the IC compressor:
	CodecInfo codecInfo_;
	HIC hic_;

	DWORD compBufferSize_;
	void* compBuffer_;
	void* lastFrameBuffer_;
	int lastKeyFrame_;
	bool temporalCompress_;
	bool fastTemporalCompress_;

	void writeMainAVIHeader();
	void enableIndeoQualityControl();

public:
	VideoFile(_TCHAR* tszFileName);
	~VideoFile();

	// bool chooseCompressor(HWND hWnd);

	void setMainAviHeader(MainAVIHeader* aviHeader);
	void setAVIStreamHeader(LecturnityAVIStreamHeader* aviStreamHeader, int aviStreamHeaderSize);
	void setBitmapInfoHeader(BITMAPINFO* bitmapInfo, int bitmapInfoSize);
	void writeAviHeaders();
   // Write a (possibly compressed) frame to the file. Returns the
   // size of the frame.
	DWORD writeFrame(void* data, int dataSize, DWORD frameQuota=0x7fffffff, DWORD dwFlags=0,
                    DWORD dwUser=0, QCDATA *pQcData=NULL, SrMouseData *pMouseData=NULL, 
                    SrClickData *pClickData=NULL, DWORD dwTimeCapturedMs=-1);
	void writeNullFrame();
	void initCodec();
	void setCodec(CodecInfo* codecInfo);
	void finalizeAviFile();

	// use this to adapt the frame rate posteriorily
	void setFrameRate(long usecsPerFrame);
   long getCurrentFrame();

   // handle callback from lsgc.dll to structure clips
   void InitStructuringOptimization();
   void CloseStructuringOptimization();
};

#endif
