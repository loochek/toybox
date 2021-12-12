#ifndef PLUGIN_WIDGET_HPP
#define PLUGIN_WIDGET_HPP

#include "../PluginTypesProxy.hpp"
#include "../../../BaseGUI/Widget.hpp"
#include "../../EditorPluginAPI/lib_std/widgets/collection.h"

class PluginWidgetImpl;

/**
 * Special widget which holds wrapper structure
 */
class PluginWidgetIntl : public Widget
{
public:
    PluginWidgetIntl(const IntRect &widgetRect, PluginWidgetIntl *parent = nullptr);

    void setImpl(PluginWidgetImpl *impl) { mImpl = impl; };

protected:
    PluginWidgetImpl *mImpl;
    friend class PluginWidgetImpl;
};

/**
 * P::Widget implementation as Widget special wrapper
 */
class PluginWidgetImpl : virtual public P::Widget
{
public:
    PluginWidgetImpl() = delete;
    PluginWidgetImpl(const P::WBody &body, PluginWidgetIntl *widget, P::Widget *parent = nullptr);

    virtual bool is_active() override;
    virtual bool is_inside(const P::Vec2f &pos) override;

    virtual bool add_child(P::Widget *child) override;

    virtual void set_to_delete() override;
    virtual bool delete_child(P::Widget *child) override;
    virtual bool delete_from_parent() override;
    
    virtual void on_render          (const P::Event::Render          &event) override {};
    virtual void on_tick            (const P::Event::Tick            &event) override {};
    virtual void on_mouse_press     (const P::Event::MousePress      &event) override {};
    virtual void on_mouse_release   (const P::Event::MouseRelease    &event) override {};
    virtual void on_mouse_move      (const P::Event::MouseMove       &event) override {};
    virtual void on_key_down        (const P::Event::KeyDown         &event) override {};
    virtual void on_key_up          (const P::Event::KeyUp           &event) override {};
    virtual void on_text_enter      (const P::Event::TextEnter       &event) override {};
    virtual void on_scroll          (const P::Event::Scroll          &event) override {};
    virtual void on_hide            (const P::Event::Hide            &event) override {};
    virtual void on_show            (const P::Event::Show            &event) override {};

    virtual void hide() override;
    virtual void show() override;

    virtual void set_caption(const char *text, size_t font_size, const P::Vec2f &pos) override;
    virtual void set_base_color(const P::RGBA &color) override;

protected:
    PluginWidgetIntl *mWidget;

    friend class PluginWidgetIntl;
    friend class WidgetFactoryImpl;
};

#endif