#include <stdio.h>                //inclusion of stdio headerfile
#include<string.h>                //string headefiles
#include "common.h"               //inclusion of user-defined headerfiles
#include "encode.h"
#include "types.h"

/* Function Definitions */

//read and validate function definition
Status read_and_validate_encode_args(char *argv[] ,EncodeInfo *encInfo)
{
	if((strstr(argv[2],".bmp") != NULL) && (strcmp(strstr(argv[2] ,".bmp") ,".bmp") == 0))        
//	if(strcmp(strstr(argv[2] ,".bmp") ,".bmp") == 0)        
		//extraction of file extension using strstr function & comparing ".bmp" using strcmp function 
	{
		encInfo->src_image_fname = argv[2];      //storing file name in the structure member whic is a pointer
	}
	else
		return e_failure;                        //else return e_failure & print error message

	if(argv[3] != NULL)                          //checking argv[3] passed though CLA
	{
		strcpy(encInfo->extn_secret_file , strstr(argv[3] ,"."));    
		//extracting the extension & copying it into array using strcpy

		encInfo->secret_fname = argv[3];         //storing the file name to a pointer declared in structure
		printf("extn_secret_file = %s\n",encInfo->extn_secret_file);
		printf("secret fname = %s\n",encInfo->secret_fname);
	}
	else
		return e_failure;

	if(argv[4] != NULL)                          //checking argv[4] passed through CLA or not
	{
		encInfo->stego_image_fname = (strtok(argv[4] ,"."));     
		      //extracting only the o/p filename using strtok fun & store in pointer 
		strcat(encInfo->stego_image_fname , ".bmp");             
		     //concartinating the filename with extension using strcat fun
		printf("stego fname = %s\n",encInfo->stego_image_fname);
	}
	else
	{
		encInfo->stego_image_fname = "stego.bmp";                //storing default name in the pointer
		printf("stego fname = %s\n",encInfo->stego_image_fname);
	}

	return e_success;
}

//do encoding function
Status do_encoding(EncodeInfo *encInfo)           
{
	if(open_files(encInfo) == e_success)              //calling openfile function to open the files
	{
		printf("------------file opened successfully-----------\n");
	}
	else
		return e_failure;

	if(check_capacity(encInfo) == e_success)          //check capacity function call
	{
		printf("-------------check capacity is success------------\n");
	}
	else
		return e_failure;

	if(copy_bmp_header(encInfo->fptr_src_image ,encInfo->fptr_stego_image) == e_success)   //copy bmp header fun call
	{
		printf("--------------copy bmp header success------------\n");
	}
	else
		return e_failure;

	if(encode_magic_string(MAGIC_STRING ,encInfo) == e_success)        //magic string function call
	{
		printf("-------------encode magic string success-------------\n");
	}
	else
		return e_failure;

	if(encode_secret_extn_size(strlen(encInfo->extn_secret_file) ,encInfo) == e_success)    //encoding secret extn size fun call
	{
		printf("--------------encoding secret file extn size success--------------\n");
	}
	else
		return e_failure;

	if(encode_secret_file_extn(encInfo->extn_secret_file ,encInfo) == e_success)        //encoding secret file extn fun call
	{
		printf("-------------encoding secret file extn success-------------\n");
	}
	else
		return e_failure;

	if(encode_secret_file_size(encInfo->size_secret_file ,encInfo) == e_success)       //encode secret file size fun call
	{
		printf("-------------encoding secret file size success--------------\n");
	}
	else
		return e_failure;

	if(encode_secret_file_data(encInfo) == e_success)           //encode secret file data fun call
	{
		printf("--------------encoding secret file data is success--------------\n");
	}
	else
		return e_failure;

	if(copy_remaining_img_data(encInfo->fptr_src_image ,encInfo->fptr_stego_image) == e_success) 
	{
		printf("-------------copy remaining data success------------\n");
	}
	else
		return e_failure;

	fclose(encInfo->fptr_src_image);                  //closing fptr_src_image file pointer using fclose file operation
	fclose(encInfo->fptr_secret);                     //closing fptr_secret file pointer
	fclose(encInfo->fptr_stego_image);                //closing fptr_stego_image file pointer
	return e_success;
}

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
	fread(&width, sizeof(int), 1, fptr_image);      //reading 4bytes to read width from beautiful.bmp file

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);     //reading 4bytes to read height from source file

	// Return image capacity
	return width * height * 3;
}

//get secret file size function definition
uint get_file_size(FILE *fptr)                     
{
	uint size;
	fseek(fptr ,0L ,SEEK_END);                     //moving offset to EOF using fseek file function
	size = ftell(fptr);                            //finding the position of offset to find file size using ftell function 
	return size;                                   //return size  
}

