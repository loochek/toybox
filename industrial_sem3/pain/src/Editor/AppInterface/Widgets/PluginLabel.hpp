#ifndef PLUGIN_LABEL_HPP
#define PLUGIN_LABEL_HPP

#include "../PluginTypesProxy.hpp"
#include "../../../BaseGUI/Label.hpp"
#include "../../EditorPluginAPI/lib_std/widgets/collection.h"
#include "PluginWidget.hpp"

class PluginLabelImpl;

class PluginLabelIntl : public Label
{
public:
    PluginLabelIntl(const Vec2i &textAxis, PUPPY::Label *impl, Widget *parent = nullptr);
    ~PluginLabelIntl();

    PUPPY::Label *getImpl() { return mImpl; };

protected:
    EVENTS_FWD_HEADER()

protected:
    PUPPY::Label *mImpl;
};

class PluginLabelImpl : public PluginWidgetImpl, public PUPPY::Label
{
public:
    PluginLabelImpl() = delete;
    PluginLabelImpl(const PUPPY::Vec2f &pos, const char *text, PUPPY::Widget *parent = nullptr);

    virtual std::string_view get_text() const override;
    virtual void set_text(std::string_view text) override;
};

#endif