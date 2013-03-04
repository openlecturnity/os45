Attribute VB_Name = "aofDefine"
Option Explicit

'--------------------
' const definition
'--------------------
  Public Const CurrentVersion As String = "1.6.1"
  Public Const aofItemInc As Long = 5
  Public Const MAXCTLPOINTS = 12                     ' Bezier curve const
  Public Const POINTDIST = 2                         ' the distance between two adjacent points in a Bezier curve
  Public Const PI As Double = 3.14159265358979

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
  
  Type OPENFILENAME
     lStructSize        As Long
     hwndOwner          As Long
     hInstance          As Long
     lpstrFilter        As String
     lpstrCustomFilter  As String
     nMaxCustFilter     As Long
     nFilterIndex       As Long
     lpstrFile          As String
     nMaxFile           As Long
     lpstrFileTitle     As String
     nMaxFileTitle      As Long
     lpstrInitialDir    As String
     lpstrTitle         As String
     flags              As Long
     nFileOffset        As Integer
     nFileExtension     As Integer
     lpstrDefExt        As String
     lCustData          As Long
     lpfnHook           As Long
     lpTemplateName     As String
  End Type

  Private Type SHITEMID
      cb        As Long
      abID      As Byte
  End Type

  Private Type ITEMIDLIST
      mkid      As SHITEMID
  End Type

  Private Type SECURITY_ATTRIBUTES
      nLength               As Long
      lpSecurityDescriptor  As Long
      bInheritHandle        As Long
  End Type

'-----------------
' Win32 APIs
'-----------------
  Declare Function GetSaveFileName Lib "comdlg32" Alias "GetSaveFileNameA" _
                        (pOpenfilename As OPENFILENAME) As Long

  Declare Function GetActiveWindow Lib "user32" () As Long

  Declare Sub GetLocalTime Lib "kernel32" (lpSystem As SYSTEMTIME)

  Declare Function ShellExecute Lib "shell32.dll" Alias "ShellExecuteA" _
                        (ByVal hWnd As Long, _
                         ByVal lpOperation As String, _
                         ByVal lpFile As String, _
                         ByVal lpParameters As String, _
                         ByVal lpDirectory As String, _
                         ByVal nShowCmd As Long) As Long

  Private Declare Function SHGetSpecialFolderLocation Lib "shell32.dll" _
                        (ByVal hwndOwner As Long, _
                         ByVal nFolder As Long, _
                         pidl As ITEMIDLIST) As Long
  
  Private Declare Function SHGetPathFromIDList Lib "shell32.dll" Alias "SHGetPathFromIDListA" _
                        (ByVal pidl As Long, _
                         ByVal pszPath As String) As Long

  Private Declare Function CreateDirectory Lib "kernel32" Alias "CreateDirectoryA" _
                        (ByVal lpPathName As String, _
                         lpSecurityAttributes As SECURITY_ATTRIBUTES) As Long
  
  ' API declarations for FindWindow() & LockWindowUpdate()
  ' Use FindWindow API to locate the PowerPoint handle.
  Declare Function FindWindow Lib "user32" Alias "FindWindowA" _
                        (ByVal lpClassName As String, _
                         ByVal lpWindowName As Long) As Long
                         
  ' Use LockWindowUpdate to prevent/enable window refresh
  Declare Function LockWindowUpdate Lib "user32" _
                        (ByVal hwndLock As Long) As Long
                        
  ' Use UpdateWindow to force a refresh of the PowerPoint window
  Declare Function UpdateWindow Lib "user32" (ByVal hWnd As Long) As Long
 
'--------------------
' dll functions
'--------------------
  ' Convert WMF files to PNG
  Declare Function ConvertWMFtoPNG Lib "ppt2lecturnity.dll" _
                        (ByVal WMFname As String, _
                         ByVal PNGname As String, _
                         ByVal bDelTrans As Integer) As Integer
  
