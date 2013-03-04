#ifndef __LEXPRESSIONPARSERS_H__1789D1EE_82E1_490b_9BE7_C661192B5986
#define __LEXPRESSIONPARSERS_H__1789D1EE_82E1_490b_9BE7_C661192B5986

#ifndef __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B
#pragma message( "Do not include \"" __FILE__ "\", include \"lutils.h\"")
#endif __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B

class LExpressionParser
{
public:
   LExpressionParser(_TCHAR *tszExp);
   ~LExpressionParser();

   bool EvaluateInt(int *pResult);

private:
   _TCHAR *m_tszExpression;
};

#endif // __LEXPRESSIONPARSERS_H__1789D1EE_82E1_490b_9BE7_C661192B5986