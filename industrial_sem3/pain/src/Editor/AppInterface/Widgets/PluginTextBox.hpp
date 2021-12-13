#ifndef PLUGIN_TEXT_BOX_HPP
#define PLUGIN_TEXT_BOX_HPP

#include "../PluginTypesProxy.hpp"
#include "../../../BaseGUI/TextBox.hpp"
#include "../../EditorPluginAPI/lib_std/widgets/collection.h"
#include "PluginWidget.hpp"

class PTextBoxDelegate;
class PluginTextBoxImpl;

/**
 * Special window which holds wrapper structure
 */
class PluginTextBoxIntl : public TextBox
{
public:
    PluginTextBoxIntl(const Vec2i &textBoxPos, int textBoxSize, PluginTextBoxImpl *impl, Widget *parent = nullptr);
    ~PluginTextBoxIntl();

    PluginTextBoxImpl *getImpl() { return mImpl; };

protected:
    PluginTextBoxImpl *mImpl;
    PTextBoxDelegate  *mDelegate;
};

class PluginTextBoxImpl : public PluginWidgetImpl, public PUPPY::TextField
{
public:
    PluginTextBoxImpl() = delete;
    PluginTextBoxImpl(const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr);

    virtual void set_handler(const HandlerType &handler_) override { mHandler = handler_; };
    virtual HandlerType &get_handler() override { return mHandler; };

    virtual std::string_view get_text() const override;
    virtual void set_text(std::string_view text) override;

protected:
    HandlerType mHandler;
};

#endif