'--------------------
' User defined types
'--------------------
  Type WBPoint
      X                 As Double
      Y                 As Double
  End Type

  Type aofNode
      X                 As Single
      Y                 As Single
      SegType           As Long     ' MsoSegmentType
  End Type

  Type aofGlobalOptions
      MacroID           As Integer      ' "Save as Lecturnity documents..." ID = 1
                                        ' "Open with Lecturnity Assistant"  ID = 2, ppt2lecturnity3 id=1
      PPTFileName       As String
  
      WBFilePath        As String
      WBFileName        As String
      ImgFileNamePrefix As String       ' the prefix of image file name: fill the blanks with "_"s
      ExpImageFormat    As Long         ' As PpShapeFormat
      ExpImageSuffix    As String
    
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
      
      NoMsgDlg          As Boolean      ' in batch mode (command line), do not show any dialogs.
  End Type

  Type aofAnimSetting
      AnimOrder         As Long         ' recorde the animation order of one shape, no anim = 0
      HasSubAnim        As Boolean         ' the animation within a shape (Text shape)
      Resistant         As Long
      AnimInReverse     As Boolean
      AnimBeforeLevel   As Long
      UpperLevelIndex   As Long
  End Type

  Type aofTmplInfo                       ' record the template setting of one slide
      Valid             As Boolean
      Layout            As Long
     'BackColor         As Long
      BackColor         As String
     'TmplName          As String
      ObjNum            As Long
      tmplIdx           As Long
  End Type
  
  Type aofTextSegment           ' record the textrange information in a Template textbox.
      ID                As Long         ' index
      ChrCount          As Long         ' number of characters
      BeginPos          As Long         ' the position of the leading character in the whole shape
      EndPos            As Long         ' end position
      Width             As Long         ' the width of the string (in points)
      ContainATF        As Boolean      ' contains Auto text field
  End Type

'---------------------
' User defined enums
'---------------------
  'Enum aofGraphStyle
  Public Const aofGraphStyleNone = 0
  Public Const aofGraphStyleOutline = 1
  Public Const aofGraphStyleFilled = 2
  Public Const aofGraphStyleFilledOutline = 3
  'End Enum

  'Enum aofShapeType
  Public Const aofShapeTypeNone = 0
  Public Const aofShapeTypeLine = 1
  Public Const aofShapeTypeText = 2
  Public Const aofShapeTypeGroup = 3
  Public Const aofShapeTypePolygon = 4
  Public Const aofShapeTypeFreehand = 5
  Public Const aofShapeTypeRect = 6
  Public Const aofShapeTypeCirc = 7
  Public Const aofShapeTypeImage = 8
  Public Const aofShapeTypeTextGroup = 9
  Public Const aofShapeTypeUserDefinedGroup = 10
  'End Enum

  'Enum aofFooterHeaderTypes
  Public Const aofNoFooterHeader = 0
  Public Const aofDateTime = 1
  Public Const aofFooter = 2
  Public Const aofSlideNumber = 3
  'End Enum

  'Enum aofErrorType
  Public Const aofErrSlideInfo = 0
  Public Const aofErrUnknown = 1
  Public Const aofErrInvalidTmplName = 2
  Public Const aofErrInvisibleLine = 3
  Public Const aofErrInvisibleShape = 4
  Public Const aofErrNonstandardLine = 5
  Public Const aofErrUnknownBullet = 6
  Public Const aofErrCreateFileFailed = 7
  Public Const aofErrArgumentError = 8
  Public Const aofErrNotSolidFilled = 9
  Public Const aofErrNoNodes = 10
  Public Const aofErrPolygonNoNodes = 11
  Public Const aofErrIllegalImage = 12
  Public Const aofErrInvalidPolygon = 13
  Public Const aofErrInfo = 14
  Public Const aofBeginTimeStamp = 15
  Public Const aofEndTimeStamp = 16
  'End Enum

  'Enum aofTextShapeType               ' no use any more
  Public Const aofTextShapeTypeContainer = 0
  Public Const aofTextShapeTypeAtom = 1                ' text segment without tabstop
  Public Const aofTextShapeTypeBullet = 2
  'End Enum

