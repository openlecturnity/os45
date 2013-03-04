VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "cEventClass"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Public WithEvents PPTEvent As Application
Attribute PPTEvent.VB_VarHelpID = -1

Private Sub PPTEvent_NewPresentation(ByVal Pres As Presentation)
    RefreshMenuItems True
End Sub

Private Sub PPTEvent_PresentationClose(ByVal Pres As Presentation)
    If Application.Presentations.Count > 1 Then
        RefreshMenuItems True
    Else
        RefreshMenuItems False
    End If
End Sub

Private Sub PPTEvent_PresentationOpen(ByVal Pres As Presentation)
    RefreshMenuItems True
End Sub

Private Sub RefreshMenuItems(Enable As Boolean)
    Dim oCmdCtl As CommandBarControl
    
    Set oCmdCtl = Application.CommandBars.FindControl(Tag:="menu_ppt2Lecturnity")
    If oCmdCtl Is Nothing Then
        ' no menu item found.
    Else
        oCmdCtl.Enabled = Enable
    End If
    Set oCmdCtl = Nothing
    
    Set oCmdCtl = Application.CommandBars.FindControl(Tag:="menu_ppt2Lecturnity2")
    If oCmdCtl Is Nothing Then
        ' no menu item found.
    Else
        oCmdCtl.Enabled = Enable
    End If
    Set oCmdCtl = Nothing
    
End Sub

