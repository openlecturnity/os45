// PublishEngine.cpp: Implementierung der Klasse CPublishEngine.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdafx.h"
#include "PublishEngine.h"

#include "PublishProgressDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CPublishEngine::CPublishEngine(CPublisherDoc *pDoc)
{
   m_pDoc = pDoc;
}

CPublishEngine::~CPublishEngine()
{
}

void CPublishEngine::Publish()
{
   CPublishProgressDlg progressDlg;
   progressDlg.DoModal();
}