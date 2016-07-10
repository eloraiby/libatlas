/*
** Atlas library Copyright 2016(c) Wael El Oraiby. All Rights Reserved
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** Under Section 7 of GPL version 3, you are granted additional
** permissions described in the GCC Runtime Library Exception, version
** 3.1, as published by the Free Software Foundation.
**
** You should have received a copy of the GNU General Public License and
** a copy of the GCC Runtime Library Exception along with this program;
** see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
** <http://www.gnu.org/licenses/>.
**
*/
#include "atlas.h"
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <png.h>

struct image_s {
	uint32			width;
	uint32			height;
	PIXEL_FORMAT	format;
	void*			pixels;
};

uint32
image_width(const image_t* img) {
	return img->width;
}

uint32
image_height(const image_t* img) {
	return img->height;
}

PIXEL_FORMAT
image_format(const image_t* img) {
	return img->format;
}

static inline image_t*
image_allocate(uint32 width, uint32 height, PIXEL_FORMAT fmt) {
	uint32		ps	= 0;
	image_t*	ret	= NULL;

	switch(fmt) {
	case PF_A8		: ps = 1; break;
	case PF_R8G8B8	: ps = 3; break;
	case PF_R8G8B8A8: ps = 4; break;
	default:
		fprintf(stderr, "ERROR: image_allocate: unsupported input format 0x%X\n", fmt);
		return NULL;
	}

	ret	= (image_t*)malloc(sizeof(image_t));
	assert( NULL != ret );

	ret->width	= width;
	ret->height	= height;
	ret->format	= fmt;
	ret->pixels	= malloc(width * height * ps);
	assert( NULL != ret->pixels );

	return ret;
}

typedef void		(*pixel_setb_fun_t)(void*, color4b_t);
typedef void		(*pixel_setf_fun_t)(void*, color4_t);
typedef color4b_t	(*pixel_getb_fun_t)(void*);
typedef color4_t	(*pixel_getf_fun_t)(void*);


static inline void
set_pixelb_a8(void* img, color4b_t col) {
	uint8*	d	= (uint8*)img;
	d[0]	= col.a;
}

static inline void
set_pixelb_r8g8b8(void* img, color4b_t col) {
	uint8*	d	= (uint8*)img;
	d[0]	= col.r;
	d[1]	= col.g;
	d[2]	= col.b;
}

static inline void
set_pixelb_r8g8b8a8(void* img, color4b_t col) {
	uint8*	d	= (uint8*)img;
	d[0]	= col.r;
	d[1]	= col.g;
	d[2]	= col.b;
	d[3]	= col.a;
}

static inline void
set_pixelf_a8(void* img, color4_t col) {
	uint8*	d	= (uint8*)img;
	d[0]	= (uint8)(col.a * 255.0f);
}

static inline void
set_pixelf_r8g8b8(void* img, color4_t col) {
	uint8*	d	= (uint8*)img;
	d[0]	= (uint8)(col.r * 255.0f);
	d[1]	= (uint8)(col.g * 255.0f);
	d[2]	= (uint8)(col.b * 255.0f);
}

static inline void
set_pixelf_r8g8b8a8(void* img, color4_t col) {
	uint8*	d	= (uint8*)img;
	color4b_t	rgba	= color4b((uint8)(col.r * 255.0f), (uint8)(col.g * 255.0f), (uint8)(col.b * 255.0f), (uint8)(col.a * 255.0f));
	d[0]	= rgba.r;
	d[1]	= rgba.g;
	d[2]	= rgba.b;
	d[3]	= rgba.a;
}

image_t*
image_initb(uint32 width, uint32 height, PIXEL_FORMAT fmt, void* initial_state, image_initb_fun_t filler) {
	void*		state	= initial_state;
	image_t*	img	= image_allocate(width, height, fmt);
	uint32		pixel_size	= 0;
	uint8*		data	= (uint8*)img->pixels;
	pixel_setb_fun_t	fun	= NULL;

	switch(fmt) {
	case PF_A8:			pixel_size	= 1; fun	= set_pixelb_a8;		break;
	case PF_R8G8B8:		pixel_size	= 3; fun	= set_pixelb_r8g8b8;	break;
	case PF_R8G8B8A8:	pixel_size	= 4; fun	= set_pixelb_r8g8b8a8;	break;
	}

	for( uint32 y = 0; y < height; ++y ) {
		for( uint32 x = 0; x < width; ++x ) {
			uint32		offset	= (x + y * width) * pixel_size;
			color4b_t	col	= filler(state, x, y);
			fun(&data[offset], col);
		}
	}

	return img;
}

