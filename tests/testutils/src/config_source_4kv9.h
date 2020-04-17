// *****************************************************************************
// ** This file is generated automatically by convertxmltocsource application **
// *****************************************************************************

#if !defined(CONFIG_SOURCE_DEFINITIONS)
#define CONFIG_SOURCE_DEFINITIONS
#define CONFIG_ARRAY_SIZE(a)   sizeof(a) / sizeof(a[0])

typedef struct config_pair_s
{
    const char* key;
    const char* value;
} config_pair;

typedef struct config_class_s
{
    const char* name;
    const char* version;
    config_pair * pairs;
    unsigned int count;
} config_class;

typedef struct config_structure_s
{
    const char * version;
    config_class * classes;
    const unsigned int count;
} config_structure;

#endif // CONFIG_SOURCE_DEFINITIONS

config_pair pairsFormatInfo_4kv9_1[] =
{
    { "name", "4kv9" },
    { "shortDescription", "4k data frame format" },
    { "description", "Frames are generated using the Generic Preservation Format v1.0 printed on a 4096x2160 raster" }
};

config_pair pairsFrameFormat_4kv9_2[] =
{
    { "type", "GPFv1.0" },
    { "width", "4096" },
    { "height", "2160" },
    { "border", "1" },
    { "borderGap", "1" },
    { "cornerMarkSize", "32" },
    { "cornerMarkGap", "1" },
    { "tilesPerColumn", "4" },
    /*  color dept = log2(maxLevelsPerSymbol) */
    { "maxLevelsPerSymbol", "4" }
};

config_pair pairsFrameRaster_4kv9_3[] =
{
    { "width", "4096" },
    { "height", "2160" },
    { "scalingFactor", "1" }
};

config_pair pairsCodecDispatcher_4kv9_4[] =
{
    /*  Frame coding scheme */
    /*  The order the encoder process is specified, default value: encode */
    { "order", "decode" },
    /*  symbol alignment default: bit */
    { "symbolAlignment", "byte" },
    { "DataCodingScheme", "SyncPointInserter,Modulator,Cipher,Interleaving_inner,ReedSolomon_inner,Interleaving_outer,ReedSolomon_outer" },
    { "MetadataCodingScheme", "MetaData_Modulator,MetaData_Cipher,MetaData_Interleaving,MetaData_ReedSolomon" }
};

config_pair pairsReedSolomon_outer_4kv9_5[] =
{
    { "codec", "ReedSolomon" },
    { "byteParityNumber", "10" },
    { "messageSize", "197" }
};

config_pair pairsInterleaving_outer_4kv9_6[] =
{
    { "codec", "Interleaving" },
    { "distance", "207" },
    /*  interleaving symboltype; block, frame */
    { "interleavingtype", "block" },
    /*  interleaving symboltype; byte,bit */
    { "symboltype", "byte" }
};

config_pair pairsReedSolomon_inner_4kv9_7[] =
{
    { "codec", "ReedSolomon" },
    { "byteParityNumber", "48" },
    { "messageSize", "207" }
};

config_pair pairsInterleaving_inner_4kv9_8[] =
{
    { "codec", "Interleaving" },
    { "distance", "248" },
    { "interleavingtype", "block" },
    /*  interleaving symboltype; byte, bit */
    { "symboltype", "byte" }
};

config_pair pairsCipher_4kv9_9[] =
{
    { "codec", "Cipher" },
    /*  chipher key ; 'auto' , 'any number supported by the chiper class' */
    { "key", "auto" }
};

config_pair pairsModulator_4kv9_10[] =
{
    { "codec", "Modulator" },
    /*  NumBitsPerPixel; auto - any supported color depth */
    { "NumBitsPerPixel", "auto" }
};

config_pair pairsSyncPointInserter_4kv9_11[] =
{
    { "codec", "SyncPointInserter" },
    { "SyncPointDistancePixel", "100" },
    { "SyncPointRadiusPixel", "3" },
    { "DataOrientation", "1" },
    /*  NumBitsPerPixel; auto - any supported color depth */
    { "NumBitsPerPixel", "auto" }
};

config_pair pairsMetaData_ReedSolomon_4kv9_12[] =
{
    { "codec", "ReedSolomon" },
    { "byteParityNumber", "40" },
    { "messageSize", "211" }
};

