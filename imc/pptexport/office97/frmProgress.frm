VERSION 5.00
Begin {C62A69F0-16DC-11CE-9E98-00AA00574A4F} frmProgress 
   Caption         =   "Progress"
   ClientHeight    =   375
   ClientLeft      =   45
   ClientTop       =   285
   ClientWidth     =   4485
   OleObjectBlob   =   "frmProgress.frx":0000
   StartUpPosition =   1  'CenterOwner
End
Attribute VB_Name = "frmProgress"
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False



Option Explicit

Public BeginPage As Long
Public PageNum As Long

Private AofDoc1 As AofDoc

Private hWnd As Long
Private Declare Function SetWindowLong Lib "user32" Alias "SetWindowLongA" (ByVal hWnd As Long, ByVal nIndex As Long, ByVal dwNewLong As Long) As Long
Private Declare Function GetWindowLong Lib "user32" Alias "GetWindowLongA" (ByVal hWnd As Long, ByVal nIndex As Long) As Long
Private Const GWL_STYLE = (-16)
Private Const WS_CAPTION = &HC00000            ' WS_BORDER Or WS_DLGFRAME
Private Const WS_MAXIMIZEBOX = &H10000
Private Const WS_MINIMIZEBOX = &H20000
Private Const WS_SYSMENU = &H80000

Private Declare Function SetWindowPos Lib "user32" (ByVal hWnd As Long, ByVal hWndInsertAfter As Long, ByVal X As Long, ByVal Y As Long, ByVal cx As Long, ByVal cy As Long, ByVal wFlags As Long) As Long

'Private Enum ESetWindowPosStyles
Private Const SWP_SHOWWINDOW = &H40
Private Const SWP_HIDEWINDOW = &H80
Private Const SWP_FRAMECHANGED = &H20     ' The frame changed: send WM_NCCALCSIZE
Private Const SWP_NOACTIVATE = &H10
Private Const SWP_NOCOPYBITS = &H100
Private Const SWP_NOMOVE = &H2
Private Const SWP_NOOWNERZORDER = &H200     ' Don't do owner Z ordering
Private Const SWP_NOREDRAW = &H8
Private Const SWP_NOREPOSITION = SWP_NOOWNERZORDER
Private Const SWP_NOSIZE = &H1
Private Const SWP_NOZORDER = &H4
Private Const SWP_DRAWFRAME = SWP_FRAMECHANGED
Private Const HWND_NOTOPMOST = -2
'End Enum

Private Declare Function GetWindowRect Lib "user32" (ByVal hWnd As Long, lpRect As RECT) As Long

Private Type RECT
    rLeft As Long
    rTop As Long
    rRight As Long
    rBottom As Long
End Type

Private Sub HideTitleBar()
    Dim lStyle As Long
    Dim tRect As RECT
    
    Dim hWnd As Long

    hWnd = GetActiveWindow()
    ' Get the window's position:
    GetWindowRect hWnd, tRect 'Me.hwnd, tR

    ' Modify whether title bar will be visible:
    lStyle = GetWindowLong(hWnd, GWL_STYLE) '(Me.hwnd, GWL_STYLE)
    
    Me.Tag = Me.Caption
    Me.Caption = ""
    lStyle = lStyle And Not WS_SYSMENU
    lStyle = lStyle And Not WS_MAXIMIZEBOX
    lStyle = lStyle And Not WS_MINIMIZEBOX
    lStyle = lStyle And Not WS_CAPTION
    
    SetWindowLong hWnd, GWL_STYLE, lStyle 'Me.hwnd, GWL_STYLE, lStyle
    
    Dim ht As Single
    ht = 0 '14      ' height of title bar
    ' Ensure the style takes and make the window the
    ' same size, regardless that the title bar etc
    ' is now a different size:
    SetWindowPos hWnd, 0, tRect.rLeft, tRect.rTop, tRect.rRight - tRect.rLeft, tRect.rBottom - tRect.rTop - ht, SWP_NOREPOSITION Or SWP_NOZORDER Or SWP_FRAMECHANGED
    'Me.Refresh
    
    ' Ensure that your resize code is fired, as the client area
    ' has changed:
    'Form_Resize

