VERSION 5.00
Object = "{0D452EE1-E08F-101A-852E-02608C4D0BB4}#2.0#0"; "FM20.DLL"
Begin VB.Form frmFile 
   BorderStyle     =   1  'Fest Einfach
   ClientHeight    =   4335
   ClientLeft      =   45
   ClientTop       =   315
   ClientWidth     =   6330
   Icon            =   "frmFile.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4335
   ScaleWidth      =   6330
   Begin VB.Frame WizardCaption 
      BackColor       =   &H00FFFFFF&
      BorderStyle     =   0  'Kein
      Caption         =   "Frame3"
      Height          =   795
      Left            =   0
      TabIndex        =   7
      Top             =   0
      Width           =   6372
      Begin VB.Image bmpLogoLeft 
         Height          =   795
         Left            =   0
         Picture         =   "frmFile.frx":0E42
         Top             =   0
         Width           =   780
      End
      Begin VB.Image bmpLogoRight 
         Height          =   795
         Left            =   4920
         Picture         =   "frmFile.frx":1D48
         Top             =   0
         Width           =   1470
      End
      Begin MSForms.Label Header 
         Height          =   255
         Left            =   840
         TabIndex        =   9
         Top             =   120
         Width           =   4455
         VariousPropertyBits=   8388627
         Size            =   "7857;450"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
      End
      Begin MSForms.Label SubHeader 
         Height          =   375
         Left            =   840
         TabIndex        =   8
         Top             =   360
         Width           =   4455
         VariousPropertyBits=   8388627
         Size            =   "7857;660"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
      End
   End
   Begin VB.Frame BottomFrame 
      BorderStyle     =   0  'Kein
      Caption         =   "Frame3"
      Height          =   855
      Left            =   0
      TabIndex        =   0
      Top             =   3480
      Width           =   6375
      Begin VB.Frame Frame2 
         Height          =   30
         Index           =   1
         Left            =   0
         TabIndex        =   1
         Top             =   120
         Width           =   6345
      End
      Begin MSForms.CommandButton cmdPrev 
         Height          =   375
         Left            =   2280
         TabIndex        =   2
         Top             =   360
         Width           =   1215
         Caption         =   "< Zurück"
         Size            =   "2139;661"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
         ParagraphAlign  =   3
      End
      Begin MSForms.CommandButton cmdNext 
         Height          =   375
         Left            =   3480
         TabIndex        =   3
         Top             =   360
         Width           =   1215
         Caption         =   "Weiter >"
         Size            =   "2139;661"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
         ParagraphAlign  =   3
      End
      Begin MSForms.CommandButton cmdCancel 
         Height          =   375
         Left            =   4920
         TabIndex        =   4
         Top             =   360
         Width           =   1215
         Caption         =   "Abbrechen"
         Size            =   "2144;661"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
         ParagraphAlign  =   3
      End
   End
   Begin VB.Frame TopSeparator 
      Height          =   30
      Left            =   0
      TabIndex        =   5
      Top             =   796
      Width           =   6345
   End
   Begin MSForms.CommandButton cmdBrowse 
      Height          =   375
      Left            =   240
      TabIndex        =   10
      Top             =   2520
      Width           =   1455
      Caption         =   "Durchsuchen"
      Size            =   "2566;661"
      FontHeight      =   165
      FontCharSet     =   0
      FontPitchAndFamily=   2
      ParagraphAlign  =   3
   End
   Begin MSForms.TextBox File 
      Height          =   330
      Left            =   240
      TabIndex        =   11
      Top             =   2040
      Width           =   5895
      VariousPropertyBits=   746604571
      Size            =   "10398;582"
      FontHeight      =   165
      FontCharSet     =   0
      FontPitchAndFamily=   2
   End
   Begin MSForms.Label SearchCaption 
      Height          =   255
      Left            =   240
      TabIndex        =   6
      Top             =   1680
      Width           =   5895
      VariousPropertyBits=   8388627
      Size            =   "10387;450"
      FontHeight      =   165
      FontCharSet     =   0
      FontPitchAndFamily=   2
   End
End
Attribute VB_Name = "frmFile"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public g_bCancelled         As Boolean
Public g_bShowPrevious      As Boolean
Public g_iLanguage          As Integer
Public g_szFilePath         As String
Public g_lTop               As Long
Public g_lLeft              As Long

Private p_iFileSDKHandle    As Long
Private p_iGdipHandle       As Long
Private p_iDrawSDKHandle    As Long

Private p_szSourceDirectory As String


