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

#include "FSNode.hxx"
#include "OSystemLIBRETRO.hxx"

#ifdef _WIN32
  const string slash = "\\";
#else
  const string slash = "/";
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystemLIBRETRO::getBaseDirAndConfig(string& basedir, string& cfgfile,
        string& savedir, string& loaddir,
        bool useappdir, const string& usedir)
{
  loaddir = savedir = cfgfile = basedir = "." + slash;
}
