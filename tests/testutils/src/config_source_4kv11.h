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
}config_pair;

typedef struct config_class_s
{
    const char* name;
    const char* version;
    config_pair * pairs;
    unsigned int count;
}config_class;

typedef struct config_structure_s
{
    const char * version;
    config_class * classes;
    const unsigned int count;
}config_structure;

#endif // CONFIG_SOURCE_DEFINITIONS

config_pair pairsFormatInfo_4kv11_1[] =
{
    { "name", "4kv11" },
    { "shortDescription", "4k data frame format" },
    { "description", "Frames are generated using the Generic Preservation Format v1.1 printed on a 4096x2160 raster" }
};

config_pair pairsFrameFormat_4kv11_2[] =
{
    { "type", "GPFv1.1" },
    { "width", "4096" },
    { "height", "2160" },
    { "border", "1" },
    { "borderGap", "1" },
    { "cornerMarkSize", "32" },
    { "cornerMarkGap", "1" },
    { "tilesPerColumn", "4" },
    { "refBarSyncDistance", "66" },
    { "refBarSyncOffset", "16" },
    /*  color dept = log2(maxLevelsPerSymbol) */
    { "maxLevelsPerSymbol", "4" }
};

config_pair pairsMultiFrameFormat_4kv11_3[] =
{
    /*  size in frames. Default is 1, no striping */
    { "DataStripeSize", "200" }
};

config_pair pairsFrameRaster_4kv11_4[] =
{
    { "width", "4096" },
    { "height", "2160" },
    { "scalingFactor", "1" }
};

config_pair pairsCodecDispatcher_4kv11_5[] =
{
    /*  Frame coding scheme */
    { "version", "1.0" },
    /*  The order the encoder process is specified, default value: encode */
    { "order", "decode" },
    /*  symbol alignment default: bit */
    { "symbolAlignment", "byte" },
    /* <property key="DataCodingScheme" value="SyncPointInserter,Modulator,Cipher,FTFInterleave,CRC,PH"/>*/
    { "DataCodingScheme", "SyncPointInserter,Modulator,Cipher,Striping,FInterleave,RS_inner,XInterleave,RS_outer,CRC,PH" },
    { "MetadataCodingScheme", "MetaData_Modulator,MetaData_Cipher,MetaData_Interleaving,MetaData_ReedSolomon,MetaData_CRC,PH" }
};

config_pair pairsPH_4kv11_6[] =
{
    { "codec", "PacketHeader" }
};

config_pair pairsCRC_4kv11_7[] =
{
    { "codec", "CRC64" },
    { "polynom", "0x42F0E1EBA9EA3693" },
    { "seed", "0x0000000000000000" }
};

config_pair pairsRS_outer_4kv11_8[] =
{
    { "codec", "ReedSolomon" },
    { "byteParityNumber", "6" },
    { "messageSize", "225" }
};

config_pair pairsXInterleave_4kv11_9[] =
{
    { "codec", "Interleaving" },
    { "distance", "231" },
    /*  interleaving symboltype; block, frame */
    { "interleavingtype", "block" },
    /*  interleaving symboltype; byte,bit */
    { "symboltype", "byte" }
};

config_pair pairsRS_inner_4kv11_10[] =
{
    { "codec", "ReedSolomon" },
    { "byteParityNumber", "24" },
    { "messageSize", "231" }
};

config_pair pairsFInterleave_4kv11_11[] =
{
    { "codec", "Interleaving" },
    { "distance", "248" },
    { "interleavingtype", "block" },
    /*  interleaving symboltype; byte, bit */
    { "symboltype", "byte" }
};

config_pair pairsStriping_4kv11_12[] =
{
    { "codec", "FTFInterleaving" },
    { "DataStripeSize", "auto" }
};

config_pair pairsCipher_4kv11_13[] =
{
    { "codec", "Cipher" },
    /*  chipher key ; 'auto' , 'any number supported by the chiper class' */
    { "key", "auto" }
};

config_pair pairsModulator_4kv11_14[] =
{
    { "codec", "Modulator" },
    /*  NumBitsPerPixel; auto - any supported color depth */
    { "NumBitsPerPixel", "auto" }
};

config_pair pairsSyncPointInserter_4kv11_15[] =
{
    { "codec", "SyncPointInserter" },
    { "SyncPointDistancePixel", "100" },
    { "SyncPointRadiusPixel", "3" },
    { "DataOrientation", "1" },
    /*  NumBitsPerPixel; auto - any supported color depth */
    { "NumBitsPerPixel", "auto" }
};

config_pair pairsMetaData_CRC_4kv11_16[] =
{
    { "codec", "CRC32" },
    { "polynom", "0x1EDC6F41" },
    { "seed", "0x00000000" }
};

