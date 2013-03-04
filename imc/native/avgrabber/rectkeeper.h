#ifndef __RECTKEEPER_H__
#define __RECTKEEPER_H__

#define VHDR_QC_DATA        0x00000004
#define VHDR_QC_NOCHANGE    0x00000020
#define VHDR_CONTAINS_MOUSE 0x00010000
#define VHDR_CONTAINS_CLICK 0x00100000

struct QCDATA
{
   char pData[1024];
   int  cbBufferSize;
};

class QCRECT 
{
public:
   QCRECT();
   QCRECT(short x1_, short y1_, short x2_, short y2_);
   short x1;
   short y1;
   short x2;
   short y2;

   bool Overlaps(const QCRECT&);
   QCRECT &Combine(const QCRECT &);
};

class CRectKeeper
{
public:
   CRectKeeper();
   ~CRectKeeper();

   void AddRect(const QCRECT &rect);
   void Clear();
   CRectKeeper &Merge(const CRectKeeper &other);
   int  Size();

   int FormatOutput(char *pData, const int len);
   static int MergeAndOutput(const CRectKeeper &c1, const CRectKeeper &c2,
                             char *pData, const int len);
   static void OutputNoChange(char *pData, const int len);

   //vector<QCRECT> &GetRectVector() { return rects_; }
   // Not possible anymore in this form: QCRECT* instead of vector<> used
private:
   QCRECT *m_paQcRects;
   int m_iQcRectCount;
   int m_iQcRectMaximum;
};

#endif
