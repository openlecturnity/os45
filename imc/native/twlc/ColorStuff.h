#ifndef __COLORSTUFF_H__
#define __COLORSTUFF_H__
#pragma warning(disable: 4786)

#include <windows.h>
#include <vector>
#include <map>


class BGR24
{
public:
   unsigned char b;
   unsigned char g;
   unsigned char r;
   BGR24(int color);
   BGR24(unsigned char *in);
   inline int getColor() { return (b << 16 | g << 8 | r); }
};

class RGB15
{
public:
   short rgb15;
   RGB15(int color);
   RGB15(unsigned char *in);
   inline int getColor() { return rgb15; }
};


class Palette
{
   static std::vector<int> somePrimes;

   int *colors;
   bool *isUsed;
   int *counts;
   int colorCount;
   int arrayLength;

public:
   Palette(int maxSize);
   ~Palette();
   void addPixel(RGB15 *color);
   void addPixel(BGR24 *color);
   void addPixel(int color);
   int countDifferentColors();
   int getRarestColor();

private:
   int indexOf(int color);
};

#endif
