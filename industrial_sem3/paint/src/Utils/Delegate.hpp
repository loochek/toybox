#ifndef DELEGATE_HPP
#define DELEGATE_HPP

/**
 * Basic delegate
 */
class Delegate
{
public:
    virtual ~Delegate() {};

    virtual void operator()() = 0;
};

#endif