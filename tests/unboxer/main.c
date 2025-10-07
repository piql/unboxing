/*****************************************************************************
**
**  Implementation of the unboxer test application
**
**  Creation date:  2016/06/28
**  Created by:     Ole Liabo
**
**
**  Copyright (c) 2016 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include "unboxingutility.h"
#include "boxing/utils.h"
#include "boxing/unboxer_utility.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#if defined (D_OS_WIN32)
#include <io.h>
#else
#endif
#include "boxing_config.h"

#if defined ( D_OS_WIN32 )
#define DFSEEK _fseeki64
#define DFTELL _ftelli64
#define access _access
#else
#define DFSEEK fseeko
#define DFTELL ftello
#endif


typedef struct command_line_parameters_s
{
    unsigned int input_start_index;
    unsigned int input_end_index;
    size_t       x_size;
    size_t       y_size;
    const char*  format;
    const char*  output;
    DBOOL        is_raw;
} command_line_parameters;

static const char * result_names[] =
{
    "OK",
    "METADATA ERROR",
    "BORDER TRACKING ERROR",
    "DATA DECODE ERROR",
    "CRC MISMATCH ERROR",
    "CONFIG ERROR",
    "PROCESS CALLBACK ABORT"
};

static const char * content_types[] =
{
    "UNKNOWN",
    "TOC",
    "DATA",
    "VISUAL",
    "CONTROL FRAME"
};


//---------------------------------------------------------------------------- 
/*! \ingroup unboxtests
 *
 *  Convert boxing_unboxer_result to string.
 *
 *  \param result  Result code.
 *  \return Result code as string.
 */

static const char * get_process_result_name(enum boxing_unboxer_result result)
{
    return result_names[result];
}


//---------------------------------------------------------------------------- 
/*! \ingroup unboxtests
 *
 *  Unboxing complete callback function. 
 *
 *  \param user   User data
 *  \param res    Unboxing result.
 *  \param stats  Unboxing statistics 
 */

#if defined (BOXINGLIB_CALLBACK)
static int unboxing_complete_callback(void * user, int* res, boxing_stats_decode * stats)
{
    BOXING_UNUSED_PARAMETER(user);

    if (*res == BOXING_UNBOXER_OK)
    {
        printf("Unboxing success!\n");

        printf("\tFEC Accumulated Amount: %f\n", stats->fec_accumulated_amount);
        printf("\tFEC Accumulated Weight: %f\n", stats->fec_accumulated_weight);
        printf("\tResolved Errors: %i\n", stats->resolved_errors);
        printf("\tUnresolved Errors: %i\n\n", stats->unresolved_errors);
    }
    else
    {
        fprintf(stderr, "Unboxing failed! Errorcode = %i - %s\n", *res, get_process_result_name(*res));
    }

    return 0;
}
#endif

//---------------------------------------------------------------------------- 
/*! \ingroup unboxtests
 *
 *  Write data to file. 
 *
 *  \param output_data       Output data.
 *  \param output_file_name  Output file name.
 *  \return 0 on success, -1 on error
 */

static int save_output_data(gvector* output_data, const char * output_file_name)
{
    // Save output data to the file
#ifndef WIN32
    FILE * out_file = fopen(output_file_name, "a+");
#else
    FILE * out_file = fopen(output_file_name, "a+b");
#endif
    if (out_file == NULL)
    {
        fprintf(stderr, "Failed to create output file: '%s'\n", output_file_name);
        return -1;
    }

    if (output_data->size != fwrite(output_data->buffer, output_data->item_size, output_data->size, out_file))
    {
        fclose(out_file);
        fprintf(stderr, "Output file write error.\n");
        return -1;
    }
    else
    {
        printf("Output data saved to: '%s'\n\n", output_file_name);
    }
    fclose(out_file);

    return 0;
}


//---------------------------------------------------------------------------- 
/*! \ingroup unboxtests
 *
 *  Unboxing metadata complete callback function.
 *
 *  \param user       User data
 *  \param res        Unboxing result.
 *  \param meta_data  Unboxing file meta data
 */

