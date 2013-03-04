Attribute VB_Name = "CommonFunctions"
Option Explicit

#Const COMPILE_VERSION = 2007

'''''''''''''''''''''''''''''''''''
' Public functions and procedures '
'''''''''''''''''''''''''''''''''''

'
' File and directory functions
'

Public Function DirectoryExists(ByVal szDirectoryName As String) As Boolean

    On Error Resume Next
    
    DirectoryExists = False
    
    ' Append slash if there is no closing one
    If Right(szDirectoryName, 1) <> "\" Then
        szDirectoryName = szDirectoryName & "\"
    End If
    
    ' Check if Directory already exists
    Dim fs, f, S
    Set fs = CreateObject("Scripting.FileSystemObject")
    Set f = fs.GetFolder(szDirectoryName)
    If Not IsEmpty(f) Then
        DirectoryExists = True
    End If

End Function

Public Function MakeDirectory(ByVal szDirectoryName As String) As Boolean

    On Error Resume Next
    
    MakeDirectory = False
    
    ' Create a new directory
    Dim fs, f
    Set fs = CreateObject("Scripting.FileSystemObject")
    Set f = fs.CreateFolder(szDirectoryName)
    
    MakeDirectory = True

End Function

Private Function DeleteConcludingDotsAndSpace(szName As String, szPrefix As String) As String

    Dim bSearchFinished As Boolean: bSearchFinished = False
    Dim bSpaceOrDotFound As Boolean: bSpaceOrDotFound = False
    Dim actualStringPosition As Long
    Dim szChar As String
    Dim szFilename As String: szFilename = szPrefix
    
     ' Delete all dots and spaces at the end of the file
    bSearchFinished = False
    bSpaceOrDotFound = False
    actualStringPosition = Len(szFilename)
    Do
        szChar = Mid(szFilename, actualStringPosition, 1)
        If (szChar = "." Or szChar = " ") Then
            actualStringPosition = actualStringPosition - 1
            szFilename = Left(szFilename, actualStringPosition)
            bSpaceOrDotFound = True
        Else
            bSearchFinished = True
        End If
    Loop While ((Not bSearchFinished) And (actualStringPosition > 0))
    
    If bSpaceOrDotFound Then
        Dim szMessage As String
        szMessage = String(256, vbNullChar)
        Dim iResult As Integer
        iResult = GetIllegalCharacterMsg(StrPtr(szName), StrPtr(szMessage))
        Dim lResult As Long
        lResult = MessageBoxW(0, StrPtr(szMessage), StrPtr(g_szDlgCaption), _
                            MB_ICONEXCLAMATION Or MB_TOPMOST)
    End If

    DeleteConcludingDotsAndSpace = szFilename
End Function

