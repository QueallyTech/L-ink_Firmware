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

int32_t NDEF_Wrapper_ReadData(uint8_t* pData, uint32_t offset, uint32_t length );
int32_t NDEF_Wrapper_WriteData(const uint8_t* pData, uint32_t offset, uint32_t length );
uint32_t NDEF_Wrapper_GetMemorySize(void);

#ifdef __cplusplus
}
#endif
#endif /* __LIB_NDEF_H */
