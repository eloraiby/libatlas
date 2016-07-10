#include <stdio.h>
#include <png.h>
#include "atlas.h"

image_t*
image_load_png(const char* path) {
	png_structp		png_ptr;
	png_infop		info_ptr;
	unsigned int	sig_read	= 0;
	int				color_type, interlace_type;
	FILE*			fp;
	png_uint_32		width, height;
	int				bit_depth;
	unsigned int	row_bytes;
	PIXEL_FORMAT	sf			= PF_A8;
	uint32			pixel_size	= 0;
	uint32			width_size	= 0;
	image_t*		tex			= NULL;
	char*			buff		= NULL;
	png_bytepp		row_pointers;
	uint32			r;	/* row */

	if( (fp = fopen(path, "rb")) == NULL ) {
		fprintf(stderr, "ERROR: load_png: %s not found\n", path);
		return NULL;
	}

	/* Create and initialize the png_struct
	* with the desired error handler
	* functions. If you want to use the
	* default stderr and longjump method,
	* you can supply NULL for the last
	* three parameters. We also supply the
	* the compiler header file version, so
	* that we know if the application
	* was compiled with a compatible version
	* of the library. REQUIRED
	*/
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if( png_ptr == NULL ) {
		fclose(fp);

		fprintf(stderr, "ERROR: load_png: %s: invalid PNG format\n", path);
	}

	/* Allocate/initialize the memory
	* for image information. REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);

		fprintf(stderr, "ERROR: load_png: %s: not enough memory or format not supported\n", path);
		return NULL;
	}

	/* Set error handling if you are
	* using the setjmp/longjmp method
	* (this is the normal method of
	* doing things with libpng).
	* REQUIRED unless you set up
	* your own error handlers in
	* the png_create_read_struct()
	* earlier.
	*/
	if (setjmp(png_jmpbuf(png_ptr))) {
		/* Free all of the memory associated
			* with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		/* If we get here, we had a
			* problem reading the file */
		fprintf(stderr, "ERROR: load_png: inconsistant file %s\n", path);
		return NULL;
	}

	/* Set up the output control if
	* you are using standard C streams */
	png_init_io(png_ptr, fp);
	/* If we have already
	* read some of the signature */
	png_set_sig_bytes(png_ptr, sig_read);
	/*
	* If you have enough memory to read
	* in the entire image at once, and
	* you need to specify only
	* transforms that can be controlled
	* with one of the PNG_TRANSFORM_*
	* bits (this presently excludes
	* dithering, filling, setting
	* background, and doing gamma
	* adjustment), then you can read the
	* entire image (including pixels)
	* into the info structure with this
	* call
	*
	* PNG_TRANSFORM_STRIP_16 |
	* PNG_TRANSFORM_PACKING forces 8 bit
	* PNG_TRANSFORM_EXPAND forces to
	* expand a palette into RGB
	*/
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

	row_bytes = png_get_rowbytes(png_ptr, info_ptr);

	switch( color_type ) {
	case PNG_COLOR_TYPE_GRAY:
	case PNG_COLOR_TYPE_GRAY_ALPHA:
		sf	= PF_A8;
		pixel_size	= 1;
		break;
	case PNG_COLOR_TYPE_RGB:
		sf	= PF_R8G8B8;
		pixel_size	= 3;
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		sf	= PF_R8G8B8A8;
		pixel_size	= 4;
		break;
	}

	// align the opengl texture to 4 byte width
	width_size	= width * pixel_size;
	tex			= image_allocate(width, height, sf);

	if( !tex ) {
		/* Clean up after the read,
		* and free any memory allocated */
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		/* Close the file */
		fclose(fp);

		return tex;
	}

	buff			= (char*)tex->pixels;
	row_pointers	= png_get_rows(png_ptr, info_ptr);

	for( r = 0; r < height; r++ ) {
		// note that png is ordered top to
		// bottom, but OpenGL expect it bottom to top
		// so the order or swapped
		memcpy(&(buff[width_size * r]), row_pointers[r], width_size);
	}

	/* Clean up after the read,
	* and free any memory allocated */
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	/* Close the file */
	fclose(fp);

	return tex;
}

int main(int argc, char *argv[])
{
	printf("Hello World!\n");
	return 0;
}
