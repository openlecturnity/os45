VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "Summary"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit

Private g_bWarningMessages As Boolean

Private g_iRowIndex As Integer


Public Function Init(lTop As Long, lLeft As Long) As Boolean
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()
    
    g_iRowIndex = 0
    g_bWarningMessages = False
    
    frmResult.Grid.Cols = 4
    frmResult.Grid.ColAlignment(0) = 1
    frmResult.Grid.ColAlignment(1) = 1
    frmResult.Grid.ColAlignment(2) = 1
    frmResult.Grid.ColAlignment(3) = 1
    
    Dim iFrameBorder As Integer: iFrameBorder = frmResult.Width - frmResult.ScaleWidth
    Dim iGridWidth As Integer: iGridWidth = frmResult.Grid.Width - iFrameBorder
    
    frmResult.Grid.ColWidth(0) = 1 * iGridWidth / 8
    frmResult.Grid.TextMatrix(0, 0) = LoadResString(GRID_CAPTION_PAGE + langID * 1000)
    
    frmResult.Grid.ColWidth(1) = 2 * iGridWidth / 8
    frmResult.Grid.TextMatrix(0, 1) = LoadResString(GRID_CAPTION_OBJECT + langID * 1000)
    
    frmResult.Grid.ColWidth(2) = 1 * iGridWidth / 8
    frmResult.Grid.TextMatrix(0, 2) = LoadResString(GRID_CAPTION_COUNT + langID * 1000)
    
    frmResult.Grid.ColWidth(3) = 4 * iGridWidth / 8
    frmResult.Grid.TextMatrix(0, 3) = LoadResString(GRID_CAPTION_DESCRIPTION + langID * 1000)
    
    frmResult.g_lTop = lTop
    frmResult.g_lLeft = lLeft
    frmResult.Top = lTop
    frmResult.Left = lLeft
    
End Function

' If button "Open with Assistant" is clicked, "bOpenFile" returns true.
' The function return value is not important at all.
Public Function Show(ByRef bOpenFile As Boolean) As Boolean
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()
    
    If langID = -1 Then
        ' GetProgramLanguage returns 1 means the security key is illegal.
        MsgBox "LECTURNITY was not installed.", vbCritical, "Error"
        Show = False
        Exit Function
    ElseIf langID > MAXLANGID Then
        MsgBox "Invalid language setting. Use German."
        langID = 0
    End If
    
    With frmResult
        .LanguageID = langID

        If Not g_bWarningMessages Then
            .DetailsFrame.Visible = False
        End If
        
        .Show vbModal        ' modal dialog
        bOpenFile = .bOpenIt
    End With
    
    Show = True
    
End Function

Public Function Add(iPageNumber As Long, iObjectType As Integer, iObjectCount As Integer) As Boolean

    Add = AddToGrid(frmResult, iPageNumber, iObjectType, iObjectCount, g_iRowIndex)
    g_bWarningMessages = True
    
'    Dim langID As Integer
'    langID = LangInfo.GetProgramLanguage()
'
'
'    If langID = -1 Then
'        ' GetProgramLanguage returns 1 means the security key is illegal.
'        MsgBox "LECTURNITY was not installed.", vbCritical, "Error"
'        Add = False
'        Exit Function
'    ElseIf langID > 1 Then
'        MsgBox "Invalid language setting. Use German."
'        langID = 0
'    End If
'
'    g_iRowIndex = g_iRowIndex + 1
'    If frmResult.Grid.Rows <= g_iRowIndex Then
'        frmResult.Grid.Rows = g_iRowIndex + 1
'    End If
'
'    If iPageNumber <> -1 Then
'        frmResult.Grid.TextMatrix(g_iRowIndex, 0) = iPageNumber
'    End If
'    frmResult.Grid.TextMatrix(g_iRowIndex, 1) = LoadResString(ERROR_MESSAGE_SHAPES_BEGIN + iObjectType + langID * 1000)
'    frmResult.Grid.TextMatrix(g_iRowIndex, 2) = iObjectCount
'    frmResult.Grid.TextMatrix(g_iRowIndex, 3) = LoadResString(ERROR_MESSAGE_WARNING_BEGIN + iObjectType + langID * 1000)
'
'    Dim i As Integer
'    For i = 0 To 3
'        Dim textWidth As Long: textWidth = frmResult.textWidth(frmResult.Grid.TextMatrix(g_iRowIndex, i)) + 80
'        If textWidth > frmResult.Grid.ColWidth(i) Then
'            frmResult.Grid.ColWidth(i) = textWidth
'        End If
'    Next
'
'    Add = True
    
End Function

