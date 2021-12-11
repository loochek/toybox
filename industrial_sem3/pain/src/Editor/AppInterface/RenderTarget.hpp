#ifndef RENDER_TARGET_HPP
#define RENDER_TARGET_HPP

#include "EditorPluginAPI/plugin_std.hpp"
#include "../../LGL/LGL.hpp"

class RenderTargetImpl : public P::RenderTarget
{
public:
    virtual P::RenderTarget *get_copy() const override;

    virtual P::Vec2s get_size() const override;

    virtual P::RGBA get_pixel(size_t x, size_t y) const override;
    virtual void    set_pixel(size_t x, size_t y, P::RGBA color) override;

    virtual P::RGBA *get_pixels() override;

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
    RenderTargetImpl(const Vec2i &size);
    void handleBlendMode(const P::RenderMode &mode);

private:
    LGL::RenderTexture mTarget;
    friend class RenderTargetFactoryImpl;
};

class RenderTargetFactoryImpl : public P::RenderTargetFactory
{
public:
    virtual P::RenderTarget *spawn(P::Vec2s size, P::RGBA color = {0, 0, 0, 255}) const override;
    virtual P::RenderTarget *from_pixels(P::Vec2s size, const P::RGBA *data) const override;
    virtual P::RenderTarget *from_file(const char *path) const override;
    virtual void release(P::RGBA *data) const override;
};

#endif