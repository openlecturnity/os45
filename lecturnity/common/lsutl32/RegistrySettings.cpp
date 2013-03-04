#include "stdafx.h"
#include "RegistrySettings.h"
#include "lutils.h"         // lutils

CRegistrySettings::CRegistrySettings(void)
{
}

CRegistrySettings::~CRegistrySettings(void)
{
}

void CRegistrySettings::GetLastSelectionRectangle(CRect &rcLastSelection) {
    unsigned long lLength = MAX_PATH;
    _TCHAR szLastSelectionRectangle[MAX_PATH];
    bool bRet = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Assistant\\SgSettings\\"), 
        _T("LastSelectionRectangleCustom"), szLastSelectionRectangle, &lLength);

    CString csLastSelectionRectangle = szLastSelectionRectangle;
    rcLastSelection.SetRectEmpty();
    if (bRet && !csLastSelectionRectangle.IsEmpty()) {
        int iPos = csLastSelectionRectangle.Find(_T(";"));
        CString csValue;
        bool bError = false;
        if (iPos > 0) {
            csValue = csLastSelectionRectangle.Left(iPos);
            rcLastSelection.left = _ttoi(csValue);
        }
        else
            bError = true;

        if (!bError) {
            int iNewLength = csLastSelectionRectangle.GetLength() - (iPos+1);
            csLastSelectionRectangle = csLastSelectionRectangle.Right(iNewLength);
            iPos = csLastSelectionRectangle.Find(_T(";"));
            if (iPos > 0) {
                csValue = csLastSelectionRectangle.Left(iPos);
                rcLastSelection.top = _ttoi(csValue);
            }
            else
                bError = true;
        }

        if (!bError) {
            int iNewLength = csLastSelectionRectangle.GetLength() - (iPos+1);
            csLastSelectionRectangle = csLastSelectionRectangle.Right(iNewLength);
            iPos = csLastSelectionRectangle.Find(_T(";"));
            if (iPos > 0) {
                csValue = csLastSelectionRectangle.Left(iPos);
                rcLastSelection.right = _ttoi(csValue);
            }
            else
                bError = true;
        }

        if (!bError) {
            int iNewLength = csLastSelectionRectangle.GetLength() - (iPos+1);
            csLastSelectionRectangle = csLastSelectionRectangle.Right(iNewLength);
            csValue = csLastSelectionRectangle;
            rcLastSelection.bottom = _ttoi(csValue);
        }
    }
}

void CRegistrySettings::SetLastSelectionRectangle(CRect &rcLastSelection) {
    CString csLastSelectionRectangle;
    csLastSelectionRectangle.Format(_T("%d;%d;%d;%d"), 
                                    rcLastSelection.left,
                                    rcLastSelection.top,
                                    rcLastSelection.right,
                                    rcLastSelection.bottom);

    LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Assistant\\SgSettings\\"), 
        _T("LastSelectionRectangleCustom"), csLastSelectionRectangle);
}

bool CRegistrySettings::GetRecordFullScreen() {
    bool bRecordFullScreen = false;

    bool bRet = LRegistry::ReadBoolRegistryEntry(HKEY_CURRENT_USER, 
                                                 _T("Software\\imc AG\\LECTURNITY\\Assistant\\SgSettings\\"), 
                                                 _T("DoRecordFullScreen"), &bRecordFullScreen);
    return bRecordFullScreen;
}

void CRegistrySettings::SetRecordFullScreen(bool bRecordFullScreen) {
    LRegistry::WriteBoolRegistryEntry(HKEY_CURRENT_USER,
                                      _T("Software\\imc AG\\LECTURNITY\\Assistant\\SgSettings\\"), 
                                      _T("DoRecordFullScreen"), bRecordFullScreen);
}