config_pair pairsMetaData_ReedSolomon_4kv11_17[] =
{
    { "codec", "ReedSolomon" },
    { "byteParityNumber", "40" },
    { "messageSize", "211" }
};

config_pair pairsMetaData_Interleaving_4kv11_18[] =
{
    { "codec", "Interleaving" },
    { "distance", "251" },
    /*  interleaving symboltype; block, frame */
    { "interleavingtype", "block" },
    /*  interleaving symboltype; byte,bit */
    { "symboltype", "byte" }
};

config_pair pairsMetaData_Modulator_4kv11_19[] =
{
    { "codec", "Modulator" },
    /*  NumBitsPerPixel; auto - any supported color depth */
    { "NumBitsPerPixel", "auto" }
};

config_pair pairsMetaData_Cipher_4kv11_20[] =
{
    { "codec", "Cipher" },
    /*  chipher key ; 'auto' , 'any number supported by the chiper class' */
    { "key", "1" }
};

config_class config_classes_4kv11[] =
{
    { "FormatInfo", NULL, pairsFormatInfo_4kv11_1, CONFIG_ARRAY_SIZE(pairsFormatInfo_4kv11_1) },
    { "FrameFormat", NULL, pairsFrameFormat_4kv11_2, CONFIG_ARRAY_SIZE(pairsFrameFormat_4kv11_2) },
    { "MultiFrameFormat", NULL, pairsMultiFrameFormat_4kv11_3, CONFIG_ARRAY_SIZE(pairsMultiFrameFormat_4kv11_3) },
    { "FrameRaster", NULL, pairsFrameRaster_4kv11_4, CONFIG_ARRAY_SIZE(pairsFrameRaster_4kv11_4) },
    { "CodecDispatcher", NULL, pairsCodecDispatcher_4kv11_5, CONFIG_ARRAY_SIZE(pairsCodecDispatcher_4kv11_5) },
    { "PH", NULL, pairsPH_4kv11_6, CONFIG_ARRAY_SIZE(pairsPH_4kv11_6) },
    { "CRC", NULL, pairsCRC_4kv11_7, CONFIG_ARRAY_SIZE(pairsCRC_4kv11_7) },
    { "RS_outer", NULL, pairsRS_outer_4kv11_8, CONFIG_ARRAY_SIZE(pairsRS_outer_4kv11_8) },
    { "XInterleave", NULL, pairsXInterleave_4kv11_9, CONFIG_ARRAY_SIZE(pairsXInterleave_4kv11_9) },
    { "RS_inner", NULL, pairsRS_inner_4kv11_10, CONFIG_ARRAY_SIZE(pairsRS_inner_4kv11_10) },
    { "FInterleave", NULL, pairsFInterleave_4kv11_11, CONFIG_ARRAY_SIZE(pairsFInterleave_4kv11_11) },
    { "Striping", NULL, pairsStriping_4kv11_12, CONFIG_ARRAY_SIZE(pairsStriping_4kv11_12) },
    { "Cipher", NULL, pairsCipher_4kv11_13, CONFIG_ARRAY_SIZE(pairsCipher_4kv11_13) },
    { "Modulator", NULL, pairsModulator_4kv11_14, CONFIG_ARRAY_SIZE(pairsModulator_4kv11_14) },
    { "SyncPointInserter", NULL, pairsSyncPointInserter_4kv11_15, CONFIG_ARRAY_SIZE(pairsSyncPointInserter_4kv11_15) },
    { "MetaData_CRC", NULL, pairsMetaData_CRC_4kv11_16, CONFIG_ARRAY_SIZE(pairsMetaData_CRC_4kv11_16) },
    { "MetaData_ReedSolomon", NULL, pairsMetaData_ReedSolomon_4kv11_17, CONFIG_ARRAY_SIZE(pairsMetaData_ReedSolomon_4kv11_17) },
    { "MetaData_Interleaving", NULL, pairsMetaData_Interleaving_4kv11_18, CONFIG_ARRAY_SIZE(pairsMetaData_Interleaving_4kv11_18) },
    { "MetaData_Modulator", NULL, pairsMetaData_Modulator_4kv11_19, CONFIG_ARRAY_SIZE(pairsMetaData_Modulator_4kv11_19) },
    { "MetaData_Cipher", NULL, pairsMetaData_Cipher_4kv11_20, CONFIG_ARRAY_SIZE(pairsMetaData_Cipher_4kv11_20) }};

config_structure config_source_4kv11 = { "1.0.0.0", config_classes_4kv11, CONFIG_ARRAY_SIZE(config_classes_4kv11) };
