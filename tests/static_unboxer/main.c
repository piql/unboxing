/*****************************************************************************
**
**  Unbox a static frame stored in a C array
**
**  Creation date:  2020/03/03
**  Created by:     Ole Liabo
**
**
**  Copyright (c) 2020 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  DEFINES
//
#define BOXINGLIB_CALLBACK
#define LOGGING_ENABLED

//  PROJECT INCLUDES
//
#include <stdio.h>
#include <stdarg.h>
#include "unboxingutility.h"
#include "boxing/utils.h"
#include "boxing/unboxer_utility.h"
#include "dataframe.h"
#include "boxing_config.h"

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
        printf("Unboxing failed! Errorcode = %i - %s\n", *res, get_process_result_name(*res));
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
      //printf("\nMeta data:\n");

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
    //printf("\n");
    return 0;
}
#endif


static int unbox_data(const unsigned char* data, unsigned int data_size, boxing_unboxer_utility* utility)
{
    const unsigned width = 4096/4;
    const unsigned height = 2160/4;

    boxing_image8* input_image = boxing_image8_create2(data, width, height);

    gvector* output_data = gvector_create(1, 0);
    int process_result = boxing_unboxer_utility_unbox(utility, input_image, output_data);
    if (process_result == BOXING_UNBOXER_OK)
    {
        if (save_output_data(output_data, "") != 0)
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


//---------------------------------------------------------------------------- 
/*! \ingroup unboxtests
 * 
 *  Unboxing sample application.
 *
 */

int main(int argc, char *argv[])
{
    DBOOL is_raw = DTRUE;
    const char* format_name = "4k-controlframe-v7";

    boxing_config * config = boxing_create_boxing_config(format_name);

#if defined (BOXINGLIB_CALLBACK)
    boxing_unboxer_utility* utility = boxing_unboxer_utility_create(config, is_raw, unboxing_complete_callback, metadata_complete_callback);
#else
    boxing_unboxer_utility* utility = boxing_unboxer_utility_create(config, is_raw);
#endif

    const unsigned char * data = tiff_fish_n_chips_x12_0000_raw;
    const unsigned int data_size = tiff_fish_n_chips_x12_0000_raw_len;
    int result = unbox_data(data, data_size, utility);

    
    boxing_unboxer_utility_free(utility);
    
    return result;
}


#if defined (LOGGING_ENABLED)
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
#else
void boxing_log(int log_level, const char * string) { BOXING_UNUSED_PARAMETER(log_level); BOXING_UNUSED_PARAMETER(string); }
void boxing_log_args(int log_level, const char * format, ...) { BOXING_UNUSED_PARAMETER(log_level); BOXING_UNUSED_PARAMETER(format); }
#endif // LOGGING_ENABLED

//void(*boxing_log_custom)(int log_level, const char * string) = NULL;
//void(*boxing_log_args_custom)(int log_level, const char * format, va_list args) = NULL;

