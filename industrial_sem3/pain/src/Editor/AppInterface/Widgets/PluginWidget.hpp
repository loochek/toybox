#ifndef PLUGIN_WIDGET_HPP
#define PLUGIN_WIDGET_HPP

#include "../PluginTypesProxy.hpp"
#include "../../../BaseGUI/Widget.hpp"
#include "../../EditorPluginAPI/lib_std/widgets/collection.h"
#include "EventsFwd.inc"

class PluginWidgetImpl;

/**
 * Special widget which holds wrapper structure
 */
class PluginWidgetIntl : public Widget
{
public:
    PluginWidgetIntl(const IntRect &widgetRect, PluginWidgetImpl *impl, Widget *parent = nullptr);
    ~PluginWidgetIntl();

    PluginWidgetImpl *getImpl() { return mImpl; };

protected:
    EVENTS_FWD_HEADER()

protected:
    PluginWidgetImpl *mImpl;
};

/**
 * PUPPY::Widget implementation as Widget special wrapper
 */
class PluginWidgetImpl : virtual public PUPPY::Widget
{
public:
    PluginWidgetImpl() = delete;
    PluginWidgetImpl(const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr, bool create = false);
    // Limited wrapper for non-special widgets
    PluginWidgetImpl(::Widget *widget);

    ::Widget *getWidget() { return mWidget; };

    virtual void set_position(const PUPPY::Vec2f &position_) override;
    virtual void set_size(const PUPPY::Vec2f &size_) override;

    virtual PUPPY::WBody get_body() override;
    virtual void set_body(const PUPPY::WBody &body_) override;

    virtual PUPPY::Widget *get_parent() const override { return mParent; };
    virtual void set_parent(PUPPY::Widget *parent_) override { mParent = parent_; };

    virtual PUPPY::RenderTarget *get_texture() override;
    virtual void set_texture(PUPPY::RenderTarget *texture_) override;

    virtual bool is_active() override;
    virtual bool is_inside(const PUPPY::Vec2f &pos) override;

    virtual bool add_child(PUPPY::Widget *child) override;

    virtual void set_to_delete() override;
    virtual bool delete_child(PUPPY::Widget *child) override;
    virtual bool delete_from_parent() override;
    
    virtual void on_render          (const PUPPY::Event::Render          &event) override {};
    virtual void on_tick            (const PUPPY::Event::Tick            &event) override {};
    virtual void on_mouse_press     (const PUPPY::Event::MousePress      &event) override {};
    virtual void on_mouse_release   (const PUPPY::Event::MouseRelease    &event) override {};
    virtual void on_mouse_move      (const PUPPY::Event::MouseMove       &event) override {};
    virtual void on_key_down        (const PUPPY::Event::KeyDown         &event) override {};
    virtual void on_key_up          (const PUPPY::Event::KeyUp           &event) override {};
    virtual void on_text_enter      (const PUPPY::Event::TextEnter       &event) override {};
    virtual void on_scroll          (const PUPPY::Event::Scroll          &event) override {};
    virtual void on_hide            (const PUPPY::Event::Hide            &event) override {};
    virtual void on_show            (const PUPPY::Event::Show            &event) override {};

    virtual void hide() override;
    virtual void show() override;
    virtual void focus() override;

    virtual void set_caption(const char *text, size_t font_size, const PUPPY::Vec2f *pos = nullptr) override;
    virtual void set_base_color(const PUPPY::RGBA &color) override;

protected:
    static ::Widget *translateParent(PUPPY::Widget *parent);

protected:
    ::Widget      *mWidget;
    PUPPY::Widget *mParent;

    friend class WidgetFactoryImpl;
};

#endif