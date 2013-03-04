//  __         ____ ___
// (_ |\/| /\ |_)|   | _  _|_ ._  _ | _  _ o _  _
// __)|  |/--\| \|   |(/_(_| || |(_)|(_)(_||(/__>
//                                       _|
// Copyright © 2003 SMART Technologies Inc.
// C++ Header File ------------------------._.-._.-._.-._.-._.-._.-._.-._.---80|

//This file is intended to list all the messages that make up the SMART Board SDK.
//The "Protocol".

#pragma once

#include <list>
using namespace std;

// GUID {71D7E79E-52BF-43fd-940C-A87854AE32F3}
//static const GUID SBSDKGUID = 
//{ 0x71d7e79e, 0x52bf, 0x43fd, { 0x94, 0xc, 0xa8, 0x78, 0x54, 0xae, 0x32, 0xf3 } };

//Formated for cross-platform
static const unsigned char SBSDKGUID[] = {0x71, 0xd7, 0xe7, 0x9e, 0x52, 0xbf, 0x43, 0xfd,
										  0x94, 0x0c, 0xa8, 0x78, 0x54, 0xae, 0x32, 0xf3}; 

#define SBSDK_MESSAGE_BASE_ID						CMessageConstants::USER_MESSAGE_BASE_ID

//-------------"PUBLIC" SBSDK MESSAGES--------------------------------------------------------------
																				//Server <--> Client
//Get the version of the SMART Board Service.
#define SBSDK_GET_VERSION_REQUEST					SBSDK_MESSAGE_BASE_ID+1		//S<-- C
#define SBSDK_GET_VERSION_RESPONSE					SBSDK_MESSAGE_BASE_ID+2		//S -->C

//Used the by client to make sure all messages have been sent before exiting.
#define SBSDK_FLUSH_REQUEST							SBSDK_MESSAGE_BASE_ID+3		//S<-- C
#define SBSDK_FLUSH_RESPONSE						SBSDK_MESSAGE_BASE_ID+4		//S -->C

//Tool changes, button events and (X,Y) data in projected and non-projected.
#define SBSDK_ON_XY									SBSDK_MESSAGE_BASE_ID+5		//S -->C
#define SBSDK_ON_TOOL								SBSDK_MESSAGE_BASE_ID+6		//S<-->C
#define SBSDK_ON_BUTTON								SBSDK_MESSAGE_BASE_ID+7		//S -->C
#define SBSDK_ON_XY_NON_PROJ						SBSDK_MESSAGE_BASE_ID+8		//S -->C

//Register a projected window to received (X,Y) data.
#define SBSDK_REGISTER_PROJECTED_WINDOW				SBSDK_MESSAGE_BASE_ID+9		//S<-- C

//Board Settings.
#define SBSDK_BOARD_SETTINGS_CHANGE					SBSDK_MESSAGE_BASE_ID+10	//S<-->C
#define SBSDK_BOARD_SETTINGS_UPDATE_REQUEST			SBSDK_MESSAGE_BASE_ID+11	//S<-- C
#define SBSDK_BOARD_SETTINGS_UPDATE_RESPONSE		SBSDK_MESSAGE_BASE_ID+12	//S -->C

// Java window registration request/response
#define SBSDK_REGISTER_WINDOW_BY_NAME_REQUEST		SBSDK_MESSAGE_BASE_ID+13	//S<-- C
#define SBSDK_REGISTER_WINDOW_BY_NAME_RESPONSE		SBSDK_MESSAGE_BASE_ID+14	//S -->C

// XML annotations
#define SBSDK_ON_XML_ANNOT							SBSDK_MESSAGE_BASE_ID+15	//S -->C

//Send whether you want mouse or board events.
#define SBSDK_SEND_MOUSE_EVENTS						SBSDK_MESSAGE_BASE_ID+16    //S<-- C

// Get the current board.
#define SBSDK_GET_CURRENT_BOARD_REQUEST				SBSDK_MESSAGE_BASE_ID+17	//S<-- C
#define SBSDK_GET_CURRENT_BOARD_RESPONSE			SBSDK_MESSAGE_BASE_ID+18	//S<-- C

