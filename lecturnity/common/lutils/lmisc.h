#ifndef __LMISC_H__7C862EAE_21B3_436a_B75C_D10B113C0AA2
#define __LMISC_H__7C862EAE_21B3_436a_B75C_D10B113C0AA2

#ifndef __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B
#pragma message( "Do not include \"" __FILE__ "\", include \"lutils.h\"")
#endif __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B

namespace LMisc {

    // Returns TRUE if the current operating system is
    // Windows NT (or 2k, or XP, ...). FALSE if Win 9x/ME.
    BOOL IsWindowsNT();

    // Returns true if operating system is exactly Vista (and not xp and not server 2008).
    bool IsWindows7();
    bool IsWindowsVista();
    bool IsWindowsVistaOrHigher();

    bool IsWindowsXp();
    bool IsWindows2000();

    __int64 GetLocalTimeMs();

    /**
     * @returns true if the user/process currently has Administrator rights.
     */
    bool IsUserAdmin();

    /**
     * @returns true if the current user can become administrator (after an elevation prompt).
     */
    bool CanUserElevate();

    // Disabled as of 4.0.p7: doesn't work on Vista and Windows 7
    //bool IsKerberokWritable();

    bool LaunchProgram(const TCHAR *tszProgram, const TCHAR *tszParameter,
        const bool bRunAsVistaAdmin=false);

    bool WaitForProgramFinish(DWORD dwProcessId, DWORD *pdwExitCode);

    bool WaitForPublisherJavaFinish(LIo::MessagePipe *pPipe, 
        _TCHAR *tszMessageBuffer, int iBufferLength, DWORD *pdwExitCode);

    bool GetLecturnityHome(_TCHAR *tszLecturnityHome);
    bool GetApplicationDataDir(_TCHAR *tszApplicationDataDir);
    bool GetMyDocumentsDir(_TCHAR *tszApplicationDataDir);

    bool CheckNeedsFinalization(const _TCHAR *tszLrdPathName, _TCHAR *tszNewLrdPathNameTarget=NULL);

    bool GetPrivateBuild(_TCHAR *tszTarget);
    __int64 GetRandomValue();

    /////////////////////////////////////////////////////////
    // from twlc but also used by lsutl32 (SG extended)
    /////////////////////////////////////////////////////////
    int determineChangeArea(unsigned char* bm1, unsigned char* bm2, 
        int width, int height, int stride, RECT* result);
    void splitChangeArea(unsigned char* bm1, unsigned char* bm2, 
        int width, int height, int stride, RECT bounds, RECT *areas, int *count);
    int calcPadding(int width, int stride);

    // for performance optimization of structuring clips (SG extended)
    struct StructuringValues{
        int iFrameNr;
        int iPixelCount;
        int iAreaCount;
    };
}

#endif // __LMISC_H__7C862EAE_21B3_436a_B75C_D10B113C0AA2