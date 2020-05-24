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

#include "FBSurfaceLIBRETRO.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FBSurfaceLIBRETRO::FBSurfaceLIBRETRO(FrameBufferLIBRETRO&,
                                     uInt32 width, uInt32 height, const uInt32* data)
{
  createSurface(width, height, data);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FBSurfaceLIBRETRO::createSurface(uInt32 width, uInt32 height,
                                      const uInt32* data)
{
  myWidth = width;
  myHeight = height;

  myPixelData = make_unique<uInt32[]>(width * height);

  ////////////////////////////////////////////////////
  // These *must* be set for the parent class
  myPixels = myPixelData.get();
  myPitch = width;
  ////////////////////////////////////////////////////
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool FBSurfaceLIBRETRO::render()
{
  return true;
}
