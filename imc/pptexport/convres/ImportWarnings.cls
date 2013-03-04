VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "ImportWarnings"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Private g_iRowIndex As Integer

Public Function Init(lTop As Long, lLeft As Long) As Boolean
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()
    
    g_iRowIndex = 0
    
    frmImportWarnings.Grid.Cols = 4
    frmImportWarnings.Grid.ColAlignment(0) = 1
    frmImportWarnings.Grid.ColAlignment(1) = 1
    frmImportWarnings.Grid.ColAlignment(2) = 1
    frmImportWarnings.Grid.ColAlignment(3) = 1
    
    Dim iFrameBorder As Integer: iFrameBorder = frmImportWarnings.Width - frmImportWarnings.ScaleWidth
    Dim iGridWidth As Integer: iGridWidth = frmImportWarnings.Grid.Width - iFrameBorder
    
    frmImportWarnings.Grid.ColWidth(0) = 1 * iGridWidth / 8
    frmImportWarnings.Grid.TextMatrix(0, 0) = LoadResString(GRID_CAPTION_PAGE + langID * 1000)
    
    frmImportWarnings.Grid.ColWidth(1) = 2 * iGridWidth / 8
    frmImportWarnings.Grid.TextMatrix(0, 1) = LoadResString(GRID_CAPTION_OBJECT + langID * 1000)
    
    frmImportWarnings.Grid.ColWidth(2) = 1 * iGridWidth / 8
    frmImportWarnings.Grid.TextMatrix(0, 2) = LoadResString(GRID_CAPTION_COUNT + langID * 1000)
    
    frmImportWarnings.Grid.ColWidth(3) = 4 * iGridWidth / 8
    frmImportWarnings.Grid.TextMatrix(0, 3) = LoadResString(GRID_CAPTION_DESCRIPTION + langID * 1000)
    
    frmImportWarnings.g_lTop = lTop
    frmImportWarnings.g_lLeft = lLeft
    frmImportWarnings.Top = lTop
    frmImportWarnings.Left = lLeft

End Function

Public Function Show() As Boolean

    Dim lShowSummary As Long
    Dim bSuccess As Boolean
    bSuccess = Registry.GetShowSummary(lShowSummary)
    
    If (bSuccess And lShowSummary = 0) Then
        Show = True
        Exit Function
    End If
    
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
    
    With frmImportWarnings
        .g_lShowNeverAgain = False
        .LanguageID = langID
        .Show vbModal        ' modal dialog
    End With
    
    Show = True
End Function

Public Function Add(iPageNumber As Long, iObjectType As Integer, iObjectCount As Integer) As Boolean
    
    Add = AddToGrid(frmImportWarnings, iPageNumber, iObjectType, iObjectCount, g_iRowIndex)
    
End Function


