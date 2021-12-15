#include "../../Editor/EditorPluginAPI/plugin_std.hpp"


class AbstractWidget : public PUPPY::Widget {
public:
    AbstractWidget(const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr) :
    body(body),
    parent(parent),
    texture(nullptr)
    {
        if (parent) {
            parent->add_child(this);
        }
    }
    virtual ~AbstractWidget() {}

    virtual bool is_active() override { return true; };
    virtual bool is_inside(const PUPPY::Vec2f &pos) override {
        return (pos.x >= 0) && (pos.y >= 0) && (pos.x < body.size.x) && (pos.y < body.size.y);
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
    virtual void on_key_down        (const PUPPY::Event::KeyDown         &) override {}
    virtual void on_key_up          (const PUPPY::Event::KeyUp           &) override {}
    virtual void on_text_enter      (const PUPPY::Event::TextEnter       &) override {}
    virtual void on_scroll          (const PUPPY::Event::Scroll          &) override {}
    virtual void on_hide            (const PUPPY::Event::Hide            &) override {}
    virtual void on_show            (const PUPPY::Event::Show            &) override {}

    virtual void hide()  override {}
    virtual void show()  override {}
    virtual void focus() override {}

    virtual void set_caption(const char */*text*/, size_t /*font_size*/, const PUPPY::Vec2f */*pos*/ = nullptr) override {}
    virtual void set_base_color(const PUPPY::RGBA &/*color*/) override {};

// ----------------

    virtual void set_position(const PUPPY::Vec2f &position_) override { body.position = position_; }
    virtual void set_size(const PUPPY::Vec2f &size_) override { body.size = size_; }

    virtual PUPPY::WBody get_body() override { return body; }
    virtual void set_body(const PUPPY::WBody &body_) override { body = body_; }

    virtual Widget *get_parent() const override { return parent; }
    virtual void set_parent(Widget *parent_) override { parent = parent_; }

    virtual PUPPY::RenderTarget *get_texture() override { return texture; }
    virtual void set_texture(PUPPY::RenderTarget *texture_) override { texture = texture_; }

protected:
    PUPPY::WBody body;
    Widget *parent;
    PUPPY::RenderTarget *texture;
};
