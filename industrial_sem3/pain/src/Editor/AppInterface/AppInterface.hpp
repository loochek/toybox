#ifndef APP_INTERFACE_HPP
#define APP_INTERFACE_HPP

#include <string_view>
#include "../EditorPluginAPI/plugin_std.hpp"

class PaintController;

class AppInterfaceImpl : public PUPPY::AppInterface
{
public:
    virtual ~AppInterfaceImpl();

    virtual bool  ext_enable       (const char *name) const override;
    virtual void *ext_get_func     (const char *extension, const char *func) const override;
    virtual void *ext_get_interface(const char *extension, const char *name) const override;
    virtual void  ext_register_as  (const char *extension) const override;

    virtual void    log(const char *fmt, ...) const override;
    virtual double  get_absolute_time() const override;

    virtual PUPPY::RGBA get_color() const override;
    virtual float   get_size()  const override;

    virtual void set_color(const PUPPY::RGBA &color) const override;
    virtual void set_size(float size) const override;

    virtual PUPPY::RenderTarget *get_target() const override;
    virtual PUPPY::RenderTarget *get_preview() const override;
    virtual void                 flush_preview() const override;

    virtual const std::vector<PUPPY::WBody> get_windows() const override;
    virtual PUPPY::Widget *get_root_widget() const override;

private:
    AppInterfaceImpl(PaintController *controller);

private:
    PaintController *mController;
    PUPPY::Widget *mRootWrap;

    friend class PluginManager;
};

#endif