config_pair pairsMetaData_Interleaving_4kv9_13[] =
{
    { "codec", "Interleaving" },
    { "distance", "251" },
    /*  interleaving symboltype; block, frame */
    { "interleavingtype", "block" },
    /*  interleaving symboltype; byte,bit */
    { "symboltype", "byte" }
};

config_pair pairsMetaData_Modulator_4kv9_14[] =
{
    { "codec", "Modulator" },
    /*  NumBitsPerPixel; auto - any supported color depth */
    { "NumBitsPerPixel", "auto" }
};

config_pair pairsMetaData_Cipher_4kv9_15[] =
{
    { "codec", "Cipher" },
    /*  chipher key ; 'auto' , 'any number supported by the chiper class' */
    { "key", "1" }
};

config_pair pairscrc64_4kv9_16[] =
{
    { "POLY64REV", "0x42F0E1EBA9EA3693" },
    { "INITIALCRC", "0x0000000000000000" }
};

config_pair pairscrc32_4kv9_17[] =
{
    { "POLY32REV", "0x42F0E1EBA9EA3693" },
    { "INITIALCRC", "0x0000000000000000" }
};

config_class config_classes_4kv9[] =
{
    { "FormatInfo", NULL, pairsFormatInfo_4kv9_1, CONFIG_ARRAY_SIZE(pairsFormatInfo_4kv9_1) },
    { "FrameFormat", NULL, pairsFrameFormat_4kv9_2, CONFIG_ARRAY_SIZE(pairsFrameFormat_4kv9_2) },
    { "FrameRaster", NULL, pairsFrameRaster_4kv9_3, CONFIG_ARRAY_SIZE(pairsFrameRaster_4kv9_3) },
    { "CodecDispatcher", NULL, pairsCodecDispatcher_4kv9_4, CONFIG_ARRAY_SIZE(pairsCodecDispatcher_4kv9_4) },
    { "ReedSolomon_outer", NULL, pairsReedSolomon_outer_4kv9_5, CONFIG_ARRAY_SIZE(pairsReedSolomon_outer_4kv9_5) },
    { "Interleaving_outer", NULL, pairsInterleaving_outer_4kv9_6, CONFIG_ARRAY_SIZE(pairsInterleaving_outer_4kv9_6) },
    { "ReedSolomon_inner", NULL, pairsReedSolomon_inner_4kv9_7, CONFIG_ARRAY_SIZE(pairsReedSolomon_inner_4kv9_7) },
    { "Interleaving_inner", NULL, pairsInterleaving_inner_4kv9_8, CONFIG_ARRAY_SIZE(pairsInterleaving_inner_4kv9_8) },
    { "Cipher", NULL, pairsCipher_4kv9_9, CONFIG_ARRAY_SIZE(pairsCipher_4kv9_9) },
    { "Modulator", NULL, pairsModulator_4kv9_10, CONFIG_ARRAY_SIZE(pairsModulator_4kv9_10) },
    { "SyncPointInserter", NULL, pairsSyncPointInserter_4kv9_11, CONFIG_ARRAY_SIZE(pairsSyncPointInserter_4kv9_11) },
    { "MetaData_ReedSolomon", NULL, pairsMetaData_ReedSolomon_4kv9_12, CONFIG_ARRAY_SIZE(pairsMetaData_ReedSolomon_4kv9_12) },
    { "MetaData_Interleaving", NULL, pairsMetaData_Interleaving_4kv9_13, CONFIG_ARRAY_SIZE(pairsMetaData_Interleaving_4kv9_13) },
    { "MetaData_Modulator", NULL, pairsMetaData_Modulator_4kv9_14, CONFIG_ARRAY_SIZE(pairsMetaData_Modulator_4kv9_14) },
    { "MetaData_Cipher", NULL, pairsMetaData_Cipher_4kv9_15, CONFIG_ARRAY_SIZE(pairsMetaData_Cipher_4kv9_15) },
    { "crc64", NULL, pairscrc64_4kv9_16, CONFIG_ARRAY_SIZE(pairscrc64_4kv9_16) },
    { "crc32", NULL, pairscrc32_4kv9_17, CONFIG_ARRAY_SIZE(pairscrc32_4kv9_17) }};

config_structure config_source_4kv9 = { "1.0.0.0", config_classes_4kv9, CONFIG_ARRAY_SIZE(config_classes_4kv9) };
