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

#ifndef OSYSTEM_WINDOWS_HXX
#define OSYSTEM_WINDOWS_HXX

#include "OSystem.hxx"

/**
  This class defines an OSystem object for Windows OS's.
  It is responsible for completely implementing getBaseDirAndConfig(),
  to set the base directory, config file location, and various other
  save/load locations.

  @author  Stephen Anthony
*/
class OSystemWINDOWS : public OSystem
{
  public:
    OSystemWINDOWS() = default;
    virtual ~OSystemWINDOWS() = default;

    /**
      Determine the base directory and main configuration file from the
      derived class.  It can also use hints, as described below.

      @param basedir  The base directory for all configuration files
      @param cfgfile  The fully qualified pathname of the config file
                      (including the base directory)
      @param savedir  The default directory to save various other files
      @param loaddir  The default directory to load various other files
      @param useappdir  A hint that the base dir should be set to the
                        app directory; not all ports can do this, so
                        they are free to ignore it
      @param usedir     A hint that the base dir should be set to this
                        parameter; not all ports can do this, so
                        they are free to ignore it
    */
    void getBaseDirAndConfig(string& basedir, string& cfgfile,
              string& savedir, string& loaddir,
              bool useappdir, const string& usedir) override;

  private:
    // Following constructors and assignment operators not supported
    OSystemWINDOWS(const OSystemWINDOWS&) = delete;
    OSystemWINDOWS(OSystemWINDOWS&&) = delete;
    OSystemWINDOWS& operator=(const OSystemWINDOWS&) = delete;
    OSystemWINDOWS& operator=(OSystemWINDOWS&&) = delete;
};

#endif