Private Declare Function GetSaveFileName Lib "comdlg32" _
                                    (pOpenfilename As OPENFILENAME) As Long
                                    
Private Declare Function GetSaveFileNameW Lib "comdlg32.dll" _
                                    (pOpenfilename As OPENFILENAMEW) As Long
                                    
Private Declare Function LoadLibrary Lib "kernel32" Alias "LoadLibraryA" _
                                    (ByVal strPath As String) As Long
Private Declare Function FreeLibrary Lib "kernel32" _
                                    (ByVal gdipHandle As Long) As Long
                      
Private Declare Function VerifyInternationalFilename Lib "filesdk.dll" Alias "FileSdk_VerifyInternationalFilename" _
                                    (ByVal hWndParent As Long, _
                                     ByVal tszFilename As String, _
                                     ByVal bSave As Boolean) As Boolean
                      
Private Declare Function lstrlenW Lib "kernel32.dll" _
                                    (ByVal lpString As Long) As Long
    
Private Declare Sub RtlMoveMemory Lib "kernel32.dll" _
                                    (ByVal Destination As Long, _
                                     ByVal Source As Long, _
                                     ByVal Length As Long)
                                     
Private Type OPENFILENAME
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
   Flags              As Long
   nFileOffset        As Integer
   nFileExtension     As Integer
   lpstrDefExt        As String
   lCustData          As Long
   lpfnHook           As Long
   lpTemplateName     As String
End Type

Private Type OPENFILENAMEW
   lStructSize          As Long     'Length of structure, in bytes
   hwndOwner            As Long     'Window that owns the dialog, or NULL
   hInstance            As Long     'Handle of mem object containing template (not used)
   lpstrFilter          As Long     'File types/descriptions, delimited with vbnullchar, ends with 2xvbnullchar
   lpstrCustomFilter    As Long     'Filters typed in by user
   nMaxCustFilter       As Long     'Length of CustomFilter, min 40x chars
   nFilterIndex         As Long     'Filter Index to use (1,2,etc) or 0 for custom
   lpstrFile            As Long     'Initial file/returned file(s), delimited with vbnullchar for multi files
   nMaxFile             As Long     'Size of Initial File long  , min 256
   lpstrFileTitle       As Long     'File.ext excluding path
   nMaxFileTitle        As Long     'Length of FileTitle
   lpstrInitialDir      As Long     'Initial file dir, null for current dir
   lpstrTitle           As Long     'Title bar of dialog
   Flags                As Long     'See OFN_Flags
   nFileOffset          As Integer  'Offset to file name in full path, 0-based
   nFileExtension       As Integer  'Offset to file ext in full path, 0-based (excl '.')
   lpstrDefExt          As Long     'Default ext appended, excl '.', max 3 chars
   lCustData            As Long     'Appl defined data for lpfnHook
   lpfnHook             As Long     'Pointer to hook procedure
   lpTemplateName       As Long     'Template Name (not used)
   pvReserved           As Long     'new Win2000 / WinXP members
   dwReserved           As Long     'new Win2000 / WinXP members
   FlagsEx              As Long     'new Win2000 / WinXP members
End Type


Private Sub LoadFileSDK()
    
    On Error Resume Next
    
    p_iFileSDKHandle = 0
    p_iGdipHandle = 0
    p_iDrawSDKHandle = 0
    
    p_iFileSDKHandle = LoadLibrary("filesdk")
    
    If p_iFileSDKHandle = 0 Then
        Dim szLecturnityInstall As String
        szLecturnityInstall = GetLecturnityInstall()
        
        Dim bLoaded As Boolean: bLoaded = False
        
        p_iGdipHandle = LoadLibrary("gdiplus")
        If p_iGdipHandle = 0 Then
            Dim gdiPlusPath As String
            gdiPlusPath = szLecturnityInstall + "ppt2LECTURNITY\gdiplus.dll"
            p_iGdipHandle = LoadLibrary(gdiPlusPath)
            If p_iGdipHandle <> 0 Then
                bLoaded = True
            Else
                bLoaded = False
            End If
        Else
            bLoaded = True
        End If
        
        If bLoaded Then
            p_iDrawSDKHandle = LoadLibrary("drawsdk")
            If p_iDrawSDKHandle = 0 Then
                Dim drawSdkPath As String
                drawSdkPath = szLecturnityInstall + "ppt2LECTURNITY\drawsdk.dll"
            
                p_iDrawSDKHandle = LoadLibrary(drawSdkPath)
                If p_iDrawSDKHandle <> 0 Then
                    bLoaded = True
                Else
                    bLoaded = False
                End If
            Else
                bLoaded = True
            End If
        End If
        
        If bLoaded Then
            Dim fileSdkPath As String
            fileSdkPath = szLecturnityInstall + "ppt2LECTURNITY\filesdk.dll"
            p_iFileSDKHandle = LoadLibrary(fileSdkPath)
            If p_iFileSDKHandle <> 0 Then
                bLoaded = True
            Else
                bLoaded = False
            End If
        End If
    Else
        bLoaded = True
    End If
    
    On Error GoTo 0
