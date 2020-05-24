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
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#ifndef DIALOG_HXX
#define DIALOG_HXX

class FBSurface;
class OSystem;
class DialogContainer;
class TabWidget;
class CommandSender;

#include "Stack.hxx"
#include "Widget.hxx"
#include "GuiObject.hxx"
#include "StellaKeys.hxx"
#include "EventHandlerConstants.hxx"
#include "bspf.hxx"

/**
  This is the base class for all dialog boxes.

  @author  Stephen Anthony
*/
class Dialog : public GuiObject
{
  friend class DialogContainer;

  public:
    Dialog(OSystem& instance, DialogContainer& parent,
           int x = 0, int y = 0, int w = 0, int h = 0);
    Dialog(OSystem& instance, DialogContainer& parent, const GUI::Font& font,
           const string& title = "", int x = 0, int y = 0, int w = 0, int h = 0);

    virtual ~Dialog();

    void open();
    void close();

    bool isVisible() const override { return _visible; }
    bool isOnTop() const { return _onTop;  }

    virtual void center();
    virtual void drawDialog();
    virtual void loadConfig()  { }
    virtual void saveConfig()  { }
    virtual void setDefaults() { }

    // A dialog being dirty indicates that its underlying surface needs to be
    // redrawn and then re-rendered; this is taken care of in ::render()
    void setDirty() override { _dirty = true; }
    bool isDirty() const { return _dirty; }
    bool render();

    void addFocusWidget(Widget* w) override;
    void addToFocusList(WidgetArray& list) override;
    void addToFocusList(WidgetArray& list, TabWidget* w, int tabId);
    void addBGroupToFocusList(WidgetArray& list) { _buttonGroup = list; }
    void addTabWidget(TabWidget* w);
    void addDefaultWidget(Widget* w) { _defaultWidget = w; }
    void addOKWidget(Widget* w)     { _okWidget = w;     }
    void addCancelWidget(Widget* w) { _cancelWidget = w; }
    void setFocus(Widget* w);

    /** Returns the base surface associated with this dialog. */
    FBSurface& surface() const { return *_surface; }

    /**
      Adds a surface to this dialog, which is rendered on top of the
      base surface whenever the base surface is re-rendered.  Since
      the surface render() call will always occur in such a case, the
      surface should call setVisible() to enable/disable its output.
    */
    void addSurface(const shared_ptr<FBSurface>& surface);

    void setFlags(int flags) { _flags |= flags;  setDirty(); }
    void clearFlags(int flags) { _flags &= ~flags; setDirty(); }
    int  getFlags() const { return _flags; }

    void setTitle(const string& title);
    bool hasTitle() { return !_title.empty(); }

    /**
      Determine the maximum width/height of a dialog based on the minimum
      allowable bounds, also taking into account the current window size.
      Currently scales the width/height to 95% of allowable area when possible.

      NOTE: This method is meant to be used for dynamic, resizeable dialogs.
            That is, those that can change size during a program run, and
            *have* to take the current window size into account.

      @param w  The resulting width to use for the dialog
      @param h  The resulting height to use for the dialog

      @return  True if the dialog fits in the current window (scaled to 90%)
               False if the dialog is smaller than the current window, and
               has to be scaled down
    */
    bool getDynamicBounds(uInt32& w, uInt32& h) const;

    /**
      Checks if the dialogs fits into the actual sizes.

      @param w  The resulting width to use for the dialog
      @param h  The resulting height to use for the dialog

      @return  True if the dialog should be resized
    */
    bool shouldResize(uInt32& w, uInt32& h) const;

  protected:
    void draw() override { }
    void releaseFocus() override;

