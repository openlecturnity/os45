Attribute VB_Name = "GlobalDefines"
Option Explicit

'--------------------
' const definition
'--------------------

Public Const NOT_SUPPORTED_AUDIO                    As Integer = 1
Public Const NOT_SUPPORTED_VIDEO                    As Integer = 2
Public Const NOT_SUPPORTED_ANIMATIONEFFECT_SHOW     As Integer = 3
Public Const NOT_SUPPORTED_ANIMATIONEFFECT_HIDE     As Integer = 4
Public Const NOT_SUPPORTED_ANIMATIONEFFECT_OTHER    As Integer = 5
Public Const NOT_SUPPORTED_OLE                      As Integer = 6
Public Const NOT_SUPPORTED_DIAGRAM                  As Integer = 7
Public Const NOT_SUPPORTED_TEXTEFFECT_SHADOW        As Integer = 8
Public Const NOT_SUPPORTED_TEXTEFFECT_RELIEF        As Integer = 9
Public Const NOT_SUPPORTED_TEXTEFFECT_UNDERLINE     As Integer = 10
Public Const NOT_SUPPORTED_TABULATOR                As Integer = 11
Public Const NOT_SUPPORTED_SPECIALCHAR              As Integer = 12
Public Const NOT_SUPPORTED_JUSTIFY                  As Integer = 13

Public Const CURRENT_VERSION                        As String = "3.0.0"
Public Const ITEM_INCREMENT                         As Long = 5
Public Const PI                                     As Double = 3.14159265358979

Public Const MAXCTLPOINTS = 12                     ' Bezier curve const
Public Const POINTDIST = 2                            ' the distance between two adjacent points in a Bezier curve



'---------------------
' User defined enums
'---------------------
  Enum LecDrawStyle
      LEC_DRAWSTYLE_NONE = 0
      LEC_DRAWSTYLE_OUTLINE = 1
      LEC_DRAWSTYLE_FILLED = 2
      LEC_DRAWSTYLE_FILLEDOUTLINE = 3
      LEC_DRAWSTYLE_TRANSPARENT = 4
  End Enum

  Enum LecShapeType
     LEC_SHAPETYPE_NONE
     LEC_SHAPETYPE_LINE
     LEC_SHAPETYPE_TEXT
     LEC_SHAPETYPE_GROUP
     LEC_SHAPETYPE_POLYGON
     LEC_SHAPETYPE_FREEHAND
     LEC_SHAPETYPE_RECTANGLE
     LEC_SHAPETYPE_CIRCLE
     LEC_SHAPETYPE_IMAGE
     LEC_SHAPETYPE_TEXTGROUP
     LEC_SHAPETYPE_USERDEFINEDGROUP
  End Enum

  Enum LecFooterHeaderTypes
      LEC_FOOTERHEADER_NONE = 0
      LEC_FOOTERHEADER_DATETIME = 1
      LEC_FOOTERHEADER_FOOTER = 2
      LEC_FOOTERHEADER_SLIDENUMBER = 3
  End Enum

  Enum LecErrorType
      LEC_ERRORTYPE_SLIDEINFO
      LEC_ERRORTYPE_UNKNOWN
      LEC_ERRORTYPE_INVISIBLESHAPE
      LEC_ERRORTYPE_UNKNOWNBULLET
      LEC_ERRORTYPE_NONODES
      LEC_ERRORTYPE_POLYGONNONODES
      LEC_ERRORTYPE_INVALIDPOLYGON
      LEC_ERRORTYPE_BEGINTIMESTAMP
      LEC_ERRORTYPE_ENDTIMESTAMP
  End Enum

  Enum LecTextShapeType               ' no use any more
      LEC_TEXTSHAPETYPE_CONTAINER
      LEC_TEXTSHAPETYPE_ATOM            ' text segment without tabstop
      LEC_TEXTSHAPETYPE_BULLET
  End Enum


