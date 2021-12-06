#ifndef IMAGE_OPEN_WINDOW_HPP
#define IMAGE_OPEN_WINDOW_HPP

#include "../BaseGUI/Window.hpp"

class ImageOpenCloseDelegate;
class PaintController;
class ImageOpenDelegate;
class TextBox;

class ImageOpenWindow : public Window
{
public:
    ImageOpenWindow() = delete;
    ImageOpenWindow(const Vec2i &windowPos, PaintController *controller, Widget *parent = nullptr);

    virtual ~ImageOpenWindow();
    
protected:
    ImageOpenCloseDelegate *mCloseButtonDelegate;
    ImageOpenDelegate      *mOpenDelegate;

    TextBox *mFileNameBox;

    friend class ImageOpenDelegate;
};

#endif