// Get the current tool
#define SBSDK_GET_CURRENT_TOOL						SBSDK_MESSAGE_BASE_ID+19	//S<-- C

#define SBSDK_TOOL_CHANGE_XML						SBSDK_MESSAGE_BASE_ID+20	//S<-->C
#define SBSDK_DEREGISTER_WINDOW						SBSDK_MESSAGE_BASE_ID+21	//S<-- C
#define SBSDK_SEND_TOOL_CHANGE_XML					SBSDK_MESSAGE_BASE_ID+22	//S<-- C

//-------------"PRIVATE" SBSDK MESSAGES--------------------------------------------------------------

//DViT Tracker data.
#define SBSDK_DVIT_START_TRACKER_MODE				SBSDK_MESSAGE_BASE_ID+50	//S<-- C
#define SBSDK_DVIT_STOP_TRACKER_MODE				SBSDK_MESSAGE_BASE_ID+51	//S<-- C
#define SBSDK_DVIT_TRACKER_DATA						SBSDK_MESSAGE_BASE_ID+52	//S -->C
#define SBSDK_DVIT_RAW_WIDTH_HEIGHT					SBSDK_MESSAGE_BASE_ID+53	//S -->C
#define SBSDK_DVIT_ASPECT_DELTA						SBSDK_MESSAGE_BASE_ID+54	//S -->C

//DViT flashing start, percent complete and result.
#define SBSDK_DVIT_FW_FLASH							SBSDK_MESSAGE_BASE_ID+55	//S<-- C
#define SBSDK_DVIT_FW_FLASH_COMPLETED_PERCENTAGE	SBSDK_MESSAGE_BASE_ID+56	//S -->C
#define SBSDK_DVIT_FW_FLASH_TOTAL_PERCENTAGE		SBSDK_MESSAGE_BASE_ID+57    //S -->C
#define SBSDK_DVIT_FW_FLASH_RESULT					SBSDK_MESSAGE_BASE_ID+58	//S -->C
#define SBSDK_DVIT_START_FLASH_WIZARD				SBSDK_MESSAGE_BASE_ID+59	//S<-- C
#define SBSDK_DVIT_STOP_FLASH_WIZARD				SBSDK_MESSAGE_BASE_ID+60	//S<-- C
#define SBSDK_DVIT_DEVICE_TARGETSECTOR				SBSDK_MESSAGE_BASE_ID+61    //S -->C
#define SBSDK_DVIT_FW_FLASHWIZARD_RESULT			SBSDK_MESSAGE_BASE_ID+62	//S -->C

//DViT camera viewing
#define SBSDK_DVIT_PXL_REQUEST                      SBSDK_MESSAGE_BASE_ID+63    //S<-- C
#define SBSDK_DVIT_VIEW_DATA                        SBSDK_MESSAGE_BASE_ID+64    //S -->C
#define SBSDK_DVIT_PXL_REQUEST_DONE                 SBSDK_MESSAGE_BASE_ID+65    //S<-- C

//DViT camera settings
#define SBSDK_GET_CAMERA_SETTINGS					SBSDK_MESSAGE_BASE_ID+66    //S<-- C
#define SBSDK_ROW_DELAY								SBSDK_MESSAGE_BASE_ID+67    //S -->C
#define SBSDK_FRAME_DELAY							SBSDK_MESSAGE_BASE_ID+68    //S -->C
#define SBSDK_START_COL								SBSDK_MESSAGE_BASE_ID+69    //S -->C
#define SBSDK_START_ROW								SBSDK_MESSAGE_BASE_ID+70    //S -->C
#define SBSDK_ATOD_GAIN								SBSDK_MESSAGE_BASE_ID+71    //S -->C
#define SBSDK_INTEGRATION							SBSDK_MESSAGE_BASE_ID+72    //S -->C

//Send raw command
#define SBSDK_DVIT_RAW_CMD                          SBSDK_MESSAGE_BASE_ID+73    //S<-- C

//Error log changed
#define SBSDK_ERROR_LOG_CHANGED						SBSDK_MESSAGE_BASE_ID+74	//S -->C

