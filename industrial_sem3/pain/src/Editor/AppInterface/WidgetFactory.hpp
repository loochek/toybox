#ifndef WIDGET_FACTORY_HPP
#define WIDGET_FACTORY_HPP

#include "../EditorPluginAPI/plugin_std.hpp"

class Widget;

class WidgetFactoryImpl : public PUPPY::WidgetFactory
{
public:
    WidgetFactoryImpl(Widget *root);

    virtual PUPPY::Button      *button      (const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr) const override;
    virtual PUPPY::Button      *button      (const PUPPY::Vec2f &pos, const char *caption, PUPPY::Widget *parent = nullptr) const override;
    virtual PUPPY::Slider      *slider      (PUPPY::Slider::Type type, const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr) const override;
    virtual PUPPY::TextField   *text_field  (const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr) const override;
    virtual PUPPY::Window      *window      (const char *name, const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr) const override;
    virtual PUPPY::ColorPicker *color_picker(const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr) const override;
    virtual PUPPY::Label       *label       (const PUPPY::Vec2f &pos, const char *text, PUPPY::Widget *parent = nullptr) const override;
    virtual PUPPY::Widget      *abstract    (const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr) const override;

private:
    Widget *mRoot;
};

#endif