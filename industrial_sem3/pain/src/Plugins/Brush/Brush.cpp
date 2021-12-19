#include "../../Editor/EditorPluginAPI/plugin_std.hpp"

class Brush : public PUPPY::PluginInterface
{
public:
    Brush() : PUPPY::PluginInterface() {};

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

private:
    virtual void draw(const PUPPY::Vec2f mousePos) const;
};

const PUPPY::AppInterface* gAppInterface = nullptr;

const Brush gPluginInterface;

const PUPPY::PluginInfo gPluginInfo =
{
    PUPPY::STD_VERSION,           // std_version
    0,                      // reserved

    &gPluginInterface,      // plugin interface

    "Brush",
    "2.0",
    "loochek",
    "Simple circle brush",

    nullptr,                // icon
    
    PUPPY::PluginType::TOOL
};

extern "C" const PUPPY::PluginInterface *get_plugin_interface()
{
    return &gPluginInterface;
}

// class TestWidget : public PUPPY::Widget
// {
// public:
//     TestWidget(const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr) :
//     mBody(body), mParent(parent)
//     {
//         mTexture = gAppInterface->factory.target->spawn(PUPPY::Vec2s(body.size.x, body.size.y));

//         if (parent)
//             parent->add_child(this);
//     }

//     virtual ~TestWidget() { delete mTexture; }

//     virtual bool is_active() override { return true; };
//     virtual bool is_inside(const PUPPY::Vec2f &pos) override {
//         return true;
//     }

//     virtual bool add_child(Widget */*child*/) override { return false; }
//     virtual bool delete_child(Widget */*child*/) override { return false; }

//     virtual bool delete_from_parent() override { return false; }

//     virtual void set_to_delete() override {};
    
//     virtual void on_render          (const PUPPY::Event::Render          &) override
//     {
//         mTexture->clear();
//         mTexture->render_rectangle(PUPPY::Vec2f(0, 0), PUPPY::Vec2f(100, 100), PUPPY::RGBA(255, 255, 255, 255));
//     }

//     virtual void on_tick            (const PUPPY::Event::Tick            &) override {}
//     virtual void on_mouse_press     (const PUPPY::Event::MousePress      &) override {}
//     virtual void on_mouse_release   (const PUPPY::Event::MouseRelease    &) override {}
//     virtual void on_mouse_move      (const PUPPY::Event::MouseMove       &) override {}
//     virtual void on_key_down        (const PUPPY::Event::KeyDown         &) override {}
//     virtual void on_key_up          (const PUPPY::Event::KeyUp           &) override {}
//     virtual void on_text_enter      (const PUPPY::Event::TextEnter       &) override {}
//     virtual void on_scroll          (const PUPPY::Event::Scroll          &) override {}
//     virtual void on_hide            (const PUPPY::Event::Hide            &) override {}
//     virtual void on_show            (const PUPPY::Event::Show            &) override {}

//     virtual void hide()  override {};
//     virtual void show()  override {};
//     virtual void focus() override {};

//     virtual void set_caption(const char */*text*/, size_t /*font_size*/, const PUPPY::Vec2f */*pos*/ = nullptr) override {}
//     virtual void set_base_color(const PUPPY::RGBA &/*color*/) override {};

//     virtual void *get_extra_data(void *) override { return nullptr; }

// // ----------------

//     virtual void set_position(const PUPPY::Vec2f &position_) override { mBody.position = position_; }
//     virtual void set_size(const PUPPY::Vec2f &size_) override { mBody.size = size_; }

//     virtual PUPPY::WBody get_body() override { return mBody; }
//     virtual void set_body(const PUPPY::WBody &body_) override { mBody = body_; }

//     virtual Widget *get_parent() const override { return mParent; }
//     virtual void set_parent(Widget *parent_) override { mParent = parent_; }

//     virtual PUPPY::RenderTarget *get_texture() override { return mTexture; }
//     virtual void set_texture(PUPPY::RenderTarget *texture_) override { mTexture = texture_; }

// protected:
//     PUPPY::WBody mBody;
//     Widget *mParent;
//     PUPPY::RenderTarget *mTexture;
// };

bool Brush::ext_enable(const char *name) const
{
    return false;
}

void *Brush::ext_get_func(const char *extension, const char *func) const
{
    return nullptr;
}

void *Brush::ext_get_interface(const char *extension, const char *name)  const
{
    return nullptr;
}

const PUPPY::PluginInfo *Brush::get_info() const
{
    return &gPluginInfo;
}

PUPPY::Status Brush::init(const PUPPY::AppInterface* appInterface, const std::filesystem::path& path)
{
    gAppInterface = appInterface;

    //PUPPY::Widget *root = appInterface->get_root_widget();
    //new TestWidget(PUPPY::WBody(PUPPY::Vec2f(100, 100), PUPPY::Vec2f(100, 100)), root);

    appInterface->log("Brush: succesful initialization!");
    return PUPPY::Status::OK;
}

PUPPY::Status Brush::deinit()
{
    return PUPPY::Status::OK;
}

void Brush::dump() const
{
}

void Brush::on_tick(double dt) const
{
}

void Brush::effect_apply() const
{
}

void Brush::tool_on_press(const PUPPY::Vec2f &position) const
{
    draw(position);
}

void Brush::tool_on_release(const PUPPY::Vec2f &position) const
{
}

void Brush::tool_on_move(const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const
{
    draw(to);
}

void Brush::show_settings() const
{
}

void Brush::draw(PUPPY::Vec2f mousePos) const
{
    PUPPY::RenderMode mode(PUPPY::BlendMode::COPY);

    PUPPY::RenderTarget *preview = gAppInterface->get_preview();
    preview->render_circle(mousePos, gAppInterface->get_size(), gAppInterface->get_color(), mode);
    delete preview;
}