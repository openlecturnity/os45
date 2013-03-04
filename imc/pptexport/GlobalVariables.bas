Attribute VB_Name = "GlobalVariables"

' Language dependent strings
Public g_szDlgCaption   As String
Public g_szMsgDiskFull As String
Public g_szMsgOverwriteImage As String

' Strings for master placeholder
Public g_szFooter               As String       ' Extract string to replace footer
Public g_szSlideNumber          As String       ' Extract string to replace slide number
Public g_szDateAndTime          As String       ' Extract string to replace date and time

' Miscellaneous variables
Public g_currentPresentation    As Presentation      ' current presentation which should be converted.

Public g_LecturnityDocument    As AofDoc

Public g_lApplicationVersion    As Long
Public g_bConversionSucceeded   As Boolean

Public g_lCurrentPageNumber     As Long         ' recorde the current slide index in order to replace the field "<Nr.>"
Public g_lCurrentIndex          As Long

Public g_lHorizontalResolution  As Long
Public g_lVerticalResolution    As Long
Public g_lSlideWidth            As Long
Public g_lSlideHeight           As Long

Public g_lSlideList()           As Long
Public g_lSlideCount            As Long

Public g_ImportWizardHwnd       As Long

Public g_conversionOptions      As aofGlobalOptions

Public g_lIndentLevel           As Long                 ' control the indent of the lsd content.

Public g_shapeIsInTemplate      As Boolean              ' tell the Shape.Script() when the field <...> should be replaced.

' Varibles for not supported shapes
Public g_lsNotSupported()       As NotSupportedObjects
Public g_iNotSupportedCount     As Integer
Public Const g_iNotSupportedObjectCount As Integer = 13

' Event handling
Public g_cPPTObject             As cEventClass       ' PowerPoint event trap class
Public g_bTrapFlag              As Boolean             ' a flag to identify the menu item status.


