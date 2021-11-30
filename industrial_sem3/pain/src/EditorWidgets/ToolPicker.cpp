#include "ToolPicker.hpp"
#include "../BaseGUI/Button.hpp"
#include "../Editor/Plugin.hpp"
#include "../EditorLogic/ToolPicker/ToolPickerController.hpp"

const Vec2i ONE_BUTTON_SIZE = Vec2i(90, 60);

const Vec2i ToolPicker::PICKER_SIZE = Vec2i(90, 360);

ToolPicker::ToolPicker(const Vec2i &position, Widget *parent) :
    Widget(IntRect(position, PICKER_SIZE), parent), mDelegate(nullptr)
{
    mController = new ToolPickerController(this);
}

void ToolPicker::addTool(Plugin *tool)
{
    Button *toolButton = new Button(IntRect(Vec2i(0, ONE_BUTTON_SIZE.y * mTools.size()), ONE_BUTTON_SIZE), this);
    toolButton->setLabel(tool->getInfo()->name);
    toolButton->setDelegate(mController);
    toolButton->setUserData(mTools.size());
    addChild(toolButton);

    mTools.push_back(tool);
}

ToolPicker::~ToolPicker()
{
    delete mController;
}