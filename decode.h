#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

/* Buffer sizes */
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 20

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Output File Info */
    char *output_fname;
    FILE *fptr_output;

    /* Secret File Info */
    char extn_secret_file[MAX_FILE_SUFFIX];
    int extn_size;
    int size_secret_file;

    /* Magic String */
    char magic[20];

} DecodeInfo;


/* Read and validate decode arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Open files */
Status open_decode_files(DecodeInfo *decInfo);

/* Skip BMP header */
Status skip_bmp_header(FILE *fptr_stego_image);

/* Decode magic string */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/* Decode extension size */
Status decode_secret_file_extn_size(int *extn_size,
                                    DecodeInfo *decInfo);

/* Decode extension */
Status decode_secret_file_extn(char *extn,
                               int extn_size,
                               DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(int *file_size,
                               DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode bytes from image */
Status decode_data_from_image(unsigned char *data,
                              int size,
                              DecodeInfo *decInfo);

/* Decode single byte */
Status decode_byte_from_lsb(unsigned char *data,
                            unsigned char *img_buffer);

/* Decode integer */
Status decode_int_from_lsb(int *data,
                           FILE *fptr_stego);

#endif