Public Function GetFormalFileName() As String

    ' get the file name
    Dim szFilename As String: szFilename = g_currentPresentation.FullName
    
    ' Find out the filename (without path)
    Dim slashPos As Long        ' the position of "\"
    Do
        slashPos = InStr(1, szFilename, "\", vbTextCompare)
        If slashPos > 0 Then
            szFilename = Right(szFilename, Len(szFilename) - slashPos)
        End If
    Loop While (slashPos > 0)
    
    ' Find out the suffix (without dot)
    Dim szSuffix As String: szSuffix = szFilename
    Dim dotPos As Long
    Do
        dotPos = InStr(1, szSuffix, ".", vbTextCompare)
        If dotPos > 0 Then
            szSuffix = Right(szSuffix, Len(szSuffix) - dotPos)
        End If
    Loop While (dotPos > 0)
    
    Dim iSuffixLength As Long: iSuffixLength = Len(szSuffix)
    
    ' Delete the suffix (with ".")
    Dim szPrefix As String: szPrefix = szFilename
    If Len(szFilename) > (iSuffixLength + 1) Then       ' if < (suffix + dot) without extension
        If StrComp(szSuffix, "ppt", vbTextCompare) = 0 Or _
            StrComp(szSuffix, "pps", vbTextCompare) = 0 Or _
            StrComp(szSuffix, "pot", vbTextCompare) = 0 Or _
            StrComp(szSuffix, "pptx", vbTextCompare) = 0 Then
                szPrefix = Left(szFilename, Len(szFilename) - (iSuffixLength + 1))
        End If
    End If
    
    szFilename = DeleteConcludingDotsAndSpace(szFilename, szPrefix)
    
    If szFilename = "" Then
        szFilename = "noname"
    End If
    
    GetFormalFileName = szFilename
    
End Function

Public Function GetFileName(szFullFilename As String) As String
    ' get the file name
            ' the position of "\"
            ' the position of "."
    
    ' Remove path
    Dim szFilename As String: szFilename = szFullFilename
    Dim slashPos As Long: slashPos = InStrRev(szFullFilename, "\", -1, vbTextCompare)
    If slashPos > 0 Then
        szFilename = Right(szFullFilename, Len(szFullFilename) - slashPos)
    End If
    
    Dim suffixPos As Long: suffixPos = InStrRev(szFilename, ".", -1, vbTextCompare)
    Dim szSuffix As String
    Dim szPrefix As String
    If suffixPos > 0 Then
        szSuffix = Right(szFilename, Len(szFilename) - suffixPos)
        szPrefix = Left(szFilename, suffixPos - 1)
    End If
    
    ' Remove spaces and dots which are direct before the suffix dot (If they are not deleted a Bug occurs)
    szFilename = DeleteConcludingDotsAndSpace(szFilename, szPrefix)
    szFilename = szFilename + "."
    szFilename = szFilename + szSuffix
    
    GetFileName = szFilename
    
End Function

Public Function GetDirectory(szFullFilename As String) As String
    
    Dim szDirectoryName As String: szDirectoryName = szFullFilename
    
    ' Remove the file name
    Dim slashPos As Long: slashPos = InStrRev(szFullFilename, "\", -1, vbTextCompare)
    If slashPos > 0 Then
        szDirectoryName = Left(szFullFilename, slashPos - 1)
    End If
    
    szDirectoryName = DeleteConcludingDotsAndSpace(szDirectoryName, szDirectoryName)
    
    GetDirectory = szDirectoryName + "\"
    
End Function

Public Function GetUserProfileDirectory() As String
    
    Const CSIDL_APPDATA  As Long = &H1A
    
    Dim szBuffer As String: szBuffer = GetSpecialfolder(CSIDL_APPDATA) & "\Lecturnity\"
    
    On Error Resume Next
    
    If Not DirectoryExists(szBuffer) Then
        MakeDirectory (szBuffer)             ' if the dir exist, a run-time error raised.
    End If
    
    If Err.Number <> 0 Then     ' directory ".\Lecturnity\" exist
        'Kill sBuffer & "*.*"    ' clear the directory
        Err.Clear
    End If
    
    On Error GoTo 0
    
    GetUserProfileDirectory = szBuffer
    
End Function


Public Function GetLecturnityInstall() As String

    On Error GoTo ErrQuit
    
    Dim retOnError As String: retOnError = ""
    
    Dim regErr As Long: regErr = ERROR_SUCCESS
    Dim hKeyLocalMachine As Long: hKeyLocalMachine = 0
    regErr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\imc AG\LECTURNITY", 0, KEY_READ, hKeyLocalMachine)
    
    If (regErr <> ERROR_SUCCESS) Then
        GetLecturnityInstall = retOnError
        Exit Function
    End If
    
    ' get buffer size
    Dim dwType As Long
    Dim dwBufSize As Long: dwBufSize = 0
    regErr = RegQueryValueEx(hKeyLocalMachine, "InstallDir", 0, dwType, ByVal vbNullString, dwBufSize)
    If (regErr <> ERROR_SUCCESS) Then
        RegCloseKey (hKeyLocalMachine)
        GetLecturnityInstall = retOnError
        Exit Function
    End If

    If (dwType <> REG_SZ) Then
        RegCloseKey (hKeyLocalMachine)
        GetLecturnityInstall = retOnError
        Exit Function
    End If

    Dim szBinData As String: szBinData = Space(dwBufSize + 8)
    If IsNull(szBinData) Then
        RegCloseKey (hKeyLocalMachine)
        GetLecturnityInstall = retOnError
        Exit Function
    End If

    regErr = RegQueryValueEx(hKeyLocalMachine, "InstallDir", 0, dwType, ByVal szBinData, dwBufSize)
    If (regErr <> ERROR_SUCCESS) Then
        'Erase binData
        RegCloseKey (hKeyLocalMachine)
        GetLecturnityInstall = retOnError
        Exit Function
    End If
    
    RegCloseKey (hKeyLocalMachine)
    
    GetLecturnityInstall = Left(szBinData, dwBufSize - 1)
    
    Dim slashPos As Long: slashPos = InStrRev(GetLecturnityInstall, "\", -1, vbTextCompare)
    If slashPos = (dwBufSize - 1) Then
        GetLecturnityInstall = Left(GetLecturnityInstall, Len(GetLecturnityInstall) - 1)
    End If
    
    Exit Function

ErrQuit:
    If hKeyLocalMachine <> 0 Then
        RegCloseKey (hKeyLocalMachine)
    End If
    
    GetLecturnityInstall = retOnError
    
    Exit Function

End Function


'
' String and codepage functions
'

Public Function IsSpecialCharacter(Unicode As Long) As Boolean

    Dim sMultiByte As Long
    Dim sTranslatedWideChar As Long
    Dim result As Integer
    
    Call WideCharToMultiByte(CP_ACP, 0, Unicode, 1, sMultiByte, 1, "", 0)
    Call MultiByteToWideChar(CP_ACP, 0, sMultiByte, 1, sTranslatedWideChar, 1)
    
    If sTranslatedWideChar = Unicode Then
        IsSpecialCharacter = False
    Else
        IsSpecialCharacter = True
    End If

End Function

Public Function GetCodepage() As Long
    GetCodepage = GetACP()
End Function

Public Function TranslateForCodepage(ByVal ssString As String) As String
    Dim i As Long
    Dim szReturnName As String
    Dim iAsc As Long
    Dim ssChr As String
    
    For i = 1 To Len(ssString)
        iAsc = Asc(Mid(ssString, i, 1))
        If iAsc > 127 Or iAsc = 63 Then
            ssChr = "_"
        Else
            ssChr = Chr(iAsc)
        End If
        szReturnName = szReturnName + ssChr
    Next
    
    TranslateForCodepage = szReturnName
End Function

Public Function ReplaceBackslash(path As String) As String
    Dim result As String
    Dim i As Long
    
    For i = 1 To Len(path)
        If Mid(path, i, 1) = "\" Then
            result = result + "\"
        End If
        result = result + Mid(path, i, 1)
    Next
            
    ReplaceBackslash = result
End Function


'
' Font functions
'

Public Function FontSupportsCharacter(txtSeg As TextRange, start As Long) As Boolean

    Dim success As Long: success = 0
        
    Dim textObject As TextRange: Set textObject = txtSeg.Characters(start, 1)
    Dim szText As String: szText = textObject.text
    Dim szFontFamily As String: szFontFamily = textObject.Font.NameAscii
    Dim isBold As Long: isBold = textObject.Font.Bold
    Dim isItalic As Long: isItalic = textObject.Font.Italic
    
    If Asc(szText) = 13 Then
        FontSupportsCharacter = True
        Exit Function
    End If
    
    If szFontFamily <> "" Then
        success = FontSupportsString(StrPtr(szText), 1, StrPtr(szFontFamily), isBold, isItalic)
    End If
    
    Dim bSuccess As Boolean: bSuccess = False
    If (success <> 0) Then
        bSuccess = True
    End If
    
    FontSupportsCharacter = bSuccess
End Function

Public Function CalculateTextAscent(ByVal szFontFamily As String, ByVal iFontSize As Long, ByVal isBold As Boolean, ByVal isItalic As Boolean) As Double

    Dim success As Boolean: success = False
        
    Dim fAscent As Double
    Dim fDescent As Double
    
    If szFontFamily <> "" Then
        success = GetFontMetrics(StrPtr(szFontFamily), iFontSize, isBold, isItalic, fAscent, fDescent)
    End If
    
    If success = 0 Then
        fAscent = 3 * iFontSize / 4
    End If

    CalculateTextAscent = fAscent
End Function

Public Function CalculateTextDescent(ByVal szFontFamily As String, ByVal iFontSize As Long, ByVal isBold As Boolean, ByVal isItalic As Boolean) As Double

    Dim success As Boolean: success = False
        
    Dim fAscent As Double
    Dim fDescent As Double
    
    If szFontFamily <> "" Then
        success = GetFontMetrics(StrPtr(szFontFamily), iFontSize, isBold, isItalic, fAscent, fDescent)
    End If

    If success = 0 Then
        fDescent = iFontSize / 4
    End If
    
    CalculateTextDescent = fDescent
End Function

Public Function GetCorrectFontFamily(ByVal imagePath As String, ByRef fontFamily As String) As Boolean
    Dim success As Boolean
    Dim szFontFamily As String
    
    szFontFamily = String(256, vbNullChar)
    Dim iStringLength As Long
    iStringLength = Len(szFontFamily)
    success = ExtractFontFamily(StrPtr(imagePath), StrPtr(szFontFamily), iStringLength)
    
    fontFamily = Left(szFontFamily, iStringLength)
    If success Then
        GetCorrectFontFamily = True
    Else
        GetCorrectFontFamily = False
    End If
End Function

'
' Conversions
'

Public Function ConvertSecondsToTimeString(sSeconds As Single) As String

    Dim szDuration As String: szDuration = ""
    Dim iMinutes As Integer: iMinutes = Int(sSeconds / 60)
    Dim iSeconds As Integer: iSeconds = sSeconds - (60 * iMinutes)
    If iSeconds < 10 Then
        szDuration = iMinutes & ":0" & iSeconds
    Else
        szDuration = iMinutes & ":" & iSeconds
    End If
    
    ConvertSecondsToTimeString = szDuration
    
End Function

Public Function ConvertDoubleToString(ByVal dValue As Double) As String

    Dim szRoundedValue As Double: szRoundedValue = Round(dValue, 2)
    
    Dim szDouble As String: szDouble = CStr(szRoundedValue)
    
    ConvertDoubleToString = Replace(szDouble, ",", ".", 1, -1, vbTextCompare)
    
End Function

'
' Image functions
'

Public Function ExportImage(ByVal imagePath As String, ByRef imageName As String, ByVal imagePrefix As String, ByVal imageSuffix As String) As Boolean
    Dim success As Boolean
    Dim szFilename As String
    
    szFilename = String(256, vbNullChar)
    Dim iStringLength As Long
    iStringLength = Len(szFilename)
    success = ConvertMetafile(StrPtr(imagePath), StrPtr(szFilename), StrPtr(imagePrefix), StrPtr(imageSuffix), iStringLength)
    
    imageName = Left(szFilename, iStringLength)
    If success Then
        ExportImage = True
    Else
        ExportImage = False
    End If
End Function
    
Public Function ExportMetafile(ByVal imagePath As String, ByRef imageName As String, ByVal imagePrefix As String, ByVal imageSuffix As String, ByRef zX As Long, ByRef zY As Long, ByRef zWidth As Long, ByRef zHeight As Long) As Boolean
    
    On Error Resume Next
    
    Dim success As Boolean: success = False
    Dim szFilename As String: szFilename = String(256, vbNullChar)
    Dim iStringLength As Long: iStringLength = Len(szFilename)
    
    
    success = SaveMetafileAsPNG(StrPtr(imagePath), StrPtr(szFilename), StrPtr(imagePrefix), StrPtr(imageSuffix), iStringLength, _
                                72, 72, zX, zY, zWidth, zHeight)
    
    imageName = Left(szFilename, iStringLength)
    If success Then
        ExportMetafile = True
    Else
        ExportMetafile = False
    End If
End Function

Public Function ExportMetafileWithoutGrid(ByVal imagePath As String, ByRef imageName As String, ByVal imagePrefix As String, ByVal imageSuffix As String, ByRef zX As Long, ByRef zY As Long, ByRef zWidth As Long, ByRef zHeight As Long) As Boolean
    
    On Error Resume Next
    
    Dim success As Boolean: success = False
    Dim szFilename As String: szFilename = String(256, vbNullChar)
    Dim iStringLength As Long: iStringLength = Len(szFilename)
    
    
    success = SaveMetafileAsPNGWithoutGrid(StrPtr(imagePath), StrPtr(szFilename), StrPtr(imagePrefix), StrPtr(imageSuffix), iStringLength, _
                                72, 72, zX, zY, zWidth, zHeight)
    
    imageName = Left(szFilename, iStringLength)
    If success Then
        ExportMetafileWithoutGrid = True
    Else
        ExportMetafileWithoutGrid = False
    End If
End Function
    
Public Function GetMetafileDimension(ByVal imagePath As String, ByVal imagePrefix As String, ByVal imageSuffix As String, _
                                     ByRef zX As Long, ByRef zY As Long, ByRef zWidth As Long, ByRef zHeight As Long) As Boolean
    On Error Resume Next
    
    Dim success As Boolean: success = False
    
    success = GetImageDimension(StrPtr(imagePath), StrPtr(imagePrefix), StrPtr(imageSuffix), _
                                72, 72, zX, zY, zWidth, zHeight)
    
    If success Then
        GetMetafileDimension = True
    Else
        GetMetafileDimension = False
    End If
End Function

Public Function GetBulletDimension(ByVal fullImageName As String, _
                                   ByRef zX As Long, ByRef zY As Long, ByRef zWidth As Long, ByRef zHeight As Long) As Boolean
    On Error Resume Next
    
    Dim success As Boolean: success = False
    
    success = GetEMFDimension(StrPtr(fullImageName), _
                                 72, 72, zX, zY, zWidth, zHeight)
    
    If success Then
        GetBulletDimension = True
    Else
        GetBulletDimension = False
    End If
End Function
    
Public Function RewriteImage(ByVal imagePath As String, ByRef imageName As String, ByVal imagePrefix As String, ByVal imageSuffix As String) As Boolean
    On Error Resume Next
    
    Dim success As Boolean: success = False
    Dim szFilename As String
    
    szFilename = String(256, vbNullChar)
    Dim iStringLength As Long
    iStringLength = Len(szFilename)
    success = RewritePng(StrPtr(imagePath), StrPtr(szFilename), StrPtr(imagePrefix), StrPtr(imageSuffix), iStringLength)
    
    imageName = Left(szFilename, iStringLength)
    If success Then
        RewriteImage = True
    Else
        RewriteImage = False
    End If
End Function

Public Function CheckIfMetafileIsEmpty(ByVal imagePath As String) As Boolean
    On Error Resume Next
    
    Dim success As Boolean: success = False
    
    success = CheckMetafile(StrPtr(imagePath))
    
    If success Then
        CheckIfMetafileIsEmpty = True
    Else
        CheckIfMetafileIsEmpty = False
    End If
    
End Function
  
'
' Library functions
'

Public Function LoadGdiplus() As Long
    Dim bLoaded As Boolean: bLoaded = False
    Dim iGdipHandle As Long
    
    On Error Resume Next
    
    Dim strGdiplus As String: strGdiplus = "gdiplus"
    iGdipHandle = LoadLibrary(StrPtr(strGdiplus))
    
    If iGdipHandle = 0 Then
        Dim gdiPlusPath As String: gdiPlusPath = GetLecturnityInstall()
        gdiPlusPath = gdiPlusPath + "\Backend\" + "gdiplus.dll"
        iGdipHandle = LoadLibrary(StrPtr(gdiPlusPath))
        If iGdipHandle <> 0 Then
            bLoaded = True
        End If
    Else
        bLoaded = True
    End If
    
    On Error GoTo 0
    LoadGdiplus = iGdipHandle
    
    Dim bGdiplusStarted As Boolean
    bGdiplusStarted = StartGdiplus()
End Function

Public Sub FreeGdiplus(gdiplusHandle As Long)
    
    On Error Resume Next
    
    Dim bGdiplusStopped As Boolean
    bGdiplusStopped = ShutdownGdiplus()
    
    Dim bResult As Boolean
    bResult = FreeLibrary(gdiplusHandle)
    
    On Error GoTo 0
End Sub

Public Function LoadPPTResources() As Long
    Dim bLoaded As Boolean: bLoaded = False
    Dim iPptResourcesHandle As Long
    
    On Error Resume Next
    
    Dim strPptResources As String: strPptResources = "pptresources"
    iPptResourcesHandle = LoadLibrary(StrPtr(strPptResources))
    
    If iPptResourcesHandle = 0 Then
        Dim szInstallDir As String: szInstallDir = GetLecturnityInstall()
        
        ' drawsdk needed by filesdk
        Dim drawsdkPath As String
        drawsdkPath = szInstallDir + "\Backend\" + "drawsdk.dll"
        
        Dim iDrawsdkHandle As Long
        iDrawsdkHandle = LoadLibrary(StrPtr(drawsdkPath))
        
        ' filesdk needed by lsutl32
        Dim filesdkPath As String
        filesdkPath = szInstallDir + "\Backend\" + "filesdk.dll"
        
        Dim iFilesdkHandle As Long
        iFilesdkHandle = LoadLibrary(StrPtr(filesdkPath))
        
        ' XTreme toolkit needed by lsutl32
        Dim xtremePath As String
        xtremePath = szInstallDir + "\Backend\" + "xtp2009.dll"
        
        Dim iXtremeHandle As Long
        iXtremeHandle = LoadLibrary(StrPtr(xtremePath))
        
        ' lsutl32 needed by pptresources
        Dim lsutl32Path As String
        lsutl32Path = szInstallDir + "\Backend\" + "lsutl32.dll"
        
        Dim iDllHandle As Long
        iDllHandle = LoadLibrary(StrPtr(lsutl32Path))
        
        Dim pptresourcesPath As String
        pptresourcesPath = szInstallDir + "\ppt2LECTURNITY\" + "pptresources.dll"
        
        iPptResourcesHandle = LoadLibrary(StrPtr(pptresourcesPath))
        
        If iPptResourcesHandle <> 0 Then
            bLoaded = True
        End If
    Else
        bLoaded = True
    End If
    
    On Error GoTo 0
    LoadPPTResources = iPptResourcesHandle
    
End Function

Public Function LoadPPT2Lecturnity() As Long
    Dim bLoaded As Boolean: bLoaded = False
    Dim iPpt2LecturnityHandle As Long: iPpt2LecturnityHandle = 0
    
    On Error Resume Next
    
    Dim strPpt2Lecturnity As String: strPpt2Lecturnity = "ppt2lecturnity"

    ' Try first to load the ppt2Lecturnity.dll in LECTURNITY install direstory
    Dim szInstallDir As String: szInstallDir = GetLecturnityInstall()
    
    Dim ppt2LecturnityPath As String
    ppt2LecturnityPath = szInstallDir + "\ppt2LECTURNITY\" + "ppt2lecturnity.dll"
    
    iPpt2LecturnityHandle = LoadLibrary(StrPtr(ppt2LecturnityPath))
    
    If iPpt2LecturnityHandle = 0 Then
        iPpt2LecturnityHandle = LoadLibrary(StrPtr(strPpt2Lecturnity))
    End If
    
    If iPpt2LecturnityHandle <> 0 Then
        bLoaded = True
    End If
    
    On Error GoTo 0
    LoadPPT2Lecturnity = iPpt2LecturnityHandle
    
End Function

Public Sub FreeLoadedLibrary(libraryHandle As Long)
    
    On Error Resume Next
    
    Dim bResult As Boolean
    bResult = FreeLibrary(libraryHandle)
    
    On Error GoTo 0
End Sub

'
' Write functions
'

Public Function NewLine() As String
    NewLine = vbLf & String(g_lIndentLevel + g_lIndentLevel, " ")
End Function

Public Function IncIndent() As Long
    g_lIndentLevel = g_lIndentLevel + 1
End Function

Public Function DecIndent() As Long
    g_lIndentLevel = g_lIndentLevel - 1
End Function

'
' Color functions
'

Public Function GetRGBColorString(col As ColorFormat) As String
    Dim str As String
    Dim i   As Long
    
    str = Hex$(col.RGB)
    For i = 1 To 6 - Len(str)
        str = "0" & str
    Next
    GetRGBColorString = "#" & Mid(str, 5, 2) & Mid(str, 3, 2) & Mid(str, 1, 2)

End Function

Public Function GetARGBColorString(transparency As Single, col As ColorFormat) As String
    Dim str As String
    Dim i   As Long
    
    str = Hex$(col.RGB)
    For i = 1 To 6 - Len(str)
        str = "0" & str
    Next
    
    Dim alpha As String
    Dim sAlpha As Single: sAlpha = transparency
    sAlpha = 255 - (sAlpha * 255)
    alpha = Hex$(sAlpha)
    If Len(alpha) = 1 Then
        alpha = "0" & alpha
    End If

    GetARGBColorString = "#" & alpha & Mid(str, 5, 2) & Mid(str, 3, 2) & Mid(str, 1, 2)

End Function

Public Function GetRGBStringFromRGBColor(col As RGBColor) As String
    Dim str As String
    Dim i   As Long
    
    str = Hex$(col)
    For i = 1 To 6 - Len(str)
        str = "0" & str
    Next
    GetRGBStringFromRGBColor = "#" & Mid(str, 5, 2) & Mid(str, 3, 2) & Mid(str, 1, 2)

End Function

Public Function GetLinkColor(sld As Slide, defaultColor As ColorFormat) As String
    
    If g_lApplicationVersion = 2007 Then
        GetLinkColor = GetLinkColor_2007(sld, defaultColor)
    Else
        GetLinkColor = GetLinkColor_2000(sld, defaultColor)
    End If
    
End Function

Public Function GetLinkColor_2000(sld As Slide, defaultColor As ColorFormat) As String
    Dim color As RGBColor
        
    If ActivePresentation.ColorSchemes.Count > 1 Then
        Set color = sld.ColorScheme.Colors(ppAccent2)
    Else
        color = defaultColor
    End If
    
    GetLinkColor_2000 = GetRGBStringFromRGBColor(color)
End Function

Public Function GetLinkColor_2007(sld As Slide, defaultColor As ColorFormat) As String
    Dim color As RGBColor
        
    If Not sld.ColorScheme Is Nothing And sld.ColorScheme.Count > 1 Then
        Set color = sld.ColorScheme.Colors(ppAccent2)
    Else
        color = defaultColor
    End If
    
    GetLinkColor_2007 = GetRGBStringFromRGBColor(color)
    
End Function

Public Function GetActiveLinkColor(sld As Slide, defaultColor As ColorFormat) As String
   
    If g_lApplicationVersion = 2007 Then
        GetActiveLinkColor = GetActiveLinkColor_2007(sld, defaultColor)
    Else
        GetActiveLinkColor = GetActiveLinkColor_2000(sld, defaultColor)
    End If
        
End Function

Public Function GetActiveLinkColor_2000(sld As Slide, defaultColor As ColorFormat) As String
    Dim color As RGBColor
        
    If ActivePresentation.ColorSchemes.Count > 1 Then
        Set color = sld.ColorScheme.Colors(ppAccent3)
    Else
        color = defaultColor
    End If
    
    GetActiveLinkColor_2000 = GetRGBStringFromRGBColor(color)
End Function

Public Function GetActiveLinkColor_2007(sld As Slide, defaultColor As ColorFormat) As String
    Dim color As RGBColor
        
    If Not sld.ColorScheme Is Nothing And sld.ColorScheme.Count > 1 Then
        Set color = sld.ColorScheme.Colors(ppAccent3)
    Else
        color = defaultColor
    End If
    
    GetActiveLinkColor_2007 = GetRGBStringFromRGBColor(color)
End Function

'
' Miscellaneous functions
'

Public Function ExtractSlideID(InternLink As String) As String
    Dim segment() As String
    Dim segmentCount
    Dim i As Long
    Dim begin As Long
    
    ReDim segment(1 To 4) As String
    
    segmentCount = 1
    begin = 1
    For i = 1 To Len(InternLink)
        If Mid(InternLink, i, 1) = "," Then
            segment(segmentCount) = Mid(InternLink, begin, i - begin)
            segmentCount = segmentCount + 1
            begin = i + 1
        End If
    Next
    If i > begin Then
        segment(segmentCount) = Mid(InternLink, begin, i - begin)
    End If
            
    If segment(1) = "-1" Then
        ExtractSlideID = segment(3)
    Else
        ExtractSlideID = segment(1)
    End If
End Function


Public Function GetBackgroundColor(sld As Slide) As String
    GetBackgroundColor = "#ffffff"
    If sld.FollowMasterBackground Then
        With sld.Parent.SlideMaster.Background
            If .Visible = msoTrue And .Fill.Visible = msoTrue Then
                Select Case .Fill.Type
                Case msoFillSolid
                    GetBackgroundColor = GetRGBColorString(sld.Parent.SlideMaster.Background.Fill.ForeColor)
                Case Else
                    'ErrLog LEC_ERRORTYPE_UNKNOWN, "Slide background is not fillsolid."
                End Select
            End If
        End With
    Else
        With sld.Background
            If .Visible = msoTrue And .Fill.Visible = msoTrue Then
                Select Case .Fill.Type
                Case msoFillSolid
                    GetBackgroundColor = GetRGBColorString(sld.Background.Fill.ForeColor)
                Case Else
                    'ErrLog LEC_ERRORTYPE_UNKNOWN, "Slide background is not fillsolid."
                End Select
            End If
        End With
    End If
End Function

Public Function GetMLBBackgroundColor(sld As Slide) As String

    GetMLBBackgroundColor = "#ffffff"
    
    With sld.Background
        If .Visible = msoTrue And .Fill.Visible = msoTrue Then
            Select Case .Fill.Type
            Case msoFillSolid
'                If sld.FollowMasterBackground = msoTrue Then    ' using the setting of Master Page
'                    GetMLBBackgroundColor = GetRGBColorString(sld.Master.Background.Fill.ForeColor)
'                Else
                    GetMLBBackgroundColor = GetRGBColorString(sld.Background.Fill.ForeColor)
'                End If
            Case Else
                'ErrLog LEC_ERRORTYPE_UNKNOWN, "Slide background is not fillsolid."
            End Select
        End If
    End With
    
End Function

Public Function GetBackgroundColor_2007(ByRef slideBackground As ShapeRange) As String

    GetBackgroundColor_2007 = "#ffffff"
    
    If Not slideBackground Is Nothing Then
        If slideBackground.Fill.Visible = msoTrue Then
            Select Case slideBackground.Fill.Type
            Case msoFillSolid
                GetBackgroundColor_2007 = GetRGBColorString(slideBackground.Fill.ForeColor)
            Case Else
                'ErrLog LEC_ERRORTYPE_UNKNOWN, "Slide background is not fillsolid."
            End Select
        End If
    End If
End Function

'
' Compare functions
'

Public Function IsEqual(txtRange1 As TextRange, txtRange2 As TextRange) As Boolean

    Dim bIsEqual As Boolean: bIsEqual = True
    
    If StrComp(txtRange1.text, txtRange2.text, vbTextCompare) Then
        bIsEqual = False
    End If
    
    If bIsEqual And txtRange1.BoundHeight <> txtRange2.BoundHeight Then
        bIsEqual = False
    End If
    
    If bIsEqual And txtRange1.BoundLeft <> txtRange2.BoundLeft Then
        bIsEqual = False
    End If
    
    If bIsEqual And txtRange1.BoundTop <> txtRange2.BoundTop Then
        bIsEqual = False
    End If
    
    If bIsEqual And txtRange1.BoundWidth <> txtRange2.BoundWidth Then
        bIsEqual = False
    End If
    
    IsEqual = bIsEqual

End Function

'
' Functions/Procedures to extract Masters/Layouts
'

Public Function GetDesignIndex(sld As Slide) As Long

    GetDesignIndex = sld.Design.index
    
End Function
 
Public Function GetTitleMaster(index As Long) As Master

    Set GetTitleMaster = g_currentPresentation.Designs.Item(index).TitleMaster
    
End Function

Public Function GetSlideMaster(iDsgnIndex As Long) As Master

    Set GetSlideMaster = g_currentPresentation.Designs(iDsgnIndex).SlideMaster
    
End Function

Public Function GetCustomLayoutCount(iDsgnIndex As Long) As Long

#If COMPILE_VERSION = 2007 Then
    GetCustomLayoutCount = g_currentPresentation.Designs(iDsgnIndex).SlideMaster.CustomLayouts.Count
#End If
End Function

Public Function GetCustomLayoutIndex(sld As Slide) As Long

#If COMPILE_VERSION = 2007 Then
    GetCustomLayoutIndex = sld.CustomLayout.index
#End If

End Function

Public Function GetCustomLayoutAsMaster(iDsgnIndex As Long, iLayoutIndex As Long) As Master

#If COMPILE_VERSION = 2007 Then
    Set GetCustomLayoutAsMaster = g_currentPresentation.Designs.Item(iDsgnIndex).SlideMaster.CustomLayouts(iLayoutIndex)
#End If
    
End Function


'
' Debug functions
'

Public Sub ErrLog(errtype As LecErrorType, Optional desc As String = "")
    If Not g_conversionOptions.WithDebugInfo Then
        Exit Sub
    End If
    Dim strlog      As String
    Select Case errtype
        Case LEC_ERRORTYPE_SLIDEINFO
            strlog = "In Slide: " & desc
        Case LEC_ERRORTYPE_BEGINTIMESTAMP
            With g_conversionOptions.TimeBegin
                strlog = "Conversion began: " & .wYear & "-" _
                    & .wMonth & "-" & .wDay & " " _
                    & .wHour & ":" & .wMinute & ":" & .wSecond & ":" _
                    & .wMilliseconds & vbLf
            End With
        Case LEC_ERRORTYPE_ENDTIMESTAMP
            With g_conversionOptions.TimeEnd
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

Public Sub WriteToLog(str As String)
    Dim Dateinummer

    Dateinummer = FreeFile    ' neue Datei-Nr.
    Open g_conversionOptions.LogFileName For Append As #Dateinummer     ' Dateiname

    Print #Dateinummer, str    ' Text ausgeben.
    Close #Dateinummer    ' Datei schliessen.
    
    str = ""
End Sub

'
' Event functions
'
      
Public Sub TrapEvents()
    If g_bTrapFlag = True Then
        'the EventHandler is already active
        Exit Sub
    End If
    Set g_cPPTObject = New cEventClass
    Set g_cPPTObject.PPTEvent = Application
    g_bTrapFlag = True
End Sub

Public Sub ReleaseTrap()
    If g_bTrapFlag = True Then
        Set g_cPPTObject.PPTEvent = Nothing
        Set g_cPPTObject = Nothing
        g_bTrapFlag = False
    End If
End Sub

'
' Import wizard functions
'

Public Function ProgressParseTitleMaster() As Boolean
    
    Dim bSuccess As Boolean
    bSuccess = ParseTitleMaster(g_ImportWizardHwnd)
    If bSuccess = 0 Then
        ProgressParseTitleMaster = False
    Else
        ProgressParseTitleMaster = True
    End If
    
End Function

Public Function ProgressParseMaster(iMasterCount As Long, iCurrentMaster As Long) As Boolean
    
    Dim bSuccess As Boolean
    bSuccess = ParseMaster(iMasterCount, iCurrentMaster, g_ImportWizardHwnd)
    If bSuccess = 0 Then
        ProgressParseMaster = False
    Else
        ProgressParseMaster = True
    End If
    
End Function

Public Function ProgressImportTemplate(iSlideCount As Long, iCurrentSlide As Long) As Boolean
    
    Dim bSuccess As Boolean
    bSuccess = ImportTemplate(iSlideCount, iCurrentSlide, g_ImportWizardHwnd)
    If bSuccess = 0 Then
        ProgressImportTemplate = False
    Else
        ProgressImportTemplate = True
    End If
    
End Function

Public Function ProgressImportSlide(iSlideCount As Long, iCurrentSlide As Long) As Boolean
    
    Dim bSuccess As Boolean
    bSuccess = ImportSlide(iSlideCount, iCurrentSlide, g_ImportWizardHwnd)
    If bSuccess = 0 Then
        ProgressImportSlide = False
    Else
        ProgressImportSlide = True
    End If
    
End Function

Public Function EndProgress() As Boolean
    
    Dim bSuccess As Boolean
    bSuccess = EndProgressDialog(g_ImportWizardHwnd)
    If bSuccess = 0 Then
        EndProgress = False
    Else
        EndProgress = True
    End If
    
End Function


'
' Private Functions
'

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
        r = SHGetPathFromIDList(ByVal IDL.mkid.cb, StrPtr(path))
        'Remove the unnecessary chr$(0)'s
        GetSpecialfolder = Left$(path, InStr(path, Chr$(0)) - 1)
        Exit Function
    End If
    GetSpecialfolder = ""
End Function

Public Function PresentationIsMarkedAsFinal() As Boolean

    Dim bPresentationIsMarkedAsFinal As Boolean: bPresentationIsMarkedAsFinal = False
    Dim i As Long
    For i = 1 To g_currentPresentation.CustomDocumentProperties.Count
        If StrComp(g_currentPresentation.CustomDocumentProperties(i).Name, "_MarkAsFinal", vbTextCompare) = 0 Then
            bPresentationIsMarkedAsFinal = True
        End If
    Next
    
    PresentationIsMarkedAsFinal = bPresentationIsMarkedAsFinal
    
End Function

Public Function PresentationIsReadOnly() As Boolean

    Dim bPresentationIsReadOnly As Boolean: bPresentationIsReadOnly = False
 
    If g_currentPresentation.ReadOnly Then
        bPresentationIsReadOnly = True
    End If
    
    PresentationIsReadOnly = bPresentationIsReadOnly
    
End Function

