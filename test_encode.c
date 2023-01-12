#include <stdio.h>            //stdio headerfile
#include<string.h>            //headerfile to perform string operations
#include "encode.h"           //user-defined headerfiles
#include "decode.h"
#include "types.h"

int main(int argc,char *argv[])           //main function to perform Command line argument
{
	if(argc < 3)                          //validating argc count  
	{
		printf("Error : please pass the command line arguments \nUsage : ./a.out -e beautiful.bmp secret.txt/secret.c\n");
		return e_failure;                 //print error message
	}

	if(check_operation_type(argv) == e_encode)                          //function call to check_operation_type              
	{ 
		printf("---------------selected encoding--------------\n");     //print message using printf  
		EncodeInfo encInfo;                                             //declaring structure variable 

		if(read_and_validate_encode_args(argv , &encInfo) == e_success) //read_and_validate_encode_args fun call
		{
			printf("---------read and validate success---------\n");
			if(do_encoding(&encInfo) == e_success)                      //do_encoding function call
			{
				printf("-----------encoding success------------\n");
			}
			else
				printf("-----------encoding failed !-----------\n");
		}
		else
		{
			printf("Error : please pass the command line arguments \nUsage : ./a.out -e beautiful.bmp secret.txt/secret.c\n");
			return e_failure;                                            //print error message     
		}
	}
	else if(check_operation_type(argv) == e_decode)   //check -d passed thorugh CLA by calling check_operation_type function    
	{
		printf("--------------selected decoding---------------\n");

		DecodeInfo decInfo;                           //declaring structure variable
		if(read_and_validate_decode_args(argv ,&decInfo) == e_success)
		{	                                                        
			printf("---------read and validate success---------\n");
			if(do_decoding(&decInfo) == e_success)   //callinf do_decoding function   
			{
				printf("------------decoding success-------------\n");
			}
			else
				printf("------------decoding failed !-------------\n");
		}
		else
		{
			printf("Error : please pass the command line arguments \nUsage : ./a.out -d stego.bmp output\n");
			return e_failure;
		}
	}
	else if(check_operation_type(argv) == e_unsupported)       //other than -e/-d passed print error message
	{
		printf("Error : please pass the command line arguments \nUsage : ./a.out -e beautiful.bmp secret.txt/secret.c\n");
		return e_failure;
	}
	return 0;
}

OperationType check_operation_type(char *argv[])
{
	if(strcmp(argv[1],"-e") == 0)                             //comparing argv[1] with "-e" using strmp function
	{
		return e_encode;                                      //return e_encode to perform encoding operation  
	}
	else if(strcmp(argv[1],"-d") == 0)                        //comparing argv[1] with "-d" using strcmp function
	{
		return e_decode;                                      //return e_decode to perform decode operation
	}
	else
		return e_unsupported;
	//if "-e"/-"d" not passed the return e_unsupported & terminate the program by printing error message
}

