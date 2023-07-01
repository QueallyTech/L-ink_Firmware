#include "lib_NDEF.h"
#include "tagtype5_wrapper.h"

uint8_t NDEF_Buffer [NDEF_MAX_SIZE];

int32_t NDEF_Wrapper_ReadData(uint8_t* pData, uint32_t offset, uint32_t length )
{
    if(NFC04A1_NFCTAG_ReadData(NFC04A1_NFCTAG_INSTANCE, pData, offset, length ) != NFCTAG_OK)
        return NDEF_ERROR;
    return NDEF_OK;
}

int32_t NDEF_Wrapper_WriteData(const uint8_t* pData, uint32_t offset, uint32_t length )
{
    if(NFC04A1_NFCTAG_WriteData(NFC04A1_NFCTAG_INSTANCE, pData, offset, length ) != NFCTAG_OK)
        return NDEF_ERROR;
    return NDEF_OK;
}

uint32_t NDEF_Wrapper_GetMemorySize(void)
{
    return NFC04A1_NFCTAG_GetByteSize(NFC04A1_NFCTAG_INSTANCE);
}

void NDEF_PrepareURIMessage( sURI_Info *pURI, uint8_t *pNDEFMessage, uint16_t *size )
{
    uint32_t uriSize, Offset = 0;
    uint32_t infoSize = 0;
    char type;

    /* We need to know the URI type in order to define if an abreviation is available */
    type = getUriType( pURI->protocol );

    /* URI : 1+URI for abreviate protocol*/
    if( type != URI_ID_0x00 )
        uriSize = 1 + strlen(pURI->URI_Message);
    else /*: 1+protocol+URI else*/
        uriSize = 1 + strlen(pURI->protocol) + strlen(pURI->URI_Message);
    /* URI header */
    pNDEFMessage[Offset] = 0x81;
    if( uriSize < 256 ) pNDEFMessage[Offset] |= 0x10;                      // Set the SR bit
    if( pURI->Information[0] == '\0' ) pNDEFMessage[Offset] |= 0x40;       // Set the ME bit
    Offset++;

    pNDEFMessage[Offset++] = URI_TYPE_STRING_LENGTH;
    if( uriSize > 255 )
    {
        pNDEFMessage[Offset++] = (uriSize & 0xFF000000) >> 24;
        pNDEFMessage[Offset++] = (uriSize & 0x00FF0000) >> 16;
        pNDEFMessage[Offset++] = (uriSize & 0x0000FF00) >> 8;
        pNDEFMessage[Offset++] = uriSize & 0x000000FF;
    }
    else
    {
        pNDEFMessage[Offset++] = (uint8_t)uriSize;
    }
    memcpy( &pNDEFMessage[Offset], URI_TYPE_STRING, URI_TYPE_STRING_LENGTH );
    Offset += URI_TYPE_STRING_LENGTH;

    pNDEFMessage[Offset++] = type;
    if( type == URI_ID_0x00 ) // No abreviation
    {
        memcpy( &pNDEFMessage[Offset], pURI->protocol, strlen(pURI->protocol) );
        Offset += strlen(pURI->protocol);
    }

    memcpy( &pNDEFMessage[Offset], pURI->URI_Message, strlen(pURI->URI_Message) );
    Offset += strlen(pURI->URI_Message);

    /* Information header */
    if( pURI->Information[0] != '\0' )
    {
        pNDEFMessage[Offset++] = 0x51;
        pNDEFMessage[Offset++] = TEXT_TYPE_STRING_LENGTH;
        pNDEFMessage[Offset++] = (uint8_t)infoSize;

        memcpy( &pNDEFMessage[Offset], TEXT_TYPE_STRING, TEXT_TYPE_STRING_LENGTH );
        Offset+=TEXT_TYPE_STRING_LENGTH;
        pNDEFMessage[Offset++] = ISO_ENGLISH_CODE_STRING_LENGTH; /* UTF-8 with x byte language code */
        memcpy( &pNDEFMessage[Offset], ISO_ENGLISH_CODE_STRING, ISO_ENGLISH_CODE_STRING_LENGTH );
        Offset += ISO_ENGLISH_CODE_STRING_LENGTH;

        /* Information payload */
        memcpy( &pNDEFMessage[Offset], pURI->Information, strlen(pURI->Information) );
        Offset += strlen(pURI->Information);
    }

    *size = Offset;

}
/**
  * @brief  This function write the NDEF file with the URI data given in the structure.
  * @param  pURI : pointer on structure that contain the URI information.
  * @retval NDEF_OK : NDEF file data written in the tag.
  * @retval NDEF_ERROR : not able to store NDEF in tag.
  * @retval NDEF_ERROR_MEMORY_INTERNAL : Cannot write to tag.
  * @retval NDEF_ERROR_NOT_FORMATED : CCFile data not supported or not present.
  * @retval NDEF_ERROR_MEMORY_TAG : Size not compatible with memory.
  * @retval NDEF_ERROR_LOCKED : Tag locked, cannot be write.
  */
uint16_t NDEF_WriteURI( sURI_Info *pURI )
{
    uint16_t status = NDEF_ERROR, Offset = 0;

    NDEF_PrepareURIMessage( pURI, NDEF_Buffer, &Offset );

    status = NfcType5_WriteNDEF( Offset , NDEF_Buffer );

    return status;
}

char getUriType( char *protocol )
{
    if( !memcmp( protocol, URI_ID_0x01_STRING, strlen(URI_ID_0x01_STRING) ) ) return URI_ID_0x01;
    else if( !memcmp( protocol, URI_ID_0x02_STRING, strlen(URI_ID_0x02_STRING) ) ) return URI_ID_0x02;
    else if( !memcmp( protocol, URI_ID_0x03_STRING, strlen(URI_ID_0x03_STRING) ) ) return URI_ID_0x03;
    else if( !memcmp( protocol, URI_ID_0x04_STRING, strlen(URI_ID_0x04_STRING) ) ) return URI_ID_0x04;
    else return URI_ID_0x00; // No abreviation for this protocol
}
