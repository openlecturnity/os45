#ifndef __LFILE_H__369E09F7_EA9E_4e13_9E43_EC463A76BBB0
#define __LFILE_H__369E09F7_EA9E_4e13_9E43_EC463A76BBB0

#ifndef __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B
#pragma message( "Do not include \"" __FILE__ "\", include \"lutils.h\"")
#endif __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B

#define LFILE_OK                    S_OK
#define LFILE_ERRBASE               0x2000

#define LFILE_ERR_CREATE            (LFILE_ERRBASE)
#define LFILE_ERR_OPEN              (LFILE_ERRBASE+0x01)
#define LFILE_ERR_NOTFOUND          (LFILE_ERRBASE+0x02)
#define LFILE_ERR_NOTOPEN           (LFILE_ERRBASE+0x03)
#define LFILE_ERR_INVALIDPARAMS     (LFILE_ERRBASE+0x04)
#define LFILE_ERR_FAILED            (LFILE_ERRBASE+0x05)
#define LFILE_ERR_ALREADY_OPEN      (LFILE_ERRBASE+0x06)
#define LFILE_ERR_READ              (LFILE_ERRBASE+0x10)
#define LFILE_ERR_OPENFORWRITE      (LFILE_ERRBASE+0x11)
#define LFILE_ERR_WRITE             (LFILE_ERRBASE+0x20)
#define LFILE_ERR_OPENFORREAD       (LFILE_ERRBASE+0x21)

#define LFILE_TYPE_BINARY 0
#define LFILE_TYPE_TEXT_UTF8 1
#define LFILE_TYPE_TEXT_UNICODE 2
#define LFILE_TYPE_TEXT_ANSI 3

class LFile
{
public:
   LFile(const _TCHAR *tszFileName);
   ~LFile();

   LURESULT Create(DWORD dwFileType=LFILE_TYPE_BINARY, BOOL bWriteBOM=FALSE);
   LURESULT Open();
   LURESULT Close();
   LURESULT Delete();
   bool     Exists(bool bWithSize = false);
   __int64  GetSize();
   __int64  LastModifiedMs();
   bool     IsEqualOrNewerThan(LFile *pOther);

   INT      ReadSingleByte();
   LURESULT ReadRaw(LBuffer *pBuffer, DWORD dwOff, DWORD dwLen, DWORD *pdwReadBytes);
   LURESULT WriteRaw(const void *pBuffer, DWORD dwSize);
   // Set nLen to -1 if tszText is NULL terminated.
   LURESULT WriteText(const _TCHAR *tszText, int nLen=-1);
   LURESULT Skip(UINT nByteCount);

private:
   DWORD  m_dwFileType;
   // TRUE if a BOM (=Byte Order Mark) is to be written (only for text files)
   BOOL   m_bWriteBOM;
   HANDLE m_hFile;

   LString m_lsFileName;
};

#endif // __LFILE_H__369E09F7_EA9E_4e13_9E43_EC463A76BBB0
