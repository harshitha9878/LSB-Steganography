#include<stdio.h>                     //declare stdio headerfile
#include<string.h>                    //declare string headerfile to perform string function
#include "common.h"                   //declare user-defined headerfile
#include "decode.h"
#include "types.h"

/*function definitions */

Status read_and_validate_decode_args(char *argv[] ,DecodeInfo *decInfo)
{
	if(strcmp(strstr(argv[2] ,".bmp") ,".bmp") == 0)     //compare argv[2] extension is ".bmp" or not using strstr & strcmp fun
	{
		decInfo->stego_image_fname = argv[2];            //storing file name into a pointer
	}
	else
		return e_failure;

	if(argv[3] != NULL)                                  //check argv[3] passed through CLA or not
	{
		strcpy(decInfo->output_fname ,argv[3]);          //copying argv[3] into the array
	}
	else
	{
		strcpy(decInfo->output_fname , "output");        //if argv[3] not passed, then store default name into the array
	}

	return e_success;
}

//Do decoding function definition

Status do_decoding(DecodeInfo *decInfo)                 //do_decoding function call                 
{
	if(open_decode_files(decInfo) == e_success)         //function call to open  the files 
	{
		printf("------------files opened successfully-----------\n");
	}
	else
		return e_failure;                               //return e_failure & terminate program by printing error message

	if(skip_bmp_header(decInfo->fptr_stego_image) == e_success)     //fun call to skip bmp header
	{
		printf("------------skip headder success------------\n");
	}
	else
		return e_failure;                                   

	//decode magic string

	if(strcmp(decode_magic_string(decInfo) ,MAGIC_STRING) == 0)
	{
		printf("------------decoding magic string success-----------\n");
	}
	else
		return e_failure;

	//decode secret file extension size

	if(decode_secret_extn_size(decInfo) == e_success)
	{
		printf("-------------decoding secret file extn size success------------\n");
	}
	else
		return e_failure;

	//decode secret file extension

	if(decode_secret_file_extn(decInfo) == e_success)
	{
		printf("--------------decoding secret file extn success-------------\n");
	}
	else
		return e_failure;

	//decode secret file size

	if(decode_secret_file_size(decInfo) == e_success)      //checking the function returned e_success or not
	{
		printf("-------------decoding secret file size success-------------\n");
	}
	else
		return e_failure;

	//decode secret file data

	if(decode_secret_file_data(decInfo) == e_success)
	{
		printf("-------------decoding secret data success------------\n");
	}
	else
		return e_failure;

	fclose(decInfo->fptr_stego_image);          //closing fptr_stego_image file pointer
	fclose(decInfo->fptr_output);                //closing fptr_output file pointer using fclose file operation 

	return e_success;
}

//open files

Status open_decode_files(DecodeInfo *decInfo)
{
	//open source image file in read mode
	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname ,"r");

	//do error handling
	if(decInfo->fptr_stego_image == NULL)           //checking file pointer return type is NULL , then file not exist
	{
		perror("fopen");
		fprintf(stderr ,"ERROR: Unable to open file %s\n", decInfo->stego_image_fname); //print error message

		return e_failure;
	}

	return e_success;
}

//Skip 54bytes bmp header

Status skip_bmp_header(FILE *fptr_stego_image)
{
	fseek(fptr_stego_image ,54 ,SEEK_SET);          //skipping 1st 54bytes using fseek

	return e_success;
}

//Decode magic string

char *decode_magic_string(DecodeInfo *decInfo)
{
	int i,size=2;                                   //variable declaration
	static char buf[3];                             //declare static array of 3 elements
	for(i=0;i<size;i++)                             //for loop
	{
		buf[i] = decode_byte_from_lsb(decInfo->fptr_stego_image);   //storing the decoded dat into the buffer
	}
	buf[i]='\0';                                    //store null character
	printf("magic string = %s\n",buf);     
	return buf;                                     //return array base address
}

//Decode secret file extension size

Status decode_secret_extn_size(DecodeInfo *decInfo)     
{
	decInfo->extn_size = decode_size_from_lsb(decInfo->fptr_stego_image);    //store decoded size into a variable 
	return e_success;
}

//Decode secret file extension

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
	char buf[decInfo->extn_size + 1];                         //declaring array 
	int i;
	for(i=0 ;i < decInfo->extn_size ;i++)                     //for loop runs extn_size times
	{
		buf[i] = decode_byte_from_lsb(decInfo->fptr_stego_image);    
		//storing each byte of file extn in the array by calling decode_byte_from_lsb function
	}
	buf[i] = '\0';                                            //storing null character at the end of the magic string
	printf("file extn = %s\n",buf); 

	//concartinating the file extension with output filename
	strcat(decInfo->output_fname ,buf);       //concartinating the file extension with output file-name using strcat function

	//open output file with extension
	decInfo->fptr_output = fopen(decInfo->output_fname ,"w");     //open o/p file in write mode

	//Do error handling
	if(decInfo->fptr_output == NULL)                              
	{
		perror("fopen");
		fprintf(stderr ,"ERROR: Unable to open file %s\n", decInfo->output_fname);

		return e_failure;
	}
	printf("argv[3] = %s\n",decInfo->output_fname);

	return e_success;
}

//Decode secret file size

Status decode_secret_file_size(DecodeInfo *decInfo)
{
	decInfo->file_size = decode_size_from_lsb(decInfo->fptr_stego_image);
	//store the decoded size to an integer variable defined inside the structure

	printf("file size = %d\n",decInfo->file_size);
	return e_success;
}

//Decode secret file data

Status decode_secret_file_data(DecodeInfo *decInfo)
{
	char buff[decInfo->file_size + 1];                //declare an array of size file_size+1
	int i;
	for(i=0 ;i < decInfo->file_size ;i++)             //for loop runninf file_size times
	{
		buff[i] = decode_byte_from_lsb(decInfo->fptr_stego_image);     //store the decoded bytes into the buffer
		//	fwrite(&buff[i] ,1 ,1 ,decInfo->fptr_output);
	}
	buff[i] = '\0';
	fwrite(buff ,1 ,decInfo->file_size ,decInfo->fptr_output);         //write decoded data into the o/p file
	
	printf("secret data = %s\n",buff);                                 //display secret data using printf function
	return e_success;                                                  //return e_success
}

//Decode byte from lsb

char decode_byte_from_lsb(FILE *fptr_stego_image)
{
	char data=0,buf[8*sizeof(char)];                            //declare an array of size 8
	fread(buf ,1 ,8 ,fptr_stego_image);                         //read 8bytes from stego file
	for(int i=0;i<8;i++)                                        //for loop running 8 times 
	{
		data = data | ((buf[i] & 0X01) << i);     //fetching only LSB bit of each byte & storing in a character variable
	}
	return data;                                  //return the data  
}

//Decode size from lsb

uint decode_size_from_lsb(FILE *fptr_stego_image)
{
	char buf[32*sizeof(char)];                             //declare an array of size 32 
	int size=0;                                            //intialize the size with 0

	fread(buf ,1 ,32 ,fptr_stego_image);                   //read 32bytes from stego file store it in a buffer using fread fun

	for(int i=0;i<32;i++)                                  //for loop running 32 times
	{
		size = size | ((buf[i] & 0X01) << i);              //fetching only LSB bit from each byte & store it into a int variable
	}

	return size;                                           //return the size 
}
