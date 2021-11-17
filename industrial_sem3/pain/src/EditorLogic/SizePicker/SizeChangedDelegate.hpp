#ifndef SIZE_CHANGED_DELEGATE_HPP
#define SIZE_CHANGED_DELEGATE_HPP

class SizeChangedDelegate
{
public:
    virtual ~SizeChangedDelegate() {};

    virtual void onSizeChange(float size, int userData) = 0;
};

#endif