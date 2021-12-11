#include "AppInterface.hpp"

// RenderTarget implementation

RenderTargetImpl::~RenderTargetImpl()
{
};

P::RenderTarget *RenderTargetImpl::get_copy() const
{
}

P::Vec2s RenderTargetImpl::get_size() const
{
}

P::RGBA RenderTargetImpl::get_pixel(size_t x, size_t y) const
{
}

void RenderTargetImpl::set_pixel(size_t x, size_t y, P::RGBA color)
{
}

P::RGBA *RenderTargetImpl::get_pixels()
{
}

void RenderTargetImpl::clear(P::RGBA color)
{
}

void RenderTargetImpl::render_circle(P::Vec2f position, float radius, P::RGBA color,
                                     const P::RenderMode *render_mode)
{
}

void RenderTargetImpl::render_line(P::Vec2f start, P::Vec2f end, P::RGBA color,
                                   const P::RenderMode *render_mode)
{
}

void RenderTargetImpl::render_triangle(P::Vec2f p1, P::Vec2f p2, P::Vec2f p3, P::RGBA color,
                                       const P::RenderMode *render_mode)
{
}

void RenderTargetImpl::render_rectangle(P::Vec2f p1, P::Vec2f p2, P::RGBA color,
                                const P::RenderMode *render_mode)
{
}

void RenderTargetImpl::render_texture(P::Vec2f position, const P::RenderTarget *texture, 
                                      const P::RenderMode *render_mode)
{
}

void RenderTargetImpl::render_pixels(P::Vec2f position, const P::RGBA *data, size_t width, size_t height,
                                     const P::RenderMode *render_mode)
{
}

void RenderTargetImpl::apply_shader(const P::Shader *shader)
{
}

// AppInterface implementation

bool AppInterfaceImpl::enable(std::string_view name) const
{
}

void *AppInterfaceImpl::get_func(std::string_view extension, std::string_view func) const
{

}

void *AppInterfaceImpl::get_interface(const char *extension, const char *name) const
{

}

void AppInterfaceImpl::log(const char *fmt, ...) const
{

}

double AppInterfaceImpl::get_absolute_time() const
{

}

P::RGBA AppInterfaceImpl::get_color() const
{

}

float AppInterfaceImpl::get_size() const
{

}

P::RenderTarget *AppInterfaceImpl::get_target() const
{

}

P::RenderTarget *AppInterfaceImpl::get_preview() const
{

}

void AppInterfaceImpl::flush_preview() const
{

}