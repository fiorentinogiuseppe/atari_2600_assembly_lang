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

#ifndef CARTRIDGE3F_WIDGET_HXX
#define CARTRIDGE3F_WIDGET_HXX

class Cartridge3F;
class PopUpWidget;

#include "CartDebugWidget.hxx"

class Cartridge3FWidget : public CartDebugWidget
{
  public:
    Cartridge3FWidget(GuiObject* boss, const GUI::Font& lfont,
                      const GUI::Font& nfont,
                      int x, int y, int w, int h,
                      Cartridge3F& cart);
    virtual ~Cartridge3FWidget() = default;

  private:
    Cartridge3F& myCart;
    PopUpWidget* myBank{nullptr};

    enum { kBankChanged = 'bkCH' };

  private:
    void loadConfig() override;
    void handleCommand(CommandSender* sender, int cmd, int data, int id) override;

    string bankState() override;

    // Following constructors and assignment operators not supported
    Cartridge3FWidget() = delete;
    Cartridge3FWidget(const Cartridge3FWidget&) = delete;
    Cartridge3FWidget(Cartridge3FWidget&&) = delete;
    Cartridge3FWidget& operator=(const Cartridge3FWidget&) = delete;
    Cartridge3FWidget& operator=(Cartridge3FWidget&&) = delete;
};

#endif