#if defined (BOXINGLIB_CALLBACK)
static int metadata_complete_callback(void * user, int* res, boxing_metadata_list * meta_data)
{
    BOXING_UNUSED_PARAMETER(user);

    if (*res == BOXING_UNBOXER_OK)
    {
        printf("\nMeta data:\n");

        GHashTableIter iter;
        gpointer key, value;

        g_hash_table_iter_init(&iter, meta_data);

        while (g_hash_table_iter_next(&iter, &key, &value))
        {
            boxing_metadata_item * item = (boxing_metadata_item *)value;

            boxing_metadata_type type = item->base.type;
            switch (type)
            {
            case BOXING_METADATA_TYPE_JOBID:
                printf("\tJob id: %d\n", ((boxing_metadata_item_job_id*)item)->value);
                break;
            case BOXING_METADATA_TYPE_FRAMENUMBER:
                printf("\tFrame number: %d\n", ((boxing_metadata_item_frame_number*)item)->value);
                break;
            case BOXING_METADATA_TYPE_FILEID:
                printf("\tFile id: %d\n", ((boxing_metadata_item_file_id*)item)->value);
                break;
            case BOXING_METADATA_TYPE_FILESIZE:
                printf("\tFile size: %llu\n", (unsigned long long)((boxing_metadata_item_file_size*)item)->value);
                break;
            case BOXING_METADATA_TYPE_DATACRC:
                printf("\tData CRC: %llu\n", (unsigned long long)((boxing_metadata_item_data_crc*)item)->value);
                break;
            case BOXING_METADATA_TYPE_DATASIZE:
                printf("\tData size: %d\n", ((boxing_metadata_item_data_size*)item)->value);
                break;
            case BOXING_METADATA_TYPE_SYMBOLSPERPIXEL:
                printf("\tSymbols per pixel: %d\n", ((boxing_metadata_item_symbols_per_pixel*)item)->value);
                break;
            case BOXING_METADATA_TYPE_CONTENTTYPE:
                printf("\tContent type: %s\n", content_types[((boxing_metadata_item_content_type*)item)->value]);
                break;
            case BOXING_METADATA_TYPE_CIPHERKEY:
                printf("\tCipher key: %d\n", ((boxing_metadata_item_cipher_key*)item)->value);
                break;
            case BOXING_METADATA_TYPE_CONTENTSYMBOLSIZE:
                printf("\tContent symbol size: %d\n", ((boxing_metadata_item_content_symbol_size*)item)->value);
                break;
            default:
                printf("\tWarning: Unknown meta data type: %d\n", type);
                break;
            }
        }
    }
    printf("\n");
    return 0;
}
#endif

//---------------------------------------------------------------------------- 
/*! \ingroup unboxtests
 *
 *  Read unboxing parameters from input command line. If return_parameters.input_start_index not equal to zero, then reading parameters is successful.
 *
 *  \param[in] argc   Number parameters in the input command line.
 *  \param[in] argv   Array parameters in the input command line.
 *  \return unboxing parameters from input command line.
 */

static command_line_parameters get_parameters(int argc, char *argv[])
{
    command_line_parameters parameters;

    parameters.input_start_index = 0;
    parameters.input_end_index = 0;
    parameters.x_size = 0;
    parameters.y_size = 0;
    parameters.format = NULL;
    parameters.output = NULL;
    parameters.is_raw = DFALSE;

    int arg_index = 1;
    while (arg_index < argc)
    {
        if (strcmp(argv[arg_index], "-i") == 0)
        {
            while (++arg_index < argc && argv[arg_index][0] != '-')
            {
                if (parameters.input_start_index == 0)
                {
                    parameters.input_start_index = arg_index;
                }
            }
            if (parameters.input_start_index != 0)
            {
                parameters.input_end_index = arg_index - 1;
            }
        }
        else if (strcmp(argv[arg_index], "-s") == 0)
        {
            if (argc <= arg_index + 2)
            {
                break;
            }
            if (argv[arg_index + 1][0] == '-' || argv[arg_index + 2][0] == '-')
            {
                break;
            }
	    int x, y;
	    boxing_string_to_integer(&x, argv[++arg_index]);
	    boxing_string_to_integer(&y, argv[++arg_index]);
	    parameters.x_size = x;
	    parameters.y_size = y;
            arg_index++;
        }
        else if (strcmp(argv[arg_index], "-f") == 0)
        {
            if (argc <= arg_index + 1)
            {
                break;
            }
            if (argv[arg_index + 1][0] == '-')
            {
                break;
            }
            parameters.format = argv[++arg_index];
            arg_index++;
        }
        else if (strcmp(argv[arg_index], "-o") == 0)
        {
            if (argc <= arg_index + 1)
            {
                break;
            }
            if (argv[arg_index + 1][0] == '-')
            {
                break;
            }

            parameters.output = argv[++arg_index];
            arg_index++;
        }
        else if (strcmp(argv[arg_index++], "-is_raw") == 0)
        {
            parameters.is_raw = DTRUE;
        }
        else
        {
            fprintf(stderr, "Unsupported input parameter in the command line!!! (%s)\n", argv[arg_index++]);
        }
    }

    if (parameters.input_end_index == 0 || parameters.x_size == 0 || parameters.y_size == 0 || parameters.format == NULL)
    {
        parameters.input_start_index = 0;
    }

    if (parameters.input_start_index == 0)
    {
        printf(
            "Unboxer sample application - decodes data in input image(s).\n"
            "\n"
            "app -i <input-file> <input-file> ... -s <width> <height> -f <4kv6|4kv7|4kv8|4kv9|4kv10> <output-file> -is_raw\n"
            "\n"
            "Where:\n"
            "   -i <input file>     : Name of the binary input file(s) in the following format: \n"
            "                         width * height bytes: 8 bit per pixel grayscale data.\n"
            "   -s <width> <height> : Specify dimension of input file(s) in pixels. All files must have same dimensions.\n"
            "   -f <boxing-format>  : Boxing format. Supported formats: 4kv6, 4kv7, 4kv8, 4kv9 and 4kv10.\n"
            "   -o <output file>    : Write decoded data to file.\n"
            "   -is_raw             : (Optional) Use if input file is in RAW format.\n"
            );
    }

    return parameters;
}


