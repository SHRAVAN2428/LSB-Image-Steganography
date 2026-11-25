#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "decode.h"
#include "common.h"
#include "types.h"

/* Read and validate decode arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (argv[2] == NULL || strstr(argv[2], ".bmp") == NULL)
    {
        printf("ERROR: Invalid stego image\n");
        return e_failure;
    }

    decInfo->stego_image_fname = argv[2];

    /* argv[3] = secret file name to be decoded */
    /* argv[4] = output file name */

    if (argv[4] != NULL)
    {
        decInfo->output_fname = argv[4];
    }
    else
    {
        decInfo->output_fname = "output";
    }

    return e_success;
}

/* Open stego image */
Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");

    if (decInfo->fptr_stego_image == NULL)
    {
        printf("ERROR: Unable to open %s\n",
               decInfo->stego_image_fname);

        return e_failure;
    }

    return e_success;
}

/* Skip BMP header */
Status skip_bmp_header(FILE *fptr_stego_image)
{
    fseek(fptr_stego_image, 54, SEEK_SET);
    return e_success;
}

/* Decode one byte */
Status decode_byte_from_lsb(unsigned char *data, unsigned char *img_buffer)
{
    *data = 0;

    for(int i = 0; i < 8; i++)
    {
        *data <<= 1;
        *data |= (img_buffer[i] & 1);
    }

    return e_success;
}

/* Decode data */
Status decode_data_from_image(unsigned char *data,
                              int size,
                              DecodeInfo *decInfo)
{
    unsigned char buffer[8];

    for (int i = 0; i < size; i++)
    {
        if (fread(buffer, 1, 8,
                  decInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }

        decode_byte_from_lsb(&data[i], buffer);
    }

    return e_success;
}

/* Decode integer */
Status decode_int_from_lsb(int *data, FILE *fptr_stego)
{
    unsigned char buffer[32];

    if(fread(buffer,1,32,fptr_stego) != 32)
        return e_failure;

    *data = 0;

    for(int i = 0; i < 32; i++)
    {
        *data <<= 1;
        *data |= (buffer[i] & 1);
    }

    return e_success;
}
/* Decode magic string */
Status decode_magic_string(char *magic,
                           DecodeInfo *decInfo)
{
    if (decode_data_from_image((unsigned char *)magic,
                               strlen(MAGIC_STRING),
                               decInfo) == e_failure)
    {
        return e_failure;
    }

    magic[strlen(MAGIC_STRING)] = '\0';

    return e_success;
}

/* Decode extension size */
Status decode_secret_file_extn_size(int *extn_size,
                                    DecodeInfo *decInfo)
{
    return decode_int_from_lsb(extn_size,
                               decInfo->fptr_stego_image);
}

/* Decode extension */
Status decode_secret_file_extn(char *extn,
                               int extn_size,
                               DecodeInfo *decInfo)
{
    if (extn_size <= 0 || extn_size >= MAX_FILE_SUFFIX)
    {
        return e_failure;
    }

    if (decode_data_from_image((unsigned char *)extn,
                               extn_size,
                               decInfo) == e_failure)
    {
        return e_failure;
    }

    extn[extn_size] = '\0';

    return e_success;
}
/* Decode secret file size */
Status decode_secret_file_size(int *file_size,
                               DecodeInfo *decInfo)
{
    return decode_int_from_lsb(file_size,
                               decInfo->fptr_stego_image);
}


/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    unsigned char ch;

    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        if (decode_data_from_image(&ch, 1, decInfo) == e_failure)
        {
            return e_failure;
        }

        fwrite(&ch, 1, 1, decInfo->fptr_output);
    }

    return e_success;
}


/* Perform complete decoding */
Status do_decoding(DecodeInfo *decInfo)
{
    printf("\nINFO: Decoding Started\n");

    /* Open stego image */
    if (open_decode_files(decInfo) == e_failure)
    {
        return e_failure;
    }

    /* Skip BMP Header */
    skip_bmp_header(decInfo->fptr_stego_image);

    /* Decode Magic String */
    if (decode_magic_string(decInfo->magic, decInfo) == e_failure)
    {
        printf("ERROR: Unable to decode magic string\n");
        return e_failure;
    }

    printf("Magic String : %s\n", decInfo->magic);

    if (strcmp(decInfo->magic, MAGIC_STRING) != 0)
    {
        printf("ERROR: Magic String Mismatch\n");
        return e_failure;
    }

    printf("Magic String Matched\n");

    /* Decode Extension Size */
    if (decode_secret_file_extn_size(&decInfo->extn_size,
                                     decInfo) == e_failure)
    {
        printf("ERROR: Extension size decoding failed\n");
        return e_failure;
    }

    printf("Extension Size : %d\n",
           decInfo->extn_size);

    if (decInfo->extn_size <= 0 ||
        decInfo->extn_size >= MAX_FILE_SUFFIX)
    {
        printf("ERROR: Invalid extension size\n");
        return e_failure;
    }

    /* Decode Extension */
    if (decode_secret_file_extn(decInfo->extn_secret_file,
                                decInfo->extn_size,
                                decInfo) == e_failure)
    {
        printf("ERROR: Extension decoding failed\n");
        return e_failure;
    }

    printf("Extension : %s\n",
           decInfo->extn_secret_file);

    /* Create output filename */
    char final_name[100];

    snprintf(final_name,
             sizeof(final_name),
             "%s%s",
             decInfo->output_fname,
             decInfo->extn_secret_file);

    decInfo->fptr_output = fopen(final_name, "wb");

    if (decInfo->fptr_output == NULL)
    {
        printf("ERROR: Unable to create output file\n");
        return e_failure;
    }

    /* Decode Secret File Size */
    if (decode_secret_file_size(&decInfo->size_secret_file,
                                decInfo) == e_failure)
    {
        printf("ERROR: File size decoding failed\n");
        return e_failure;
    }

    printf("Secret File Size : %d bytes\n",
           decInfo->size_secret_file);

    if (decInfo->size_secret_file < 0)
    {
        printf("ERROR: Invalid file size\n");
        return e_failure;
    }

    /* Decode Secret File Data */
    if (decode_secret_file_data(decInfo) == e_failure)
    {
        printf("ERROR: Secret file decoding failed\n");
        return e_failure;
    }

    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);

    printf("Output File : %s\n", final_name);
    printf("INFO: Decoding Completed Successfully\n");

    return e_success;
}