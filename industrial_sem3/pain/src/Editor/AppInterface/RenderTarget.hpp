#ifndef RENDER_TARGET_HPP
#define RENDER_TARGET_HPP

#include "../EditorPluginAPI/plugin_std.hpp"
#include "../../LGL/LGL.hpp"

/**
 * P::RenderTarget implementation as LGL::RenderTexture wrapper
 */
class RenderTargetImpl : public P::RenderTarget
{
public:
    RenderTargetImpl() = delete;
    
    /**
     * \param isRef Passed render texture will be deleted in destructor if false
     */
    RenderTargetImpl(LGL::RenderTexture *texture, bool isRef = false);

    virtual ~RenderTargetImpl();

    virtual P::RenderTarget *get_copy() const override;

    virtual P::Vec2s get_size() const override;

    virtual P::RGBA get_pixel(size_t x, size_t y) const override;
    virtual void    set_pixel(size_t x, size_t y, const P::RGBA &color) override;

    virtual P::RGBA *get_pixels() const override;

    virtual void clear(const P::RGBA &color = 0) override; // fills the target with `color`
    
    virtual void render_circle(const P::Vec2f &position, float radius, const P::RGBA &color, const P::RenderMode &render_mode = {}) override;
    virtual void render_line(const P::Vec2f &start, const P::Vec2f &end, const P::RGBA &color, const P::RenderMode &render_mode = {}) override;
    virtual void render_triangle(const P::Vec2f &p1, const P::Vec2f &p2, const P::Vec2f &p3, const P::RGBA &color, const P::RenderMode &render_mode = {}) override;
    virtual void render_rectangle(const P::Vec2f &p1, const P::Vec2f &p2, const P::RGBA &color, const P::RenderMode &render_mode = {}) override;
    
    virtual void render_texture(const P::Vec2f &position, const RenderTarget *texture, const P::RenderMode &render_mode = {}) override;
    virtual void render_pixels(const P::Vec2f &position, const P::Vec2s &size, const P::RGBA *data, const P::RenderMode &render_mode = {}) override;

    virtual void apply_shader(const P::Shader *shader) override;

private:
    void handleBlendMode(const P::RenderMode &mode);

private:
    bool mIsRef;

    LGL::RenderTexture *mTarget;
    friend class RenderTargetFactoryImpl;
};

class RenderTargetFactoryImpl : public P::RenderTargetFactory
{
public:
    virtual P::RenderTarget *spawn(const P::Vec2s &size, const P::RGBA &color = {0, 0, 0, 255}) const override;
    virtual P::RenderTarget *from_pixels(const P::Vec2s &size, const P::RGBA *data) const override;
    virtual P::RenderTarget *from_file(const char *path) const override;
};

#endif