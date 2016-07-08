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

#include "stb/stb_rect_pack.h"

struct atlas_s {
	image_t*		baked_image;
	uint32			image_count;
	rect_t*			coordinates;
};


static stbrp_rect
image_to_rect(uint32 id, const image_t* img) {
	stbrp_rect	rect;
	rect.w	= (uint16) (image_width(img)  + 1);
	rect.h	= (uint16) (image_height(img) + 1);
	rect.id	= (uint16)id;
	rect.was_packed	= 0;
	rect.x	= 0;
	rect.y	= 0;
	return rect;
}

static uint32
find_best_size(uint32 img_count, const image_t** imgs) {
	static uint32	texture_size[] = { 128,	256, 512, 1024, 2048 };
	bool			success	= true;
	uint32			size	= 0;
	uint32			r;
	uint32			s;

	stbrp_rect*		rects	= (stbrp_rect*)malloc(sizeof(stbrp_rect) * img_count);
	for( r = 0; r < img_count; ++r ) {
		rects[r]	= image_to_rect(r, imgs[r]);
	}

	for( s = 0; s < sizeof(texture_size) / sizeof(uint32); ++s ) {
		stbrp_context	ctx;
		uint32			r;
		uint32			width	= texture_size[s];
		stbrp_node*		nodes	= (stbrp_node*)malloc(sizeof(stbrp_node) * width * 2);
		memset(nodes, 0, sizeof(stbrp_node) * width * 2);

		stbrp_init_target(&ctx, (sint32)width, (sint32)width, nodes, (sint32)width * 2);
		stbrp_pack_rects(&ctx, rects, (sint32)img_count);

		free(nodes);

		/* check if all rectangles were packed */
		success	= true;
		for( r = 0; r < img_count; ++r ) {
			if( !rects[r].was_packed ) {
				success	= false;
				size	= 0;
				break;
			}
		}

		if( success ) {
			size	= width;
			break;
		}
	}

	free(rects);

	if( !success ) return 0;
	else return size;
}


atlas_t*
atlas_make(uint32 image_count, const image_t** images) {
	stbrp_rect*	rects	= NULL;
	stbrp_node*	nodes	= NULL;
	stbrp_context	ctx;
	uint32		r;
	uint32		best_size;
	image_t*	tex		= NULL;
	atlas_t*	atlas	= NULL;
	rect_t*		drects	= NULL;

	/* try to find the best texture size */
	best_size	= find_best_size(image_count, images);

	/* create the texture and fill in the pixels */
	tex	= image_allocate(best_size, best_size, PF_R8G8B8A8);
	assert( NULL != tex );

	/* image to rect */
	rects	= (stbrp_rect*)malloc(sizeof(stbrp_rect) * image_count);
	assert( NULL != rects );

	for( r = 0; r < image_count; ++r ) {
		rects[r]	= image_to_rect(r, images[r]);
	}

	/* nodes */
	nodes	= (stbrp_node*)malloc(sizeof(stbrp_node) * best_size * 2);
	assert( NULL != nodes );

	memset(nodes, 0, sizeof(stbrp_node) * best_size * 2);

	/* desitnation rectangles */
	drects	= (rect_t*)malloc(sizeof(rect_t) * image_count);
	assert( NULL != drects );

	memset(drects, 0, sizeof(rect_t) * image_count);

	/* pack */
	stbrp_init_target(&ctx, (int)best_size, (int)best_size, nodes, (int)best_size * 2);
	stbrp_pack_rects(&ctx, rects, (int)image_count);

	free(nodes);

	/* copy the rectangle */
	for( r = 0; r < image_count; ++r ) {
		uint32	y;
		uint32	h	= image_height(images[r]);
		uint32	w	= image_width(images[r]);

		assert( rects[r].was_packed );

		drects[r].x	= rects[r].x;
		drects[r].y	= rects[r].y;
		drects[r].width	= rects[r].w;
		drects[r].height= rects[r].h;

		for( y = 0; y < h ; ++y ) {
			uint32	x;
			for( x = 0; x < w; ++x ) {
				color4b_t	src	= image_get_pixelb(images[r], x, y);
				image_set_pixelb(tex, rects[r].x + x, rects[r].y + y, src);
			}
		}
	}

	/* release resources */
	free(rects);

	/* final result */
	atlas	= (atlas_t*)malloc(sizeof(atlas_t));
	assert( NULL != atlas );

	atlas->baked_image	= tex;
	atlas->coordinates	= drects;
	atlas->image_count	= image_count;
	return atlas;
}

void
atlas_release(atlas_t* atlas) {
	image_release(atlas->baked_image);
	free(atlas->coordinates);
	free(atlas);
}
