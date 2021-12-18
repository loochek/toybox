#ifndef RENDER_TARGET_HPP
#define RENDER_TARGET_HPP

#include "../EditorPluginAPI/plugin_std.hpp"
#include "../../LGL/LGL.hpp"

/**
 * PUPPY::RenderTarget implementation as LGL::RenderTexture wrapper
 */
class RenderTargetImpl : public PUPPY::RenderTarget
{
public:
    RenderTargetImpl() = delete;
    
    /**
     * \param isRef Passed render texture will be deleted in destructor if false
     */
    RenderTargetImpl(LGL::RenderTexture *texture, bool isRef = false);

    virtual ~RenderTargetImpl();

    virtual PUPPY::RenderTarget *get_copy() const override;

    virtual PUPPY::Vec2s get_size() const override;

    virtual PUPPY::RGBA get_pixel(size_t x, size_t y) const override;
    virtual void    set_pixel(size_t x, size_t y, const PUPPY::RGBA &color) override;

    virtual PUPPY::RGBA *get_pixels() const override;

    virtual void clear(const PUPPY::RGBA &color = 0) override; // fills the target with `color`
    
    virtual void render_circle(const PUPPY::Vec2f &position, float radius, const PUPPY::RGBA &color, const PUPPY::RenderMode &render_mode = {}) override;
    virtual void render_line(const PUPPY::Vec2f &start, const PUPPY::Vec2f &end, const PUPPY::RGBA &color, const PUPPY::RenderMode &render_mode = {}) override;
    virtual void render_triangle(const PUPPY::Vec2f &p1, const PUPPY::Vec2f &p2, const PUPPY::Vec2f &p3, const PUPPY::RGBA &color, const PUPPY::RenderMode &render_mode = {}) override;
    virtual void render_rectangle(const PUPPY::Vec2f &p1, const PUPPY::Vec2f &p2, const PUPPY::RGBA &color, const PUPPY::RenderMode &render_mode = {}) override;
    
    virtual void render_texture(const PUPPY::Vec2f &position, const RenderTarget *texture, const PUPPY::RenderMode &render_mode = {}) override;
    virtual void render_pixels(const PUPPY::Vec2f &position, const PUPPY::Vec2s &size, const PUPPY::RGBA *data, const PUPPY::RenderMode &render_mode = {}) override;

    virtual void apply_shader(const PUPPY::Shader *shader) override;

    LGL::RenderTexture *getTexture() { return mTarget; };

private:
    void handleBlendMode(const PUPPY::RenderMode &mode);

private:
    bool mIsRef;

    LGL::RenderTexture *mTarget;
    friend class RenderTargetFactoryImpl;
};

class RenderTargetFactoryImpl : public PUPPY::RenderTargetFactory
{
public:
    virtual PUPPY::RenderTarget *spawn(const PUPPY::Vec2s &size, const PUPPY::RGBA &color = {0, 0, 0, 255}) const override;
    virtual PUPPY::RenderTarget *from_pixels(const PUPPY::Vec2s &size, const PUPPY::RGBA *data) const override;
    virtual PUPPY::RenderTarget *from_file(const char *path) const override;
};

#endif