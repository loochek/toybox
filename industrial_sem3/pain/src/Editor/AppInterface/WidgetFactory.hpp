#ifndef WIDGET_FACTORY_HPP
#define WIDGET_FACTORY_HPP

#include "../EditorPluginAPI/plugin_std.hpp"

class Widget;

class WidgetFactoryImpl : public P::WidgetFactory
{
public:
    WidgetFactoryImpl(Widget *root);

    virtual P::Button      *button      (const P::WBody &body, P::Widget *parent = nullptr) const override;
    virtual P::Button      *button      (const P::Vec2f &pos, const char *caption, P::Widget *parent = nullptr) const override;
    virtual P::Slider      *slider      (P::Slider::Type type, const P::WBody &body, P::Widget *parent = nullptr) const override;
    virtual P::TextField   *text_field  (const P::WBody &body, P::Widget *parent = nullptr) const override;
    virtual P::Window      *window      (const char *name, const P::WBody &body, P::Widget *parent = nullptr) const override;
    virtual P::ColorPicker *color_picker(const P::WBody &body, P::Widget *parent = nullptr) const override;
    virtual P::Label       *label       (const P::Vec2f &pos, const char *text, P::Widget *parent = nullptr) const override;
    virtual P::Widget      *abstract    (const P::WBody &body, P::Widget *parent = nullptr) const override;

private:
    Widget *mRoot;
};

#endif