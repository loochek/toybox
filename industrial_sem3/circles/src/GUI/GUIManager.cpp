#include "GUIManager.hpp"

GUIManager::GUIManager(Graphics &graphics) : graphics(graphics), mouseManager(this)
{
}

void GUIManager::addWidget(GUIWidget *widget)
{
    widgets.push_back(widget);
}

void GUIManager::update(float elapsedTime)
{
    mouseManager.sendMouseEvents();

    for (GUIWidget *widget : widgets)
        widget->update(elapsedTime);
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