Status check_capacity(EncodeInfo *encInfo)
{
	encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
	//storing source image file size in a structure member 
	printf("bmp image size = %d\n",encInfo -> image_capacity);
	encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
	//storing secret file size in a variable
	printf("secret file size = %d\n",encInfo -> size_secret_file);

	//validating the size
	if(encInfo->image_capacity > (54 + (strlen(MAGIC_STRING) + 4 + strlen(encInfo->extn_secret_file) + 4 + encInfo->size_secret_file)*8)) 
	{
		return e_success;                        //return e_success to the function
	}
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
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
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

//copy bmp header

Status copy_bmp_header(FILE *fptr_src_image ,FILE *fptr_stego_image)
{
	char buf[54];
	rewind(fptr_src_image);                 //moving the offset to 1st position
	fread(buf ,1 ,54 ,fptr_src_image);      //reading 54bytes from source image
	fwrite(buf ,1 ,54 ,fptr_stego_image);   //writing it into output file
	return e_success;
}

//encode magic string

Status encode_magic_string(const char *magic_string ,EncodeInfo *encInfo)
{
	//call encode data to image fun
	encode_data_to_image(magic_string ,strlen(magic_string) ,encInfo->fptr_src_image ,encInfo->fptr_stego_image);
	return e_success;
}

//encode secret file extension size

Status encode_secret_extn_size(uint size ,EncodeInfo *encInfo)
{
	encode_secret_file_size(size ,encInfo);                 //function call
	return e_success;
}

//encode secret file extension

Status encode_secret_file_extn(const char *file_extn ,EncodeInfo *encInfo)
{
	encode_data_to_image(file_extn ,strlen(file_extn) ,encInfo->fptr_src_image ,encInfo->fptr_stego_image);
	return e_success;
}

//encode secret file data

Status encode_secret_file_data(EncodeInfo *encInfo)
{
	rewind(encInfo->fptr_secret);                   //moving offset to 1st position
	char secret_data[encInfo->size_secret_file];    //declaring an array of size secret file size
	fread(secret_data ,1 ,encInfo->size_secret_file ,encInfo->fptr_secret);     //read secret size bytes from source file

	encode_data_to_image(secret_data ,encInfo->size_secret_file ,encInfo->fptr_src_image ,encInfo->fptr_stego_image);

	return e_success;
}

//encode data to image function

Status encode_data_to_image(const char *data ,int size ,FILE *fptr_src_image ,FILE *fptr_stego_image)
{
	char buf[8*sizeof(char)];
	for(int i=0;i<size;i++)
	{
		fread(buf ,1 ,8 ,fptr_src_image);        //read 8bytes from source file & store in buffer
		encode_byte_to_lsb(data[i] ,buf);        //fun call encode_byte_to_lsb
		fwrite(buf ,1 ,8 ,fptr_stego_image);     //write encoded byte to the output file
	}
	return e_success;
}

//encode byte to lsb

Status encode_byte_to_lsb(char data ,char *buf)
{
	for(int i=0;i<8;i++)                               //for loop runs 8 times
	{
		buf[i] = buf[i] & 0xFE;                        //clearing the lsb bit
		buf[i] = buf[i] | ((data & (1 << i)) >> i);    //storing each bit of source image to the LSB bit of buffer   
	}
	return e_success;
}

//encode secret file size & file extension size

Status encode_secret_file_size(uint file_size ,EncodeInfo *encInfo)
{
	char buf[sizeof(int)*8];                           //declaring static array of 32 elements

	fread(buf ,1 ,32 ,encInfo->fptr_src_image);        //reading 32 bytes from source image using fread function
	encode_size_to_lsb(file_size ,buf);                //functino call
	fwrite(buf ,1 ,32 ,encInfo->fptr_stego_image);     //writing encoded bytes to output file

	return e_success;
}

//encode size to lsb

Status encode_size_to_lsb(uint size ,char *buf)
{
	for(int i=0;i<32;i++)                              //for loop, looping 32 times
	{
		buf[i] = buf[i] & 0xFE;                        //clearing LSB bit
		buf[i] = buf[i] | ((size & (1 << i)) >> i);    //storing each bit of size into LSB bit of buffer
	}
	return e_success;
}

//copy remaining data

Status copy_remaining_img_data(FILE *fptr_src_image ,FILE *fptr_stego_image)
{
	char *buf;                                         //declaring pointer

	while(fread(buf, 1, 1, fptr_src_image) > 0)       
		//fread inside while loop checking return value of fread ,by reading 1byte from source file until file reaches EOF 
		fwrite(buf, 1, 1, fptr_stego_image);
	//writing 1byte into the output file using fwrite, until the file reaches EOF
	return e_success;
}
