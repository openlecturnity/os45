#include "global.h"
//#include "rectkeeper.h"

QCRECT::QCRECT()
{
   x1 = 0; x2 = 0; y1 = 0; y2 = 0;
}

QCRECT::QCRECT(short x1_, short y1_, short x2_, short y2_)
{
   if (x1_ <= x2_)
   {
      x1 = x1_;
      x2 = x2_;
   }
   else
   {
      x1 = x2_;
      x2 = x1_;
   }

   if (y1_ <= y2_)
   {
      y1 = y1_;
      y2 = y2_;
   }
   else
   {
      y1 = y2_;
      y2 = y1_;
   }
}

bool QCRECT::Overlaps(const QCRECT &other)
{
   bool i1 = (other.x2 >= x1) && (x2 >= other.x1);
   bool i2 = (other.y2 >= y1) && (y2 >= other.y1);

   return i1 && i2;
}

QCRECT &QCRECT::Combine(const QCRECT &other)
{
   x1 = x1 < other.x1 ? x1 : other.x1;
   x2 = x2 > other.x2 ? x2 : other.x2;
   y1 = y1 < other.y1 ? y1 : other.y1;
   y2 = y2 > other.y2 ? y2 : other.y2;

   return (*this);
}

CRectKeeper::CRectKeeper()
{
   m_paQcRects = new QCRECT[10];
   m_iQcRectCount = 0;
   m_iQcRectMaximum = 10;
}

CRectKeeper::~CRectKeeper()
{
   if (m_paQcRects != NULL)
      delete[] m_paQcRects;
}

void CRectKeeper::AddRect(const QCRECT &rect)
{
   QCRECT r = rect;

   bool ready = false;

   while (!ready)
   {
      ready = true;
      for (int i=0; i<m_iQcRectCount && ready; ++i)
      {
         if (m_paQcRects[i].Overlaps(r))
         {
            m_paQcRects[i].Combine(r);
            r = m_paQcRects[i];
            Clear();

            ready = false;
         }
      }
   }

   if (m_iQcRectCount == m_iQcRectMaximum)
   {
      QCRECT *paQcRects = new QCRECT[m_iQcRectMaximum * 2];
      memcpy(paQcRects, m_paQcRects, m_iQcRectCount * sizeof QCRECT);
      delete[] m_paQcRects;
      m_paQcRects = paQcRects;
      m_iQcRectMaximum *= 2;
   }

   m_paQcRects[m_iQcRectCount++] = r;
}

void CRectKeeper::Clear()
{
   m_iQcRectCount = 0;
}

int CRectKeeper::Size()
{
   return m_iQcRectCount;
}

CRectKeeper &CRectKeeper::Merge(const CRectKeeper &other)
{
   
   for (int i=0; i<other.m_iQcRectCount; ++i)
   {
      AddRect(other.m_paQcRects[i]);
   }

   return (*this);
}

int CRectKeeper::FormatOutput(char *pData, const int len)
{
   int n = Size();

   unsigned char *pSig   = (unsigned char *) (pData + len - 4);
   short         *pRects = (short *) (pData + len - 4 - n*8);

   if (len < 4 + n*8)
   {
      pSig[1] = '\000';
      pSig[2] = '\000';
      pSig[3] = '\000';
      return 0;
   }

   pSig[0] = (unsigned char) Size();
   pSig[1] = 'Q';
   pSig[2] = 'u';
   pSig[3] = 'C';

   for (int i=0; i<n; ++i)
   {
      pRects[4*i]     = m_paQcRects[i].x1;
      pRects[4*i + 1] = m_paQcRects[i].y1;
      pRects[4*i + 2] = m_paQcRects[i].x2;
      pRects[4*i + 3] = m_paQcRects[i].y2;
   }

   return (4 + n*8);
}

int CRectKeeper::MergeAndOutput(const CRectKeeper &c1, const CRectKeeper &c2,
                                 char *pData, const int len)
{
   CRectKeeper c3;
   c3.Merge(c1);
   c3.Merge(c2);

   return c3.FormatOutput(pData, len);
}

void CRectKeeper::OutputNoChange(char *pData, const int len)
{
   static CRectKeeper empty;

   empty.FormatOutput(pData, len);
}
