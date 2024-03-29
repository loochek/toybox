#include <cassert>
#include "PluginPickerWindow.hpp"
#include "PluginPicker.hpp"
#include "../Editor/PluginManager.hpp"
#include "../BaseGUI/BaseButton.hpp"
#include "../EditorLogic/PluginPickerWindow/PluginPickerCloseDelegate.hpp"

PluginPickerWindow::PluginPickerWindow(const Vec2f &pickerPos, PaintController *controller,
                                       PUPPY::PluginType pickerType, Widget *parent) :
    Window(IntRect(pickerPos, PluginPicker::PICKER_INIT_SIZE), parent)
{
    switch (pickerType)
    {
    case PUPPY::PluginType::TOOL:
        setTitle("Tools");
        break;

    case PUPPY::PluginType::EFFECT:
        setTitle("Effects");
        break;

    default:
        assert(false);
    }

    mPluginPicker = new PluginPicker(Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT), pickerType, this);
    addChild(mPluginPicker);

    PluginManager *pluginMgr = PluginManager::getInstance();

    for (int i = 0; i < pluginMgr->getPluginsCount(); i++)
    {
        Plugin *plugin = pluginMgr->getPlugin(i);
        if (plugin->get_info()->type == pickerType)
            mPluginPicker->addPlugin(plugin);
    }
        
    mCloseButtonDelegate = new PluginPickerCloseDelegate(controller, this);
    mCloseButton->setDelegate(mCloseButtonDelegate);
}

PluginPickerWindow::~PluginPickerWindow()
{
    delete mCloseButtonDelegate;
}