VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "Progress"
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

Const c_iTitleMaster = 3
Const c_iMasterMax = 12
Const c_iTemplateMax = 15
Const c_iSliderMax = 70

Public Function Show() As Boolean
    
    Dim iLanguage As Integer: iLanguage = LangInfo.GetProgramLanguage()
    
    If iLanguage = -1 Then
        ' English message
        MsgBox LoadResString(ERR_LEC_NOT_INSTALLED + 1000), vbCritical, "Error"
        Show = False
        Exit Function
    End If
    
    With frmProgress
        .g_iLanguage = iLanguage
        .g_bWizard = False
        .Show vbModeless
    End With
    
    Show = True
    
End Function

Public Sub Hide()

    frmProgress.g_lTop = frmProgress.Top
    frmProgress.g_lLeft = frmProgress.Left
    
    Unload frmProgress
End Sub

Public Function GetTop() As Long
    GetTop = frmProgress.g_lTop
End Function

Public Function GetLeft() As Long
    GetLeft = frmProgress.g_lLeft
End Function

Public Function ImportTemplate(lTotalTemplates As Long, lCurrentTemplate As Long)
    
    Dim iLanguage As Integer: iLanguage = LangInfo.GetProgramLanguage()
    
    If frmProgress.g_bImportFinished Or frmProgress.Visible = False Then
        Unload frmProgress
        ImportTemplate = False
        Exit Function
    End If
    
    frmProgress.txtProgressSlide.Caption = LoadResString(TXT_IMPORT_TEMPLATE + iLanguage * 1000)
    'frmProgress.txtProgressSlide.Refresh
    
    Dim iProgressValue As Integer
    iProgressValue = c_iTitleMaster + c_iMasterMax + (c_iTemplateMax * lCurrentTemplate / lTotalTemplates)
    frmProgress.ProgressBar.Value = iProgressValue
    
    'frmProgress.Refresh
    
    ImportTemplate = True
End Function

Public Function ImportSlide(lTotalSlides As Long, lCurrentSlide As Long) As Boolean
    
    Dim iLanguage As Integer: iLanguage = LangInfo.GetProgramLanguage()
    
    If frmProgress.g_bImportFinished Or frmProgress.Visible = False Then
        Unload frmProgress
        ImportSlide = False
        Exit Function
    End If
        
    frmProgress.txtProgressSlide.Caption = LoadResString(TXT_IMPORT_SLIDE + iLanguage * 1000) & _
                                           " " & lCurrentSlide & " " & _
                                           LoadResString(TXT_IMPORT_SLIDE + 1 + iLanguage * 1000) & _
                                           " " & lTotalSlides
    'frmProgress.txtProgressSlide.Refresh
                                           
    Dim iProgressValue As Integer
    iProgressValue = c_iTitleMaster + c_iMasterMax + c_iTemplateMax + (c_iSliderMax * lCurrentSlide / lTotalSlides)
    frmProgress.ProgressBar.Value = iProgressValue
    
    ImportSlide = True
End Function

Public Function TimerUpdate(szTimeLeft As String) As Boolean
    
    Dim iLanguage As Integer: iLanguage = LangInfo.GetProgramLanguage()
    
    If frmProgress.g_bImportFinished Or frmProgress.Visible = False Then
        Unload frmProgress
        TimerUpdate = False
        Exit Function
    End If

    frmProgress.txtProgressTimeLeft = LoadResString(TXT_TIME_LEFT + iLanguage * 1000) & _
                                      " " & szTimeLeft
    'frmProgress.txtProgressTimeLeft.Refresh
                                      
    TimerUpdate = True
End Function

Public Function ParseTitleMaster() As Boolean
    
    Dim iLanguage As Integer: iLanguage = LangInfo.GetProgramLanguage()
    
    If frmProgress.g_bImportFinished Or frmProgress.Visible = False Then
        Unload frmProgress
        ParseTitleMaster = False
        Exit Function
    End If
    
    frmProgress.txtProgressSlide.Caption = LoadResString(TXT_IMPORT_TEMPLATE + iLanguage * 1000)
    'frmProgress.txtProgressSlide.Refresh
    
    Dim iProgressValue As Integer
    iProgressValue = c_iTitleMaster
    frmProgress.ProgressBar.Value = iProgressValue
    
    ParseTitleMaster = True
End Function


Public Function ParseMaster(lTotalMasters As Long, lCurrentMaster As Long) As Boolean

    Dim iLanguage As Integer: iLanguage = LangInfo.GetProgramLanguage()
    
    If frmProgress.g_bImportFinished Or frmProgress.Visible = False Then
        Unload frmProgress
        ParseMaster = False
        Exit Function
    End If
    
    frmProgress.txtProgressSlide.Caption = LoadResString(TXT_IMPORT_TEMPLATE + iLanguage * 1000)
    'frmProgress.txtProgressSlide.Refresh
    
    Dim iProgressValue As Integer
    iProgressValue = c_iTitleMaster + (c_iMasterMax * lCurrentMaster / lTotalMasters)
    frmProgress.ProgressBar.Value = iProgressValue
    
    ParseMaster = True
End Function

