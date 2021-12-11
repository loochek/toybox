#ifndef APP_INTERFACE_HPP
#define APP_INTERFACE_HPP

#include <string_view>
#include "EditorPluginAPI/plugin_std.hpp"
#include "PluginManager.hpp"

/**
 * Interfaces which provided to plugins by application
 */

class RenderTargetImpl : public P::RenderTarget
{
public:
    RenderTargetImpl(PluginManager *pluginMgr);

    virtual ~RenderTargetImpl() {};

    virtual P::RenderTarget *get_copy() const override;

    virtual P::Vec2s get_size() const override;

    virtual P::RGBA get_pixel(size_t x, size_t y) const override;
    virtual void    set_pixel(size_t x, size_t y, P::RGBA color) override;

    virtual P::RGBA *get_pixels() = 0;

    virtual void clear(P::RGBA color = 0) override;

    virtual void render_circle(P::Vec2f position, float radius, P::RGBA color,
                               const P::RenderMode *render_mode) override;
    virtual void render_line(P::Vec2f start, P::Vec2f end, P::RGBA color,
                             const P::RenderMode *render_mode) override;
    virtual void render_triangle(P::Vec2f p1, P::Vec2f p2, P::Vec2f p3, P::RGBA color,
                                 const P::RenderMode *render_mode) override;
    virtual void render_rectangle(P::Vec2f p1, P::Vec2f p2, P::RGBA color,
                                  const P::RenderMode *render_mode) override;
    
    virtual void render_texture(P::Vec2f position, const P::RenderTarget *texture, 
                                const P::RenderMode *render_mode) override;
    virtual void render_pixels(P::Vec2f position, const P::RGBA *data, size_t width, size_t height,
                               const P::RenderMode *render_mode) override;

    virtual void apply_shader(const P::Shader *shader) override;

private:
};

struct AppInterfaceImpl : public P::AppInterface
{
    virtual bool  enable(std::string_view name) const override;
    virtual void *get_func(std::string_view extension, std::string_view func) const override;
    virtual void *get_interface(const char *extension, const char *name) const override;

    virtual void    log(const char *fmt, ...) const override;
    virtual double  get_absolute_time() const override;

    virtual P::RGBA get_color() const override;
    virtual float   get_size()  const override;

    virtual P::RenderTarget *get_target() const override;
    virtual P::RenderTarget *get_preview() const override;
    virtual void             flush_preview() const override;
};

#endif