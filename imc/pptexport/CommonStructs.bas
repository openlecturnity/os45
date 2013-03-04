Attribute VB_Name = "CommonStructs"
Option Explicit


'-----------------------------
' Type definition for Win API
'-----------------------------
  Type SYSTEMTIME
     wYear          As Integer
     wMonth         As Integer
     wDayOfWeek     As Integer
     wDay           As Integer
     wHour          As Integer
     wMinute        As Integer
     wSecond        As Integer
     wMilliseconds  As Integer
  End Type
  

  Private Type SECURITY_ATTRIBUTES
      nLength               As Long
      lpSecurityDescriptor  As Long
      bInheritHandle        As Long
  End Type

' Structures for not supported shapes
Type ObjectList
    iObject As Integer
    iObjectCount As Integer
End Type

Type NotSupportedObjects
    iPage As Long
    iObjectList() As ObjectList
End Type


'--------------------
' User defined types
'--------------------
  Type ExtendedShape
    pptShape            As Shape
    lecturnityShape     As aofShape
    IsAnimated          As Boolean
    lObjectLinkId       As Long
    animatedParagraph() As Long ' list of animated paragraphs
    ParagraphCount      As Long
  End Type
  
  Type WBPoint
      x                 As Double
      y                 As Double
  End Type

  Type aofNode
      x                 As Single
      y                 As Single
      SegType           As MsoSegmentType
  End Type

  Type aofGlobalOptions
      MacroID           As Integer      ' "Save as Lecturnity documents..." ID = 1
                                        ' "Open with Lecturnity Assistant"  ID = 2, ppt2lecturnity3 id=1
      pptFileName       As String
  
      WBFilePath        As String
      WBFileName        As String
      ImgFileNamePrefix As String       ' the prefix of image file name: fill the blanks with "_"s
      ExpImageFormat    As PpShapeFormat
      ExpImageSuffix    As String
      ExpMetafileFormat As PpShapeFormat
      ExpMetafileSuffix As String
      ExpTextFormat     As PpShapeFormat
      ExpTextSuffix     As String
      ExpOLEFormat      As PpShapeFormat
      ExpOLESuffix      As String
    
      WithImageSize     As Boolean
    
      ConvertBackgAsImage As Boolean
      NoGroup           As Boolean
      WithBackBox       As Boolean
      WithDebugInfo     As Boolean
      LogFileName       As String
    
      TimeBegin         As SYSTEMTIME
      TimeEnd           As SYSTEMTIME
    
      Failed            As Boolean
      FailInfo          As Long
      FailMsg           As String
      
  End Type

  Type aofAnimSetting
      AnimationId       As Long
      TriggerId         As Long
      AnimationType     As Long         ' 0 = Exit, 1 = Enter
      AnimOrder         As Long         ' recorde the animation order of one shape, no anim = 0
      HasSubAnim        As Boolean         ' the animation within a shape (Text shape)
      AfterEffect       As Long
      AnimInReverse     As Boolean
      AnimBeforeLevel   As Long
      UpperLevelIndex   As Long
      CallObjectId      As Long
  End Type

  Type aofTmplInfo                       ' record the template setting of one slide
      Valid             As Boolean
      layout            As Long
     'BackColor         As Long
      BackColor         As String
     'TmplName          As String
      ObjNum            As Long
      tmplIdx           As Long
  End Type
  