//Firmware versions
#define SBSDK_DVIT_GET_SERIAL_NUMS                  SBSDK_MESSAGE_BASE_ID+75    //S<-- C
#define SBSDK_DVIT_SERIAL_NUMS                      SBSDK_MESSAGE_BASE_ID+76    //S<-- C
#define SBSDK_DVIT_GET_FW_VERSIONS                  SBSDK_MESSAGE_BASE_ID+77    //S<-- C
#define SBSDK_DVIT_FW_VERSIONS                      SBSDK_MESSAGE_BASE_ID+78    //S<-- C
#define SBSDK_DVIT_GET_FW_CHECKSUMS                 SBSDK_MESSAGE_BASE_ID+79    //S<-- C
#define SBSDK_DVIT_FW_CHECKSUMS                     SBSDK_MESSAGE_BASE_ID+80    //S<-- C
#define SBSDK_DVIT_GET_FW_UPGRADE_INFO              SBSDK_MESSAGE_BASE_ID+81    //S<-- C
#define SBSDK_DVIT_FW_UPGRADE_INFO                  SBSDK_MESSAGE_BASE_ID+82    //S<-- C
#define SBSDK_WBAR_GET_FW_VERSIONS                  SBSDK_MESSAGE_BASE_ID+83    //S<-- C
#define SBSDK_WBAR_FW_VERSIONS                      SBSDK_MESSAGE_BASE_ID+84    //S<-- C
#define SBSDK_WBAR_GET_FW_CHECKSUMS                 SBSDK_MESSAGE_BASE_ID+85    //S<-- C
#define SBSDK_WBAR_FW_CHECKSUMS                     SBSDK_MESSAGE_BASE_ID+86    //S<-- C

//Register to receive raw (x,y) data.
#define SBSDK_REGISTER_MONITOR_WINDOW				SBSDK_MESSAGE_BASE_ID+87	//S<-- C
#define SBSDK_ON_XY_RAW								SBSDK_MESSAGE_BASE_ID+88	//S<-- C

//Mouse state
#define SBSDK_GET_MOUSE_STATE_REQUEST				SBSDK_MESSAGE_BASE_ID+89	//S<-- C
#define SBSDK_GET_MOUSE_STATE_RESPONSE				SBSDK_MESSAGE_BASE_ID+90	//S -->C
#define SBSDK_SET_MOUSE_STATE						SBSDK_MESSAGE_BASE_ID+91	//S<-- C

//Check for firmware updates
#define SBSDK_CHECK_FOR_FW_UPDATES					SBSDK_MESSAGE_BASE_ID+92	//S<-- C

//Orient
#define SBSDK_REGISTER_ORIENT						SBSDK_MESSAGE_BASE_ID+93	//S<-- C
#define SBSDK_DEREGISTER_ORIENT						SBSDK_MESSAGE_BASE_ID+94	//S<-- C
#define SBSDK_REQUEST_CAMERA_RAW_DATA				SBSDK_MESSAGE_BASE_ID+95	//S<-- C
#define SBSDK_STOP_CAMERA_RAW_DATA					SBSDK_MESSAGE_BASE_ID+96	//S<-- C
#define SBSDK_GO_BACK_ONE_PT_IN_CALIBRATION			SBSDK_MESSAGE_BASE_ID+97	//S<-- C
#define SBSDK_ORIENT_DISPLAY_ROWS_COLUMNS			SBSDK_MESSAGE_BASE_ID+98	//S<-- C
#define SBSDK_DVIT_CALIBRATE_GOTO_NEXT_PT			SBSDK_MESSAGE_BASE_ID+99	//S -->C

//Launch flashing wizard
#define SBSDK_DVIT_LAUNCHED_FLASH_WIZARD			SBSDK_MESSAGE_BASE_ID+100   //S<-- C

//Camera enumeration
#define SBSDK_BEGIN_CAMERA_ENUMERATION				SBSDK_MESSAGE_BASE_ID+101	//S<-- C
#define SBSDK_DVIT_CAMERA_ENUMERATION_STATE			SBSDK_MESSAGE_BASE_ID+102	//S<-- C

//Report PNP and DIP.
#define SBSDK_DO_PNP_ENUMERATION					SBSDK_MESSAGE_BASE_ID+103	//S<-- C
#define SBSDK_REPORT_DIP_SWITCHES					SBSDK_MESSAGE_BASE_ID+104	//S<-- C