Public Function CreateNewDirectory(NewDirectory As String) As Boolean
    Dim sDirTest As String
    Dim SecAttrib As SECURITY_ATTRIBUTES
    Dim bSuccess As Long
    Dim sPath As String
    Dim iCounter As Integer
    Dim sTempDir As String
    Dim iFlag As Long
    
    On Error Resume Next
    iFlag = 0
    sPath = NewDirectory
    If Right(sPath, 1) <> "\" Then
        sPath = sPath & "\"
    End If
    iCounter = 1
    Do Until InStr(iCounter, sPath, "\") = 0
        iCounter = InStr(iCounter, sPath, "\")
        sTempDir = Left(sPath, iCounter)
        sDirTest = Dir(sTempDir, vbDirectory)
        iCounter = iCounter + 1
        
        If sDirTest = "" Then
        'create directory
            SecAttrib.lpSecurityDescriptor = &O0
            SecAttrib.bInheritHandle = False
            SecAttrib.nLength = Len(SecAttrib)
            bSuccess = CreateDirectory(sTempDir, SecAttrib)
            If bSuccess = 0 Then
                CreateNewDirectory = False
                Exit Function
            End If
        End If
    Loop
    CreateNewDirectory = True
End Function

Private Function GetSpecialfolder(CSIDL As Long) As String
    Dim r As Long
    Dim IDL As ITEMIDLIST
    Dim path As String
    'Get the special folder
    r = SHGetSpecialFolderLocation(100, CSIDL, IDL)
    If r = 0 Then
        'Create a buffer
        path = Space$(1024)
        'Get the path from the IDList
        r = SHGetPathFromIDList(ByVal IDL.mkid.cb, ByVal path$)
        'Remove the unnecessary chr$(0)'s
        GetSpecialfolder = Left$(path, InStr(path, Chr$(0)) - 1)
        Exit Function
    End If
    GetSpecialfolder = ""
End Function

Function GetUserProfDir() As String
    
    Const CSIDL_APPDATA  As Long = &H1A
    
    Dim sBuffer As String
    
    sBuffer = GetSpecialfolder(CSIDL_APPDATA) & "\Lecturnity\"
    
    On Error Resume Next
    
    MkDir (sBuffer)             ' if the dir exist, a run-time error raised.
    If Err.Number <> 0 Then     ' directory ".\Lecturnity\" exist
        'Kill sBuffer & "*.*"    ' clear the directory
        Err.Clear
    End If
    On Error GoTo 0
    
    GetUserProfDir = sBuffer
    
End Function

Function RunLsdFile(lsdFile As String) As Boolean
    
    Dim ret As Long
    Dim ErrMsg As String
    
    ret = ShellExecute(0, "open", lsdFile, "", "", 1) ' winuser.h: #define SW_SHOWNORMAL       1
    If ret > 32 Then            ' return value greater than 32 if successful.
        RunLsdFile = True
    Else
        Dim extRes As Object
        
        Set extRes = CreateObject("ConvRes.ExtResource")
        ErrMsg = extRes.GetOpenLSDFileErrorMsg()
        Set extRes = Nothing
        
        MsgBox ErrMsg, vbCritical, DlgCaption
        RunLsdFile = False
    End If
    
End Function

Function NewLine() As String
    NewLine = vbLf & String(IndentLevel + IndentLevel, " ")
End Function

Function IncIndent() As Long
    IndentLevel = IndentLevel + 1
End Function

Function DecIndent() As Long
    IndentLevel = IndentLevel - 1
End Function

Public Function MLBColor(col As ColorFormat) As String
    Dim str As String
    Dim i   As Integer
    
    str = Hex$(col.RGB)
    For i = 1 To 6 - Len(str)
        str = "0" & str
    Next
    MLBColor = "#" & Mid(str, 5, 2) & Mid(str, 3, 2) & Mid(str, 1, 2)

End Function

Function GetFormalFileName() As String
    ' get the file name
    Dim defName As String
    Dim suff As String
    Dim slashPos As Long        ' the position of "\"
    
    defName = GlobalPres.FullName
    
    Do    ' find out the filename (without path)
        slashPos = InStr(1, defName, "\", vbTextCompare)
        If slashPos > 0 Then
            defName = Right(defName, Len(defName) - slashPos)
        End If
    Loop While (slashPos > 0)
    
    If Len(defName) > 4 Then        ' if < 4 without extension
        suff = Mid(defName, Len(defName) - 3, 4)
        If StrComp(suff, ".ppt", vbTextCompare) = 0 Or _
            StrComp(suff, ".pps", vbTextCompare) = 0 Or _
            StrComp(suff, ".pot", vbTextCompare) = 0 Then
                defName = Left(defName, Len(defName) - 4)
        End If
    End If
    
    If defName = "" Then
        defName = "noname"
    End If
    
    GetFormalFileName = defName
End Function

Function GetSaveInfo(ByRef fpath As String, ByRef fname As String, Optional langID As Integer = 0) As Boolean
    Dim ofn As OPENFILENAME
    Dim initDir As String
    Dim FileName As String
    Dim LsdDesc     As String
    Dim AllFileDesc As String
    
    Dim extRes As Object
    
    Set extRes = CreateObject("ConvRes.ExtResource")
    LsdDesc = extRes.GetSaveAsLsdDesc()
    AllFileDesc = extRes.GetSaveAsAllFileDesc()
    Set extRes = Nothing
    
    FileName = GetFormalFileName() & ".lsd"
    
    With ofn
        .lStructSize = Len(ofn)
        .hwndOwner = GetActiveWindow()
        .flags = 33798   '0x8406 = OFN_EXTENSIONDIFFERENT | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN | OFN_HIDEREADONLY
        .lpstrDefExt = "lsd"
        .lpstrFilter = LsdDesc & " (*.lsd)" & Chr(0) & "*.lsd" & Chr(0) _
                    & AllFileDesc & " (*.*)" & Chr(0) & "*.*" & Chr(0) & Chr(0)
        .nMaxFile = 1024
        .lpstrFile = FileName & Space(.nMaxFile - LenB(FileName) - 1) & Chr(0)
        .nMaxFileTitle = 1024
        .lpstrFileTitle = Space(.nMaxFileTitle - 1) & Chr(0)
        .lpstrInitialDir = initDir
    End With
    
    If GetSaveFileName(ofn) = 0 Then
        GetSaveInfo = False
        Exit Function
    End If
    
    fname = Left(ofn.lpstrFileTitle, InStr(ofn.lpstrFileTitle, Chr(0)) - 1)
    fpath = Left(ofn.lpstrFile, InStr(ofn.lpstrFile, fname) - 1)
    
    GetSaveInfo = True

End Function

'Sub ErrLog(errtype As aofErrorType, Optional desc As String = "")
Sub ErrLog(errtype As Long, Optional desc As String = "")
    If Not CvtOpt.WithDebugInfo Then
        Exit Sub
    End If
    Dim strlog      As String
    Select Case errtype
        Case aofErrSlideInfo
            strlog = "In Slide: " & desc
        Case aofBeginTimeStamp
            With CvtOpt.TimeBegin
                strlog = "Conversion began: " & .wYear & "-" _
                    & .wMonth & "-" & .wDay & " " _
                    & .wHour & ":" & .wMinute & ":" & .wSecond & ":" _
                    & .wMilliseconds & vbLf
            End With
        Case aofEndTimeStamp
            With CvtOpt.TimeEnd
                strlog = "Conversion ended: " & .wYear & "-" _
                    & .wMonth & "-" & .wDay & " " _
                    & .wHour & ":" & .wMinute & ":" & .wSecond & ":" _
                    & .wMilliseconds & vbLf
            End With
        Case Else
            strlog = desc
    End Select
    
    WriteToLog strlog
End Sub

Sub WriteToLog(str As String)
    Dim Dateinummer

    Dateinummer = FreeFile    ' neue Datei-Nr.
    Open CvtOpt.LogFileName For Append As #Dateinummer     ' Dateiname

    Print #Dateinummer, str    ' Text ausgeben.
    Close #Dateinummer    ' Datei schliessen.
    
    str = ""
End Sub

Function MyReplace(strSrc As String, strFind As String, strRep As String) As String
    Dim pos1 As Long
    Dim strRet As String
    
    If Len(strSrc) < 1 Then
        MyReplace = ""
        Exit Function
    End If
    
    pos1 = InStr(strSrc, strFind)
    If pos1 > 0 Then
        strRet = Left(strSrc, pos1 - 1) & strRep & MyReplace(Mid(strSrc, pos1 + Len(strFind)), strFind, strRep)
    Else
        strRet = strSrc
    End If
    
    MyReplace = strRet
    Debug.Print strRet
End Function


