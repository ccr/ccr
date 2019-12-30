 /*
 * This file is an example of an HDF5 filter plugin.
 * The filter function  H5Z_filter_bzip2 was adopted from
 * PyTables http://www.pytables.org.
 * The plugin can be used with the HDF5 library vesrion 1.8.11+ to read
 * HDF5 datasets compressed with bzip2 created by PyTables.
 */

/*
 *
Copyright Notice and Statement for PyTables Software Library and Utilities:

Copyright (c) 2002-2004 by Francesc Alted
Copyright (c) 2005-2007 by Carabos Coop. V.
Copyright (c) 2008-2010 by Francesc Alted
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

a. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

b. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

c. Neither the name of Francesc Alted nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "config.h"
#include <stdio.h>
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
#ifdef STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif
#ifdef HAVE_STRING_H
# if !defined STDC_HEADERS && defined HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#endif
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <assert.h>

#include "H5PLextern.h"

#include "bzlib.h"

size_t H5Z_filter_bzip2(unsigned int flags, size_t cd_nelmts,
                     const unsigned int cd_values[], size_t nbytes,
                     size_t *buf_size, void **buf);

/*
 * BZIP2 compression was the first external filter registered by PyTables developers
 * with The HDF Group
 * See http://www.hdfgroup.org/services/contributions.html for more information.
 *
 * If you intend your plugin to be used by others, please register your filter
 * with The HDF Group.
 */
#define H5Z_FILTER_BZIP2 307

const H5Z_class2_t H5Z_BZIP2[1] = {{
    H5Z_CLASS_T_VERS,       /* H5Z_class_t version */
    (H5Z_filter_t)H5Z_FILTER_BZIP2,         /* Filter id number             */
#ifdef FILTER_DECODE_ONLY
    0,                   /* encoder_present flag (false is not available) */
#else
    1,                   /* encoder_present flag (set to true) */
#endif
    1,              /* decoder_present flag (set to true) */
    "HDF5 bzip2 filter; see http://www.hdfgroup.org/services/contributions.html",
                                /* Filter name for debugging    */
    NULL,                       /* The "can apply" callback     */
    NULL,                       /* The "set local" callback     */
    (H5Z_func_t)H5Z_filter_bzip2,         /* The actual filter function   */
}};

H5PL_type_t   H5PLget_plugin_type(void) {return H5PL_TYPE_FILTER;}
const void *H5PLget_plugin_info(void) {return H5Z_BZIP2;}

size_t H5Z_filter_bzip2(unsigned int flags, size_t cd_nelmts,
                     const unsigned int cd_values[], size_t nbytes,
                     size_t *buf_size, void **buf)
{
  char *outbuf = NULL;
  size_t outbuflen, outdatalen;
  int ret;

  if (flags & H5Z_FLAG_REVERSE) {

    /** Decompress data.
     **
     ** This process is troublesome since the size of uncompressed data
     ** is unknown, so the low-level interface must be used.
     ** Data is decompressed to the output buffer (which is sized
     ** for the average case); if it gets full, its size is doubled
     ** and decompression continues.  This avoids repeatedly trying to
     ** decompress the whole block, which could be really inefficient.
     **/

    bz_stream stream;
    char *newbuf = NULL;
    size_t newbuflen;

    /* Prepare the output buffer. */
    outbuflen = nbytes * 3 + 1;  /* average bzip2 compression ratio is 3:1 */
    outbuf = malloc(outbuflen);
    if (outbuf == NULL) {
      fprintf(stderr, "memory allocation failed for bzip2 decompression\n");
      goto cleanupAndFail;
    }

    /* Use standard malloc()/free() for internal memory handling. */
    stream.bzalloc = NULL;
    stream.bzfree = NULL;
    stream.opaque = NULL;

    /* Start decompression. */
    ret = BZ2_bzDecompressInit(&stream, 0, 0);
    if (ret != BZ_OK) {
      fprintf(stderr, "bzip2 decompression start failed with error %d\n", ret);
      goto cleanupAndFail;
    }

    /* Feed data to the decompression process and get decompressed data. */
    stream.next_out = outbuf;
    stream.avail_out = outbuflen;
    stream.next_in = *buf;
    stream.avail_in = nbytes;
    do {
      ret = BZ2_bzDecompress(&stream);
      if (ret < 0) {
  fprintf(stderr, "BUG: bzip2 decompression failed with error %d\n", ret);
  goto cleanupAndFail;
      }

      if (ret != BZ_STREAM_END && stream.avail_out == 0) {
        /* Grow the output buffer. */
        newbuflen = outbuflen * 2;
        newbuf = realloc(outbuf, newbuflen);
        if (newbuf == NULL) {
          fprintf(stderr, "memory allocation failed for bzip2 decompression\n");
          goto cleanupAndFail;
        }
        stream.next_out = newbuf + outbuflen;  /* half the new buffer behind */
        stream.avail_out = outbuflen;  /* half the new buffer ahead */
        outbuf = newbuf;
        outbuflen = newbuflen;
      }
    } while (ret != BZ_STREAM_END);

    /* End compression. */
    outdatalen = stream.total_out_lo32;
    ret = BZ2_bzDecompressEnd(&stream);
    if (ret != BZ_OK) {
      fprintf(stderr, "bzip2 compression end failed with error %d\n", ret);
      goto cleanupAndFail;
    }

  } else {

    /** Compress data.
     **
     ** This is quite simple, since the size of compressed data in the worst
     ** case is known and it is not much bigger than the size of uncompressed
     ** data.  This allows us to use the simplified one-shot interface to
     ** compression.
     **/

    unsigned int odatalen;  /* maybe not the same size as outdatalen */
    int blockSize100k = 9;

    /* Get compression block size if present. */
    if (cd_nelmts > 0) {
      blockSize100k = cd_values[0];
      if (blockSize100k < 1 || blockSize100k > 9) {
  fprintf(stderr, "invalid compression block size: %d\n", blockSize100k);
  goto cleanupAndFail;
      }
    }

    /* Prepare the output buffer. */
    outbuflen = nbytes + nbytes / 100 + 600;  /* worst case (bzip2 docs) */
    outbuf = malloc(outbuflen);
    if (outbuf == NULL) {
      fprintf(stderr, "memory allocation failed for bzip2 compression\n");
      goto cleanupAndFail;
    }

    /* Compress data. */
    odatalen = outbuflen;
    ret = BZ2_bzBuffToBuffCompress(outbuf, &odatalen, *buf, nbytes,
                                   blockSize100k, 0, 0);
    outdatalen = odatalen;
    if (ret != BZ_OK) {
      fprintf(stderr, "bzip2 compression failed with error %d\n", ret);
      goto cleanupAndFail;
    }
  }

  /* Always replace the input buffer with the output buffer. */
  free(*buf);
  *buf = outbuf;
  *buf_size = outbuflen;
  return outdatalen;

 cleanupAndFail:
  if (outbuf)
    free(outbuf);
  return 0;
}
