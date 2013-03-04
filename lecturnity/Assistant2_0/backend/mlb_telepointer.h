/*********************************************************************

 program  : mlb_telepointer.h
 authors  : Gabriela Maass
 date     : 21.01.2002
 revision : $Id: mlb_telepointer.h,v 1.7 2007-08-03 09:27:00 maass Exp $
 desc     : 

 **********************************************************************/

#ifndef __ASSISTANT_TELEPOINTER_H__
#define __ASSISTANT_TELEPOINTER_H__


#include "mlb_generic.h"
#include "mlb_objects.h"

namespace ASSISTANT 
{
  class  Telepointer: public ColoredObject {
  public:
    enum types {
      // display
      ACTIVE   = 0,
      INACTIVE = 1
    };
    
    Telepointer(double _x, double _y, Gdiplus::ARGB argbColor, int _status, uint32 _id);
    ~Telepointer();

    virtual int GetType() {return GenericObject::TELEPOINTER;}

    virtual DrawObject *Copy() {return NULL;}
    
    virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
    virtual void DrawSelection(CDC *pDC, float _zoomFactor, double dOffX, double dOffY) {}
    virtual void Hide();

    virtual bool IsEditablePoint(Gdiplus::PointF &ptObject, UINT &nEditTyp, int &iPointIndex, double dZoomFactor) {return false;}

    void Update(double _x, double _y);
    void SetActive();
    void SetInactive();
    bool IsActive();

    virtual void SaveAsAOF(CFileOutput *fp);
    virtual int RecordableObjects() {return 0;}
    virtual void AdjustGui() {};


  private:
    int    status;
    Gdiplus::Bitmap *m_pBmpTelepointer;
  };

}


#endif