static DBOOL has_wildcard(char* file_name)
{
    for (unsigned int i = 0; i < strlen(file_name); i++)
    {
        if (file_name[i] == '%' || file_name[i] == '*')
        {
            return DTRUE;
        }
    }

    return DFALSE;
}


static int unbox_file(const char* file_name, command_line_parameters input_parameters, boxing_unboxer_utility* utility)
{
    const size_t width = input_parameters.x_size;
    const size_t height = input_parameters.y_size;

#ifndef WIN32
    FILE * file = fopen(file_name, "r");
#else
    FILE * file = fopen(file_name, "rb");
#endif
    if (file == NULL)
    {
        fprintf(stderr, "Failed to read file '%s'.\n", file_name);
        return -1;
    }

    // Obtain file size
    if (DFSEEK(file, 0L, SEEK_END) != 0)
    {
        return -1;
    }

    const boxing_int64 file_size = DFTELL(file);
    if(file_size < 0)
    {
        return -1;
    }

    // Go to the starting data position
    if (DFSEEK(file, 0L, SEEK_SET) != 0)
    {
        return -1;
    }

    // Files size must match image dimensions
    if (file_size != (boxing_int64)(width * height))
    {
        printf("File '%s' size (%lld) does not match with input image dimensions (width=%lu height=%lu).\n", file_name, (unsigned long long)file_size, width, height);
        return -1;
    }

    printf("Reading image data file '%s' ...\n", file_name);

    boxing_image8* input_image = boxing_image8_create((unsigned int)width, (unsigned int)height);
    if (1 != fread(input_image->data, width * height, 1, file))
    {
        fprintf(stderr, "Failed to read image data.\n");
        return -1;
    }

    printf("The image data file read successfully!\nStarting unboxing...\n");

    fclose(file);

    gvector* output_data = gvector_create(1, 0);
    int process_result = boxing_unboxer_utility_unbox(utility, input_image, output_data);
    if (process_result == BOXING_UNBOXER_OK)
    {
        if (save_output_data(output_data, input_parameters.output) != 0)
        {
            return -1;
        }
    }
    gvector_free(output_data);
    boxing_image8_free(input_image);

    if (process_result != BOXING_UNBOXER_OK)
    {
        return -1;
    }

    return 0;
}


static void replace_wildcard(char* file_name)
{
    for (unsigned int i = 0; i < strlen(file_name); i++)
    {
        if (file_name[i] == '*')
        {
            file_name[i] = '%';
        }
    }
}


//---------------------------------------------------------------------------- 
/*! \ingroup unboxtests
 * 
 *  Unboxing sample application.
 *
 */

int main(int argc, char *argv[])
{
    command_line_parameters parameters = get_parameters(argc, argv);

    // If the parameters in the command line is invalid, then return
    if (parameters.input_start_index == 0)
    {
        return 1;
    }

    boxing_config * config = boxing_create_boxing_config(parameters.format);

#if defined (BOXINGLIB_CALLBACK)
    boxing_unboxer_utility* utility = boxing_unboxer_utility_create(config, parameters.is_raw, unboxing_complete_callback, metadata_complete_callback);
#else
    boxing_unboxer_utility* utility = boxing_unboxer_utility_create(config, parameters.is_raw);
#endif

    int result = 1;
    while (parameters.input_start_index <= parameters.input_end_index)
    {
        char* file_name = argv[parameters.input_start_index++];

        if (has_wildcard(file_name) == DFALSE)
        {
            printf("Input file: '%s'\n", file_name);
            result = unbox_file(file_name, parameters, utility);
        }
        else
        {
            int current_file = 0;
            replace_wildcard(file_name);

            printf("Input file wildcard: '%s'\n", file_name);
            
            while (1) {
                char* current_file_name = malloc(strlen(file_name) + 8 + 1);
                sprintf(current_file_name, file_name, current_file);

                {
                    FILE *access_check = fopen(current_file_name, "r");
                    if (access_check == NULL) {
                        if (current_file == 0) {
                            fprintf(stderr, "Failed to read files from input wildcard!\n");
                        }
                        break;
                    }
                    fclose(access_check);
                }

                result = unbox_file(current_file_name, parameters, utility);
                free(current_file_name);

                if (result != BOXING_UNBOXER_OK) break;

                current_file++;
            }
        }
    }

    boxing_unboxer_utility_free(utility);
    
    return result;
}

void boxing_log( int log_level, const char * string )
{
    printf( "%d : %s\n", log_level, string );
}

void boxing_log_args( int log_level, const char * format, ... )
{
    va_list args;
    va_start(args, format);

    printf( "%d : ", log_level );
    vprintf( format, args );
    printf( "\n" );

    va_end(args);
}

void(*boxing_log_custom)(int log_level, const char * string) = NULL;
void(*boxing_log_args_custom)(int log_level, const char * format, va_list args) = NULL;
