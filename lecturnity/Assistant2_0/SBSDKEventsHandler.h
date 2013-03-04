
#pragma once

#include "SBSDKComWrapper_h.h"

class SBSDKEventIntf
{
public:
	// contact events
	virtual void OnXYDown(LONG x, LONG y, LONG z, LONG iPointerID){};
	virtual void OnXYMove(LONG x, LONG y, LONG z, LONG iPointerID){};
	virtual void OnXYUp(LONG x, LONG y, LONG z, LONG iPointerID){};
	// non-projected contact events
	virtual void OnXYNonProjectedDown(LONG x, LONG y, LONG z, LONG iPointerID){};
	virtual void OnXYNonProjectedMove(LONG x, LONG y, LONG z, LONG iPointerID){};
	virtual void OnXYNonProjectedUp(LONG x, LONG y, LONG z, LONG iPointerID){};
	// xml annotation event
	virtual void OnXMLAnnotation(BSTR bstrXMLAnnotation){};
	// tool change events
	virtual void OnNoTool(LONG iPointerID){};
	virtual void OnPen(LONG iPointerID){};
	virtual void OnEraser(LONG iPointerID){};
	virtual void OnRectangle(LONG iPointerID){};
	virtual void OnLine(LONG iPointerID){};
	virtual void OnCircle(LONG iPointerID){};
	virtual void OnXMLToolChange(LONG iBoardNumber, BSTR bstrXMLTool){};
	// pen tray button events
	virtual void OnPrint(LONG iPointerID){};
	virtual void OnNext(LONG iPointerID){};
	virtual void OnPrevious(LONG iPointerID){};
	virtual void OnClear(LONG iPointerID){};
	// board status event
	virtual void OnSBSDKBoardStatusChange(void){};
};

// CSBSDKEventsHandler command target

class CSBSDKEventsHandler : public CCmdTarget
{
	DECLARE_DYNAMIC(CSBSDKEventsHandler)

public:
	CSBSDKEventsHandler();
	virtual ~CSBSDKEventsHandler();

	virtual void OnFinalRelease();
	void SetSDKIntf(SBSDKEventIntf *pIntf);

protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	SBSDKEventIntf *m_pIntf;

	void OnXYDown(LONG x, LONG y, LONG z, LONG iPointerID);
	void OnXYMove(LONG x, LONG y, LONG z, LONG iPointerID);
	void OnXYUp(LONG x, LONG y, LONG z, LONG iPointerID);
	void OnXYNonProjectedDown(LONG x, LONG y, LONG z, LONG iPointerID);
	void OnXYNonProjectedMove(LONG x, LONG y, LONG z, LONG iPointerID);
	void OnXYNonProjectedUp(LONG x, LONG y, LONG z, LONG iPointerID);
	void OnXMLAnnotation(BSTR bstrXMLAnnotation);
	void OnNoTool(LONG iPointerID);
	void OnPen(LONG iPointerID);
	void OnEraser(LONG iPointerID);
	void OnRectangle(LONG iPointerID);
	void OnLine(LONG iPointerID);
	void OnCircle(LONG iPointerID);
	void OnPrint(LONG iPointerID);
	void OnNext(LONG iPointerID);
	void OnPrevious(LONG iPointerID);
	void OnClear(LONG iPointerID);
	void OnBoardStatusChange(void);

	void OnXMLToolChange(LONG iBoardNumber, BSTR bstrXMLTool);

	void OnXYDownRaw(LONG x, LONG y, LONG z, LONG iPointerID);
	void OnXYMoveRaw(LONG x, LONG y, LONG z, LONG iPointerID);
	void OnXYUpRaw(LONG x, LONG y, LONG z, LONG iPointerID);

	void OnDViTTrackerData(LONG iBoardNumber, unsigned char ucData1, unsigned char ucData2, unsigned char ucData3);
	void OnDViTRawWidthHeight(LONG iWidth, LONG iHeight, LONG iPointerID);
	void OnDViTAspectRatioAndDeltaAngles(LONG iBoardNumber, double dAspectRatio, double dAngle1, double dAngle2, double dAngle3, double dAngle4);

	void OnDViTMultiPointerMode(LONG iBoardNumber, VARIANT_BOOL bMultiPointerModeOn);

	void OnRightMouse(LONG iPointerID);
	void OnMiddleMouse(LONG iPointerID);
	void OnFloatMouse(LONG iPointerID);
	void OnKeyboard(LONG iPointerID);

	void OnMouseStateChange(SBCSDK_MOUSE_STATE mouseState);
};


