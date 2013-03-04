Attribute VB_Name = "batchConv"
Option Explicit

Const CONV_SUCCESS As Integer = 1
Const ERR_DISK_FULL As Integer = 9

Public Function MuteConvert() As Integer
    Dim AofDoc1 As AofDoc
    Dim BeginPage As Long
    Dim PageNum As Long
    Dim i As Long
    
    Set AofDoc1 = New AofDoc
    BeginPage = 1
    PageNum = GlobalPres.Slides.Count
    
    AofDoc1.PreConversion GlobalPres, BeginPage, PageNum
    
    For i = 1 To PageNum
        If Not CvtOpt.Failed Then
            AofDoc1.ConvertSlide (BeginPage + i - 1)
        Else
            Exit For
        End If
    Next i
    
    AofDoc1.PostConversion
    
    If CvtOpt.Failed Then
        'MsgBox msgDiskFull, vbCritical, DlgCaption
        MuteConvert = ERR_DISK_FULL
    End If
    
    Set AofDoc1 = Nothing
    
    MuteConvert = CONV_SUCCESS

End Function
