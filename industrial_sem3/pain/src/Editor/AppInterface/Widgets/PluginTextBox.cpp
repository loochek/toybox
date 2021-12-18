#include "../RenderTarget.hpp"
#include "Logic/PTextBoxDelegate.hpp"
#include "PluginTextBox.hpp"

PluginTextBoxIntl::PluginTextBoxIntl(const Vec2i &textBoxPos, int textBoxSize, PluginTextBoxImpl *impl, Widget *parent) :
    TextBox(textBoxPos, textBoxSize, parent), mImpl(impl)
{
    mDelegate = new PTextBoxDelegate(this);
    setDelegate(mDelegate);
}

PluginTextBoxIntl::~PluginTextBoxIntl()
{
    delete mDelegate;
    delete mImpl;
}

EVENTS_FWD(PluginTextBoxIntl, TextBox)

PluginTextBoxImpl::PluginTextBoxImpl(const PUPPY::WBody &body, PUPPY::Widget *parent) : 
    PluginWidgetImpl(body, parent, false)
{
    mWidget = new PluginTextBoxIntl(fromPluginVec(body.position), body.size.x, this, translateParent(parent));
    if (parent != nullptr)
        parent->add_child(this);
}

std::string_view PluginTextBoxImpl::get_text() const
{
    return static_cast<TextBox*>(mWidget)->getText();
}

void PluginTextBoxImpl::set_text(std::string_view text)
{
    static_cast<TextBox*>(mWidget)->setText(text.data());
}