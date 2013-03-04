Attribute VB_Name = "LangInfo"
Option Explicit

Public Enum InterfaceLanguage
    GERMAN = 0
    ENGLISH = 1
    VIETNAM = 2
End Enum

Public Function GetProgramLanguage() As Integer

    On Error GoTo ErrQuit
    
    Dim retOnError As Integer            '= -1;
    
    Dim dwType As Long
    Dim dwBufSize As Long
    Dim regErr As Long
    
    Dim i As Long
    Dim binData As String
    
    Dim hKeyMachine As Long
    Dim sLanguage As String
    Dim sPassWord As String
    Dim iPassWordLength  As Integer
    
    'initialization
    dwBufSize = 0
    retOnError = -1
    
    hKeyMachine = 0
    
    
    sPassWord = "passwortlecturnity"
    iPassWordLength = 18
    
    
    regErr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\Kerberok", 0, KEY_READ, hKeyMachine)
    
    If (regErr <> ERROR_SUCCESS) Then
        GetProgramLanguage = retOnError
        Exit Function
    End If
    
    ' get buffer size
    regErr = RegQueryValueEx(hKeyMachine, "Security", 0, dwType, ByVal vbNullString, dwBufSize)
    
    If (regErr <> ERROR_SUCCESS) Then
        RegCloseKey (hKeyMachine)
        GetProgramLanguage = retOnError
        Exit Function
    End If

    If (dwType <> REG_BINARY) Then
        RegCloseKey (hKeyMachine)
        GetProgramLanguage = retOnError
        Exit Function
    End If

    binData = Space(dwBufSize + 8)
    
    If IsNull(binData) Then
        RegCloseKey (hKeyMachine)
        GetProgramLanguage = retOnError
        Exit Function
    End If

    regErr = RegQueryValueEx(hKeyMachine, "Security", 0, dwType, ByVal binData, dwBufSize)

    If (regErr <> ERROR_SUCCESS) Then
        'Erase binData
        RegCloseKey (hKeyMachine)
        GetProgramLanguage = retOnError
        Exit Function
    End If
    
    Dim tmpBinData As String
    Dim bTmp    As Byte
    
    tmpBinData = ""
    
    For i = 0 To dwBufSize - 1
'       binData[i] = (char)(binData[i] ^ passWord[i%passWordLength]);
        bTmp = Asc(Mid(binData, i + 1, 1)) Xor Asc(Mid(sPassWord, (i Mod iPassWordLength + 1), 1))
        If bTmp = 0 Then
            Exit For
        End If
        tmpBinData = tmpBinData & Chr(bTmp)
    Next i
    
    Dim regData As String
    Dim posAnf  As Long, posEnd As Long
    
    regData = tmpBinData
    
    ' DATUM
    posAnf = 1
    posEnd = InStr(posAnf, regData, ".", vbTextCompare) 'regData.find('.',posAnf);
    
    posAnf = posEnd + 1
    posEnd = InStr(posAnf, regData, ".", vbTextCompare) 'regData.find('.',posAnf);
    
    posAnf = posEnd + 1
    posEnd = InStr(posAnf, regData, ";", vbTextCompare) 'regData.find(';',posAnf);
    
    ' VERSION
    posAnf = posEnd + 1
    posEnd = InStr(posAnf, regData, ".", vbTextCompare) 'regData.find('.',posAnf);
    
    posAnf = posEnd + 1
    posEnd = InStr(posAnf, regData, ".", vbTextCompare) 'regData.find('.',posAnf);
    
    posAnf = posEnd + 1
    posEnd = InStr(posAnf, regData, ".", vbTextCompare) 'regData.find('.',posAnf);

'   if (posEnd == std::string::npos)
'      posEnd = regData.find(':',posAnf);
    If (posEnd = 0) Then
        posEnd = InStr(posAnf, regData, ":", vbTextCompare) 'regdata.find(':',posAnf);
    End If
    
    ' TYP
    posAnf = posEnd + 1
    posEnd = InStr(posAnf, regData, ",", vbTextCompare) ' regdata.find(',',posAnf);
    
    ' EVALUATIONSPERIODE
    posAnf = posEnd + 1
    posEnd = InStr(posAnf, regData, "!", vbTextCompare) ' regData.find('!',posAnf);
'    if (posEnd == std::string::npos)
'       posEnd = std::string::npos;
    If (posEnd = 0) Then
        posEnd = 0
    End If
    
    ' SPRACHE
    If (posEnd <> 0) Then '(posEnd != std::string::npos)
        posAnf = posEnd + 1
        'posEnd = Len(regData)  'std::string::npos;
        sLanguage = Mid(regData, posAnf) ', (posEnd - posAnf)) ' regData.substr(posAnf,(posEnd-posAnf));
    Else
        sLanguage = "de"
    End If
    
    RegCloseKey (hKeyMachine)
    
    If (sLanguage = "vi") Then
        GetProgramLanguage = InterfaceLanguage.VIETNAM '2 'Language.VIETNAM
    ElseIf (sLanguage = "en") Then
        GetProgramLanguage = InterfaceLanguage.ENGLISH '1 'Language.ENGLISH
    Else
        GetProgramLanguage = InterfaceLanguage.GERMAN '0 'Language.GERMAN
    End If
    Exit Function

ErrQuit:
    If hKeyMachine <> 0 Then
        RegCloseKey (hKeyMachine)
    End If
    
    GetProgramLanguage = retOnError
    Exit Function

End Function


