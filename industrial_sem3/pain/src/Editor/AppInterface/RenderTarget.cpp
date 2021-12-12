#include "../../Utils/Logger.hpp"
#include "PluginTypesProxy.hpp"
#include "RenderTarget.hpp"

const float LINE_THICKNESS = 1.0f;

RenderTargetImpl::RenderTargetImpl(LGL::RenderTexture *texture, bool isRef) :
    mTarget(texture), mIsRef(isRef) {}

RenderTargetImpl::~RenderTargetImpl()
{
    if (!mIsRef)
        delete mTarget;
}

P::RenderTarget *RenderTargetImpl::get_copy() const
{
    LGL::Texture texture;
    texture.loadFromRenderTexture(*mTarget);

    LGL::RenderTexture *copyRT = new LGL::RenderTexture(mTarget->getSize());
    RenderTargetImpl *copy = new RenderTargetImpl(copyRT);
    copy->mTarget->drawTexture(texture, Vec2i());
    return copy;
}

P::Vec2s RenderTargetImpl::get_size() const
{
    return toPluginVec(Vec2<size_t>(mTarget->getSize()));
}

P::RGBA RenderTargetImpl::get_pixel(size_t x, size_t y) const
{
    LGL::Texture texture;
    texture.loadFromRenderTexture(*mTarget);

    return toPluginColor(texture.copyToImage().getPixel(x, y));
}

void RenderTargetImpl::set_pixel(size_t x, size_t y, const P::RGBA &color)
{
    LGL::Texture texture;
    texture.loadFromRenderTexture(*mTarget);
    LGL::Image image = texture.copyToImage();
    image.setPixel(x, y, fromPluginColor(color));
    texture.loadFromImage(image);
    mTarget->drawTexture(texture, Vec2i());
}

P::RGBA *RenderTargetImpl::get_pixels() const
{
    LGL::Texture texture;
    texture.loadFromRenderTexture(*mTarget);

    LGL::Image image = texture.copyToImage();
    Vec2i imageSize = image.getSize();

    P::RGBA *pixels = new P::RGBA[imageSize.x * imageSize.y];
    memcpy(pixels, image.getPixels(), sizeof(P::RGBA) * imageSize.x * imageSize.y);
    return pixels;
}

void RenderTargetImpl::clear(const P::RGBA &color)
{
    mTarget->clear(fromPluginColor(color));
}

void RenderTargetImpl::render_circle(const P::Vec2f &position, float radius, const P::RGBA &color,
                                     const P::RenderMode &render_mode)
{
    handleBlendMode(render_mode);
    mTarget->drawCircle(fromPluginVec(position), radius, fromPluginColor(color));
}

void RenderTargetImpl::render_line(const P::Vec2f &start, const P::Vec2f &end, const P::RGBA &color,
                                   const P::RenderMode &render_mode)
{
    handleBlendMode(render_mode);
    mTarget->drawLine(fromPluginVec(start),
                      fromPluginVec(end),
                      LINE_THICKNESS,
                      fromPluginColor(color));
}

void RenderTargetImpl::render_triangle(const P::Vec2f &p1, const P::Vec2f &p2, const P::Vec2f &p3,
                                       const P::RGBA &color, const P::RenderMode &render_mode)
{
    handleBlendMode(render_mode);
    mTarget->drawTriangle(fromPluginVec(p1),
                          fromPluginVec(p2),
                          fromPluginVec(p3),
                          fromPluginColor(color));
}

void RenderTargetImpl::render_rectangle(const P::Vec2f &p1_ext, const P::Vec2f &p2_ext, const P::RGBA &color,
                                        const P::RenderMode &render_mode)
{
    handleBlendMode(render_mode);

    Vec2f p1 = fromPluginVec(p1_ext);
    Vec2f p2 = fromPluginVec(p2_ext);

    mTarget->drawRect(FloatRect(p1, p2 - p1), fromPluginColor(color));
}

void RenderTargetImpl::render_texture(const P::Vec2f &position, const P::RenderTarget *texture, 
                                      const P::RenderMode &render_mode)
{
    handleBlendMode(render_mode);

    RenderTargetImpl *impl = (RenderTargetImpl*)texture;
    mTarget->drawRenderTexture(*impl->mTarget, Vec2f());
}

void RenderTargetImpl::render_pixels(const P::Vec2f &position, const P::Vec2s &size, const P::RGBA *data,
                                     const P::RenderMode &render_mode)
{
    handleBlendMode(render_mode);

    LGL::Image image;
    image.create(size.x, size.y, (const uint8_t*)data);

    LGL::Texture texture;
    texture.loadFromImage(image);

    mTarget->drawTexture(texture, fromPluginVec(position));
}

void RenderTargetImpl::apply_shader(const P::Shader *shader)
{
    Logger::log(LogLevel::Warning, "Some plugin tried to apply shader, but it's not supported");
}

void RenderTargetImpl::handleBlendMode(const P::RenderMode &mode)
{
    if (mode.blend == P::BlendMode::ALPHA_BLEND)
        mTarget->setBlendMode(true);
    else
        mTarget->setBlendMode(false);
}

P::RenderTarget *RenderTargetFactoryImpl::spawn(const P::Vec2s &size, const P::RGBA &color) const
{
    LGL::RenderTexture *rt = new LGL::RenderTexture(fromPluginVec(size));
    rt->clear(fromPluginColor(color));

    RenderTargetImpl *target = new RenderTargetImpl(rt);
    return target;
}

P::RenderTarget *RenderTargetFactoryImpl::from_pixels(const P::Vec2s &size, const P::RGBA *data) const
{
    LGL::RenderTexture *rt = new LGL::RenderTexture(fromPluginVec(size));
    P::RenderTarget *target = new RenderTargetImpl(rt);

    target->render_pixels(P::Vec2f(), size, data);
    return target;
}

P::RenderTarget *RenderTargetFactoryImpl::from_file(const char *path) const
{
    LGL::Texture texture;
    if (!texture.loadFromFile(path))
        return nullptr;

    LGL::RenderTexture *rt = new LGL::RenderTexture(texture.getSize());
    rt->drawTexture(texture, Vec2f());
    
    RenderTargetImpl *target = new RenderTargetImpl(rt);
    return target;
}