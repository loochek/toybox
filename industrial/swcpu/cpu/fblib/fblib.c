#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "fblib.h"

static int fbfd;
static char* fbp;
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
static int screensize;

int fb_init()
{
	// Open the file for reading and writing
#ifdef __ANDROID__
	fbfd = open("/dev/graphics/fb0", O_RDWR, 0);
#else
	fbfd = open("/dev/fb0", O_RDWR, 0);
#endif
    if (fbfd < 0)
	{
        printf("fblib: cannot open framebuffer device\n");
        return -1;
    }

    // Get fixed screen information  
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
	{
        printf("fblib: reading fixed information failed\n");
        return -2;
    }

    // Get variable screen information  
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, & vinfo))
	{
        printf("fblib: reading variable information failed\n");
        return -3;
    }

    screensize = finfo.line_length * vinfo.yres_virtual;
    // Map the device to memory
    fbp = (char*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (fbp == (void*)-1)
	{
        printf("fblib: failed to map framebuffer device to memory\n");
        return -4;
    }
	return 0;
}

void fb_close()
{
	munmap(fbp, screensize);
    close(fbfd);
}

int update()
{
	if (ioctl(fbfd, FBIOPAN_DISPLAY, &vinfo))
        return -1;
	return 0;
}

void put_pixel(int x, int y, int r, int g, int b, int a)
{
	if (x >= vinfo.xres || y >= vinfo.yres)
		return;
	
	int location = x * (vinfo.bits_per_pixel / 8) + y * finfo.line_length;
	*(fbp + location    ) = (*(fbp + location    ) * (255 - a) + r * a) / 255;
	*(fbp + location + 1) = (*(fbp + location + 1) * (255 - a) + g * a) / 255;
	*(fbp + location + 2) = (*(fbp + location + 2) * (255 - a) + b * a) / 255;
	*(fbp + location + 3) = 0;
}

void get_resolution(size_t *width, size_t *height)
{
	*width  = vinfo.xres;
	*height = vinfo.yres;
}