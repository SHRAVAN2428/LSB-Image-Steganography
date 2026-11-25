#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"


/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
      
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}


Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // argv[2] -> .bmp
    //     -> yes -> store it in structure
    //     -> error - return e_failure;
      if(strstr(argv[2],".bmp") != NULL){
        encInfo->src_image_fname  = argv[2];
      }
      else{
      printf("error,the source image should be .bmp file\n");
      return e_failure;
      }

    // argv[3] -> .txt
    //     -> yes -> store it in structure and store the extn in structure.
    //     -> error - return e_failure;
    if (strstr(argv[3], ".txt") != NULL)
{
    encInfo->secret_fname = argv[3];
    encInfo->extn_secret_file = strrchr(argv[3], '.');

   //// printf("Secret file : %s\n", encInfo->secret_fname);
    printf("Extension   : %s\n", encInfo->extn_secret_file);
}
else
{
    printf("error,the secret file should be .txt file\n");
    return e_failure;
}

    // argv[4] -> passed or not
    //     -> passed -> .bmp
    //         -> yes -> store it in structure
    //         -> (extn)error - return e_failure;
    //     -> not passed -> store the "stego.bmp" into the structure.
    
      if(argv[4] == NULL){
        encInfo->stego_image_fname  = "stego.bmp";
      }
       else if(strstr(argv[4],".bmp") != NULL){
        encInfo->stego_image_fname = argv[4];
        //enfo->
      }
      else{
      printf("error,the output file  should be .bmp file\n");
      return e_failure;
      }

    return e_success;
}


   Status do_encoding(EncodeInfo *encInfo)
{

    if (open_files(encInfo) == e_failure)
    {
        printf("ERROR: Failed to open files\n");
        return e_failure;
    }
   
    if (check_capacity(encInfo) == e_failure)
    {
        printf("ERROR: Image does not have enough capacity\n");
        return e_failure;
    }
    
    if (copy_bmp_header(encInfo->fptr_src_image,
                        encInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Failed to copy BMP header\n");
        return e_failure;
    }
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        printf("ERROR: Failed to encode magic string\n");
        return e_failure;
    }

    //printf("INFO: Encoding Secret File Extension...\n");
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure){
        printf("ERROR: Failed to encode secret file extension\n");
        return e_failure;
    }

  //  printf("INFO: Encoding Secret File Size...\n");
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure){
        printf("ERROR: Failed to encode secret file size\n");
        return e_failure;
    }

   // printf("INFO: Encoding Secret File Data...\n");
    if (encode_secret_file_data(encInfo) == e_failure){
        printf("ERROR: Failed to encode secret file data\n");
        return e_failure;
    }

   // printf("INFO: Copying Remaining Image Data...\n");
    if (copy_remaining_img_data( encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
     { printf("ERROR: Failed to copy remaining image data\n");
         return e_failure; }

    printf("INFO: Encoding Completed Successfully\n");

    return e_success;
}
    //printf("INFO: BMP header copied successfully\n");
    // ==> call function ==> check_capacity();
    //     => call get_image_size_for_bmp
    //     => call get file size
    //     stego.bmp > secret.txt * 8
  
    
Status check_capacity(EncodeInfo *encInfo)
{
   
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    if(!(encInfo->image_capacity > (2 + 4 + 4 + 4 + (encInfo->size_secret_file))*8))
    {
        return e_failure;
    }
    return e_success;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    uint size = ftell(fptr);
    rewind(fptr);

    return size;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char header[54];                                   //  54 bytes of memory 
    rewind(fptr_src_image);                            //  starts reading from byte 0 in fptr_src_image
    fread(header, sizeof(char), 54, fptr_src_image);   // scoop 54 bytes FROM source INTO header
    fwrite(header, sizeof(char), 54, fptr_dest_image); // pour those 54 bytes FROM header INTO dest

    return e_success;
}
  Status encode_magic_string(const char *magic_string,
                           EncodeInfo *encInfo)
{
    return encode_data_to_image((char *)magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image);
}
  Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i = 0; i < 8; i++)
    {
        
        int bit = (data >> (7 - i)) & 1;

        image_buffer[i] &= ~1;  // Clear LSB
        image_buffer[i] |= bit;     // Set LSB
    }

    return e_success;
}
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char image_buffer[8];

    for(int i = 0; i < size; i++)
    {
        fread(image_buffer, 8, 1, fptr_src_image); //fread(address,sizeof each element, no.of elements,filepointer);
                                                 //from filepointer to address 

        encode_byte_to_lsb(data[i], image_buffer);

        fwrite(image_buffer, 8, 1, fptr_stego_image); //fwrite(address,sizeof each element, no.of elements,filepointer);                                                    //from filepointer to address 
                                                      //from  address to filepointer. 

    }

    return e_success;
}

Status encode_secret_file_extn(const char *file_extn,
                               EncodeInfo *encInfo)
{
    /* Encode extension size */
    int extn_size = strlen(file_extn);

    char image_buffer[32];

    fread(image_buffer, 32, 1, encInfo->fptr_src_image);

    for (int i = 0; i < 32; i++)
    {
        int bit = (extn_size >> (31 - i)) & 1;
        image_buffer[i] &= ~1;
        image_buffer[i] |= bit;
    }

    fwrite(image_buffer, 32, 1, encInfo->fptr_stego_image);

    /* Encode extension data */
    return encode_data_to_image((char *)file_extn,
                                extn_size,
                                encInfo->fptr_src_image,
                                encInfo->fptr_stego_image);
}
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char image_buffer[32];

    fread(image_buffer, 32, 1, encInfo->fptr_src_image);

    for(int i = 0; i < 32; i++)
    {
        int bit = (file_size >> (31 - i)) & 1;

        image_buffer[i] &= ~1;
        image_buffer[i] |= bit;
    }

    fwrite(image_buffer, 32, 1, encInfo->fptr_stego_image);

    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;

    while((ch = fgetc(encInfo->fptr_secret)) != EOF)
    {
        char image_buffer[8];

        fread(image_buffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(ch, image_buffer);

        fwrite(image_buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char ch;

    while (fread(&ch, 1, 1, fptr_src_image) > 0)
    {
        fwrite(&ch, 1, 1, fptr_stego_image);
    }

    return e_success;
}