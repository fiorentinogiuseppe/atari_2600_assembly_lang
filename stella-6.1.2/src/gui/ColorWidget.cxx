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

#include "bspf.hxx"
#include "Command.hxx"
#include "Dialog.hxx"
#include "FBSurface.hxx"
#include "GuiObject.hxx"
#include "OSystem.hxx"
#include "ColorWidget.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ColorWidget::ColorWidget(GuiObject* boss, const GUI::Font& font,
                         int x, int y, int w, int h, int cmd)
  : Widget(boss, font, x, y, w, h),
    CommandSender(boss),
    _cmd(cmd)
{
  _flags = Widget::FLAG_ENABLED | Widget::FLAG_CLEARBG | Widget::FLAG_RETAIN_FOCUS;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ColorWidget::setColor(ColorId color)
{
  _color = color;
  setDirty();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ColorWidget::drawWidget(bool hilite)
{
  FBSurface& s = dialog().surface();
  bool onTop = _boss->dialog().isOnTop();

  // Draw a thin frame around us.
  s.frameRect(_x, _y, _w, _h + 1, kColor);

  // Show the currently selected color
  s.fillRect(_x+1, _y+1, _w-2, _h-1, onTop ? isEnabled() ? _color : kWidColor : kBGColorLo);

  // Cross out the grid?
  if(_crossGrid)
  {
    s.line(_x + 1, _y + 1, _x + _w - 2, _y + _h - 1, kColor);
    s.line(_x + _w - 2, _y + 1, _x + 1, _y + _h - 1, kColor);
  }
}
