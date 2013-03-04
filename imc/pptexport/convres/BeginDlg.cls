VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "Selection"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit

' This DLL contains the resource of the ppt2Lecturnity converter.
' Version, 0.0.2
'
' Jul. 16. 2002


' Public Properties

' if the security key is illegal, the function returns false.
' Program language setting is returned by argument "progLang".
' If button "continue" is clicked, argument "bContinue" returns true.
Public Function Show(iTotalSlideCount As Long, iCurrentSlide As Long, szDefaultName As String, szSlideSelection As String, szFilePath As String) As Boolean
    
    Dim iLanguage As Integer: iLanguage = LangInfo.GetProgramLanguage()
    
    If iLanguage = -1 Then
        ' English message
        MsgBox LoadResString(ERR_LEC_NOT_INSTALLED + 1000), vbCritical, "Error"
        Show = False
        Exit Function
    End If
    
    If iTotalSlideCount < 1 Then
        MsgBox LoadResString(ERR_NOPAGE + iLanguage * 1000), vbExclamation, "Error"
        Show = False
        Exit Function
    End If
    
    Dim lTop As Long: lTop = -1
    Dim lLeft As Long: lLeft = -1
    
    szSlideSelection = ""
    szFilePath = GetSourceDir() & "\" & szDefaultName
    Dim iSelection As Integer: iSelection = 0
    Dim bContinue As Boolean: bContinue = True
    While bContinue
        With frmStart
            If lTop <> -1 And lLeft <> -1 Then
                .g_lTop = lTop
                .g_lLeft = lLeft
                .Top = lTop
                .Left = lLeft
            End If
            .g_iLanguage = iLanguage
            .g_iTotalSlideCount = iTotalSlideCount
            .g_iCurrentSlide = iCurrentSlide
            .g_iSelection = iSelection
            If .g_iSelection = 2 Then
                .g_szSlideSelection = szSlideSelection
            Else
                .g_szSlideSelection = ""
            End If
            
            .Show vbModal        ' modal dialog
            
            iSelection = .g_iSelection
            szSlideSelection = .g_szSlideSelection
            bContinue = Not .g_bCancelled
            lTop = .g_lTop
            lLeft = .g_lLeft
        End With
        
        If Not bContinue Then
            Show = False
            Exit Function
        End If
        
        With frmFile
            If lTop <> -1 And lLeft <> -1 Then
                .g_lTop = lTop
                .g_lLeft = lLeft
                .Top = lTop
                .Left = lLeft
            End If
            .g_iLanguage = iLanguage
            .g_szFilePath = szFilePath
            
            .Show vbModal
            
            If .g_bCancelled Then
                Show = False
                Exit Function
            End If
            
            If .g_bShowPrevious Then
                bContinue = True
            Else
                bContinue = False
            End If
            
            szFilePath = .g_szFilePath
            lTop = .g_lTop
            lLeft = .g_lLeft
        End With
    Wend
    
    ShowProgress lTop, lLeft
    
    Show = True
    
End Function

Public Function ShowProgress(lTop As Long, lLeft As Long) As Boolean
    
    Dim iLanguage As Integer: iLanguage = LangInfo.GetProgramLanguage()
    
    If iLanguage = -1 Then
        ' English message
        MsgBox LoadResString(ERR_LEC_NOT_INSTALLED + 1000), vbCritical, "Error"
        ShowProgress = False
        Exit Function
    End If
    
    With frmProgress
        If lTop <> -1 And lLeft <> -1 Then
            .g_lTop = lTop
            .g_lLeft = lLeft
            .Top = lTop
            .Left = lLeft
        End If
        .g_iLanguage = iLanguage
        .Show vbModeless
        lTop = .g_lTop
        lLeft = .g_lLeft
    End With
    
    ShowProgress = True
    
End Function
