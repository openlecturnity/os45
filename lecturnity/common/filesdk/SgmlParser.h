#ifndef __SGMLPARSER__
#define __SGMLPARSER__

#ifndef WIN32
#  pragma warning (disable: 4786)
#  include <unistd.h>
#endif
#include <afxtempl.h>
#include "export.h"

#include "FileOutput.h"
#include "MiscFunctions.h"

class Attribute
{
public:
   Attribute();
   ~Attribute();
   
public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();
   // return a copy
   Attribute* Copy();
   
public:
   // get and set functions for member variables
   CString GetName() { return m_csName; }
   void SetName(LPCTSTR csName) { m_csName = csName; }
   
   CString GetValue() { return m_csValue; }
   bool GetBoolValue();
   int GetIntValue();
   double GetDoubleValue();
   COLORREF GetColorValue();
   COLORREF GetColorValue(bool &transparent);
   Gdiplus::ARGB GetARGBValue();
   void SetValue(LPCTSTR csValue) { m_csValue = csValue; }
   
protected: 
   CString m_csName;
   CString m_csValue;
   
};

class FILESDK_EXPORT SgmlElement
{
public:
   SgmlElement();
   ~SgmlElement();
   
public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();
   // get a copy (without parent)
   SgmlElement* Copy();
   
   // get and set functions for member variables
   LPCTSTR GetName() {return (LPCTSTR)(m_csName);}
   void SetName(LPCTSTR tszNewName) {m_csName = tszNewName;}
   
   LPCTSTR GetParameter() {return m_csParameter.GetStringBuffer();}
   void SetParameter(LPCTSTR newParameter);
   
   // Append a new "son" node
   void Append(SgmlElement *pElement);
   SgmlElement *Append(LPCTSTR tszName, LPCTSTR tszParameter=NULL);
   void InsertAtBegin(SgmlElement *pElement);
   void InsertAfter(SgmlElement *pElement, SgmlElement *pPreviousElement);
   void InsertAfter(CArray<SgmlElement *, SgmlElement *> &aElements, SgmlElement *pPreviousElement);

   // PZI: set attribute with specified name and value to attribute list
   // create new attribute if not defined yet
   void SetAttribute(LPCTSTR tszName, LPCTSTR tszValue);
 
   // write node and next node
   bool Write(CFileOutput *pOutput, int level);
   // search functions
   
   SgmlElement *Find(const _TCHAR *node, _TCHAR *parent=NULL);
   void FindAll(const _TCHAR *tszNodeName, CArray<SgmlElement *, SgmlElement *> &aElements);

   SgmlElement *FindElement(CString csNodeName);
   HRESULT GetElementParameter(CString csNodeName, CString &csParameter);
   HRESULT SetElementParameter(CString csNodeName, CString csParameter);

public:
   
   // TODO in later versions you'll probably need two versions of this GetValue()
   // one for data returning plain char* (LPCSTR) and one for real
   // user texts (UTF-8, Unicode, ...)

   LPCTSTR GetValue(LPCTSTR tszName);
   bool GetBoolValue(LPCTSTR tszName);
   int GetIntValue(LPCTSTR tszName);
   double GetDoubleFallback(LPCTSTR tszName);
   COLORREF GetColorValue(LPCTSTR tszName);
   COLORREF GetColorValue(LPCTSTR tszName, bool &transparent);
   Gdiplus::ARGB SgmlElement::GetARGBValue(LPCTSTR tszName);

   SgmlElement *GetParent() {return m_pParent;}
   void SetParent(SgmlElement *pParent) {m_pParent = pParent;}
   void GetElements(CArray<SgmlElement *, SgmlElement *> &aElements);
   SgmlElement *GetFirstElement();
   int GetElementCount() {return m_aElements.GetCount();}
   void SetElements(CArray<SgmlElement *, SgmlElement *> &aElements);
   void RemoveElement(SgmlElement *pElement);
   void RemoveAllElements(bool bDoDelete = true);

   void Dump();

   // functions to parse sgml-file
   
   // append next character from sgml-file
   void AppendTChar(const _TCHAR newChar, bool isParameter, bool isMasked);
   void AppendChar(const char newChar, bool isParameter, bool isMasked);

