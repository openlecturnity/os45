// RecordingProfileGallery.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "RecordingProfileGallery.h"
#include "RecordingsToolBar.h"


// CRecordingProfileGallery

CRecordingProfileGallery::CRecordingProfileGallery(CRecordingsToolBar *pToolbar):CXTPControlGallery()
{
    m_pToolbar = pToolbar;
}

CRecordingProfileGallery::~CRecordingProfileGallery()
{
}

void CRecordingProfileGallery::OnMouseMove(CPoint point)
{
	if (IsShowAsButton())
	{
		CXTPControlPopup::OnMouseMove(point);
		return;
	}

    if (m_pToolbar != NULL && point.x == -1 && point.y == -1) {      
        m_pToolbar->HideProfileGallery(true);
        return;
    }

	PerformMouseMove(point);

	int ht = HitTestScrollBar(point);

	if (ht != m_spi.ht)
	{
		m_spi.ht = ht;
		RedrawParent();
	}
}

CRecordingProfileItem::CRecordingProfileItem() : CXTPControlGalleryItem()
{
	m_lCustomProfileID = 0;
}