    virtual void handleText(char text);
    virtual void handleKeyDown(StellaKey key, StellaMod modifiers, bool repeated = false);
    virtual void handleKeyUp(StellaKey key, StellaMod modifiers);
    virtual void handleMouseDown(int x, int y, MouseButton b, int clickCount);
    virtual void handleMouseUp(int x, int y, MouseButton b, int clickCount);
    virtual void handleMouseWheel(int x, int y, int direction);
    virtual void handleMouseMoved(int x, int y);
    virtual bool handleMouseClicks(int x, int y, MouseButton b);
    virtual void handleJoyDown(int stick, int button, bool longPress = false);
    virtual void handleJoyUp(int stick, int button);
    virtual void handleJoyAxis(int stick, JoyAxis axis, JoyDir adir, int button = JOY_CTRL_NONE);
    virtual bool handleJoyHat(int stick, int hat, JoyHatDir hdir, int button = JOY_CTRL_NONE);
    void handleCommand(CommandSender* sender, int cmd, int data, int id) override;
    virtual Event::Type getJoyAxisEvent(int stick, JoyAxis axis, JoyDir adir, int button);

    Widget* findWidget(int x, int y) const; // Find the widget at pos x,y if any

    void addOKCancelBGroup(WidgetArray& wid, const GUI::Font& font,
                           const string& okText = "OK",
                           const string& cancelText = "Cancel",
                           bool focusOKButton = true,
                           int buttonWidth = 0);

    void addDefaultsOKCancelBGroup(WidgetArray& wid, const GUI::Font& font,
                                   const string& okText = "OK",
                                   const string& cancelText = "Cancel",
                                   const string& defaultsText = "Defaults",
                                   bool focusOKButton = true);

    void processCancelWithoutWidget(bool state) { _processCancel = state; }
    virtual void processCancel() { close(); }

    /** Define the size (allowed) for the dialog. */
    void setSize(uInt32 w, uInt32 h, uInt32 max_w, uInt32 max_h);
    void positionAt(uInt32 pos);

    virtual bool repeatEnabled() { return true; }

  private:
    void buildCurrentFocusList(int tabID = -1);
    bool handleNavEvent(Event::Type e, bool repeated = false);
    void getTabIdForWidget(Widget* w);
    bool cycleTab(int direction);

  protected:
    const GUI::Font& _font;

    Widget* _mouseWidget{nullptr};
    Widget* _focusedWidget{nullptr};
    Widget* _dragWidget{nullptr};
    Widget* _defaultWidget{nullptr};
    Widget* _okWidget{nullptr};
    Widget* _cancelWidget{nullptr};

    bool    _visible{false};
    bool    _onTop{true};
    bool    _processCancel{false};
    string  _title;
    int     _th{0};
    int     _layer{0};

    Common::FixedStack<shared_ptr<FBSurface>> mySurfaceStack;

  private:
    struct Focus {
      Widget* widget{nullptr};
      WidgetArray list;

      explicit Focus(Widget* w = nullptr) : widget(w) { }
      virtual ~Focus() = default;

      Focus(const Focus&) = default;
      Focus& operator=(const Focus&) = default;
    };
    using FocusList = vector<Focus>;

    struct TabFocus {
      TabWidget* widget{nullptr};
      FocusList focus;
      uInt32 currentTab{0};

      explicit TabFocus(TabWidget* w = nullptr) : widget(w) { }
      virtual ~TabFocus() = default;

      TabFocus(const TabFocus&) = default;
      TabFocus& operator=(const TabFocus&) = default;

      void appendFocusList(WidgetArray& list);
      void saveCurrentFocus(Widget* w);
      Widget* getNewFocus();
    };
    using TabFocusList = vector<TabFocus>;

    Focus        _myFocus;    // focus for base dialog
    TabFocusList _myTabList;  // focus for each tab (if any)

    WidgetArray _buttonGroup;
    shared_ptr<FBSurface> _surface;

    int _tabID{0};
    int _flags{0};
    bool _dirty{false};
    uInt32 _max_w{0}; // maximum wanted width
    uInt32 _max_h{0}; // maximum wanted height

  private:
    // Following constructors and assignment operators not supported
    Dialog() = delete;
    Dialog(const Dialog&) = delete;
    Dialog(Dialog&&) = delete;
    Dialog& operator=(const Dialog&) = delete;
    Dialog& operator=(Dialog&&) = delete;
};

#endif