   // keep information if there is a return after open tag
   void UseOneLine(bool use) {m_bUseOneLine = use;}
   // set element closed (closed tag appeared)
   void Close() {m_bIsClosed = true;}
   // return if tag is closed or not
   bool IsClosed() {return m_bIsClosed;}
   // announce that end of parameter
   void EndParams() {m_bIsParameterList = true;}
 
   
protected:
   // Note: The reason why the member CString, CStringBuffer 
   // and CList<Attribute *, Attribute *> variables are
   // pointers is that otherwise it is not possible to use
   // the SgmlParser without creating linker warnings 
   // from outside the File SDK, as CString does not carry
   // the __declspec(dllexport) modifier.
   CString m_csName;
   CStringBuffer m_csParameter;
   bool    m_bIsClosed;
   bool    m_bIsParameterList;
   bool    m_bIsNotInitialized;
   bool    m_bUseOneLine;

   SgmlElement *m_pParent;
   
   CArray<Attribute *, Attribute *> m_aAttributes;
   CArray<SgmlElement *, SgmlElement *> m_aElements;

private:
   Attribute *FindAttribute(LPCTSTR tszName);
};

class FILESDK_EXPORT SgmlFile
{
public:
   SgmlFile();
   ~SgmlFile();
   
public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();
   
public:
   // open sgml-file, create and fill sgml-tree 
   bool Read(LPCTSTR tszFilename, _TCHAR *identifier=NULL);
   // Parse an SGML from memory
   bool ReadMemory(BYTE *aSgmlData, int iSgmlBufferSize, _TCHAR *identifier=NULL);

   // Use filename from last read or write call
   bool Write();
   // write sgml-tree into file
   bool Write(LPCTSTR tszFilename, bool isObjFile=false);
   // find node with name "node" [which is son from "parent"] in sgml-tree
   SgmlElement *Find(const _TCHAR *node, _TCHAR *parent=NULL);
   // get root node from sgml-tree
   SgmlElement *GetRoot() {return m_pRoot;}
   void SetRoot(SgmlElement *pRoot);

   // Resolves file names from SGML nodes of the following form
   // <file [writeToRoot="true|false"]
   //       [prefix="<prefix>"]
   //       [isIndex="true|false"]> [filename] </file>
   // Is used in the Template Parser of LPLibs and
   // in the LocationUpdate utility.
   static bool ResolveFileNode(IN SgmlElement *pNode, OUT _TCHAR *tszSrcFile, OUT _TCHAR *tszTargetFile, 
                               IN bool bTemplate, OUT bool *pbIsIndex, IN bool bCreateDirs,
                               IN const _TCHAR *tszSourcePath,
                               IN const _TCHAR *tszTargetPath);
   
   void Dump();

protected:
   SgmlElement *m_pRoot;
   
private:
   // This is the real Read() method; the public ones call this.
   // If szFilename is not NULL, the SGML file is read from there,
   // otherwise the SGML file is read from a memory block given
   // in szSgmlFile
   bool Read(LPCTSTR tszFilename, BYTE *aSgmlData, int iSgmlBufferSize, _TCHAR *identifier=NULL);
   // open sgml-file, create and fill sgml-tree 
   //bool ReadOld(LPCTSTR tszFilename, char *szSgmlData, DWORD cbSize, _TCHAR *identifier=NULL);
   // This is used to keep track of how much data
   // from the memory buffer has been read.
   DWORD m_nDataPointer;

   // Reads either from a given buffer or from a given
   // file handle. If hFile is not NULL, ::ReadFile() is used,
   // otherwise pData is used to read from. m_nDataPointer
   // is used to keep track of how much data has been read.
   //BOOL PrivateReadFile(HANDLE hFile, 
   //   char *pData, DWORD cbSize, 
   //   void *pBuffer, DWORD cbBytesToRead, DWORD *lpBytesRead);

   bool ReadNextBuffer(HANDLE hFile, BYTE *aBuffer, int iBufferSize, int *piBytesRead);
   bool ReadLine(BYTE *aBuffer, int iBufferLength, int *piReadPointer, CString &csLine, UINT nCodepage);
   bool ReadLine(HANDLE hFile, CString &csLine, UINT nCodepage);
   bool GetNextLineFromBuffer(BYTE *pBuffer, CArray<BYTE, BYTE> &aLineBytes, int &iCurrentPos, 
                              int &iBufferLength, bool &bLineComplete);
   bool ConvertBufferToString(CArray<BYTE, BYTE> &aLineBytes, CString &csLine, UINT nCodepage);

private:
    BYTE m_aBuffer[4096];
    int m_iBytesInBuffer;
    int m_iBufferPointer;

    CString m_csFilename;
};

#endif // __SGMLPARSER__
