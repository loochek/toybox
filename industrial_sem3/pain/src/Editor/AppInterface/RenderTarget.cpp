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

PUPPY::RenderTarget *RenderTargetImpl::get_copy() const
{
    LGL::Texture texture;
    texture.loadFromRenderTexture(*mTarget);

    LGL::RenderTexture *copyRT = new LGL::RenderTexture(mTarget->getSize());
    RenderTargetImpl *copy = new RenderTargetImpl(copyRT);
    copy->mTarget->drawTexture(texture, Vec2i());
    return copy;
}

PUPPY::Vec2s RenderTargetImpl::get_size() const
{
    return toPluginVec(Vec2<size_t>(mTarget->getSize()));
}

PUPPY::RGBA RenderTargetImpl::get_pixel(size_t x, size_t y) const
{
    LGL::Texture texture;
    texture.loadFromRenderTexture(*mTarget);

    return toPluginColor(texture.copyToImage().getPixel(x, y));
}

void RenderTargetImpl::set_pixel(size_t x, size_t y, const PUPPY::RGBA &color)
{
    LGL::Texture texture;
    texture.loadFromRenderTexture(*mTarget);
    LGL::Image image = texture.copyToImage();
    image.setPixel(x, y, fromPluginColor(color));
    texture.loadFromImage(image);
    mTarget->drawTexture(texture, Vec2i());
}

PUPPY::RGBA *RenderTargetImpl::get_pixels() const
{
    LGL::Texture texture;
    texture.loadFromRenderTexture(*mTarget);

    LGL::Image image = texture.copyToImage();
    Vec2i imageSize = image.getSize();

    PUPPY::RGBA *pixels = new PUPPY::RGBA[imageSize.x * imageSize.y];
    memcpy(pixels, image.getPixels(), sizeof(PUPPY::RGBA) * imageSize.x * imageSize.y);
    return pixels;
}

void RenderTargetImpl::clear(const PUPPY::RGBA &color)
{
    mTarget->clear(fromPluginColor(color));
}

void RenderTargetImpl::render_circle(const PUPPY::Vec2f &position, float radius, const PUPPY::RGBA &color,
                                     const PUPPY::RenderMode &render_mode)
{
    handleBlendMode(render_mode);
    mTarget->drawCircle(fromPluginVec(position), radius, fromPluginColor(color));
}

void RenderTargetImpl::render_line(const PUPPY::Vec2f &start, const PUPPY::Vec2f &end, const PUPPY::RGBA &color,
                                   const PUPPY::RenderMode &render_mode)
{
    handleBlendMode(render_mode);
    mTarget->drawLine(fromPluginVec(start),
                      fromPluginVec(end),
                      LINE_THICKNESS,
                      fromPluginColor(color));
}

void RenderTargetImpl::render_triangle(const PUPPY::Vec2f &p1, const PUPPY::Vec2f &p2, const PUPPY::Vec2f &p3,
                                       const PUPPY::RGBA &color, const PUPPY::RenderMode &render_mode)
{
    handleBlendMode(render_mode);
    mTarget->drawTriangle(fromPluginVec(p1),
                          fromPluginVec(p2),
                          fromPluginVec(p3),
                          fromPluginColor(color));
}

void RenderTargetImpl::render_rectangle(const PUPPY::Vec2f &p1_ext, const PUPPY::Vec2f &p2_ext, const PUPPY::RGBA &color,
                                        const PUPPY::RenderMode &render_mode)
{
    handleBlendMode(render_mode);

    Vec2f p1 = fromPluginVec(p1_ext);
    Vec2f p2 = fromPluginVec(p2_ext);

    mTarget->drawRect(FloatRect(p1, p2 - p1), fromPluginColor(color));
}

void RenderTargetImpl::render_texture(const PUPPY::Vec2f &position, const PUPPY::RenderTarget *texture, 
                                      const PUPPY::RenderMode &render_mode)
{
    handleBlendMode(render_mode);

    RenderTargetImpl *impl = (RenderTargetImpl*)texture;
    mTarget->drawRenderTexture(*impl->mTarget, Vec2f());
}

void RenderTargetImpl::render_pixels(const PUPPY::Vec2f &position, const PUPPY::Vec2s &size, const PUPPY::RGBA *data,
                                     const PUPPY::RenderMode &render_mode)
{
    handleBlendMode(render_mode);

    LGL::Image image;
    image.create(size.x, size.y, (const uint8_t*)data);

    LGL::Texture texture;
    texture.loadFromImage(image);

    mTarget->drawTexture(texture, fromPluginVec(position));
}

void RenderTargetImpl::apply_shader(const PUPPY::Shader *shader)
{
    Logger::log(LogLevel::Warning, "Some plugin tried to apply shader, but it's not supported");
}

void RenderTargetImpl::handleBlendMode(const PUPPY::RenderMode &mode)
{
    if (mode.blend == PUPPY::BlendMode::ALPHA_BLEND)
        mTarget->setBlendMode(true);
    else
        mTarget->setBlendMode(false);
}

PUPPY::RenderTarget *RenderTargetFactoryImpl::spawn(const PUPPY::Vec2s &size, const PUPPY::RGBA &color) const
{
    LGL::RenderTexture *rt = new LGL::RenderTexture(fromPluginVec(size));
    rt->clear(fromPluginColor(color));

    RenderTargetImpl *target = new RenderTargetImpl(rt);
    return target;
}

PUPPY::RenderTarget *RenderTargetFactoryImpl::from_pixels(const PUPPY::Vec2s &size, const PUPPY::RGBA *data) const
{
    LGL::RenderTexture *rt = new LGL::RenderTexture(fromPluginVec(size));
    PUPPY::RenderTarget *target = new RenderTargetImpl(rt);

    target->render_pixels(PUPPY::Vec2f(), size, data);
    return target;
}

PUPPY::RenderTarget *RenderTargetFactoryImpl::from_file(const char *path) const
{
    LGL::Texture texture;
    if (!texture.loadFromFile(path))
        return nullptr;

    LGL::RenderTexture *rt = new LGL::RenderTexture(texture.getSize());
    rt->drawTexture(texture, Vec2f());
    
    RenderTargetImpl *target = new RenderTargetImpl(rt);
    return target;
}