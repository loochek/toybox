#include "../../Editor/EditorPluginAPI/plugin_std.hpp"
#include "GBEmu.hpp"

PUPPY::RGBA gb_screen_colors[] =
{
    { 0xFF, 0xFF, 0xFF, 0xFF },
    { 0xAA, 0xAA, 0xAA, 0xFF },
    { 0x55, 0x55, 0x55, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF }
};

const int SCREEN_SCALE = 4;

class GBWidget;

class GB : public PUPPY::PluginInterface
{
public:
    GB() : PUPPY::PluginInterface(),
        mJoypadState(0), mGBFramebuffer(nullptr), mGBFrameReadyPtr(nullptr), mGbWidget(nullptr) {};

    virtual bool ext_enable(const char *name) const override;

    virtual void *ext_get_func(const char *extension, const char *func) const override;

    virtual void *ext_get_interface(const char *extension, const char *name) const override;

    virtual const PUPPY::PluginInfo *get_info() const override;
    virtual PUPPY::Status init(const PUPPY::AppInterface*, const std::filesystem::path& path) override;
    virtual PUPPY::Status deinit() override;
    virtual void dump()const override;

    virtual void on_tick(double dt) const override;

    virtual void effect_apply() const override;

    virtual void tool_on_press(const PUPPY::Vec2f &position) const override;
    virtual void tool_on_release(const PUPPY::Vec2f &position) const override;
    virtual void tool_on_move(const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const override;

    virtual void show_settings() const override;

public:
    int mJoypadState;

    const char *mGBFramebuffer;
    const bool *mGBFrameReadyPtr;

    GBWidget *mGbWidget;
};

GB gPluginInterface;
gb_emu_t gGBEmu;

const PUPPY::AppInterface *gAppInterface = nullptr;

const PUPPY::PluginInfo gPluginInfo =
{
    PUPPY::STD_VERSION,     // std_version
    0,                      // reserved

    &gPluginInterface,      // plugin interface

    "GB",
    "2.0",
    "loochek",
    "Gameboy emulator",

    nullptr,                // icon
    
    PUPPY::PluginType::EXTENSION
};

extern "C" const PUPPY::PluginInterface *get_plugin_interface()
{
    return &gPluginInterface;
}

class GBWidget : public PUPPY::Widget
{
public:
    GBWidget(const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr) :
    mBody(body), mParent(parent)
    {
        mTexture = gAppInterface->factory.target->spawn(PUPPY::Vec2s(body.size.x, body.size.y));

        if (parent)
            parent->add_child(this);
    }

    virtual ~GBWidget() { delete mTexture; }

    virtual bool is_active() override { return true; };
    virtual bool is_inside(const PUPPY::Vec2f &pos) override {
        return true;
    }

    virtual bool add_child(Widget */*child*/) override { return false; }
    virtual bool delete_child(Widget */*child*/) override { return false; }

    virtual bool delete_from_parent() override { return false; }

    virtual void set_to_delete() override {};
    
    virtual void on_render          (const PUPPY::Event::Render          &) override {}

    virtual void on_tick            (const PUPPY::Event::Tick            &) override {}
    virtual void on_mouse_press     (const PUPPY::Event::MousePress      &) override {}
    virtual void on_mouse_release   (const PUPPY::Event::MouseRelease    &) override {}
    virtual void on_mouse_move      (const PUPPY::Event::MouseMove       &) override {}
    virtual void on_key_down        (const PUPPY::Event::KeyDown         &) override;
    virtual void on_key_up          (const PUPPY::Event::KeyUp           &) override;
    virtual void on_text_enter      (const PUPPY::Event::TextEnter       &) override {}
    virtual void on_scroll          (const PUPPY::Event::Scroll          &) override {}
    virtual void on_hide            (const PUPPY::Event::Hide            &) override {}
    virtual void on_show            (const PUPPY::Event::Show            &) override {}

    virtual void hide()  override {};
    virtual void show()  override {};
    virtual void focus() override {};

    virtual void set_caption(const char */*text*/, size_t /*font_size*/, const PUPPY::Vec2f */*pos*/ = nullptr) override {}
    virtual void set_base_color(const PUPPY::RGBA &/*color*/) override {};

    virtual void *get_extra_data(void *) override { return nullptr; }

// ----------------

    virtual void set_position(const PUPPY::Vec2f &position_) override { mBody.position = position_; }
    virtual void set_size(const PUPPY::Vec2f &size_) override { mBody.size = size_; }

    virtual PUPPY::WBody get_body() override { return mBody; }
    virtual void set_body(const PUPPY::WBody &body_) override { mBody = body_; }

    virtual Widget *get_parent() const override { return mParent; }
    virtual void set_parent(Widget *parent_) override { mParent = parent_; }

