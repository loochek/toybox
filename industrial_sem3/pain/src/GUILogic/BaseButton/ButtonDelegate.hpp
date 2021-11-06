#ifndef BUTTON_DELEGATE_HPP
#define BUTTON_DELEGATE_HPP

class ButtonDelegate
{
public:
    virtual ~ButtonDelegate() {};

    virtual void onClick(int userData) = 0;
};

#endif