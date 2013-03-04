#ifndef __LPROGESS_H_30EF287C_DD7D_4334_985C_368C0D360752
#define __LPROGESS_H_30EF287C_DD7D_4334_985C_368C0D360752

#ifndef __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B
#pragma message( "Do not include \"" __FILE__ "\", include \"lutils.h\"")
#endif __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B

/**
 * This is an interface to use for reporting progress.
 * Progress can have an arbitrary scaling or unit therefore
 * each call for achieved progress also has the maximum reachable value
 * included.
 * However usually the same maximum value over a couple of
 * method calls makes sense.
 */
class LProgressListener
{
public:
   /**
    * The class handling these reports should definitely make sure
    * that this method call is non-blocking and not taking long.
    */
   void virtual ReportProgress(UINT nAchieved, UINT nMaximum) = 0;
};



#endif // __LPROGESS_H_30EF287C_DD7D_4334_985C_368C0D360752