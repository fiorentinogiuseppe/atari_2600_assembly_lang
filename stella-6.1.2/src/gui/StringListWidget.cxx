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
#include "Dialog.hxx"
#include "FBSurface.hxx"
#include "Settings.hxx"
#include "ScrollBarWidget.hxx"
#include "StringListWidget.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StringListWidget::StringListWidget(GuiObject* boss, const GUI::Font& font,
                                   int x, int y, int w, int h, bool hilite)
  : ListWidget(boss, font, x, y, w, h),
    _hilite(hilite)
{
  _bgcolorlo = kDlgColor;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StringListWidget::setList(const StringList& list)
{
  _list = list;

  ListWidget::recalc();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StringListWidget::handleMouseEntered()
{
  setFlags(Widget::FLAG_HILITED);
  setDirty();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StringListWidget::handleMouseLeft()
{
  clearFlags(Widget::FLAG_HILITED);
  setDirty();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StringListWidget::drawWidget(bool hilite)
{
  FBSurface& s = _boss->dialog().surface();
  bool onTop = _boss->dialog().isOnTop();
  int i, pos, len = int(_list.size());

  // Draw a thin frame around the list.
  s.frameRect(_x, _y, _w + 1, _h, onTop && hilite && _hilite ? kWidColorHi : kColor);

  if (!isEnabled())
    s.fillRect(_x + 1, _y + 1, _w - 1, _h - 2, onTop ? kDlgColor : kBGColorLo);

  // Draw the list items
  for (i = 0, pos = _currentPos; i < _rows && pos < len; i++, pos++)
  {
    const int y = _y + 2 + _fontHeight * i;
    ColorId textColor = onTop ? kTextColor : kShadowColor;

    // Draw the selected item inverted, on a highlighted background.
    if (onTop && _selectedItem == pos && _hilite)
    {
      if(_hasFocus && !_editMode)
      {
        s.fillRect(_x + 1, _y + 1 + _fontHeight * i, _w - 1, _fontHeight, kTextColorHi);
        textColor = kTextColorInv;
      }
      else
        s.frameRect(_x + 1, _y + 1 + _fontHeight * i, _w - 1, _fontHeight, kWidColorHi);
    }

    Common::Rect r(getEditRect());
    if (_selectedItem == pos && _editMode)
    {
      adjustOffset();

      s.drawString(_font, editString(), _x + r.x(), y, r.w(), textColor,
                   TextAlign::Left, -_editScrollOffset, false);
    }
    else
      s.drawString(_font, _list[pos], _x + r.x(), y, r.w(), textColor);
  }

  // Only draw the caret while editing, and if it's in the current viewport
  if(_editMode && (_selectedItem >= _scrollBar->_currentPos) &&
    (_selectedItem < _scrollBar->_currentPos + _rows))
    drawCaret();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Common::Rect StringListWidget::getEditRect() const
{
  const int offset = std::max(0, (_selectedItem - _currentPos) * _fontHeight);
  return Common::Rect(2, 1 + offset, _w - 2, _fontHeight + offset);
}