//3 camera support mode.
#define SBSDK_DVIT_SET_3_CAMERA_SUPPORT_MODE		SBSDK_MESSAGE_BASE_ID+105	//S<-- C
#define SBSDK_DVIT_GET_3_CAMERA_SUPPORT_MODE		SBSDK_MESSAGE_BASE_ID+106	//S<-- C
#define SBSDK_DVIT_3_CAMERA_SUPPORT_MODE			SBSDK_MESSAGE_BASE_ID+107	//S<-- C

//Flash wizard filename.
#define SBSDK_DVIT_FW_FLASHWIZARD_FILENAME			SBSDK_MESSAGE_BASE_ID+108   //S -->C

//Special messages added for the marker application
#define SBSDK_REGISTER_MARKER						SBSDK_MESSAGE_BASE_ID+109	//S<-- C
#define SBSDK_REGISTER_ANNWND						SBSDK_MESSAGE_BASE_ID+110	//S<-- C
#define SBSDK_DEREGISTER_ANNWND						SBSDK_MESSAGE_BASE_ID+111	//S<-- C
#define SBSDK_BRING_UP_ACETATE						SBSDK_MESSAGE_BASE_ID+112	//S -->C
#define SBSDK_ON_XY_TO_WND							SBSDK_MESSAGE_BASE_ID+113	//S -->C

//2 and 3 camera support for system type.
#define SBSDK_DVIT_SYSTEM_TYPE_REQUEST				SBSDK_MESSAGE_BASE_ID+114	//S<-- C
#define SBSDK_DVIT_SYSTEM_TYPE_RESPONSE				SBSDK_MESSAGE_BASE_ID+115	//S -->C
#define SBSDK_DVIT_CHECK_NUM_ALIVE_CAMS				SBSDK_MESSAGE_BASE_ID+116   //S<-- C
#define SBSDK_DVIT_SET_SYSTEM_TYPE					SBSDK_MESSAGE_BASE_ID+117	//S<-- C
#define SBSDK_DVIT_ALL_ALIVE_CAMS					SBSDK_MESSAGE_BASE_ID+118   //S<-- C

//DViT flash application is started.
#define SBSDK_DVIT_START_FLASH_APP					SBSDK_MESSAGE_BASE_ID+119	//S<-- C
#define SBSDK_GET_CALIBRATION_DATA					SBSDK_MESSAGE_BASE_ID+120	//S<-- C
#define SBSDK_DVIT_CALIBRATION_DATA					SBSDK_MESSAGE_BASE_ID+121	//S<-- C
#define SBSDK_DVIT_STOP_FLASH_APP					SBSDK_MESSAGE_BASE_ID+122	//S<-- C
#define SBSDK_SEND_RAW_INPUT						SBSDK_MESSAGE_BASE_ID+123	//S<-- C
#define SBSDK_DVIT_MULTIPOINTER_MODE_CHANGE			SBSDK_MESSAGE_BASE_ID+124	//S -->C
#define SBSDK_DVIT_GET_MULTIPOINTER_MODE			SBSDK_MESSAGE_BASE_ID+125	//S<-- C

#define SBSDK_WBAR_PRESENT							SBSDK_MESSAGE_BASE_ID+126	//S<-->C
#define SBSDK_WBAR_CHECK_UPGRADE					SBSDK_MESSAGE_BASE_ID+127	//S<-->C
#define SBSDK_WBAR_START_FLASH						SBSDK_MESSAGE_BASE_ID+128   //S<-- C
#define SBSDK_WBAR_STOP_FLASH						SBSDK_MESSAGE_BASE_ID+129   //S<-- C
#define SBSDK_WBAR_FLASH_PECNTAGE					SBSDK_MESSAGE_BASE_ID+130   //S -->C
#define SBSDK_WBAR_FLASH_STEP						SBSDK_MESSAGE_BASE_ID+131   //S -->C
#define SBSDK_WBAR_FLASH_ERROR						SBSDK_MESSAGE_BASE_ID+132   //S -->C

