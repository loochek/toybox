#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "../../common/headers/lerror.h"

#include "fblib.h"

int fb_init(framebuffer_t *fb)
{
    __lerrno = LERR_NO_ERROR;

    if (fb == NULL)
    {
        LERR(LERR_FB, "null pointer was passed");
        return -1;
    }

	// Open the file for reading and writing
#ifdef __ANDROID__
	fb->fbfd = open("/dev/graphics/fb0", O_RDWR, 0);
#else
	fb->fbfd = open("/dev/fb0", O_RDWR, 0);
#endif
    if (fb->fbfd < 0)
	{
        LERR(LERR_FB, "can't open framebuffer device");
        return -1;
    }

    // Get fixed screen information  
    if (ioctl(fb->fbfd, FBIOGET_FSCREENINFO, &fb->finfo))
	{
        LERR(LERR_FB, "reading fixed information failed");
        return -1;
    }

    // Get variable screen information  
    if (ioctl(fb->fbfd, FBIOGET_VSCREENINFO, &fb->vinfo))
	{
        LERR(LERR_FB, "reading variable information failed");
        return -1;
    }

    fb->screensize = fb->finfo.line_length * fb->vinfo.yres_virtual;
    // Map the device to memory
    fb->fbptr = (char*)mmap(0, fb->screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb->fbfd, 0);
    if (fb->fbptr == (void*)-1)
	{
        LERR(LERR_FB, "failed to map framebuffer device to memory");
        return -1;
    }
	return 0;
}

void fb_deinit(framebuffer_t *fb)
{
    if (fb == NULL)
        return;

	munmap(fb->fbptr, fb->screensize);
    close(fb->fbfd);
}

int fb_update(framebuffer_t *fb)
{
    if (fb == NULL)
        return -1;

	if (ioctl(fb->fbfd, FBIOPAN_DISPLAY, &fb->vinfo))
        return -1;
	return 0;
}

void fb_put_pixel(framebuffer_t *fb, int x, int y, int r, int g, int b, int a)
{
    if (fb == NULL)
        return;
        
	if (x >= fb->vinfo.xres || y >= fb->vinfo.yres)
		return;
	
	int location = x * (fb->vinfo.bits_per_pixel / 8) + y * fb->finfo.line_length;
	*(fb->fbptr + location    ) = (*(fb->fbptr + location    ) * (255 - a) + b * a) / 255;
	*(fb->fbptr + location + 1) = (*(fb->fbptr + location + 1) * (255 - a) + g * a) / 255;
	*(fb->fbptr + location + 2) = (*(fb->fbptr + location + 2) * (255 - a) + r * a) / 255;
	*(fb->fbptr + location + 3) = 0;
}

void fb_get_resolution(framebuffer_t *fb, size_t *width, size_t *height)
{
	*width  = fb->vinfo.xres;
	*height = fb->vinfo.yres;
}