#include "lib_NDEF.h"

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
