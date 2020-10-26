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
struct fb_var_screeninfo vinfo; // because of getting resolution
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
        printf("Error: cannot open framebuffer device.%x\n", fbfd);
        return -1;
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information  
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
	{
        printf("Error reading fixed information.\n");
        return -2;
    }
    printf("\ntype:0x%x\n", finfo.type);
    printf("visual:%d\n", finfo.visual);
    printf("line_length:%d\n", finfo.line_length);
    printf("\nsmem_start:0x%lx,smem_len:%u\n", finfo.smem_start, finfo.smem_len);
    printf("mmio_start:0x%lx ,mmio_len:%u\n", finfo.mmio_start, finfo.mmio_len);

    // Get variable screen information  
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, & vinfo))
	{
        printf("Error reading variable information.\n");
        return -3;
    }
    printf("%dx%d, %dbpp,xres_virtual=%d,yres_virtual=%dvinfo.xoffset=%d,vinfo.yoffset=%d\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, vinfo.xres_virtual, vinfo.yres_virtual, vinfo.xoffset, vinfo.yoffset);

    screensize = finfo.line_length * vinfo.yres_virtual;
    // Map the device to memory
    fbp = (char*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (fbp == (void*)-1)
	{
        printf("Error: failed to map framebuffer device to memory.\n");
        return -4;
    }
    printf("The framebuffer device was mapped to memory successfully.\n");
	return 0;
}

void fb_close()
{
	munmap(fbp, screensize);
    close(fbfd);
}

void update()
{
	if (ioctl(fbfd, FBIOPAN_DISPLAY, &vinfo))
        printf("Update error!\n");
}

void clear()
{
	memset(fbp, 0, screensize);
}

void put_pixel(int x, int y, int r, int g, int b, int a)
{
	if (x >= vinfo.xres || y >= vinfo.yres)
		return;
	
	int location = x * (vinfo.bits_per_pixel / 8) + y * finfo.line_length;
	*(fbp + location) = (*(fbp + location) * (255 - a) + r * a) / 255;
	*(fbp + location + 1) = (*(fbp + location + 1) * (255 - a) + g * a) / 255;
	*(fbp + location + 2) = (*(fbp + location + 2) * (255 - a) + b * a) / 255;
	*(fbp + location + 3) = 0;
}

void fill_rect(int x_offset, int y_offset, int w, int h, int r, int g, int b, int a)
{
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			put_pixel(x_offset + x, y_offset + y, r, g, b, a);
}

void rect(int x_offset, int y_offset, int w, int h, int r, int g, int b, int a, int border_thickness)
{
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			if ((y <= border_thickness || h - y <= border_thickness) || (x <= border_thickness || w - x <= border_thickness))
				put_pixel(x_offset + x, y_offset + y, r, g, b, a);
}

void sub_image(int x_offset, int y_offset, int x_img_offset, int y_img_offset, int width, int height, const struct image img)
{
	for (int y = y_img_offset; y < y_img_offset + height; y++)
	{
		for (int x = x_img_offset; x < x_img_offset + width; x++)
		{
			int img_loc = y * img.width * 4 + x * 4;
			put_pixel(x_offset + x - x_img_offset, y_offset + y - y_img_offset, img.data[img_loc], img.data[img_loc + 1], img.data[img_loc + 2], img.data[img_loc + 3]); 
		}
	}
}

void letter(int x_offset, int y_offset, const char let, const struct font fnt)
{
	const char* pos = strchr(fnt.charset, let);
	int x_let_offset = 0;
	if (pos != NULL)
		x_let_offset += fnt.char_width * (pos - fnt.charset);
	sub_image(x_offset, y_offset, x_let_offset, 0, fnt.char_width, fnt.char_height, *fnt.graphics);
}

void label(int x_offset, int y_offset, const char* str, const struct font fnt)
{
	int line = 0;
	int col = 0;
	int i = 0;
	while (*(str + i) != '\0')
	{
		if (*(str + i) == '\n')
		{
			line++;
			col = 0;
		}
		else
		{
			letter(x_offset + fnt.char_width * col, y_offset + fnt.char_height * line, *(str + i), fnt);
			col++;
		}
		i++;
	}
}

void image(int x_offset, int y_offset, const struct image img)
{
	sub_image(x_offset, y_offset, 0, 0, img.width, img.height, img);
}
