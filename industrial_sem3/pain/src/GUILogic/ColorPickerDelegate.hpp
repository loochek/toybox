#ifndef COLOR_PICKER_DELEGATE_HPP
#define COLOR_PICKER_DELEGATE_HPP

#include <unordered_set>
#include "../LGL/LGL.hpp"
#include "../GUIBase/Canvas.hpp"
#include "../GUIElements/ColorPicker.hpp"
#include "ButtonDelegate.hpp"

class ColorPickerDelegate : public ButtonDelegate
{
public:
    virtual void onClick(int userData) override
    {
        // for (Canvas *canvas : mCanvases)
        //     canvas->setDrawingColor(ColorPicker::PICKER_COLORS[userData]);
    }

    void addCanvas   (Canvas *canvas) { mCanvases.insert(canvas); };
    void removeCanvas(Canvas *canvas) { mCanvases.erase(canvas); };

protected:
    std::unordered_set<Canvas*> mCanvases;
};

#endif