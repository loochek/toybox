#include "../../Utils/Logger.hpp"
#include "PluginTypesProxy.hpp"
#include "RenderTarget.hpp"

const float LINE_THICKNESS = 1.0f;

RenderTargetImpl::RenderTargetImpl(const Vec2i &size) : mTarget(size) {}

P::RenderTarget *RenderTargetImpl::get_copy() const
{
    LGL::Texture texture;
    texture.loadFromRenderTexture(mTarget);

    RenderTargetImpl *copy = new RenderTargetImpl(mTarget.getSize());
    copy->mTarget.drawTexture(texture, Vec2i());

    return copy;
}

P::Vec2s RenderTargetImpl::get_size() const
{
    Vec2i size = mTarget.getSize();
    return P::Vec2s(size.x, size.y);
}

P::RGBA RenderTargetImpl::get_pixel(size_t x, size_t y) const
{
    LGL::Texture texture;
    texture.loadFromRenderTexture(mTarget);

    return toPluginColor(texture.copyToImage().getPixel(x, y));
}

void RenderTargetImpl::set_pixel(size_t x, size_t y, P::RGBA color)
{
    LGL::Texture texture;
    texture.loadFromRenderTexture(mTarget);
    LGL::Image image = texture.copyToImage();
    image.setPixel(x, y, fromPluginColor(color));
    texture.loadFromImage(image);
    mTarget.drawTexture(texture, Vec2i());
}

P::RGBA *RenderTargetImpl::get_pixels()
{
    LGL::Texture texture;
    texture.loadFromRenderTexture(mTarget);

    LGL::Image image = texture.copyToImage();
    Vec2i imageSize = image.getSize();

    P::RGBA *pixels = new P::RGBA[imageSize.x * imageSize.y];
    memcpy(pixels, image.getPixels(), sizeof(P::RGBA) * imageSize.x * imageSize.y);
    return pixels;
}

void RenderTargetImpl::clear(P::RGBA color)
{
    mTarget.clear(fromPluginColor(color));
}

void RenderTargetImpl::render_circle(P::Vec2f position, float radius, P::RGBA color,
                                     const P::RenderMode *render_mode)
{
    handleBlendMode(*render_mode);
    mTarget.drawCircle(fromPluginVec(position), radius, fromPluginColor(color));
}

void RenderTargetImpl::render_line(P::Vec2f start, P::Vec2f end, P::RGBA color,
                                   const P::RenderMode *render_mode)
{
    handleBlendMode(*render_mode);
    mTarget.drawLine(fromPluginVec(start),
                     fromPluginVec(end),
                     LINE_THICKNESS,
                     fromPluginColor(color));
}

void RenderTargetImpl::render_triangle(P::Vec2f p1, P::Vec2f p2, P::Vec2f p3, P::RGBA color,
                                       const P::RenderMode *render_mode)
{
    handleBlendMode(*render_mode);
    mTarget.drawTriangle(fromPluginVec(p1),
                         fromPluginVec(p2),
                         fromPluginVec(p3),
                         fromPluginColor(color));
}

void RenderTargetImpl::render_rectangle(P::Vec2f p1_ext, P::Vec2f p2_ext, P::RGBA color,
                                        const P::RenderMode *render_mode)
{
    handleBlendMode(*render_mode);

    Vec2f p1 = fromPluginVec(p1_ext);
    Vec2f p2 = fromPluginVec(p2_ext);

    mTarget.drawRect(FloatRect(p1, p2 - p1), fromPluginColor(color));
}

void RenderTargetImpl::render_texture(P::Vec2f position, const P::RenderTarget *texture, 
                                      const P::RenderMode *render_mode)
{
    handleBlendMode(*render_mode);
}

void RenderTargetImpl::render_pixels(P::Vec2f position, const P::RGBA *data, size_t width, size_t height,
                                     const P::RenderMode *render_mode)
{
    handleBlendMode(*render_mode);

    LGL::Image image;
    image.create(width, height, (const uint8_t*)data);

    LGL::Texture texture;
    texture.loadFromImage(image);

    mTarget.drawTexture(texture, fromPluginVec(position));
}

void RenderTargetImpl::apply_shader(const P::Shader *shader)
{
    Logger::log(LogLevel::Warning, "Some plugin tried to apply shader, but it's not supported");
}

void RenderTargetImpl::handleBlendMode(const P::RenderMode &mode)
{
    if (mode.blend == P::BlendMode::ALPHA_BLEND)
        mTarget.setBlendMode(true);
    else
        mTarget.setBlendMode(false);
}

P::RenderTarget *RenderTargetFactoryImpl::spawn(P::Vec2s size, P::RGBA color) const
{
    RenderTargetImpl *target = new RenderTargetImpl(fromPluginVec(size));
    target->mTarget.clear();

    return target;
}

P::RenderTarget *RenderTargetFactoryImpl::from_pixels(P::Vec2s size, const P::RGBA *data) const
{
    P::RenderTarget *target = new RenderTargetImpl(fromPluginVec(size));

    P::RenderMode mode = { P::BlendMode::COPY, nullptr };
    target->render_pixels(P::Vec2f(), data, size.x, size.y, &mode);
    return target;
}

P::RenderTarget *RenderTargetFactoryImpl::from_file(const char *path) const
{
    LGL::Texture texture;
    if (!texture.loadFromFile(path))
        return nullptr;

    RenderTargetImpl *target = new RenderTargetImpl(texture.getSize());
    target->mTarget.drawTexture(texture, Vec2i());
    return target;
}

void RenderTargetFactoryImpl::release(P::RGBA *data) const
{
    delete[] data;
}