#include "../TextureManager.hpp"
#include "../BaseGUI/Label.hpp"
#include "../Editor/AppInterface/PluginTypesProxy.hpp"
#include "PluginChooseButton.hpp"

const int LABEL_HEIGHT_OFFSET = 20;
const int ICON_HEIGHT_OFFSET  = 10;

PluginChooseButton::PluginChooseButton(const IntRect &widgetRect, Plugin *plugin, Widget *parent) :
    Button(widgetRect, parent), mPlugin(plugin)
{
    mDefaultIconTexture = TextureManager::getInstance()->getTexture("default_plugin");
    if (mDefaultIconTexture == nullptr)
        throw std::runtime_error("Default plugin icon is not loaded");

    mPluginIcon = dynamic_cast<const RenderTargetImpl*>(plugin->get_info()->icon);
    if (mPluginIcon != nullptr)
        mIconPos = fromPluginVec(mPluginIcon->get_size());
    else
        mIconPos = mDefaultIconTexture->getSize();

    mIconPos = Vec2i(mRect.size.x / 2 - mIconPos.x / 2,
                     mRect.size.y / 2 - mIconPos.y / 2 - ICON_HEIGHT_OFFSET);

    AnimatedButton::setLabel(plugin->get_info()->name);
    mLabel->setPosition(Vec2i(mRect.size.x / 2 - mLabel->getRect().size.x / 2,
                              mRect.size.y - LABEL_HEIGHT_OFFSET));
}

void PluginChooseButton::onRedrawThis()
{
    Button::onRedrawThis();

    if (mPluginIcon != nullptr)
        mTexture.drawRenderTexture(*mPluginIcon->getTexture(), mIconPos);
    else
        mTexture.drawTexture(*mDefaultIconTexture, mIconPos);
}