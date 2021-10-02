#include "GUIManager.hpp"

GUIManager::GUIManager(Graphics &graphics) : graphics(graphics)
{
}

void GUIManager::addWidget(GUIWidget *widget)
{
    widgets.push_back(widget);
}

void GUIManager::update(const Vec2f &mousePosition, bool mousePressed)
{
    for (GUIWidget *widget : widgets)
    {
        bool selected = widget->testShape(mousePosition);

        if (selected && mousePressed)
        {
            if (widget->state == WidgetState::Selected)
                widget->state = WidgetState::Clicked;
            else
                widget->state = WidgetState::Pressed;
        }
        else if (selected)
        {
            if (widget->state == WidgetState::Pressed)
                widget->state = WidgetState::Released;
            else
                widget->state = WidgetState::Selected;
        }
        else
        {
            if (widget->state == WidgetState::Pressed)
                widget->state = WidgetState::Released;
            else
                widget->state = WidgetState::Idle;
        }

        widget->update();
    }
}

void GUIManager::draw()
{
    for (GUIWidget *widget : widgets)
        widget->draw(graphics);
}

GUIManager::~GUIManager()
{
    for (GUIWidget *widget : widgets)
        delete widget;
}