End Sub

Private Sub FreeFileSDK()
    
    On Error Resume Next
    
    Dim bFileSDKResult As Boolean
    If p_iFileSDKHandle <> 0 Then
        bFileSDKResult = FreeLibrary(p_iFileSDKHandle)
    End If
    p_iFileSDKHandle = 0
    
    Dim bDrawSDKResult As Boolean
    If p_iDrawSDKHandle <> 0 Then
        bDrawSDKResult = FreeLibrary(p_iDrawSDKHandle)
    End If
    p_iDrawSDKHandle = 0
    
    Dim bGdipResult As Boolean
    If p_iGdipHandle <> 0 Then
        bGdipResult = FreeLibrary(p_iGdipHandle)
    End If
    p_iGdipHandle = 0
    
    On Error GoTo 0
End Sub

Private Function GetInternationalFilename(szNormalFilename As String) As String
    
    Dim bResult As Boolean
    Dim hwnd As Long: hwnd = 0
    Dim bI As Boolean: bI = True
    Dim tszFilename As String
    Dim lMaxStringLength As Long: lMaxStringLength = 1024 - Len(szNormalFilename)
    If lMaxStringLength > 0 Then
        tszFilename = String(lMaxStringLength, vbNullChar)
    Else
        tszFilename = ""
    End If
        
    tszFilename = szNormalFilename & tszFilename
    bResult = VerifyInternationalFilename(hwnd, tszFilename, True)
    
    Dim nullPos As Long
    Dim szFilename As String: szFilename = tszFilename
    nullPos = InStr(szFilename, vbNullChar)
    If nullPos > 0 Then
        szFilename = Left(szFilename, nullPos - 1)
    End If
    
    GetInternationalFilename = szFilename
    
End Function

Private Sub Form_Initialize()

    g_lTop = -1
    g_lLeft = -1
    Me.Top = (Screen.Height - Me.Height) / 2
    Me.Left = (Screen.Width - Me.Width) / 2
    
End Sub

Private Sub Form_Activate()

    LoadFileSDK
    
    Me.Caption = LoadResString(DLG_CAPTION + g_iLanguage * 1000)

    Me.Header.Caption = LoadResString(HEADER_FILE + g_iLanguage * 1000)
    Me.SubHeader.Caption = LoadResString(SUBHEADER_FILE + g_iLanguage * 1000)
    
    Me.SearchCaption.Caption = LoadResString(TXT_BROWSE_CAPTION + g_iLanguage * 1000)
    
    Me.cmdBrowse.Caption = LoadResString(CMD_BROWSE + g_iLanguage * 1000)
    Me.cmdBrowse.Enabled = True
    
    Me.cmdPrev.Caption = LoadResString(CMD_PREV + g_iLanguage * 1000)
    Me.cmdPrev.Enabled = True
    
    Me.cmdNext.Caption = LoadResString(CMD_FINISH + g_iLanguage * 1000)
    Me.cmdNext.Enabled = True
    
    Me.cmdCancel.Caption = LoadResString(CMD_CANCEL + g_iLanguage * 1000)
    Me.cmdCancel.Enabled = True
    
    p_szSourceDirectory = GetSourceDir()
    
    Me.File.Text = g_szFilePath
    Me.cmdNext.SetFocus
    
    g_bCancelled = False
    g_bShowPrevious = False
    
End Sub

