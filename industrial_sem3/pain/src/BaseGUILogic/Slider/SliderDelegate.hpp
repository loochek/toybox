#ifndef SLIDER_DELEGATE_HPP
#define SLIDER_DELEGATE_HPP

class SliderDelegate
{
public:
    virtual ~SliderDelegate() {};

    virtual void onValueChange(int newValue, int userData) = 0;
};

#endif