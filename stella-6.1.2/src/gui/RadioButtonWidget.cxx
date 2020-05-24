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

#include "FBSurface.hxx"
#include "Font.hxx"
#include "Dialog.hxx"
#include "RadioButtonWidget.hxx"

/*  Radiobutton bitmaps */
static constexpr std::array<uInt32, 14> radio_img_outercircle = {
  0b00001111110000,  0b00110000001100,  0b01000000000010,
  0b01000000000010,  0b10000000000001,  0b10000000000001,
  0b10000000000001,  0b10000000000001,  0b10000000000001,
  0b10000000000001,  0b01000000000010,  0b01000000000010,
  0b00110000001100,  0b00001111110000
};
static constexpr std::array<uInt32, 12> radio_img_innercircle = {
  0b000111111000,  0b011111111110,  0b011111111110,  0b111111111111,
  0b111111111111,  0b111111111111,  0b111111111111,  0b111111111111,
  0b111111111111,  0b011111111110,  0b011111111110,  0b000111111000
};
static constexpr uInt32 RADIO_IMG_FILL_SIZE = 10;
static constexpr std::array<uInt32, RADIO_IMG_FILL_SIZE> radio_img_active = {
  0b0011111100,  0b0111111110,  0b1111111111,  0b1111111111,  0b1111111111,
  0b1111111111,  0b1111111111,  0b1111111111,  0b0111111110,  0b0011111100,
};
static constexpr std::array<uInt32, RADIO_IMG_FILL_SIZE> radio_img_inactive = {
  0b0011111100,  0b0111111110,  0b1111001111,  0b1110000111,  0b1100000011,
  0b1100000011,  0b1110000111,  0b1111001111,  0b0111111110,  0b0011111100
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RadioButtonWidget::RadioButtonWidget(GuiObject* boss, const GUI::Font& font,
                                     int x, int y, const string& label,
                                     RadioButtonGroup* group, int cmd)
  : CheckboxWidget(boss, font, x, y, label, cmd),
    myGroup(group)
{
  _flags = Widget::FLAG_ENABLED;
  _bgcolor = _bgcolorhi = kWidColor;

  _editable = true;

  if(label == "")
    _w = 14;
  else
    _w = font.getStringWidth(label) + 20;
  _h = font.getFontHeight() < 14 ? 14 : font.getFontHeight();

  // Depending on font size, either the font or box will need to be
  // centered vertically
  if(_h > 14)  // center box
    _boxY = (_h - 14) / 2;
  else         // center text
    _textY = (14 - _font.getFontHeight()) / 2;

  setFill(CheckboxWidget::FillType::Normal);
  myGroup->addWidget(this);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RadioButtonWidget::handleMouseUp(int x, int y, MouseButton b, int clickCount)
{
  if(isEnabled() && _editable && x >= 0 && x < _w && y >= 0 && y < _h)
  {
    if(!_state)
      setState(true);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RadioButtonWidget::setState(bool state, bool send)
{
  if(_state != state)
  {
    _state = state;
    setDirty();
    if(_state && send)
      sendCommand(_cmd, _state, _id);
    if (state)
      myGroup->select(this);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RadioButtonWidget::setFill(FillType type)
{
  switch(type)
  {
    case CheckboxWidget::FillType::Normal:
      _img = radio_img_active.data();
      break;
    case CheckboxWidget::FillType::Inactive:
      _img = radio_img_inactive.data();
      break;
    default:
      break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RadioButtonWidget::drawWidget(bool hilite)
{
  FBSurface& s = _boss->dialog().surface();

  // Draw the outer bounding circle
  s.drawBitmap(radio_img_outercircle.data(), _x, _y + _boxY,
               hilite ? kWidColorHi : kColor,
               static_cast<uInt32>(radio_img_outercircle.size()),
               static_cast<uInt32>(radio_img_outercircle.size()));

  // Draw the inner bounding circle with enabled color
  s.drawBitmap(radio_img_innercircle.data(), _x + 1, _y + _boxY + 1,
               isEnabled() ? _bgcolor : kColor,
               static_cast<uInt32>(radio_img_innercircle.size()),
               static_cast<uInt32>(radio_img_innercircle.size()));

  // draw state
  if(_state)
    s.drawBitmap(_img, _x + 2, _y + _boxY + 2, isEnabled()
                 ? hilite ? kWidColorHi : kCheckColor
                 : kColor, RADIO_IMG_FILL_SIZE);

  // Finally draw the label
  s.drawString(_font, _label, _x + 20, _y + _textY, _w,
               isEnabled() ? kTextColor : kColor);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RadioButtonGroup::addWidget(RadioButtonWidget* widget)
{
  myWidgets.push_back(widget);
  // set first button as default
  widget->setState(myWidgets.size() == 1, false);
  mySelected = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RadioButtonGroup::select(RadioButtonWidget* widget)
{
  uInt32 i = 0;

  for(const auto& w : myWidgets)
  {
    if(w == widget)
    {
      setSelected(i);
      break;
    }
    ++i;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RadioButtonGroup::setSelected(uInt32 selected)
{
  uInt32 i = 0;

  mySelected = selected;
  for(const auto& w : myWidgets)
  {
    (static_cast<RadioButtonWidget*>(w))->setState(i == mySelected);
    ++i;
  }
}
