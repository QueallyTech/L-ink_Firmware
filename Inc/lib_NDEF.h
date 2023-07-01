/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LIB_NDEF_H
#define __LIB_NDEF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <common.h>
#include <stdint.h>
#include <string.h>

/* include file which match the HW configuration */

#define NDEF_ACTION_COMPLETED       0x9000

#ifndef errorchk
#define errorchk(fCall) if (status = (fCall), status != NDEF_ACTION_COMPLETED) \
  {goto Error;} else
#endif

/* Error codes for Higher level */
#define NDEF_OK                     RESULTOK
#define NDEF_ERROR                  ERRORCODE_GENERIC
#define NDEF_ERROR_MEMORY_TAG       2
#define NDEF_ERROR_NOT_FORMATED     5

#define NDEF_MAX_SIZE               NFC_DEVICE_MAX_NDEFMEMORY

#define SP_MAX_RECORD               4

#define URI_TYPE_STRING                         "U"
#define URI_TYPE_STRING_LENGTH                  1

#define TEXT_TYPE_STRING                        "T"
#define TEXT_TYPE_STRING_LENGTH                 1

#define ISO_ENGLISH_CODE_STRING                 "en"
#define ISO_ENGLISH_CODE_STRING_LENGTH          2

#define URI_ID_0x00                 0x00
#define URI_ID_0x01                 0x01
#define URI_ID_0x02                 0x02
#define URI_ID_0x03                 0x03
#define URI_ID_0x04                 0x04

#define URI_ID_0x01_STRING          "http://www.\0"
#define URI_ID_0x02_STRING          "https://www.\0"
#define URI_ID_0x03_STRING          "http://\0"
#define URI_ID_0x04_STRING          "https://\0"

typedef struct sRecordInfo sRecordInfo_t;

struct sRecordInfo
{
  uint8_t RecordFlags;
  uint8_t TypeLength;
  uint32_t PayloadLength;
  uint8_t IDLength;
  uint8_t Type[0xFF];
  uint8_t ID[0xFF];
  uint16_t PayloadOffset;
  uint8_t* PayloadBufferAdd;    /* add where payload content has been stored */
  sRecordInfo_t *SPRecordStructAdd[SP_MAX_RECORD]; /*in case of smart poster array to store add of other sRecordInfo struct */
  uint8_t NbOfRecordInSPPayload;
};

typedef struct
{
    char protocol[80];
    char URI_Message[400];
    char Information[400];
}sURI_Info;

int32_t NDEF_Wrapper_ReadData(uint8_t* pData, uint32_t offset, uint32_t length );
int32_t NDEF_Wrapper_WriteData(const uint8_t* pData, uint32_t offset, uint32_t length );
uint32_t NDEF_Wrapper_GetMemorySize(void);
uint16_t NDEF_WriteURI(sURI_Info *pURI);

void NDEF_PrepareURIMessage( sURI_Info *pURI, uint8_t *pNDEFMessage, uint16_t *size );
char getUriType( char *protocol );

#ifdef __cplusplus
}
#endif
#endif /* __LIB_NDEF_H */
