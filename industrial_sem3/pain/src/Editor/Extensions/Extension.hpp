#ifndef EXTENSION_HPP
#define EXTENSION_HPP

/**
 * Base class for extensions
 */
class Extension
{
public:
    Extension(const char *name) : mName(name) {}
    virtual ~Extension() {}

    const char *getName() { return mName; };

    virtual void *getFunc(const char *funcName) = 0;
    virtual void *getInterface(const char *interfaceName) = 0;

protected:
    const char *mName;
};

#endif