#define SBSDK_DEVICE_CHECK_UPGRADE					SBSDK_MESSAGE_BASE_ID+133   //S<-->C
#define SBSDK_DEVICE_START_FLASH					SBSDK_MESSAGE_BASE_ID+134	//S<-- C
#define SBSDK_DEVICE_FLASH_PECNTAGE					SBSDK_MESSAGE_BASE_ID+135   //S -->C
#define SBSDK_DEVICE_FLASH_STEP						SBSDK_MESSAGE_BASE_ID+136   //S -->C
#define SBSDK_DEVICE_FLASH_ERROR					SBSDK_MESSAGE_BASE_ID+137   //S -->C

#define SBSDK_BOARD_TEST							SBSDK_MESSAGE_BASE_ID+138   //S<-->C
#define SBSDK_SET_MARKER_APP						SBSDK_MESSAGE_BASE_ID+139   //S<-- C
#define SBSDK_GET_PEN_TRAY_TOOL_TYPE_REQUEST		SBSDK_MESSAGE_BASE_ID+140   //S<-- C
#define SBSDK_GET_PEN_TRAY_TOOL_TYPE_RESPONSE		SBSDK_MESSAGE_BASE_ID+141   //S -->C
#define SBSDK_DEVICE_HARDWARE_VERSION				SBSDK_MESSAGE_BASE_ID+142   //S -->C
#define SBSDK_SET_PEN_TRAY_TOOL_XML					SBSDK_MESSAGE_BASE_ID+143   //S<-- C
#define SBSDK_GET_PEN_TRAY_TOOL_XML_REQUEST			SBSDK_MESSAGE_BASE_ID+144   //S<-- C
#define SBSDK_GET_PEN_TRAY_TOOL_XML_RESPONSE		SBSDK_MESSAGE_BASE_ID+145   //S -->C
#define SBSDK_SET_PEN_TRAY_BUTTON_ACTION			SBSDK_MESSAGE_BASE_ID+146   //S<-- C
#define SBSDK_GET_PEN_TRAY_BUTTON_ACTION_REQUEST	SBSDK_MESSAGE_BASE_ID+147   //S<-- C
#define SBSDK_GET_PEN_TRAY_BUTTON_ACTION_RESPONSE	SBSDK_MESSAGE_BASE_ID+148   //S -->C

//LinQ
#define SBSDK_GET_LINQ_SETTINGS_REQUEST				SBSDK_MESSAGE_BASE_ID+149	//S<-- C
#define SBSDK_GET_LINQ_SETTINGS_RESPONSE			SBSDK_MESSAGE_BASE_ID+150	//S -->C
#define SBSDK_GET_LINQ_BOARDNAME_REQUEST			SBSDK_MESSAGE_BASE_ID+151	//S<-- C
#define SBSDK_GET_LINQ_BOARDNAME_RESPONSE			SBSDK_MESSAGE_BASE_ID+152	//S -->C
#define SBSDK_SET_LINQ_SETTINGS						SBSDK_MESSAGE_BASE_ID+153	//S -->C
#define SBSDK_SET_LINQ_BOARDNAME					SBSDK_MESSAGE_BASE_ID+154	//S -->C

#define SBSDK_MOUSE_STATE_CHANGE					SBSDK_MESSAGE_BASE_ID+155	//S -->C
#define SBSDK_DVIT_SET_MULTIPOINTER_MODE			SBSDK_MESSAGE_BASE_ID+156	//S<-- C
#define SBSDK_RAW_DATA_TO_CONTROLLER				SBSDK_MESSAGE_BASE_ID+157	//S<-->C

//Slate
#define SBSDK_BATTERY_STATE_UPDATE					SBSDK_MESSAGE_BASE_ID+158	//S -->C
#define SBSDK_GET_BATTERY_STATE_UPDATE				SBSDK_MESSAGE_BASE_ID+159	//S<-- C
#define SBSDK_STYLUS_TIP_CONFIG_CHANGE				SBSDK_MESSAGE_BASE_ID+160	//S<-- C
#define SBSDK_GET_STYLUS_TIP_STATE_UPDATE			SBSDK_MESSAGE_BASE_ID+161	//S<-- C
#define SBSDK_STYLUS_TIP_STATE_UPDATE				SBSDK_MESSAGE_BASE_ID+162	//S -->C