Function GetDirectory(fullFilename As String) As String
    ' get the file name
    Dim directoryName As String
    Dim slashPos As Long        ' the position of "\"
    
    directoryName = fullFilename
    
    slashPos = InStrRev(fullFilename, "\", -1, vbTextCompare)
    If slashPos > 0 Then
        directoryName = Left(fullFilename, slashPos)
    End If
    
    GetDirectory = directoryName
End Function

Function DirExists(ByVal DName As String) As Boolean

    On Error Resume Next
    
    DirExists = False
    
    If Right(DName, 1) <> "\" Then
        DName = DName & "\"
    End If
    
    Dim fs, f, s
    Set fs = CreateObject("Scripting.FileSystemObject")
    Set f = fs.GetFolder(DName)
    If Not IsEmpty(f) Then
        DirExists = True
    End If

End Function

Private Function PtrToStrW(ByVal Pointer As Long) As String
    Dim Length As Long
    
    If Pointer Then
        Length = lstrlenW(Pointer)
        If Length > 0 Then
            PtrToStrW = String$(Length, 0)
            RtlMoveMemory StrPtr(PtrToStrW), Pointer, Length * 2
        End If
    End If
End Function

Private Sub cmdBrowse_Click()
    Dim ofn As OPENFILENAMEW
    
    Dim szLsdDescription As String
    szLsdDescription = LoadResString(EXT_LSD_DESC + g_iLanguage * 1000)
    
    Dim szAllFileDescription As String
    szAllFileDescription = LoadResString(EXT_ALL_FILE_DESC + g_iLanguage * 1000)
    
    With ofn
        .lStructSize = Len(ofn)
        .hwndOwner = Me.hwnd
        .Flags = 33796 '0x8406 = OFN_EXTENSIONDIFFERENT | OFN_NOREADONLYRETURN | OFN_HIDEREADONLY
        Dim szExt As String: szExt = "lsd"
        .lpstrDefExt = StrPtr(szExt)
        Dim szFilter As String
        szFilter = szLsdDescription & " (*.lsd)" & Chr(0) & "*.lsd" & Chr(0) _
                     & szAllFileDescription & " (*.*)" & Chr(0) & "*.*" & Chr(0) & Chr(0)
        .lpstrFilter = StrPtr(szFilter)
        .nMaxFile = 1024
        Dim szFile As String: szFile = g_szFilePath & Space(.nMaxFile - LenB(g_szFilePath) - 1) & Chr(0)
        .lpstrFile = StrPtr(szFile)
        .nMaxFileTitle = 1024
        Dim szTitle As String: szTitle = Space(.nMaxFileTitle - 1) & Chr(0)
        .lpstrFileTitle = StrPtr(szTitle)
        .lpstrInitialDir = StrPtr(p_szSourceDirectory)
    End With
    
    If GetSaveFileNameW(ofn) <> 0 Then
        Me.File.Text = PtrToStrW(ofn.lpstrFile)
    End If

End Sub

Private Sub cmdCancel_Click()
    g_bCancelled = True
    
    FreeFileSDK
    Unload Me
End Sub

Private Sub cmdNext_Click()
    g_lTop = Me.Top
    g_lLeft = Me.Left
    
    Dim bWriteFile As Boolean: bWriteFile = True
    Dim szFileDir As String
    szFileDir = GetDirectory(Me.File.Text)
    
    Dim iButton As Long
    If Not DirExists(szFileDir) Then
        Dim szDirectoryTitle As String
        szDirectoryTitle = LoadResString(DLG_CAPTION + g_iLanguage * 1000)
        Dim szDirectoryText As String
        szDirectoryText = LoadResString(ERR_DIRECTORY_ERROR + g_iLanguage * 1000)
        iButton = MessageBoxW(0, StrPtr(szDirectoryText), StrPtr(szDirectoryTitle), MB_ICONINFORMATION Or MB_TASKMODAL)
        bWriteFile = False
    End If
    
    If bWriteFile Then
        g_szFilePath = Me.File.Text
        
        If FileExists(g_szFilePath) Then
            Dim szOverwriteTitle As String
            szOverwriteTitle = LoadResString(DLG_CAPTION + g_iLanguage * 1000)
            Dim szOverwriteText As String
            szOverwriteText = LoadResString(ERR_OVERWRITE_FILE1 + g_iLanguage * 1000)
            szOverwriteText = szOverwriteText & GetFileName(g_szFilePath)
            szOverwriteText = szOverwriteText & LoadResString(ERR_OVERWRITE_FILE2 + g_iLanguage * 1000)
            iButton = MessageBoxW(0, StrPtr(szOverwriteText), StrPtr(szOverwriteTitle), MB_ICONQUESTION Or MB_YESNO Or MB_TASKMODAL)
            If iButton = IDYES Then
                bWriteFile = True
            Else
                bWriteFile = False
            End If
        End If
        bWriteFile = True
    End If
        
    If bWriteFile Then
        FreeFileSDK
        Unload Me
    End If
    
End Sub

Private Sub cmdPrev_Click()
    g_lTop = Me.Top
    g_lLeft = Me.Left
    
    g_bShowPrevious = True
    
    g_szFilePath = Me.File.Text
    FreeFileSDK
    Unload Me
End Sub

