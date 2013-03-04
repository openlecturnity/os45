#include "stdafx.h"
#include "DrawingToolSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDrawingToolSettings::CDrawingToolSettings(void)
{
	m_aTools.RemoveAll();
	m_nToolSelected = 0;
	m_bActive = false;
}

CDrawingToolSettings::~CDrawingToolSettings(void)
{
	if ( m_aTools.IsEmpty() == false)
		m_aTools.RemoveAll();
}

void CDrawingToolSettings::Add(UINT nToolSelected)
{
	if ( m_aTools.IsEmpty() == TRUE )
		m_nToolSelected = nToolSelected;
	m_aTools.Add(nToolSelected);
}

bool CDrawingToolSettings::Find(UINT nToolId)
{
	for ( int i = 0; i < m_aTools.GetCount(); i++ )
	{
		if ( nToolId == m_aTools.GetAt(i) )
			return true;
	}
	return false;
}

void CDrawingToolSettings::set_Selected(UINT nToolSelected)
{
	// Throw an assertion failure if the specified tool is not in current list. Selected tool cannot be outside of our list.
	ASSERT(Find(nToolSelected));
	m_nToolSelected = nToolSelected;
}