// Popup and touch-hold handling
#define SBSDK_GET_HARDWARE_POPUP_REQUEST			SBSDK_MESSAGE_BASE_ID+163	//S<-- C
#define SBSDK_GET_HARDWARE_POPUP_RESPONSE			SBSDK_MESSAGE_BASE_ID+164	//S -->C
#define SBSDK_SET_HARDWARE_POPUP					SBSDK_MESSAGE_BASE_ID+165	//S<-- C
#define SBSDK_SET_TOUCH_HOLD						SBSDK_MESSAGE_BASE_ID+166	//S<-- C
#define SBSDK_GET_TOUCH_HOLD_REQUEST				SBSDK_MESSAGE_BASE_ID+167	//S<-- C
#define SBSDK_GET_TOUCH_HOLD_RESPONSE				SBSDK_MESSAGE_BASE_ID+168	//S -->C

// bluewave
#define SBSDK_PEN_FINGER_MODE						SBSDK_MESSAGE_BASE_ID+169	//S<-- C
#define SBSDK_SET_BLUEWAVE_PEN_ACTION				SBSDK_MESSAGE_BASE_ID+170	//S<-- C
#define SBSDK_GET_BLUEWAVE_PEN_ACTION_REQUEST		SBSDK_MESSAGE_BASE_ID+171	//S<-- C
#define SBSDK_GET_BLUEWAVE_PEN_ACTION_RESPONSE		SBSDK_MESSAGE_BASE_ID+172	//S -->C
#define SBSDK_SET_BLUEWAVE_ROOM_CONTROL_BTN_APP			SBSDK_MESSAGE_BASE_ID+173	//S<-- C
#define SBSDK_BLUEWAVE_ROOM_CONTROL_BTN_APP_REQUEST		SBSDK_MESSAGE_BASE_ID+174	//S<-- C
#define SBSDK_BLUEWAVE_ROOM_CONTROL_BTN_APP_RESPONSE	SBSDK_MESSAGE_BASE_ID+175	//S -->C
#define SBSDK_PEN_FINGER_MODE_REQUEST				SBSDK_MESSAGE_BASE_ID+176	//S<-- C
#define SBSDK_PEN_FINGER_MODE_RESPONSE				SBSDK_MESSAGE_BASE_ID+177	//S<-- C

#define SBSDK_RIGHT_CLICKBUBBLE						SBSDK_MESSAGE_BASE_ID+178



//PDA (Chinook)
#define SBSDK_GET_PDA_SETTINGS_REQUEST				SBSDK_MESSAGE_BASE_ID+179	//S<-- C
#define SBSDK_GET_PDA_SETTINGS_RESPONSE				SBSDK_MESSAGE_BASE_ID+180	//S -->C
#define SBSDK_SET_PDA_SETTINGS						SBSDK_MESSAGE_BASE_ID+181	//S<-- C

#define SBSDK_PREPARE_REPORT						SBSDK_MESSAGE_BASE_ID+182	//S<-- C
#define SBSDK_REPORT_READY							SBSDK_MESSAGE_BASE_ID+183	//S-->C

// !!! WHEN YOU ADD A NEW MESSAGE YOU MUST UPDATE THESE SO THEY ARE THE LARGEST ID'S !!!
#define MSG_MAX_CLIENT_INCOMING_MESSAGE				SBSDK_REPORT_READY
#define MSG_MAX_SERVER_INCOMING_MESSAGE				SBSDK_REPORT_READY

