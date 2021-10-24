#ifndef SIZE_PICKER_DELEGATE_HPP
#define SIZE_PICKER_DELEGATE_HPP

#include <unordered_set>
#include "../GUIBase/Canvas.hpp"
#include "../GUIElements/SizePicker.hpp"
#include "ButtonDelegate.hpp"

class SizePickerDelegate : public ButtonDelegate
{
public:
    virtual void onClick(int userData) override
    {
        for (Canvas *canvas : mCanvases)
            canvas->setPenSize(SizePicker::PICKER_SIZES[userData]);
    }

    void addCanvas   (Canvas *canvas) { mCanvases.insert(canvas); };
    void removeCanvas(Canvas *canvas) { mCanvases.erase(canvas); };

protected:
    std::unordered_set<Canvas*> mCanvases;
};

#endif