image_t*
image_initf(uint32 width, uint32 height, PIXEL_FORMAT fmt, void* initial_state, image_initf_fun_t filler) {
	void*		state	= initial_state;
	image_t*	img	= image_allocate(width, height, fmt);
	uint32		pixel_size	= 0;
	uint8*		data	= (uint8*)img->pixels;
	pixel_setf_fun_t	fun	= NULL;

	switch(fmt) {
	case PF_A8:			pixel_size	= 1; fun	= set_pixelf_a8;		break;
	case PF_R8G8B8:		pixel_size	= 3; fun	= set_pixelf_r8g8b8;	break;
	case PF_R8G8B8A8:	pixel_size	= 4; fun	= set_pixelf_r8g8b8a8;	break;
	}

	for( uint32 y = 0; y < height; ++y ) {
		for( uint32 x = 0; x < width; ++x ) {
			uint32		offset	= (x + y * width) * pixel_size;
			color4_t	col	= filler(state, x, y);
			fun(&data[offset], col);
		}
	}

	return img;
}

static inline color4b_t
get_pixelb_a8(void* img) {
	uint8*	pixels	= (uint8*)img;
	return color4b(0xFF, 0xFF, 0xFF, pixels[0]);
}

static inline color4b_t
get_pixelb_r8g8b8(void* img) {
	uint8*	pixels	= (uint8*)img;
	return color4b(pixels[0], pixels[1], pixels[2], 0xFF);
}

static inline color4b_t
get_pixelb_r8g8b8a8(void* img) {
	uint8*	pixels	= (uint8*)img;
	return color4b(pixels[0], pixels[1], pixels[2], pixels[3]);
}

static inline color4_t
get_pixelf_a8(void* img) {
	uint8*	pixels	= (uint8*)img;
	return color4(1.0f, 1.0f, 1.0f, ((float)pixels[0]) / 255.0f);
}

static inline color4_t
get_pixelf_r8g8b8(void* img) {
	uint8*	pixels	= (uint8*)img;
	return color4(((float)pixels[0]) / 255.0f, ((float)pixels[1]) / 255.0f, ((float)pixels[2]) / 255.0f, 1.0f);
}

static inline color4_t
get_pixelf_r8g8b8a8(void* img) {
	uint8*	pixels	= (uint8*)img;
	return color4(((float)pixels[0]) / 255.0f, ((float)pixels[1]) / 255.0f, ((float)pixels[2]) / 255.0f, ((float)pixels[3]) / 255.0f);
}

void*
image_foldb(const image_t* img, void* initial_state, image_foldb_fun_t f) {
	void*				state	= initial_state;
	uint32				pixel_size	= 0;
	uint8*				data	= (uint8*)img->pixels;
	pixel_getb_fun_t	fun	= NULL;
	color4b_t			col;
	uint32				width	= img->width;
	uint32				height	= img->height;

	switch(img->format) {
	case PF_A8:			pixel_size	= 1; fun	= get_pixelb_a8;		break;
	case PF_R8G8B8:		pixel_size	= 3; fun	= get_pixelb_r8g8b8;	break;
	case PF_R8G8B8A8:	pixel_size	= 4; fun	= get_pixelb_r8g8b8a8;	break;
	}

	for( uint32 y = 0; y < height; ++y ) {
		for( uint32 x = 0; x < width; ++x ) {
			uint32	offset	= (x + y * width) * pixel_size;
			col		= fun(&data[offset]);
			state	= f(state, x, y, col);
		}
	}

	return state;
}

void*
image_foldf(const image_t* img, void* initial_state, image_foldf_fun_t f) {
	void*				state	= initial_state;
	uint32				pixel_size	= 0;
	uint8*				data	= (uint8*)img->pixels;
	pixel_getf_fun_t	fun	= NULL;
	color4_t			col;
	uint32				width	= img->width;
	uint32				height	= img->height;

	switch(img->format) {
	case PF_A8:			pixel_size	= 1; fun	= get_pixelf_a8;		break;
	case PF_R8G8B8:		pixel_size	= 3; fun	= get_pixelf_r8g8b8;	break;
	case PF_R8G8B8A8:	pixel_size	= 4; fun	= get_pixelf_r8g8b8a8;	break;
	}

	for( uint32 y = 0; y < height; ++y ) {
		for( uint32 x = 0; x < width; ++x ) {
			uint32	offset	= (x + y * width) * pixel_size;
			col		= fun(&data[offset]);
			state	= f(state, x, y, col);
		}
	}

	return state;
}

void
image_release(image_t* img) {
	free(img->pixels);
	free(img);
}

