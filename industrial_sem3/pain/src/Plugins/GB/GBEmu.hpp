#ifndef GB_EMU_HPP
#define GB_EMU_HPP

#include <cstdint>

#define MAX_STATUS_STR_LENGTH 100

typedef enum
{
    GBSTATUS_OK,
    GBSTATUS_BAD_ALLOC,
    GBSTATUS_IO_FAIL,
    GBSTATUS_CPU_ILLEGAL_OP,
    GBSTATUS_CART_FAIL,
    GBSTATUS_SFML_FAIL,
    GBSTATUS_NOT_IMPLEMENTED
} gbstatus_e;

/// Stores an extended info message about last non-OK status
extern "C" char gbstatus_str[];

extern "C" const char *gbstatus_str_repr[];

struct gb;
struct gb_cart;

typedef enum
{
    BUTTON_RIGHT  = 1,
    BUTTON_LEFT   = 2,
    BUTTON_UP     = 4,
    BUTTON_DOWN   = 8,
    BUTTON_A      = 16,
    BUTTON_B      = 32,
    BUTTON_SELECT = 64,
    BUTTON_START  = 128
} gb_buttons_e;

typedef struct
{
    int state;

    uint8_t reg_joyp;

    /// Pointer to the parent Gameboy structure
    struct gb *gb;
} gb_joypad_t;

/**
 * Represents timer and divider. 
 */
typedef struct gb_timer
{
    uint8_t reg_div;
    uint8_t reg_tima;
    uint8_t reg_tma;
    uint8_t reg_tac;

    /// Clock cycles counter for updating divider
    int div_cycles;

    /// Clock cycles counter for updating timer
    int timer_cycles;

    /// Pointer to the parent Gameboy structure
    struct gb *gb;
} gb_timer_t;

#define GB_SCREEN_WIDTH 160
#define GB_SCREEN_HEIGHT 144

#define MAX_SPRITE_PER_LINE 10

struct gb;

typedef enum
{
    STATE_OBJ_SEARCH,
    STATE_DRAWING,
    STATE_HBLANK,
    STATE_HBLANK_INC,
    STATE_VBLANK,
    STATE_VBLANK_INC,
    STATE_VBLANK_LAST_LINE,
    STATE_VBLANK_LAST_LINE_INC
} ppu_state_e;

/**
 * Gameboy PPU representation
 */
typedef struct gb_ppu
{
    // Control registers

    uint8_t reg_lcdc;
    uint8_t reg_stat;
    uint8_t reg_ly;
    uint8_t reg_lyc;

    // Background scrolling

    uint8_t reg_scx;
    uint8_t reg_scy;

    // Window position

    uint8_t reg_wx;
    uint8_t reg_wy;

    // Palletes

    uint8_t reg_bgp;
    uint8_t reg_obp0;
    uint8_t reg_obp1;

    uint8_t *vram;
    uint8_t *oam;

    /// Internal window line counter
    int window_line;

    /// WY update takes effect only on next frame
    int delayed_wy;

    /// Clock cycles counter for PPU updating
    int cycles_counter;

    int clocks_to_next_state;

    /// LCDC interrupt can be requested once per line
    bool lcdc_blocked;

    ppu_state_e next_state;

    bool new_frame_ready;
    char *framebuffer;

    /// Holds original BG and Window colors to handle OBJ priority bit
    char *bg_scanline_buffer;

    int sprite_draw_order[MAX_SPRITE_PER_LINE];

    /// Draw order size
    int line_sprite_count;

    /// Pointer to the parent Gameboy structure
    struct gb *gb;
} gb_ppu_t;

/**
 * Representation of Gameboy interrupt controller
 */
typedef struct
{
    // Interrupt enable register
    uint8_t reg_ie;

    // Interrupt flags register
    uint8_t reg_if;

    /// Pointer to the parent Gameboy structure
    struct gb *gb;
} gb_int_controller_t;

/**
 * Represents Gameboy memory bus. 
 * Handles RAM, cartridge and passes MMIO requests to peripheral devices. 
 */
typedef struct gb_mmu
{
    /// Internal RAM
    uint8_t *ram;

    /// Internal RAM 2
    uint8_t *hram;

    /// Current cartridge
    struct gb_cart *cart;

    /// Pointer to the parent Gameboy structure
    struct gb *gb;

    /// First 256 bytes are mapped to the bootrom instead of cartridge after power-on
    bool bootrom_mapped;
} gb_mmu_t;

/**
 * Gameboy CPU representation
 */
typedef struct gb_cpu
{
    /// TODO: deal with big-endian machines

    /// Registers

    union
    {
        struct
        {
            uint8_t reg_f;
            uint8_t reg_a;
        };

        uint16_t reg_af;
    };

    union
    {
        struct
        {
            uint8_t reg_c;
            uint8_t reg_b;
        };

        uint16_t reg_bc;
    };

    union
    {
        struct
        {
            uint8_t reg_e;
            uint8_t reg_d;
        };

        uint16_t reg_de;
    };

    union
    {
        struct
        {
            uint8_t reg_l;
            uint8_t reg_h;
        };

        uint16_t reg_hl;
    };

    bool halted;

    int ei_delay;

    /// Interrupt master switch
    bool ime;

    /// Program counter
    uint16_t pc;

    /// Stack pointer
    uint16_t sp;

    /// Pointer to the parent Gameboy structure
    struct gb *gb;

} gb_cpu_t;

