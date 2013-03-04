#include "StdAfx.h"

#include "objects.h"

DrawSdk::ZoomManager::ZoomManager(int sx, int sy)
{
   dimX_ = sx;
   dimY_ = sy;

   useZoomFactor_ = false;
}

DrawSdk::ZoomManager::ZoomManager(double zoomFactor)
{
   zoomFactor_ = zoomFactor;

   useZoomFactor_ = true;
}

DrawSdk::ZoomManager::~ZoomManager()
{
}

int DrawSdk::ZoomManager::GetScreenX(double x)
{
   if (useZoomFactor_)
      return (int)(x * zoomFactor_);
   else
      return (int)(x * dimX_);
}

int DrawSdk::ZoomManager::GetScreenY(double y)
{
   if (useZoomFactor_)
      return (int)(y * zoomFactor_);
   else
      return (int)(y * dimY_);
}

int DrawSdk::ZoomManager::GetRelativeX(int x)
{
   return x;
}

int DrawSdk::ZoomManager::GetRelativeY(int y)
{
   return y;
}


double DrawSdk::ZoomManager::GetZoomFactor()
{
   return zoomFactor_;
}