    virtual PUPPY::RenderTarget *get_texture() override { return mTexture; }
    virtual void set_texture(PUPPY::RenderTarget *texture_) override { mTexture = texture_; }

protected:
    PUPPY::WBody mBody;
    Widget *mParent;
    PUPPY::RenderTarget *mTexture;
};

void GBWidget::on_key_down(const PUPPY::Event::KeyDown& event)
{
    if (event.code == PUPPY::Keyboard::Key::x)
        gPluginInterface.mJoypadState |= BUTTON_A;

    if (event.code == PUPPY::Keyboard::Key::z)
        gPluginInterface.mJoypadState |= BUTTON_B;

    if (event.code == PUPPY::Keyboard::Key::space)
        gPluginInterface.mJoypadState |= BUTTON_SELECT;

    if (event.code == PUPPY::Keyboard::Key::enter)
        gPluginInterface.mJoypadState |= BUTTON_START;

    if (event.code == PUPPY::Keyboard::Key::up)
        gPluginInterface.mJoypadState |= BUTTON_UP;

    if (event.code == PUPPY::Keyboard::Key::down)
        gPluginInterface.mJoypadState |= BUTTON_DOWN;

    if (event.code == PUPPY::Keyboard::Key::left)
        gPluginInterface.mJoypadState |= BUTTON_LEFT;

    if (event.code == PUPPY::Keyboard::Key::right)
        gPluginInterface.mJoypadState |= BUTTON_RIGHT;

    gb_emu_update_input(&gGBEmu, gPluginInterface.mJoypadState);
}

void GBWidget::on_key_up(const PUPPY::Event::KeyUp& event)
{
    if (event.code == PUPPY::Keyboard::Key::x)
        gPluginInterface.mJoypadState &= ~BUTTON_A;

    if (event.code == PUPPY::Keyboard::Key::z)
        gPluginInterface.mJoypadState &= ~BUTTON_B;

    if (event.code == PUPPY::Keyboard::Key::space)
        gPluginInterface.mJoypadState &= ~BUTTON_SELECT;

    if (event.code == PUPPY::Keyboard::Key::enter)
        gPluginInterface.mJoypadState &= ~BUTTON_START;

    if (event.code == PUPPY::Keyboard::Key::up)
        gPluginInterface.mJoypadState &= ~BUTTON_UP;

    if (event.code == PUPPY::Keyboard::Key::down)
        gPluginInterface.mJoypadState &= ~BUTTON_DOWN;

    if (event.code == PUPPY::Keyboard::Key::left)
        gPluginInterface.mJoypadState &= ~BUTTON_LEFT;

    if (event.code == PUPPY::Keyboard::Key::right)
        gPluginInterface.mJoypadState &= ~BUTTON_RIGHT;

    gb_emu_update_input(&gGBEmu, gPluginInterface.mJoypadState);
}

bool GB::ext_enable(const char *name) const
{
    return false;
}

void *GB::ext_get_func(const char *extension, const char *func) const
{
    return nullptr;
}

void *GB::ext_get_interface(const char *extension, const char *name)  const
{
    return nullptr;
}

const PUPPY::PluginInfo *GB::get_info() const
{
    return &gPluginInfo;
}

PUPPY::Status GB::init(const PUPPY::AppInterface* appInterface, const std::filesystem::path& path)
{
    gAppInterface = appInterface;

    gbstatus_e status = gb_emu_init(&gGBEmu);
    if (status != GBSTATUS_OK)
    {
        appInterface->log("GB error: %s - %s", gbstatus_str_repr[status], gbstatus_str);
        return PUPPY::Status::ERR;
    }

    status = gb_emu_change_rom(&gGBEmu, "./plugins/gb/zelda.gb");
    if (status != GBSTATUS_OK)
    {
        appInterface->log("GB error: %s - %s", gbstatus_str_repr[status], gbstatus_str);
        return PUPPY::Status::ERR;
    }

    mGBFramebuffer   = gb_emu_framebuffer_ptr(&gGBEmu);
    mGBFrameReadyPtr = gb_emu_frame_ready_ptr(&gGBEmu);

    PUPPY::Vec2f screenSize(GB_SCREEN_WIDTH * SCREEN_SCALE, GB_SCREEN_HEIGHT * SCREEN_SCALE);

    PUPPY::Widget *root = appInterface->get_root_widget();
    PUPPY::Window *window = appInterface->factory.widget->window("GB", PUPPY::WBody(PUPPY::Vec2f(200, 100),
                                                                                    screenSize), nullptr);
    mGbWidget = new GBWidget(PUPPY::WBody(PUPPY::Vec2f(8, 30), screenSize), window);

    appInterface->log("GB: succesful initialization!");
    return PUPPY::Status::OK;
}

PUPPY::Status GB::deinit()
{
    gb_emu_deinit(&gGBEmu);
    return PUPPY::Status::OK;
}

void GB::dump() const
{
}

void GB::on_tick(double dt) const
{
    while (!(*mGBFrameReadyPtr))
    {
        gbstatus_e status = gb_emu_step(&gGBEmu);
        if (status != GBSTATUS_OK)
            gAppInterface->log("GB warning: execution error: %s - %s", gbstatus_str_repr[status], gbstatus_str);
    }

    gb_emu_grab_frame(&gGBEmu);

    PUPPY::RenderTarget *rt = mGbWidget->get_texture();

    PUPPY::RGBA *pixels = rt->get_pixels();

    for (int gbY = 0; gbY < GB_SCREEN_HEIGHT; gbY++)
    {
        for (int gbX = 0; gbX < GB_SCREEN_WIDTH; gbX++)
        {
            int x = gbX * SCREEN_SCALE;
            int y = gbY * SCREEN_SCALE;

            int rowC = GB_SCREEN_WIDTH * SCREEN_SCALE;

            int color = mGBFramebuffer[gbY * GB_SCREEN_WIDTH + gbX];

            for (int i = 0; i < SCREEN_SCALE; i++)
            {
                for (int j = 0; j < SCREEN_SCALE; j++)
                {
                    pixels[(y + i) * rowC + x + j] = gb_screen_colors[color];
                }
            }
        }
    }

    mGbWidget->get_texture()->render_pixels(PUPPY::Vec2f(),
        PUPPY::Vec2s(GB_SCREEN_WIDTH * SCREEN_SCALE, GB_SCREEN_HEIGHT * SCREEN_SCALE), pixels);

    delete[] pixels;
}

void GB::effect_apply() const
{
}

void GB::tool_on_press(const PUPPY::Vec2f &position) const
{
}

void GB::tool_on_release(const PUPPY::Vec2f &position) const
{
}

void GB::tool_on_move(const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const
{
}

void GB::show_settings() const
{
}