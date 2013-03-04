#if !defined(AFX_LECTURNITYSOURCEDEFINES_H__10E70854_4F35_44e4_B301_59537D5C16DF__INCLUDED_)
#define AFX_LECTURNITYSOURCEDEFINES_H__10E70854_4F35_44e4_B301_59537D5C16DF__INCLUDED_

#include <initguid.h>

// {382EC707-C428-4d6d-8F62-185FBB93F74E}
DEFINE_GUID(IID_ILecturnitySourceControl, 
0x382ec707, 0xc428, 0x4d6d, 0x8f, 0x62, 0x18, 0x5f, 0xbb, 0x93, 0xf7, 0x4e);

// {A3DE1703-7FBF-40e9-9F56-080F218084C7}
DEFINE_GUID(CLSID_LecturnitySource, 
0xa3de1703, 0x7fbf, 0x40e9, 0x9f, 0x56, 0x8, 0xf, 0x21, 0x80, 0x84, 0xc7);

#define E_LS_VIDEO_FAILED           0x83820001
#define E_NOT_ENOUGH_DISK_SPACE     0x80070070

interface ILecturnitySourceControl : public IUnknown
{
   STDMETHOD(SetVideoSizes)(UINT nWidth, UINT nHeight, UINT nInputWidth, UINT nInputHeight) = 0;
   STDMETHOD(SetFrameRate)(float fFrameRate) = 0;
   STDMETHOD(SetIsEvaluation)(bool bIsEvaluation) = 0;
   STDMETHOD(SetMinimumLength)(UINT nLengthMs) = 0;
   STDMETHOD(ParseFiles)(LPCTSTR tszEvqName, LPCTSTR tszObjName) = 0;
   STDMETHOD(AddClip)(LPCTSTR tszClipName, int iClipStartMs) = 0;
   STDMETHOD(GetLastSampleTime)(UINT *pnSampleTimeMs) = 0;
};

#endif // !defined(AFX_LECTURNITYSOURCEDEFINES_H__10E70854_4F35_44e4_B301_59537D5C16DF__INCLUDED_)
