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

typedef color4b_t		image_initb_fun_t(void* state, uint32 x, uint32 y);
typedef void*			image_foldb_fun_t(void* state, uint32 x, uint32 y, color4b_t col);

typedef color4_t		image_initf_fun_t(void* state, uint32 x, uint32 y);
typedef void*			image_foldf_fun_t(void* state, uint32 x, uint32 y, color4_t col);

uint32					image_width(const image_t* img);
uint32					image_height(const image_t* img);
PIXEL_FORMAT			image_format(const image_t* img);

image_t*				image_initb(uint32 width, uint32 height, PIXEL_FORMAT fmt, void* initial_state, image_initb_fun_t filler);
image_t*				image_initf(uint32 width, uint32 height, PIXEL_FORMAT fmt, void* initial_state, image_initf_fun_t filler);

void					image_release(image_t* img);

void*					image_foldb(const image_t* img, void* initial_state, image_foldb_fun_t f);
void*					image_foldf(const image_t* img, void* initial_state, image_foldf_fun_t f);

/*
 * atlas.c
 */
typedef struct atlas_s atlas_t;

atlas_t*				atlas_make(const image_t **images, uint32 image_count);
void					atlas_release(atlas_t* atlas);

const image_t*			atlas_baked_image(const atlas_t* atlas);
uint32					atlas_image_count(const atlas_t* atlas);
rect_t					atlas_image_coordinates(const atlas_t* atlas, uint32 img);

#endif	/* __ATLAS_LIB__H__ */
