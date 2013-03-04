#include "ColorStuff.h"

BGR24::BGR24(int color)
{
   b = (unsigned char)(color >> 16);
   g = (unsigned char)(color >> 8);
   r = (unsigned char)(color);
}

BGR24::BGR24(unsigned char *in)
{
   b = *(in);
   g = *(in+1);
   r = *(in+2);
}


RGB15::RGB15(int color)
{
   rgb15 = (short)color;
}

RGB15::RGB15(unsigned char *in)
{
   rgb15 = *(in+1) | *(in) << 8;         
}



std::vector<int> Palette::somePrimes(26);

Palette::Palette(int maxSize)
{ 
   if (somePrimes.size() == 0)
   {
      somePrimes.push_back(23); somePrimes.push_back(37); 
      somePrimes.push_back(53); somePrimes.push_back(89);
      somePrimes.push_back(131); somePrimes.push_back(181);
      somePrimes.push_back(241); somePrimes.push_back(307);
      somePrimes.push_back(379); somePrimes.push_back(463);
      somePrimes.push_back(613); somePrimes.push_back(809);
      somePrimes.push_back(1163); somePrimes.push_back(1409);
      somePrimes.push_back(1867); somePrimes.push_back(2543);
      somePrimes.push_back(4079); somePrimes.push_back(6361);
      somePrimes.push_back(9007); somePrimes.push_back(13883);
      somePrimes.push_back(18301); somePrimes.push_back(25189);
      somePrimes.push_back(33937); somePrimes.push_back(44129);
      somePrimes.push_back(59753); somePrimes.push_back(74357);
   }

   unsigned int idx = 0;
   for (; idx<somePrimes.size()-1; ++idx)
   {
      if (somePrimes[idx] > maxSize)
         break;
   }

   // arrayLength will usually be a prime number
   // this is important for the latter hashing algorithms
   arrayLength = max(somePrimes[idx], (maxSize/2)*2+1); 
   colorCount = 0;

   colors = (int*)malloc(arrayLength*sizeof(int));
   isUsed = (bool*)malloc(arrayLength*sizeof(bool));
   counts = (int*)malloc(arrayLength*sizeof(int));

   memset(isUsed, 0, arrayLength*sizeof(bool)); // hopefully this means false
   memset(counts, 0, arrayLength*sizeof(int));
}

Palette::~Palette()
{ 
   free(colors);
   free(isUsed);
   free(counts);
}


void Palette::addPixel(RGB15 *color)
{ 
   addPixel(color->getColor());
}

void Palette::addPixel(BGR24 *color)
{ 
   addPixel(color->getColor());
}

void Palette::addPixel(int color)
{
   int idx = indexOf(color);
   
   if (!isUsed[idx])
   {
      colors[idx] = color;
      isUsed[idx] = true;
      
      colorCount++;
   }
   
   counts[idx]++;
   
}

int Palette::countDifferentColors()
{ 
   return colorCount;
}

int Palette::getRarestColor()
{ 
   int rarestColorIndex = 0;
   int itsCount = INT_MAX;
   
   for (int i=0; i<arrayLength; ++i)
   {
      if (isUsed[i] && counts[i] < itsCount) 
      {
         rarestColorIndex = i;
         itsCount = counts[i];
      }
   }
   
   return colors[rarestColorIndex];
}

//
// search for position of color
// or the (next) free entry if it doesn't exist
//
int Palette::indexOf(int color) 
{
   int index = abs(color % arrayLength);
   while(isUsed[index] && colors[index] != color)
   {
      // search (linear) for the requested position
      ++index;
      if (index == arrayLength) index = 0;
   }
			
   return index;
}
