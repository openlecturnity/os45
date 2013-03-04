#if !defined(AFX_LISEDRAWERS_H__1B66FB41_FCB0_5CF0_8B4B_F27FED11CDC0__INCLUDED_)
#define AFX_LISEDRAWERS_H__1B66FB41_FCB0_5CF0_8B4B_F27FED11CDC0__INCLUDED_

/**
 * CDrawingToolSettings
 * Handles markers and pencils. Contains a lists of all markers id's. Sets/Gets active marker (if one set).
 * By tool I mean marker or pencil.
 * OBS: 
 *		- A tool is SET if the holding button contains it.
 *		- A tool is ACTIVE if it is selected and it is the current drawing tool.

 *    //TODO - First element in the list is considered default selected.
 *		- For the moment please be aware that these lists are dependent of the image/button created from CMainFrame
 *		e.g.: There are two places where you must pay attention:
 *				m_pItemsMarkers->AddItem(ID_TOOL_MARKERBLUE + i, ... ) must be first in the list!.
 *				Set default icon for ID_BUTTON_MARKERS by changing the icon item array of it's command bar owner.
*/
class CDrawingToolSettings
{
	private:
      // A list of all tools id's
		CArray<UINT, UINT> m_aTools;
		// Id of the tool selected. Default value is 0. This doesn't mean that the tool is active.
		UINT m_nToolSelected;
		// Set to TRUE if tool is active.
		bool m_bActive;

public:
	CDrawingToolSettings(void);
public:
	~CDrawingToolSettings(void);
	//-----------------------------------------------------------------------
	// Summary:
	//     Ads a tool id at the end of the list
	// Parameters:
	//     toolId - the tool resource id that will be added to the list.
	//-----------------------------------------------------------------------
	void Add(UINT nToolSelected);
	inline UINT GetAt(UINT nPos){return m_aTools.GetAt(nPos);}

	__declspec(property(get=get_Active, put=set_Active)) bool Active;
	__declspec(property(get=get_Selected, put=set_Selected)) UINT Selected;
	//-----------------------------------------------------------------------
	// Summary:
	//     Return the active tool
	// Parameters:
	//     
	//-----------------------------------------------------------------------
	inline bool get_Active(){return m_bActive;}
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the active tool
	// Parameters:
	//     bActive - the new state
	//-----------------------------------------------------------------------
	inline void set_Active(bool bActive){m_bActive = bActive;}
	//-----------------------------------------------------------------------
	// Summary:
	//     Return the id of the selected tool
	//-----------------------------------------------------------------------
	inline UINT get_Selected(){return m_nToolSelected;}
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the active tool
	// Parameters:
	//     nToolSelected - the id of the new tool selected
	//-----------------------------------------------------------------------
	void set_Selected(UINT nToolSelected);
	//-----------------------------------------------------------------------
	// Summary:
	//     Search for an ID in the list
	// Parameters:
	//     nToolId - The tool id that is searched in the list.
	// Returns:
	//		True if the item is in the lis. False otherwise
	//-----------------------------------------------------------------------	
	bool Find(UINT nToolId);

};

#endif // !defined(AFX_LISEDRAWERS_H__1B66FB41_FCB0_5CF0_8B4B_F27FED11CDC0__INCLUDED_)