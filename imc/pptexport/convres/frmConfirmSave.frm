VERSION 5.00
Begin VB.Form frmConfirmSave 
   BorderStyle     =   1  'Fest Einfach
   Caption         =   "Form1"
   ClientHeight    =   1644
   ClientLeft      =   48
   ClientTop       =   312
   ClientWidth     =   5064
   Icon            =   "frmConfirmSave.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1644
   ScaleWidth      =   5064
   StartUpPosition =   2  'Bildschirmmitte
   Begin VB.PictureBox Picture1 
      BorderStyle     =   0  'Kein
      Height          =   735
      Left            =   240
      Picture         =   "frmConfirmSave.frx":0E42
      ScaleHeight     =   732
      ScaleWidth      =   732
      TabIndex        =   4
      Top             =   360
      Width           =   735
   End
   Begin VB.CommandButton CmdCancel 
      Caption         =   "Abbrechen"
      Height          =   375
      Left            =   3480
      TabIndex        =   2
      Top             =   1200
      Width           =   975
   End
   Begin VB.CommandButton CmdNo 
      Caption         =   "Nein"
      Height          =   375
      Left            =   2040
      TabIndex        =   1
      Top             =   1200
      Width           =   975
   End
   Begin VB.CommandButton CmdYes 
      Caption         =   "Ja"
      Height          =   375
      Left            =   600
      TabIndex        =   0
      Top             =   1200
      Width           =   975
   End
   Begin VB.Label lblSaveWarning 
      Caption         =   "Ihre PowerPoint-Präsentation ist nicht gespeichert worden. Wollen Sie sie speichern, bevor die Konvertierung vorgenommen wird?"
      Height          =   975
      Left            =   1080
      TabIndex        =   3
      Top             =   360
      Width           =   3735
      WordWrap        =   -1  'True
   End
End
Attribute VB_Name = "frmConfirmSave"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Public Version As String
Public LanguageID As Integer
Public RetValue     As Long

Private Sub cmdCancel_Click()
    RetValue = 0
    Unload Me
End Sub

Private Sub CmdNo_Click()
    RetValue = -1
    Unload Me
End Sub

Private Sub CmdYes_Click()
    RetValue = 1
    Unload Me
End Sub

Private Sub Form_Activate()
    Me.Caption = LoadResString(DLG_CAPTION + LanguageID * 1000) ' & Version
    
    Me.lblSaveWarning.Caption = LoadResString(STR_CONFIRM_SAVE + LanguageID * 1000)
    
    Me.CmdYes.Caption = LoadResString(CMD_YES + LanguageID * 1000)
    Me.CmdNo.Caption = LoadResString(CMD_NO + LanguageID * 1000)
    Me.cmdCancel.Caption = LoadResString(CMD_CANCEL + LanguageID * 1000)
    RetValue = 0
End Sub

