// PublishEngine.h: Schnittstelle für die Klasse CPublishEngine.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBLISHENGINE_H__9AD0B68B_D729_497F_9439_39D2610DCEAF__INCLUDED_)
#define AFX_PUBLISHENGINE_H__9AD0B68B_D729_497F_9439_39D2610DCEAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PublisherDoc.h"

class CPublishEngine  
{
public:
	CPublishEngine(CPublisherDoc *pDoc);
	virtual ~CPublishEngine();

   virtual void Publish();

private:
   CPublisherDoc *m_pDoc;
};

#endif // !defined(AFX_PUBLISHENGINE_H__9AD0B68B_D729_497F_9439_39D2610DCEAF__INCLUDED_)
