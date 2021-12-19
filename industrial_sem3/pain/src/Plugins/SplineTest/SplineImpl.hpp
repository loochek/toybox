#ifndef SPLINE_IMPL_HPP
#define SPLINE_IMPL_HPP

#include <vector>
#include "SplineWidget.hpp"
#include "../../Utils/CatmullRomSpline.hpp"

class AbstractImpl : public PUPPY::Widget
{
public:
    AbstractImpl(const PUPPY::WBody &body, const PUPPY::AppInterface *appInterface, PUPPY::Widget *parent = nullptr);

    virtual ~AbstractImpl() { delete mTexture; }

    virtual bool is_active() override { return true; };
    virtual bool is_inside(const PUPPY::Vec2f &pos) override { return true; }

    virtual bool add_child(Widget */*child*/) override { return false; }
    virtual bool delete_child(Widget */*child*/) override { return false; }

    virtual bool delete_from_parent() override { return false; }

    virtual void set_to_delete() override {};
    
    virtual void on_render(const PUPPY::Event::Render &) override {}

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

    virtual void hide()  override {};
    virtual void show()  override {};
    virtual void focus() override {};

    virtual void set_caption(const char *, size_t, const PUPPY::Vec2f*) override {}
    virtual void set_base_color(const PUPPY::RGBA &/*color*/) override {}

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

class SplinePoint : public AbstractImpl
{
public:
    SplinePoint(const PUPPY::WBody &body, const PUPPY::AppInterface *appInterface, PUPPY::Widget *parent = nullptr);

    virtual void on_render(const PUPPY::Event::Render &) override;
};

class SplineImpl : public AbstractImpl
{
public:
    SplineImpl(const PUPPY::WBody &body, const PUPPY::AppInterface *appInterface, PUPPY::Widget *parent = nullptr);

    virtual void on_render(const PUPPY::Event::Render &) override;
    
protected:
    void recalcSpline();

protected:
    CatmullRomSpline mSpline;
    std::vector<PUPPY::Widget*> mPoints;

    PUPPY::Widget *mHolder;
};

#endif