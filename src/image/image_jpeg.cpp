#include "image.h"

#include "common/log.h"

#include "stream/file_stream.h"

#include "jpeglib.h"

#include <csetjmp> // for error handling

namespace scythe {

	bool Image::SaveJpeg(const char *filename, int quality)
	{
		assert(bpp_ == 3 && channels_ == 3);

		/* This struct contains the JPEG compression parameters and pointers to
		* working space (which is allocated as needed by the JPEG library).
		* It is possible to have several such structures, representing multiple
		* compression/decompression processes, in existence at once.  We refer
		* to any one struct (and its associated working data) as a "JPEG object".
		*/
		struct jpeg_compress_struct cinfo;
		/* This struct represents a JPEG error handler.  It is declared separately
		* because applications often want to supply a specialized error handler
		* (see the second half of this file for an example).  But here we just
		* take the easy way out and use the standard error handler, which will
		* print a message on stderr and call exit() if compression fails.
		* Note that this struct must live as long as the main JPEG parameter
		* struct, to avoid dangling-pointer problems.
		*/
		struct jpeg_error_mgr jerr;
		/* More stuff */
		FileStream stream;
		JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
		int row_stride;		/* physical row width in image buffer */

		/* Step 1: allocate and initialize JPEG compression object */

		/* We have to set up the error handler first, in case the initialization
		* step fails.  (Unlikely, but it could happen if you are out of memory.)
		* This routine fills in the contents of struct jerr, and returns jerr's
		* address which we place into the link field in cinfo.
		*/
		cinfo.err = jpeg_std_error(&jerr);
		/* Now we can initialize the JPEG compression object. */
		jpeg_create_compress(&cinfo);

		/* Step 2: specify data destination (eg, a file) */
		/* Note: steps 2 and 3 can be done in either order. */

		/* Here we use the library-supplied code to send compressed data to a
		* stdio stream.  You can also write your own code to do something else.
		* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
		* requires it in order to write binary files.
		*/
		if (!stream.Open(filename, StreamAccess::kWriteBinary))
		{
			SC_ERROR("can't open %s", filename);
			return false;
		}
		jpeg_stdio_dest(&cinfo, stream.GetFilePointer());

		/* Step 3: set parameters for compression */

		/* First we supply a description of the input image.
		* Four fields of the cinfo struct must be filled in:
		*/
		cinfo.image_width = width_; 	/* image width and height, in pixels */
		cinfo.image_height = height_;
		cinfo.input_components = bpp_;		/* # of color components per pixel */
		cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
		/* Now use the library's routine to set default compression parameters.
		* (You must set at least cinfo.in_color_space before calling this,
		* since the defaults depend on the source color space.)
		*/
		jpeg_set_defaults(&cinfo);
		/* Now you can set any non-default parameters you wish to.
		* Here we just illustrate the use of quality (quantization table) scaling:
		*/
		jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

		/* Step 4: Start compressor */

		/* TRUE ensures that we will write a complete interchange-JPEG file.
		* Pass TRUE unless you are very sure of what you're doing.
		*/
		jpeg_start_compress(&cinfo, TRUE);

		/* Step 5: while (scan lines remain to be written) */
		/*           jpeg_write_scanlines(...); */

		/* Here we use the library's state variable cinfo.next_scanline as the
		* loop counter, so that we don't have to keep track ourselves.
		* To keep things simple, we pass one scanline per call; you can pass
		* more if you wish, though.
		*/
		row_stride = width_ * bpp_;	/* JSAMPLEs per row in image_buffer */

		while (cinfo.next_scanline < cinfo.image_height) {
			/* jpeg_write_scanlines expects an array of pointers to scanlines.
			* Here the array is only one element long, but you could pass
			* more than one scanline at a time if that's more convenient.
			*/
			if (inverted_row_order_)
			{
				int row_number = cinfo.image_height - 1 - cinfo.next_scanline;
				row_pointer[0] = &pixels_[row_number * row_stride];
			}
			else // normal row order
				row_pointer[0] = &pixels_[cinfo.next_scanline * row_stride];
			(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
		}

		/* Step 6: Finish compression */

		jpeg_finish_compress(&cinfo);
		/* After finish_compress, we can close the output file. */
		stream.Close();

		/* Step 7: release JPEG compression object */

		/* This is an important step since it will release a good deal of memory. */
		jpeg_destroy_compress(&cinfo);

		/* And we're done! */
		return true;
	}
	struct jpegErrorManager {
		/* "public" fields */
		struct jpeg_error_mgr pub;
		/* for return to caller */
		jmp_buf setjmp_buffer;
	};
	void jpegErrorExit(j_common_ptr cinfo)
	{
		/* cinfo->err actually points to a jpegErrorManager struct */
		jpegErrorManager* myerr = (jpegErrorManager*)cinfo->err;
		/* note : *(cinfo->err) is now equivalent to myerr->pub */

		/* output_message is a method to print an error message */
		(* (cinfo->err->output_message) ) (cinfo);

		/* Create the message */
		/*(*(cinfo->err->format_message)) (cinfo, jpegLastErrorMsg);*/

		/* Jump to the setjmp point */
		longjmp(myerr->setjmp_buffer, 1);
	}
	bool Image::LoadJpeg(const char *filename)
	{
		/* This struct contains the JPEG decompression parameters and pointers to
		* working space (which is allocated as needed by the JPEG library).
		*/
		struct jpeg_decompress_struct cinfo;
		/* We use our private extension JPEG error handler.
		* Note that this struct must live as long as the main JPEG parameter
		* struct, to avoid dangling-pointer problems.
		*/
		jpegErrorManager jerr;
		/* More stuff */
		FileStream stream;
		unsigned char * rowptr[1];
		int row_stride;		/* physical row width in output buffer */

		/* In this example we want to open the input file before doing anything else,
		* so that the setjmp() error recovery below can assume the file is open.
		* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
		* requires it in order to read binary files.
		*/

		if (!stream.Open(filename, StreamAccess::kReadBinary))
		{
			SC_ERROR("can't open %s", filename);
			return false;
		}

		/* Step 1: allocate and initialize JPEG decompression object */

		/* We set up the normal JPEG error routines, then override error_exit. */
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = jpegErrorExit;
		/* Establish the setjmp return context for my_error_exit to use. */
		if (setjmp(jerr.setjmp_buffer)) {
			/* If we get here, the JPEG code has signaled an error. */
			jpeg_destroy_decompress(&cinfo);
			stream.Close();
			return false;
		}

		/* Now we can initialize the JPEG decompression object. */
		jpeg_create_decompress(&cinfo);

		/* Step 2: specify data source (eg, a file) */

		jpeg_stdio_src(&cinfo, stream.GetFilePointer());

		/* Step 3: read file parameters with jpeg_read_header() */

		(void)jpeg_read_header(&cinfo, TRUE);
		/* We can ignore the return value from jpeg_read_header since
		*   (a) suspension is not possible with the stdio data source, and
		*   (b) we passed TRUE to reject a tables-only JPEG file as an error.
		* See libjpeg.txt for more info.
		*/

		/* Step 4: set parameters for decompression */

		/* In this example, we don't need to change any of the defaults set by
		* jpeg_read_header(), so we do nothing here.
		*/

		/* Step 5: Start decompressor */

		(void)jpeg_start_decompress(&cinfo);
		/* We can ignore the return value since suspension is not possible
		* with the stdio data source.
		*/

		width_ = cinfo.output_width;
		height_ = cinfo.output_height;
		channels_ = cinfo.num_components;
		data_type_ = DataType::kUint8;
		switch (channels_)
		{
		case 4:
			bpp_ = 4;
			format_ = Format::kRGBA8;
			break;
		case 3:
			bpp_ = 3;
			format_ = Format::kRGB8;
			break;
		case 2:
			bpp_ = 2;
			format_ = Format::kRG8;
			break;
		case 1:
			bpp_ = 1;
			format_ = Format::kR8;
			break;
			
		default:
			assert(!"Implement this case");
			break;
		}

		pixels_ = new U8[width_ * height_ * bpp_];

		/* We may need to do some setup of our own at this point before reading
		* the data.  After jpeg_start_decompress() we have the correct scaled
		* output image dimensions available, as well as the output colormap
		* if we asked for color quantization.
		* In this example, we need to make an output work buffer of the right size.
		*/
		/* JSAMPLEs per row in output buffer */
		row_stride = cinfo.output_width * cinfo.output_components;

		/* Step 6: while (scan lines remain to be read) */
		/*           jpeg_read_scanlines(...); */

		/* Here we use the library's state variable cinfo.output_scanline as the
		* loop counter, so that we don't have to keep track ourselves.
		*/
		while (cinfo.output_scanline < cinfo.output_height) {
			if (inverted_row_order_)
			{
				int row_number = cinfo.output_height - 1 - cinfo.output_scanline;
				rowptr[0] = pixels_ + row_stride * row_number;
			}
			else // normal row order
				rowptr[0] = pixels_ + row_stride * cinfo.output_scanline;
			/* jpeg_read_scanlines expects an array of pointers to scanlines.
			* Here the array is only one element long, but you could ask for
			* more than one scanline at a time if that's more convenient.
			*/
			(void)jpeg_read_scanlines(&cinfo, rowptr, 1);
		}

		/* Step 7: Finish decompression */

		(void)jpeg_finish_decompress(&cinfo);
		/* We can ignore the return value since suspension is not possible
		* with the stdio data source.
		*/

		/* Step 8: Release JPEG decompression object */

		/* This is an important step since it will release a good deal of memory. */
		jpeg_destroy_decompress(&cinfo);

		/* After finish_decompress, we can close the input file.
		* Here we postpone it until after no more JPEG errors are possible,
		* so as to simplify the setjmp error logic above.  (Actually, I don't
		* think that jpeg_destroy can do an error exit, but why assume anything...)
		*/
		stream.Close();

		/* At this point you may want to check to see whether any corrupt-data
		* warnings occurred (test whether jerr.pub.num_warnings is nonzero).
		*/

		return true;
	}
	bool Image::LoadFromBufferJpeg(const U8* buffer, size_t length)
	{
		/* This struct contains the JPEG decompression parameters and pointers to
		* working space (which is allocated as needed by the JPEG library).
		*/
		struct jpeg_decompress_struct cinfo;
		/* We use our private extension JPEG error handler.
		* Note that this struct must live as long as the main JPEG parameter
		* struct, to avoid dangling-pointer problems.
		*/
		jpegErrorManager jerr;
		/* More stuff */
		unsigned char * rowptr[1];
		int row_stride;		/* physical row width in output buffer */

		/* Step 1: allocate and initialize JPEG decompression object */

		/* We set up the normal JPEG error routines, then override error_exit. */
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = jpegErrorExit;
		/* Establish the setjmp return context for my_error_exit to use. */
		if (setjmp(jerr.setjmp_buffer)) {
			/* If we get here, the JPEG code has signaled an error. */
			jpeg_destroy_decompress(&cinfo);
			return false;
		}

		/* Now we can initialize the JPEG decompression object. */
		jpeg_create_decompress(&cinfo);

		/* Step 2: specify data source (eg, a file) */

		// Configure this decompressor to read its data from a memory 
		// buffer starting at unsigned char *jpg_buffer, which is jpg_size
		// long, and which must contain a complete jpg already.
		//
		// If you need something fancier than this, you must write your 
		// own data source manager, which shouldn't be too hard if you know
		// what it is you need it to do. See jpeg-8d/jdatasrc.c for the 
		// implementation of the standard jpeg_mem_src and jpeg_stdio_src 
		// managers as examples to work from.
		jpeg_mem_src(&cinfo, const_cast<unsigned char*>(buffer), length);

		/* Step 3: read file parameters with jpeg_read_header() */

		(void)jpeg_read_header(&cinfo, TRUE);
		/* We can ignore the return value from jpeg_read_header since
		*   (a) suspension is not possible with the stdio data source, and
		*   (b) we passed TRUE to reject a tables-only JPEG file as an error.
		* See libjpeg.txt for more info.
		*/

		/* Step 4: set parameters for decompression */

		/* In this example, we don't need to change any of the defaults set by
		* jpeg_read_header(), so we do nothing here.
		*/

		/* Step 5: Start decompressor */

		(void)jpeg_start_decompress(&cinfo);
		/* We can ignore the return value since suspension is not possible
		* with the stdio data source.
		*/

		width_ = cinfo.output_width;
		height_ = cinfo.output_height;
		channels_ = cinfo.num_components;
		data_type_ = DataType::kUint8;
		switch (channels_)
		{
		case 4:
			bpp_ = 4;
			format_ = Format::kRGBA8;
			break;
		case 3:
			bpp_ = 3;
			format_ = Format::kRGB8;
			break;
		case 2:
			bpp_ = 2;
			format_ = Format::kRG8;
			break;
		case 1:
			bpp_ = 1;
			format_ = Format::kR8;
			break;

		default:
			assert(!"Implement this case");
			break;
		}

		pixels_ = new U8[width_ * height_ * bpp_];

		/* We may need to do some setup of our own at this point before reading
		* the data.  After jpeg_start_decompress() we have the correct scaled
		* output image dimensions available, as well as the output colormap
		* if we asked for color quantization.
		* In this example, we need to make an output work buffer of the right size.
		*/
		/* JSAMPLEs per row in output buffer */
		row_stride = cinfo.output_width * cinfo.output_components;

		/* Step 6: while (scan lines remain to be read) */
		/*           jpeg_read_scanlines(...); */

		/* Here we use the library's state variable cinfo.output_scanline as the
		* loop counter, so that we don't have to keep track ourselves.
		*/
		while (cinfo.output_scanline < cinfo.output_height) {
			if (inverted_row_order_)
			{
				int row_number = cinfo.output_height - 1 - cinfo.output_scanline;
				rowptr[0] = pixels_ + row_stride * row_number;
			}
			else // normal row order
				rowptr[0] = pixels_ + row_stride * cinfo.output_scanline;
			/* jpeg_read_scanlines expects an array of pointers to scanlines.
			* Here the array is only one element long, but you could ask for
			* more than one scanline at a time if that's more convenient.
			*/
			(void)jpeg_read_scanlines(&cinfo, rowptr, 1);
		}

		/* Step 7: Finish decompression */

		(void)jpeg_finish_decompress(&cinfo);
		/* We can ignore the return value since suspension is not possible
		* with the stdio data source.
		*/

		/* Step 8: Release JPEG decompression object */

		/* This is an important step since it will release a good deal of memory. */
		jpeg_destroy_decompress(&cinfo);

		/* At this point you may want to check to see whether any corrupt-data
		* warnings occurred (test whether jerr.pub.num_warnings is nonzero).
		*/

		return true;
	}

} // namespace scythe