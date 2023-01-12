#ifndef DECODE_H            //conditional compilation
#define DECODE_H

#include "types.h"          //contains user defined header-files

#define MAX_OUT_BUF_SIZE 50

typedef struct _DecodeInfo            //structure data type
{
	/*Source Image info */
	char *stego_image_fname;
	FILE *fptr_stego_image;
	uint extn_size;
	uint file_size;

	/*output image info*/
	char output_fname[MAX_OUT_BUF_SIZE];
	FILE *fptr_output;

} DecodeInfo;

/* Decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* perform decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* Skip bmp header */
Status skip_bmp_header(FILE *fptr_stego_image);

/* Decode magic string */
char *decode_magic_string(DecodeInfo *decInfo); 

/* Decode secret file extension size */
Status decode_secret_extn_size(DecodeInfo *decInfo);

/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode byte from lsb */
char decode_byte_from_lsb(FILE *fptr_stego_image);

/* Decode size from lsb */
uint decode_size_from_lsb(FILE *fptr_stego_image);

#endif
