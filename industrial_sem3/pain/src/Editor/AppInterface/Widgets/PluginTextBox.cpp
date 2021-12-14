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
}

PluginTextBoxImpl::PluginTextBoxImpl(const PUPPY::WBody &body, PUPPY::Widget *parent) : 
    PluginWidgetImpl(body, false, parent)
{
    ::Widget *parentWidget = nullptr;
    if (parent != nullptr)
    {
        PluginWidgetImpl *parentImpl = dynamic_cast<PluginWidgetImpl*>(parent);
        parentWidget = parentImpl->getWidget();
    }

    mWidget = new PluginTextBoxIntl(fromPluginVec(body.position), body.size.x, this, parentWidget);
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