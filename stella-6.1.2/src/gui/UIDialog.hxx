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

#ifndef UI_DIALOG_HXX
#define UI_DIALOG_HXX

class BrowserDialog;

class UIDialog : public Dialog, public CommandSender
{
  public:
    UIDialog(OSystem& osystem, DialogContainer& parent, const GUI::Font& font,
             GuiObject* boss, int max_w, int max_h);
    virtual ~UIDialog();

  private:
    void loadConfig() override;
    void saveConfig() override;
    void setDefaults() override;

    void handleCommand(CommandSender* sender, int cmd, int data, int id) override;
    //void handleLauncherSize();
    void handleRomViewer();
    void createBrowser(const string& title);

  private:
    enum
    {
      kListDelay  = 'UILd',
      kMouseWheel = 'UIMw',
      kControllerDelay = 'UIcd',
      kChooseRomDirCmd = 'LOrm', // rom select
      kRomViewer = 'UIRv',
      kChooseSnapLoadDirCmd = 'UIsl', // snapshot dir (load files)
      kSnapLoadDirChosenCmd = 'UIsc' // snap chosen (load files)
    };

    const GUI::Font& myFont;
    TabWidget* myTab{nullptr};

    // Launcher options
    EditTextWidget*   myRomPath{nullptr};
    SliderWidget*     myLauncherWidthSlider{nullptr};
    SliderWidget*     myLauncherHeightSlider{nullptr};
    PopUpWidget*      myLauncherFontPopup{nullptr};
    SliderWidget*     myRomViewerSize{nullptr};
    ButtonWidget*     myOpenBrowserButton{nullptr};
    EditTextWidget*   mySnapLoadPath{nullptr};
    CheckboxWidget*   myLauncherExitWidget{nullptr};

    // Misc options
    PopUpWidget*      myPalettePopup{nullptr};
    CheckboxWidget*   myHidpiWidget{nullptr};
    PopUpWidget*      myPositionPopup{nullptr};
    CheckboxWidget*   myConfirmExitWidget{nullptr};
    SliderWidget*     myListDelaySlider{nullptr};
    SliderWidget*     myWheelLinesSlider{nullptr};
    SliderWidget*     myControllerRateSlider{nullptr};
    SliderWidget*     myControllerDelaySlider{nullptr};
    SliderWidget*     myDoubleClickSlider{nullptr};

    unique_ptr<BrowserDialog> myBrowser;

    // Indicates if this dialog is used for global (vs. in-game) settings
    bool myIsGlobal{false};

  private:
    // Following constructors and assignment operators not supported
    UIDialog() = delete;
    UIDialog(const UIDialog&) = delete;
    UIDialog(UIDialog&&) = delete;
    UIDialog& operator=(const UIDialog&) = delete;
    UIDialog& operator=(UIDialog&&) = delete;
};

#endif
