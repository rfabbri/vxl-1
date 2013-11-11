#ifdef __cplusplus
extern "C" {
#endif

    /*
 * cjpeg.c
 *
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains a command-line user interface for the JPEG compressor.
 * It should work on any system with Unix- or MS-DOS-style command lines.
 *
 * Two different command line styles are permitted, depending on the
 * compile-time switch TWO_FILE_COMMANDLINE:
 *    cjpeg [options]  inputfile outputfile
 *    cjpeg [options]  [inputfile]
 * In the second style, output is always to standard output, which you'd
 * normally redirect to a file or pipe to some other program.  Input is
 * either from a named file or from standard input (typically redirected).
 * The second style is convenient on Unix but is unhelpful on systems that
 * don't support pipes.  Also, you MUST use the first style if your system
 * doesn't do binary I/O to stdin/stdout.
 * To simplify script writing, the "-outfile" switch is provided.  The syntax
 *    cjpeg [options]  -outfile outputfile  inputfile
 * works regardless of which command line style is used.
 */

//#include <cdjpeg.h>        /* Common decls for cjpeg/djpeg applications */
//#include <jversion.h>        /* for version message */

/* Create the add-on message string table. */

#define JMESSAGE(code,string)    string ,

static const char * const cdjpeg_message_table[] = {
//#include "cderror.h"
  NULL
};


/*
 * The main program.
 */

void outputimgjpg(char *outfilename,unsigned char *RGB,int NY,int NX,int dim)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  int file_index;
  FILE * input_file;
  FILE * output_file;
  JDIMENSION num_scanlines;
  JSAMPLE *image_buffer;
  JSAMPROW row_pointer[1];      /* pointer to JSAMPLE row[s] */
  int row_stride;               /* physical row width in image buffer */

  image_buffer = (JSAMPLE *) RGB;

  /* Initialize the JPEG compression object with default error handling. */
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  /* Open the output file. */
  if (outfilename != NULL) {
    if ((output_file = fopen(outfilename, WRITE_BINARY)) == NULL) {
      fprintf(stderr, "can't open %s\n", outfilename);
      exit(EXIT_FAILURE);
    }
  } 

  /* Specify data destination for compression */
  jpeg_stdio_dest(&cinfo, output_file);

  cinfo.image_width = NX;      /* image width and height, in pixels */
  cinfo.image_height = NY;
  if (dim==3)
  {
    cinfo.input_components = 3;           /* # of color components per pixel */
    cinfo.in_color_space = JCS_RGB;       /* colorspace of input image */
  }
  else if (dim==1)
  {
    cinfo.input_components = 1;           /* # of color components per pixel */
    cinfo.in_color_space = JCS_GRAYSCALE;      /* colorspace of input image */
  }
  jpeg_set_defaults(&cinfo);

  /* Start compressor */
  jpeg_start_compress(&cinfo, TRUE);
  row_stride = cinfo.image_width * cinfo.input_components; /* JSAMPLEs per row in image_buffer */

  /* Process data */
  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  /* Finish compression and release memory */
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  /* Close files, if we opened them */
  if (output_file != stdout) fclose(output_file);
}


#ifdef __cplusplus
}
#endif







