//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2020 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#include "FBSurfaceSDL2.hxx"

#include "Logger.hxx"
#include "ThreadDebugging.hxx"
#include "sdl_blitter/BlitterFactory.hxx"

namespace {
  BlitterFactory::ScalingAlgorithm scalingAlgorithm(FrameBuffer::ScalingInterpolation interpolation)
  {
    switch (interpolation) {
      case FrameBuffer::ScalingInterpolation::none:
        return BlitterFactory::ScalingAlgorithm::nearestNeighbour;

      case FrameBuffer::ScalingInterpolation::blur:
        return BlitterFactory::ScalingAlgorithm::bilinear;

      case FrameBuffer::ScalingInterpolation::sharp:
        return BlitterFactory::ScalingAlgorithm::quasiInteger;

      default:
        throw runtime_error("unreachable");
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FBSurfaceSDL2::FBSurfaceSDL2(FrameBufferSDL2& buffer,
                             uInt32 width, uInt32 height,
                             FrameBuffer::ScalingInterpolation interpolation,
                             const uInt32* staticData)
  : myFB(buffer),
    myInterpolationMode(interpolation)
{
  createSurface(width, height, staticData);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FBSurfaceSDL2::~FBSurfaceSDL2()
{
  ASSERT_MAIN_THREAD;

  if(mySurface)
  {
    SDL_FreeSurface(mySurface);
    mySurface = nullptr;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceSDL2::fillRect(uInt32 x, uInt32 y, uInt32 w, uInt32 h, ColorId color)
{
  ASSERT_MAIN_THREAD;

  // Fill the rectangle
  SDL_Rect tmp;
  tmp.x = x;
  tmp.y = y;
  tmp.w = w;
  tmp.h = h;
  SDL_FillRect(mySurface, &tmp, myPalette[color]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt32 FBSurfaceSDL2::width() const
{
  return mySurface->w;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt32 FBSurfaceSDL2::height() const
{
  return mySurface->h;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const Common::Rect& FBSurfaceSDL2::srcRect() const
{
  return mySrcGUIR;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const Common::Rect& FBSurfaceSDL2::dstRect() const
{
  return myDstGUIR;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceSDL2::setSrcPos(uInt32 x, uInt32 y)
{
  if(x != static_cast<uInt32>(mySrcR.x) || y != static_cast<uInt32>(mySrcR.y))
  {
    setSrcPosInternal(x, y);
    reinitializeBlitter();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceSDL2::setSrcSize(uInt32 w, uInt32 h)
{
  if(w != static_cast<uInt32>(mySrcR.w) || h != static_cast<uInt32>(mySrcR.h))
  {
    setSrcSizeInternal(w, h);
    reinitializeBlitter();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceSDL2::setDstPos(uInt32 x, uInt32 y)
{
  if(x != static_cast<uInt32>(myDstR.x) || y != static_cast<uInt32>(myDstR.y))
  {
    setDstPosInternal(x, y);
    reinitializeBlitter();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceSDL2::setDstSize(uInt32 w, uInt32 h)
{
  if(w != static_cast<uInt32>(myDstR.w) || h != static_cast<uInt32>(myDstR.h))
  {
    setDstSizeInternal(w, h);
    reinitializeBlitter();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceSDL2::setVisible(bool visible)
{
  myIsVisible = visible;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceSDL2::translateCoords(Int32& x, Int32& y) const
{
  x -= myDstR.x;  x /= myDstR.w / mySrcR.w;
  y -= myDstR.y;  y /= myDstR.h / mySrcR.h;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool FBSurfaceSDL2::render()
{
  if (!myBlitter) reinitializeBlitter();

  if(myIsVisible && myBlitter)
  {
    myBlitter->blit(*mySurface);

    return true;
  }
  return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceSDL2::invalidate()
{
  ASSERT_MAIN_THREAD;

  SDL_FillRect(mySurface, nullptr, 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceSDL2::free()
{
  myBlitter.reset();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceSDL2::reload()
{
  free();
  reinitializeBlitter();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceSDL2::resize(uInt32 width, uInt32 height)
{
  ASSERT_MAIN_THREAD;

  if(mySurface)
    SDL_FreeSurface(mySurface);

  free();

  // NOTE: Currently, a resize changes a 'static' surface to 'streaming'
  //       No code currently does this, but we should at least check for it
  if(myIsStatic)
    Logger::error("Resizing static texture!");

  createSurface(width, height, nullptr);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceSDL2::createSurface(uInt32 width, uInt32 height,
                                  const uInt32* data)
{
  ASSERT_MAIN_THREAD;

  // Create a surface in the same format as the parent GL class
  const SDL_PixelFormat& pf = myFB.pixelFormat();

  mySurface = SDL_CreateRGBSurface(0, width, height,
      pf.BitsPerPixel, pf.Rmask, pf.Gmask, pf.Bmask, pf.Amask);

  // We start out with the src and dst rectangles containing the same
  // dimensions, indicating no scaling or re-positioning
  setSrcPosInternal(0, 0);
  setDstPosInternal(0, 0);
  setSrcSizeInternal(width, height);
  setDstSizeInternal(width, height);

  ////////////////////////////////////////////////////
  // These *must* be set for the parent class
  myPixels = reinterpret_cast<uInt32*>(mySurface->pixels);
  myPitch = mySurface->pitch / pf.BytesPerPixel;
  ////////////////////////////////////////////////////

  myIsStatic = data != nullptr;
  if(myIsStatic)
    SDL_memcpy(mySurface->pixels, data, mySurface->w * mySurface->h * 4);

  reinitializeBlitter();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceSDL2::reinitializeBlitter()
{
  if (!myBlitter && myFB.isInitialized())
    myBlitter = BlitterFactory::createBlitter(myFB, scalingAlgorithm(myInterpolationMode));

  if (myBlitter)
    myBlitter->reinitialize(mySrcR, myDstR, myAttributes, myIsStatic ? mySurface : nullptr);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceSDL2::applyAttributes()
{
  reinitializeBlitter();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceSDL2::setScalingInterpolation(FrameBuffer::ScalingInterpolation interpolation)
{
  if (interpolation == myInterpolationMode) return;

  myInterpolationMode = interpolation;
  reload();
}
