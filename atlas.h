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
#ifndef __ATLAS_LIB__H__
#define __ATLAS_LIB__H__
#include "c99-3d-math/3dmath.h"

/*
 * image.c
 */
typedef enum {
	PF_A8,
	PF_R8G8B8,
	PF_R8G8B8A8
} PIXEL_FORMAT;

typedef struct image_s	image_t;

typedef struct image_loader_state_s image_loader_state_t;

typedef image_loader_state_t*	image_loader_init_fun_t();
typedef image_t*				image_loader_fun_t(image_loader_state_t* state, uint8* bytes, uint32 count);

uint32					image_register_loader(image_loader_init_fun_t initializer, image_loader_fun_t loader);
void					image_unregister_loader(uint32 loader_id, image_loader_state_t* state);

uint32					image_width(const image_t* img);
uint32					image_height(const image_t* img);
PIXEL_FORMAT			image_format(const image_t* img);

image_t*				image_allocate(uint32 width, uint32 height, PIXEL_FORMAT fmt);
void					image_release(image_t* img);
image_t*				image_load_png(const char* path);

/* TODO: these are slow to use for iteration, best case would be more granular function table */
color4b_t				image_get_pixelb(const image_t* img, uint32 x, uint32 y);
color4_t				image_get_pixelf(const image_t* img, uint32 x, uint32 y);

void					image_set_pixelb(image_t* img, uint32 x, uint32 y, color4b_t rgba);
void					image_set_pixelf(image_t* img, uint32 x, uint32 y, color4_t rgba);

/*
 * atlas.c
 */
typedef struct atlas_s atlas_t;

atlas_t*				atlas_make(uint32 image_count, const image_t **images);
void					atlas_release(atlas_t* atlas);

#endif	/* __ATLAS_LIB__H__ */