enum SBSDK_CONTACT_STATE
{
	SB_CONTACT_NONE = 0,
	SB_CONTACT_DOWN,
	SB_CONTACT_MOVE,
	SB_CONTACT_UP
};
enum SBSDK_BUTTON_ACTION
{
	SB_PRINT = 1,
	SB_NEXT,
	SB_PREVIOUS,
	SB_CLEAR,
	SB_RIGHT_MOUSE,
	SB_MIDDLE_MOUSE,
	SB_FLOAT_MOUSE,
	SB_KEYBOARD,
	SB_FLOATINGTOOLS,
	SB_NOTEBOOK,
	SB_SCREENCAPTURE,
	SB_NOTOOLS,
	SB_ERASERBUTTON,
	SB_BLACKPEN,
	SB_BLUEPEN,
	SB_REDPEN,
	SB_GREENPEN,
	SB_DISABLED
};
enum DVIT_TARGET
{
	DVIT_CAMERA_1 = 0,
	DVIT_CAMERA_2,
	DVIT_CAMERA_3,
	DVIT_CAMERA_4,
	DVIT_CAMERA_ALL,
	DVIT_MASTER = 8
};
enum DVIT_FW_SECTOR
{
	DVIT_BOOT = 0,
	DVIT_BOOT_OLD = 0,
	DVIT_USER_OLD = 1,
	DVIT_ALT_BOOT = 1,
	DVIT_USER,
	DVIT_MANUFACTURING
};


//Board Status (From ServiceSettings.h)
enum SB_BOARD_STATUS {
	SB_BOARD_OPEN,					//This SMART Board is working properly.
	SB_BOARD_CLOSED,				//This SMART Board is not currently active. (The port is closed)
	SB_BOARD_DEVICE_IN_USE,			//The port for this device is in use by another application.
	SB_BOARD_DEVICE_NOT_FOUND,		//The port for this device could not be found
	SB_BOARD_DEVICE_NOT_PRESENT,	//This SMART Board is unplugged or not available.
	SB_BOARD_NO_DATA,				//No data is being received on this port.
	SB_BOARD_ANALYZING_DATA,		//Analyzing incoming data to determine if it is a SMART Board...
	SB_BOARD_NOT_RESPONDING,		//Data is being received, but communication to the SMART Board is not working correctly.  The LEDs will not function properly.
	SB_BOARD_DISABLED,				//Board has been disabled by registry setting.
	SB_BOARD_USB_UNPLUGGED			//The USB dongle is plugged in but not connected to the SMART Board.
};

enum SBSDK_MOUSE_STATE
{
	SB_MOUSE_LEFT_CLICK,
	SB_MOUSE_MIDDLE_CLICK,
	SB_MOUSE_RIGHT_CLICK,
	SB_MOUSE_CTRL_CLICK,
	SB_MOUSE_ALT_CLICK,
	SB_MOUSE_SHIFT_CLICK,
	SB_MOUSE_FLOAT_CLICK,
	SB_MOUSE_NEXT_CLICK_NOT_SET
};

enum APP_TYPE{
	NORMAL,
	MARKER,
	ANNWND
};

enum WBAR_FW_TYPE{
	SB_WB_FW_BOOT = 0,
	SB_WB_FW_VERSION
};

enum SBSDK_BLUEWAVE_PEN_ACTION {
	SB_BWPA_NONE = 0,
	SB_BWPA_BLACK_PEN,
	SB_BWPA_BLUE_PEN,
	SB_BWPA_RED_PEN,
	SB_BWPA_GREEN_PEN,
	SB_BWPA_POINTER
};

//We should share this with SMARTBoardStreamAnalyzer.h somehow.
#define SB_MODEL_SC4							0x80//SC4					(byte 4)
#define SB_MODEL_SC5							0x81//SC5					(byte 4)
#define SB_MODEL_SC6							0x82//SC6					(byte 4)
#define SB_MODEL_SC4_PLASMA						0x83//SC4 Plasma			(byte 4)
											  //0x84//?						(byte 4)
#define SB_MODEL_SC7							0x85//SC7					(byte 4)
#define SB_MODEL_CRANE							0x86//Crane					(byte 4)
#define SB_MODEL_SYMPODIUM						0x87//Sympodium				(byte 4)
#define SB_MODEL_ROOM_CONTROL					0x88//Room Control			(byte 4)
#define SB_MODEL_XPORT30						0x89//XPort 30				(byte 4)
#define SB_MODEL_WONDERBAR						0x8A//Wonderbar				(byte 4)
#define SB_MODEL_XBOW							0x8B//SC8(DViT)				(byte 4)
#define SB_MODEL_BLUEWAVE						0x8e//Bluewave
#define SB_MODEL_MAGNUS		 					0x8f//Magnus
#define SB_MODEL_COMPASS						0x90//compass
#define SB_MODEL_WIRELESS_SLATE					0x91//WirelessSlate

