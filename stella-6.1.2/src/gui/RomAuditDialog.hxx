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

#ifndef ROM_AUDIT_DIALOG_HXX
#define ROM_AUDIT_DIALOG_HXX

class OSystem;
class GuiObject;
class DialogContainer;
class EditTextWidget;
class StaticTextWidget;
class BrowserDialog;
namespace GUI {
  class MessageBox;
}

#include "Dialog.hxx"
#include "Command.hxx"
#include "FSNode.hxx"

class RomAuditDialog : public Dialog
{
  public:
    RomAuditDialog(OSystem& osystem, DialogContainer& parent,
                   const GUI::Font& font, int max_w, int max_h);
    virtual ~RomAuditDialog();

  private:
    void loadConfig() override;
    void auditRoms();
    void createBrowser(const string& title);
    void handleCommand(CommandSender* sender, int cmd, int data, int id) override;

  private:
    enum {
      kChooseAuditDirCmd = 'RAsl', // audit dir select
      kAuditDirChosenCmd = 'RAch', // audit dir changed
      kConfirmAuditCmd   = 'RAcf'  // confirm rom audit
    };

    // Select a new ROM audit path
    unique_ptr<BrowserDialog> myBrowser;
    const GUI::Font& myFont;

    // ROM audit path
    EditTextWidget* myRomPath{nullptr};

    // Show the results of the ROM audit
    EditTextWidget* myResults1{nullptr};
    EditTextWidget* myResults2{nullptr};

    // Show a message about the dangers of using this function
    unique_ptr<GUI::MessageBox> myConfirmMsg;

    // Maximum width and height for this dialog
    int myMaxWidth{0}, myMaxHeight{0};

  private:
    // Following constructors and assignment operators not supported
    RomAuditDialog() = delete;
    RomAuditDialog(const RomAuditDialog&) = delete;
    RomAuditDialog(RomAuditDialog&&) = delete;
    RomAuditDialog& operator=(const RomAuditDialog&) = delete;
    RomAuditDialog& operator=(RomAuditDialog&&) = delete;
};

#endif
