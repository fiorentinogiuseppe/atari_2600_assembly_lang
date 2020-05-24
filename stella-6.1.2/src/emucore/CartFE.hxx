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

#ifndef CARTRIDGEFE_HXX
#define CARTRIDGEFE_HXX

class System;

#include "bspf.hxx"
#include "Cart.hxx"
#ifdef DEBUGGER_SUPPORT
  #include "CartFEWidget.hxx"
#endif

/**
  Bankswitching method used by Activision's Robot Tank and Decathlon.

  This scheme was originally designed to have up to 8 4K banks, and is
  triggered by monitoring the address bus for address $01FE.  All released
  carts had only two banks, and this implementation assumes that (ie, ROM
  is always 8K, and there are two 4K banks).

  The following is paraphrased from the original patent by David Crane,
  European Patent Application # 84300730.3, dated 06.02.84:

  ---------------------------------------------------------------------------
  The twelve line address bus is connected to a plurality of 4K by eight bit
  memories.

  The eight line data bus is connected to each of the banks of memory, also.
  An address comparator is connected to the bus for detecting the presence of
  the 01FE address.  Actually, the comparator will detect only the lowest 12
  bits of 1FE, because of the twelve bit limitation of the address bus.  Upon
  detection of the 01FE address, a one cycle delay is activated which then
  actuates latch connected to the data bus.  The three most significant bits
  on the data bus are latched and provide the address bits A13, A14, and A15
  which are then applied to a 3 to 8 de-multiplexer.  The 3 bits A13-A15
  define a code for selecting one of the eight banks of memory which is used
  to enable one of the banks of memory by applying a control signal to the
  enable, EN, terminal thereof.  Accordingly, memory bank selection is
  accomplished from address codes on the data bus following a particular
  program instruction, such as a jump to subroutine.
  ---------------------------------------------------------------------------

  Note that in the general scheme, we use D7, D6 and D5 for the bank number
  (3 bits, so 8 possible banks).  However, the scheme as used historically
  by Activision only uses two banks.  Furthermore, the two banks it uses
  are actually indicated by binary 110 and 111, and translated as follows:

    binary 110 -> decimal 6 -> Upper 4K ROM (bank 1) @ $D000 - $DFFF
    binary 111 -> decimal 7 -> Lower 4K ROM (bank 0) @ $F000 - $FFFF

  Since the actual bank numbers (0 and 1) do not map directly to their
  respective bitstrings (7 and 6), we simply test for D5 being 0 or 1.
  This is the significance of the test '(value & 0x20) ? 0 : 1' in the code.

  NOTE: Consult the patent application for more specific information, in
        particular *why* the address $01FE will be placed on the address
        bus after both the JSR and RTS opcodes.

  @author  Stephen Anthony; with ideas/research from Christian Speckner and
           alex_79 and TomSon (of AtariAge)
*/
class CartridgeFE : public Cartridge
{
  friend class CartridgeFEWidget;

  public:
    /**
      Create a new cartridge using the specified image

      @param image     Pointer to the ROM image
      @param size      The size of the ROM image
      @param md5       The md5sum of the ROM image
      @param settings  A reference to the various settings (read-only)
    */
    CartridgeFE(const ByteBuffer& image, size_t size, const string& md5,
                const Settings& settings);
    virtual ~CartridgeFE() = default;

  public:
    /**
      Reset device to its power-on state
    */
    void reset() override;

    /**
      Install cartridge in the specified system.  Invoked by the system
      when the cartridge is attached to it.

      @param system The system the device should install itself in
    */
    void install(System& system) override;

    /**
      Install pages for the specified bank in the system.

      @param bank The bank that should be installed in the system
    */
    bool bank(uInt16 bank) override;

    /**
      Get the current bank.

      @param address The address to use when querying the bank
    */
    uInt16 getBank(uInt16 address = 0) const override;

    /**
      Query the number of banks supported by the cartridge.
    */
    uInt16 bankCount() const override;

    /**
      Patch the cartridge ROM.

      @param address  The ROM address to patch
      @param value    The value to place into the address
      @return    Success or failure of the patch operation
    */
    bool patch(uInt16 address, uInt8 value) override;

    /**
      Access the internal ROM image for this cartridge.

      @param size  Set to the size of the internal ROM image data
      @return  A pointer to the internal ROM image data
    */
    const uInt8* getImage(size_t& size) const override;

    /**
      Save the current state of this cart to the given Serializer.

      @param out  The Serializer object to use
      @return  False on any errors, else true
    */
    bool save(Serializer& out) const override;

    /**
      Load the current state of this cart from the given Serializer.

      @param in  The Serializer object to use
      @return  False on any errors, else true
    */
    bool load(Serializer& in) override;

    /**
      Get a descriptor for the device name (used in error checking).

      @return The name of the object
    */
    string name() const override { return "CartridgeFE"; }

  #ifdef DEBUGGER_SUPPORT
    /**
      Get debugger widget responsible for accessing the inner workings
      of the cart.
    */
    CartDebugWidget* debugWidget(GuiObject* boss, const GUI::Font& lfont,
        const GUI::Font& nfont, int x, int y, int w, int h) override
    {
      return new CartridgeFEWidget(boss, lfont, nfont, x, y, w, h, *this);
    }
  #endif

  public:
    /**
      Get the byte at the specified address.

      @return The byte at the specified address
    */
    uInt8 peek(uInt16 address) override;

    /**
      Change the byte at the specified address to the given value.

      @param address The address where the value should be stored
      @param value The value to be stored at the address
      @return  True if the poke changed the device address space, else false
    */
    bool poke(uInt16 address, uInt8 value) override;

  private:
    /**
      Perform bankswitch when necessary, by monitoring for $01FE
      on the address bus and getting the bank number from the data bus.
    */
    void checkBankSwitch(uInt16 address, uInt8 value);

  private:
    // The 8K ROM image of the cartridge
    std::array<uInt8, 8_KB> myImage;

    // Indicates the offset into the ROM image (aligns to current bank)
    uInt16 myBankOffset{0};

    // Whether previous address by peek/poke equals $01FE (hotspot)
    bool myLastAccessWasFE{false};

  private:
    // Following constructors and assignment operators not supported
    CartridgeFE() = delete;
    CartridgeFE(const CartridgeFE&) = delete;
    CartridgeFE(CartridgeFE&&) = delete;
    CartridgeFE& operator=(const CartridgeFE&) = delete;
    CartridgeFE& operator=(CartridgeFE&&) = delete;
};

#endif