typedef struct gb
{
    gb_cpu_t            cpu;
    gb_mmu_t            mmu;
    gb_ppu_t            ppu;
    gb_int_controller_t intr_ctrl;
    gb_timer_t          timer;
    gb_joypad_t         joypad;
} gb_t;

#define MAX_ROM_PATH_LEN 100
#define GAME_TITLE_LEN   16

#define ROM_BANK_SIZE  0x4000
#define SRAM_BANK_SIZE 0x2000

typedef uint8_t (*cart_read_func_t )(struct gb_cart *cart, uint16_t addr);
typedef void    (*cart_write_func_t)(struct gb_cart *cart, uint16_t addr, uint8_t byte);
typedef void    (*cart_misc_func_t )(struct gb_cart *cart);

/**
 * Represents Gameboy cartridge. 
 */
typedef struct gb_cart
{
    /// Cartridge ROM banks
    uint8_t *rom;

    /// Cartridge RAM banks
    uint8_t *ram;

    int curr_rom_bank;
    int curr_ram_bank;

    /// ROM size in banks
    int rom_size;

    /// RAM size in banks
    int ram_size;

    bool battery_backed;

    char rom_file_path[MAX_ROM_PATH_LEN + 1];

    char game_title[GAME_TITLE_LEN + 1];

    cart_read_func_t  mbc_read_func;
    cart_write_func_t mbc_write_func;

    cart_misc_func_t  mbc_reset_func;
    cart_misc_func_t  mbc_deinit_func;
    
    void *mbc_state;
} gb_cart_t;

/**
 * Initializes the instance of the cartridge
 * 
 * \param cart Cartridge instance
 * \param rom_path ROM file path
 */
extern "C" gbstatus_e cart_init(gb_cart_t *cart, const char *rom_path);

/**
 * Resets the cartridge
 * 
 * \param cart Cartridge instance
 */
extern "C" void cart_reset(gb_cart_t *cart);

/**
 * Emulates a memory read request to the cartridge
 * 
 * \param cart Cartridge instance
 * \param addr Address to read
 * \return Byte read
 */
extern "C" uint8_t cart_read(gb_cart_t *cart, uint16_t addr);

/**
 * Emulates a memory write request to the cartridge
 * 
 * \param cart Cartridge instance
 * \param addr Address to write
 * \param byte Byte to write
 */
extern "C" void cart_write(gb_cart_t *cart, uint16_t addr, uint8_t byte);

/**
 * Deinitializes the instance of the cartridge
 * 
 * \param cart Cartridge instance
 */
extern "C" void cart_deinit(gb_cart_t *cart);

/**
 * Gameboy emulator interface
 */
typedef struct
{
    gb_t        gb;
    
    gb_cart_t   cart;
    bool        cart_inserted;
} gb_emu_t;

/**
 * Initializes the instance of the emulator
 * 
 * \param gb_emu Emulator instance
 */
extern "C" gbstatus_e gb_emu_init(gb_emu_t *gb_emu);

/**
 * Returns pointer to the PPU framebuffer
 * 
 * \param gb_emu Emulator instance
 * \return Pointer
 */
extern "C"  const char *gb_emu_framebuffer_ptr(gb_emu_t *gb_emu);

/**
 * Returns pointer to the PPU frame ready flag
 * 
 * \param gb_emu Emulator instance
 * \return Pointer
 */
extern "C" const bool *gb_emu_frame_ready_ptr(gb_emu_t *gb_emu);

/**
 * Resets PPU frame ready flag
 * 
 * \param gb_emu Emulator instance
 */
extern "C" void gb_emu_grab_frame(gb_emu_t *gb_emu);

/**
 * Returns pointer to the game title or NULL if no ROM loaded
 * 
 * \param gb_emu Emulator instance
 * \return Pointer
 */
extern "C" const char *gb_emu_game_title_ptr(gb_emu_t *gb_emu);

/**
 * Changes current ROM
 * 
 * \param gb_emu Emulator instance
 * \param rom_file_path ROM file path
 */
extern "C" gbstatus_e gb_emu_change_rom(gb_emu_t *gb_emu, const char *rom_file_path);

/**
 * Unloads current ROM
 * 
 * \param gb_emu Emulator instance
 */
extern "C" void gb_emu_unload_rom(gb_emu_t *gb_emu);

/**
 * Resets Gameboy
 * 
 * \param gb_emu Emulator instance
 */
extern "C" void gb_emu_reset(gb_emu_t *gb_emu);

/**
 * Skips scrolling Nintendo logo. 
 * Must be called ONLY after reset!
 * 
 * \param gb_emu Emulator instance
 */
extern "C" void gb_emu_skip_bootrom(gb_emu_t *gb_emu);

/**
 * Takes one step of emulation
 * 
 * \param gb_emu Emulator instance
 */
extern "C" gbstatus_e gb_emu_step(gb_emu_t *gb_emu);

/**
 * Updates state of the joypad
 * 
 * \param gb_emu Emulator instance
 * \param new_state Information about pressed buttons
 */
extern "C" void gb_emu_update_input(gb_emu_t *gb_emu, int new_state);

/**
 * Deinitializes the instance of the emulator
 * 
 * \param gb_emu Emulator instance
 */
extern "C" void gb_emu_deinit(gb_emu_t *gb_emu);

#endif