End Sub

Property Let ScreenUpdating(State As Boolean)
'    Static hwnd As Long
    Dim VersionNo As String
      
    ' Get Version Number
    If State = False Then
        VersionNo = Left(Application.Version, _
                    InStr(1, Application.Version, ".") - 1)

        ' Get handle to the main application window using ClassName
        Select Case VersionNo
    '        Case "8"
    '            ' For PPT97:
    '            hwnd = FindWindow("PP97FrameClass", 0&)
            Case "9"
                ' For PPT2K:
                hWnd = FindWindow("PP9FrameClass", 0&)
            Case "10" ' For XP:
                hWnd = FindWindow("PP10FrameClass", 0&)
            Case Else
                Exit Property
        End Select
        
        If hWnd = 0 Then
            Exit Property
        End If
        
        If LockWindowUpdate(hWnd) = 0 Then
            Exit Property
        End If
    Else
        ' Unlock the Window to refresh
        LockWindowUpdate (0&)
        UpdateWindow (hWnd)
        hWnd = 0
    End If
End Property


Private Sub UserForm_Activate()
    Dim i As Long
    Dim j As Long
    Dim dump As Double
    Dim percent As Single
    Dim extRes As Object
    Dim rEnd  As Single
    Dim msgDiskFull As String
    
    DoEvents
    Set extRes = CreateObject("ConvRes.ExtResource")
    
    With extRes
        Me.Caption = .GetProgressCaption()
        Select Case CvtOpt.MacroID
            Case 1
                lblPercent.Caption = .GetProgSaving()
            Case 2
                lblPercent.Caption = .GetProgOpening()
            Case Else
                lblPercent.Caption = .GetProgSaving()
        End Select
        msgDiskFull = .GetDiskFullMsg()
    End With
    
    Set extRes = Nothing
    
    lblPercent.Caption = lblPercent.Caption & BetterFileName() '
    
    HideTitleBar
    Me.Repaint
    rEnd = Me.Label1.Left + Me.Label1.Width
    
    ' lock the PowerPoint window.
    ScreenUpdating = False
    
    Set AofDoc1 = New AofDoc
    AofDoc1.PreConversion GlobalPres, BeginPage, PageNum
    
    
        
    For i = 1 To PageNum
        
        If Not CvtOpt.Failed Then
            AofDoc1.ConvertSlide (BeginPage + i - 1)
            
            ' Increase size of Label1 incrementally
            percent = i / PageNum
            
            ' new progress bar.
            Me.Label1.Left = CLng(percent * Me.Image1.Width / 6.75) * 6.75 + 5
            Me.Label1.Width = IIf(rEnd > Me.Label1.Left, rEnd - Me.Label1.Left, 0)
            Frame2.Repaint
            
            ' old progress bar.
            lblProgBar.Left = CLng(percent * fraProgBar.Width / 9.75) * 9.75 - 2
            ' Repaint allows Label1 to display as it is enlarging
            fraProgBar.Repaint
            Frame1.Repaint
            DoEvents
        
        Else
            Exit For
        End If
    
    Next i
    
    AofDoc1.PostConversion
    
    If CvtOpt.Failed Then
        MsgBox msgDiskFull, vbCritical, DlgCaption
    End If
    
    Set AofDoc1 = Nothing
    
    ' unlock the powerpoint window.
    ScreenUpdating = True
    
    ' Unload UserForm when process finished
    Unload Me
    

End Sub

Private Function BetterFileName() As String
    Dim lenStr As Long: lenStr = 50
    Dim strPath As String
    Dim lenPath As Long
    
    If Len(lblPercent.Caption) + Len(CvtOpt.WBFileName) + Len(CvtOpt.WBFilePath) > lenStr Then
        lenPath = lenStr - Len(lblPercent.Caption) - Len(CvtOpt.WBFileName)
        lenPath = IIf(lenPath < 7, 7, lenPath)
        strPath = Mid(CvtOpt.WBFilePath, 1, lenPath - 4) & "...\"
    Else
        strPath = CvtOpt.WBFilePath
    End If
    BetterFileName = strPath & CvtOpt.WBFileName
End Function
