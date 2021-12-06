#ifndef BUTTON_DELEGATE_HPP
#define BUTTON_DELEGATE_HPP

class ButtonDelegate
{
public:
    virtual ~ButtonDelegate() {};

    virtual void onClick(uint64_t userData) = 0;
};

#endif