typedef struct tagSBSDK_TOOL
{
	int iPointerID, iToolType, iStrokeWidth;
	COLORREF rgbStroke, rgbFill;
	float fStrokeOpacity, fFillOpacity;
} SBSDK_TOOL;

typedef list<SBSDK_TOOL> SBSDK_TOOL_LIST;

#define DEFAULT_TOOL_XML L"<no_tool />"

typedef struct tagSBSDK_ORIENT_PTS
{
	long lX;	//original X and Y value
	long lY;

	long lRX;	//raw X and Y value
	long lRY;
} SBSDK_ORIENT_PTS;

// flag values for changes to SBSDK_BOARD_SETTINGS
// board number never changes
// model number never changes
#define SB_SETTINGS_STATUS						0x0001
#define SB_SETTINGS_PROJECTED					0x0002
#define SB_SETTINGS_ORIENT_PTS					0x0004
#define SB_SETTINGS_DISPLAY_RECT				0x0008
#define SB_SETTINGS_DISPLAY_NAME				0x0010
#define SB_SETTINGS_PEN_TRAY_STRIP_RECT			0x0020
#define SB_SETTINGS_DISPLAY_STRIP_RECT			0x0040
#define SB_SETTINGS_CUSTOM_STRIP_RECT			0x0080
#define SB_SETTINGS_NON_PROJ_ORIENT				0x0100
#define SB_SETTINGS_ORIENT_PRECISION			0x0200
#define SB_SETTINGS_MOUSE_ASSIST				0x0400	
#define SB_SETTINGS_ISHOVER						0x0800	
#define SB_SETTINGS_DOUBLECLICK_ZONE_SIZE		0x1000
#define	SB_SETTINGS_DOUBLECLICK_EDGE			0x2000
#define SB_SETTINGS_ISSWAP_HORIZONTAL			0x4000
#define SB_SETTINGS_ISSWAP_VERTICAL				0x8000
#define SB_SETTINGS_ISSWAP_AXIS				   0x10000
#define SB_SETTINGS_ISSWAP_AUTO				   0x20000
#define SB_SETTINGS_GESTURE_RECOGNITION		   0x40000

#define MAX_ORIENT_PTS 80
#define MAX_DISPLAY_NAME 32

typedef struct tagSBSDK_BOARD_SETTINGS
{
	unsigned int uFlags;
	int iBoardNumber;
	unsigned char m_ucSCModel;
	SB_BOARD_STATUS boardStatus;
	bool bIsProjected;

	// orient pts
	int iOrientPtsHeight;
	int iOrientPtsWidth;
	SBSDK_ORIENT_PTS pOrientPts[MAX_ORIENT_PTS];

	// display rect
	int iDisplayTop;
	int iDisplayLeft;
	int iDisplayBottom;
	int iDisplayRight;

	wchar_t wszDisplayName[MAX_DISPLAY_NAME];

	// pen tray strip
	int iPenTrayStripTop;
	int iPenTrayStripLeft;
	int iPenTrayStripBottom;
	int iPenTrayStripRight;
	int iPenTrayStripButtonCount;

	// display strip
	int iDisplayStripTop;
	int iDisplayStripLeft;
	int iDisplayStripBottom;
	int iDisplayStripRight;
	int iDisplayStripButtonCount;

	// custom strip
	int iCustomStripTop;
	int iCustomStripLeft;
	int iCustomStripBottom;
	int iCustomStripRight;

	// non-proj orient data
	int iNonProjTop;
	int iNonProjLeft;
	int iNonProjHeight;
	int iNonProjWidth;

	//mouse settings
	bool bIsMouseAssistOn;
	int iIsDoHover;
	int iDoubleClickZoneSize;
	int iDoubleClickEdge;

	//Axis settings
	bool bIsSwapHorizontal;
	bool bIsSwapVertical;
	bool bIsSwapAxis;
	bool bIsSwapAuto;

	// more orient settings
	int iOrientPrecision;

	bool bGestureRecognition;
} SBSDK_BOARD_SETTINGS;

typedef list<SBSDK_BOARD_SETTINGS> SBSDK_BOARD_SETTINGS_LIST;
