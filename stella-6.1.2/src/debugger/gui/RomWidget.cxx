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

#include "Settings.hxx"
#include "Debugger.hxx"
#include "CartDebug.hxx"
#include "DiStella.hxx"
#include "CpuDebug.hxx"
#include "GuiObject.hxx"
#include "Font.hxx"
#include "DataGridWidget.hxx"
#include "EditTextWidget.hxx"
#include "PopUpWidget.hxx"
#include "ContextMenu.hxx"
#include "RomListWidget.hxx"
#include "RomWidget.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RomWidget::RomWidget(GuiObject* boss, const GUI::Font& lfont, const GUI::Font& nfont,
                     int x, int y, int w, int h)
  : Widget(boss, lfont, x, y, w, h),
    CommandSender(boss)
{
  int xpos, ypos;
  StaticTextWidget* t;
  WidgetArray wid;

  // Show current bank state
  xpos = x;  ypos = y + 7;
  t = new StaticTextWidget(boss, lfont, xpos, ypos,
                           lfont.getStringWidth("Bank"),
                           lfont.getFontHeight(),
                           "Bank", TextAlign::Left);

  xpos += t->getWidth() + 5;
  myBank = new EditTextWidget(boss, nfont, xpos, ypos-2,
                              _w - 2 - xpos, nfont.getLineHeight());
  myBank->setEditable(false);

  // Create rom listing
  xpos = x;  ypos += myBank->getHeight() + 4;

  myRomList = new RomListWidget(boss, lfont, nfont, xpos, ypos, _w - 4, _h - ypos - 2);
  myRomList->setTarget(this);
  addFocusWidget(myRomList);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::loadConfig()
{
  Debugger& dbg = instance().debugger();
  CartDebug& cart = dbg.cartDebug();
  const CartState& state = static_cast<const CartState&>(cart.getState());
  const CartState& oldstate = static_cast<const CartState&>(cart.getOldState());

  // Fill romlist the current bank of source or disassembly
  myListIsDirty |= cart.disassemble(myListIsDirty);
  if(myListIsDirty)
  {
    myRomList->setList(cart.disassembly());
    myListIsDirty = false;
  }

  // Update romlist to point to current PC (if it has changed)
  int pcline = cart.addressToLine(dbg.cpuDebug().pc());
  if(pcline >= 0 && pcline != myRomList->getHighlighted())
    myRomList->setHighlighted(pcline);

  // Set current bank state
  myBank->setText(state.bank, state.bank != oldstate.bank);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::handleCommand(CommandSender* sender, int cmd, int data, int id)
{
  switch(cmd)
  {
    case RomListWidget::kBPointChangedCmd:
      // 'data' is the line in the disassemblylist to be accessed
      toggleBreak(data);
      // Refresh the romlist, since the breakpoint may not have
      // actually changed
      myRomList->setDirty();
      myRomList->draw();
      break;

    case RomListWidget::kRomChangedCmd:
      // 'data' is the line in the disassemblylist to be accessed
      // 'id' is the base to use for the data to be changed
      patchROM(data, myRomList->getText(), Common::Base::Fmt(id));
      break;

    case RomListWidget::kSetPCCmd:
      // 'data' is the line in the disassemblylist to be accessed
      setPC(data);
      break;

    case RomListWidget::kRuntoPCCmd:
      // 'data' is the line in the disassemblylist to be accessed
      runtoPC(data);
      break;

    case RomListWidget::kDisassembleCmd:
      invalidate();
      break;

    case RomListWidget::kTentativeCodeCmd:
    {
      // 'data' is the boolean value
      DiStella::settings.resolveCode = data;
      instance().settings().setValue("dis.resolve",
          DiStella::settings.resolveCode);
      invalidate();
      break;
    }

    case RomListWidget::kPCAddressesCmd:
      // 'data' is the boolean value
      DiStella::settings.showAddresses = data;
      instance().settings().setValue("dis.showaddr",
          DiStella::settings.showAddresses);
      invalidate();
      break;

    case RomListWidget::kGfxAsBinaryCmd:
      // 'data' is the boolean value
      if(data)
      {
        DiStella::settings.gfxFormat = Common::Base::Fmt::_2;
        instance().settings().setValue("dis.gfxformat", "2");
      }
      else
      {
        DiStella::settings.gfxFormat = Common::Base::Fmt::_16;
        instance().settings().setValue("dis.gfxformat", "16");
      }
      invalidate();
      break;

    case RomListWidget::kAddrRelocationCmd:
      // 'data' is the boolean value
      DiStella::settings.rFlag = data;
      instance().settings().setValue("dis.relocate",
          DiStella::settings.rFlag);
      invalidate();
      break;

    default:
      break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::toggleBreak(int disasm_line)
{
  const CartDebug::DisassemblyList& list =
      instance().debugger().cartDebug().disassembly().list;
  if(disasm_line >= int(list.size()))  return;

  if(list[disasm_line].address != 0 && list[disasm_line].bytes != "")
    instance().debugger().toggleBreakPoint(list[disasm_line].address,
                                           instance().debugger().cartDebug().getBank(list[disasm_line].address));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::setPC(int disasm_line)
{
  const CartDebug::DisassemblyList& list =
      instance().debugger().cartDebug().disassembly().list;
  if(disasm_line >= int(list.size()))  return;

  if(list[disasm_line].address != 0)
  {
    ostringstream command;
    command << "pc #" << list[disasm_line].address;
    instance().debugger().run(command.str());
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::runtoPC(int disasm_line)
{
  const CartDebug::DisassemblyList& list =
      instance().debugger().cartDebug().disassembly().list;
  if(disasm_line >= int(list.size()))  return;

  if(list[disasm_line].address != 0)
  {
    ostringstream command;
    command << "runtopc #" << list[disasm_line].address;
    instance().debugger().run(command.str());
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::patchROM(int disasm_line, const string& bytes,
                         Common::Base::Fmt base)
{
  const CartDebug::DisassemblyList& list =
      instance().debugger().cartDebug().disassembly().list;
  if(disasm_line >= int(list.size()))  return;

  if(list[disasm_line].address != 0)
  {
    ostringstream command;

    // Temporarily set to correct base, so we don't have to prefix each byte
    // with the type of data
    Common::Base::Fmt oldbase = Common::Base::format();

    Common::Base::setFormat(base);
    command << "rom #" << list[disasm_line].address << " " << bytes;
    instance().debugger().run(command.str());

    // Restore previous base
    Common::Base::setFormat(oldbase);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::scrollTo(int line)
{
  myRomList->setSelected(line);
}
