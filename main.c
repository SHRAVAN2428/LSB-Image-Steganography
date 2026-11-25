// Project: Image Steganography Using LSB Technique
// This message is hidden inside a BMP image using the Least Significant Bit (LSB) algorithm.

// Objectives:
// - Hide confidential information inside an image.
// - Preserve the visual quality of the cover image.
// - Recover the original message using a decoding program.

// Developed By:
// Shravan Gopal -- 26005D_188
// Embedded Systems Trainee at Emertxe Information Technologies

// This demonstrates secure data hiding for educational purposes.
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "decode.h"
OperationType check_operation_type(char *argv[])
{
    // argv[1] -> -e -> e_encode;
    //         -> -d -> e_decode;
    //         -> e_unsupported;';
    if(strcmp(argv[1],"-e") == 0){
        //printf("ENcode is selected\n");
        return e_encode;
    }
    else if(strcmp(argv[1], "-d") == 0){
       // printf("decode is selected\n");
        return e_decode;
    }
    else
    return e_unsupported;
}

int main(int argc, char *argv[])
{
    // data_type variable = function call();
    OperationType res = check_operation_type(argv);
    if(res == e_encode)
    {
        // create a structure variable
        EncodeInfo encInfo;
        if(read_and_validate_encode_args(argv,  &encInfo) == e_success)
        {
            //do encoding
            do_encoding(&encInfo);

        }
         else
        {
            printf("Error: Invalid encode arguments\n");
        }
    }
        else if (res == e_decode)
{
    DecodeInfo decInfo;

    if (read_and_validate_decode_args(argv, &decInfo) == e_success)
    {
        do_decoding(&decInfo);
    }
    else
    {
        printf("ERROR: Invalid Decode Arguments\n");
    }
}
else
{
    printf("Unsupported Operation\n");
}
        

        // -> e_decode ->

        // -> e_unsupported -> error -> exit
}
