/*********************************************************************
 program : mlb_telepointer.cpp
 authors : Gabriela Maass
 date    : 
 desc    : 

 **********************************************************************/

#include "StdAfx.h"

#include "pointer.h"

#include "mlb_objects.h"
#include "la_project.h" 
#include "mlb_telepointer.h"


ASSISTANT::Telepointer::Telepointer(double _x, double _y, Gdiplus::ARGB argbColor, int _status, uint32 _id)
                       : ColoredObject(_x, _y, 0, 0, -1, argbColor, 0, 0, Gdiplus::DashStyleSolid, _id, 
                                       NULL, NULL, NULL) 
{
   status = _status;

   if (_status == ACTIVE)
      visible = true;
   else 
      visible = false;

   HICON hIcon = AfxGetApp()->LoadIcon(IDI_TELEPOINTER);
   m_pBmpTelepointer = Gdiplus::Bitmap::FromHICON(hIcon);

}

ASSISTANT::Telepointer::~Telepointer() 
{
   if (m_pBmpTelepointer != NULL)
      delete m_pBmpTelepointer;
}

void ASSISTANT::Telepointer::Update(double _x, double _y) 
{
   m_dX = _x; 
   m_dY = _y;
}

void ASSISTANT::Telepointer::Draw(CDC *pDC, float zoomFactor, double dOffX, double dOffY) 
{
   if (!visible || status != ACTIVE) return;

   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform(dOffX, dOffY);

   double dX = m_dX * zoomFactor - 29;
   double dY = m_dY * zoomFactor - 2;
   graphics.DrawImage(m_pBmpTelepointer, (Gdiplus::REAL)dX, (Gdiplus::REAL)dY);
}

void ASSISTANT::Telepointer::Hide() 
{
   if (!visible) return;

   visible = false;
}

void ASSISTANT::Telepointer::SetActive() 
{
   if (visible) 
      Hide();

   visible = true;
   status = ACTIVE;
}

void ASSISTANT::Telepointer::SetInactive() 
{
   Hide();
   status = INACTIVE;
}

bool ASSISTANT::Telepointer::IsActive()
{
   if (status == ACTIVE) 
      return true;
    else 
      return false;
}

void ASSISTANT::Telepointer::SaveAsAOF(CFileOutput *fp)
{
   if (status != ACTIVE || recorded) return;

   recorded = true;

   recID = ASSISTANT::Project::active->recCount;

   CString line;

   line.Format(_T("<POINTER x=%d y=%d xf=%2.2f yf=%2.2f status=1></POINTER>\n"), 
               (int)m_dX, (int)m_dY,
               m_dX, m_dY);
   fp->Append(line);

   ASSISTANT::Project::active->recCount++;	
}
