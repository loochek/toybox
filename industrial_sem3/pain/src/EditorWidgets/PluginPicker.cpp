#include <cassert>
#include "PluginPicker.hpp"
#include "PluginChooseButton.hpp"
#include "../EditorLogic/PluginPicker/PluginPickerController.hpp"

const Vec2i ONE_BUTTON_SIZE = Vec2i(120, 80);

const Vec2i PluginPicker::PICKER_INIT_SIZE = ONE_BUTTON_SIZE;

PluginPicker::PluginPicker(const Vec2i &position, PUPPY::PluginType pickerType, Widget *parent) :
    Widget(IntRect(position, PICKER_INIT_SIZE), parent), mDelegate(nullptr), mType(pickerType), mButtonsCount(0)
{
    mController = new PluginPickerController(this);
}

void PluginPicker::addPlugin(Plugin *plugin)
{
    assert(plugin->get_info()->type == mType);

    mButtonsCount++;
    resize(Vec2i(ONE_BUTTON_SIZE.x, ONE_BUTTON_SIZE.y * mButtonsCount));
    mParent->resize(mRect.size);

    IntRect pluginButtonRect(Vec2i(0, ONE_BUTTON_SIZE.y * mPlugins.size()), ONE_BUTTON_SIZE);
    Button *pluginButton = new PluginChooseButton(pluginButtonRect, plugin, this);

    pluginButton->setDelegate(mController);
    pluginButton->setUserData(mPlugins.size());
    addChild(pluginButton);

    mPlugins.push_back(plugin);
}

PluginPicker::~PluginPicker()
{
    delete mController;
}