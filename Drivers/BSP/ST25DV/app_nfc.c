#include "../E-Paper-Display/Epd.h"
#include "app_nfc.h"
#include "main.h"
#include "nfc04a1_nfctag.h"
#include "tagtype5_wrapper.h"
#include <stdio.h>

uint8_t cnt = 0;
uint16_t mblength;
volatile uint8_t GPOActivated = 0;

ST25DV_MB_CTRL_DYN_STATUS mbctrldynstatus;
ST25DV_EN_STATUS MB_mode;
ST25DV_PASSWD passwd;
ST25DV_I2CSSO_STATUS i2csso;

void MX_NFC_Init(void)
{
    /* Init ST25DV driver */
    while (NFC04A1_NFCTAG_Init(NFC04A1_NFCTAG_INSTANCE) != NFCTAG_OK);

    /* Energy harvesting activated after Power On Reset */
    NFC04A1_NFCTAG_WriteEHMode(NFC04A1_NFCTAG_INSTANCE, ST25DV_EH_ACTIVE_AFTER_BOOT);
    NFC04A1_NFCTAG_SetEHENMode_Dyn(NFC04A1_NFCTAG_INSTANCE);

//    NFC04A1_NFCTAG_ResetMBEN_Dyn(NFC04A1_NFCTAG_INSTANCE);

    MX_NFC4_NDEF_URI_Init();
    MX_NFC4_MAILBOX_Init();
}

void MX_NFC_Process(void)
{
    MX_NFC4_MAILBOX_Process();
}

void MX_NFC4_NDEF_URI_Init(void)
{
    /* Check if no NDEF detected, init mem in Tag Type 5 */
    if( NfcType5_NDEFDetection( ) != NDEF_OK )
    {
		// CC File
	    const uint8_t CCbuffer[9]={NFCT5_MAGICNUMBER_E2_CCFILE, (NFCT5_VERSION_V1_0 + NFCT5_WRITE_ACCESS), 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, NFCT5_NDEF_MSG_TLV};
		// NDEF Record:
	    // Record #1: External type record
	    // Type: com.queallytech:nfc ,Data: L-ink
	    // Record #2: URI record
	    // Type: U ,Protocol field: https://www. ,URI field: github.com/QueallyTech/L-ink_Firmware
	    const uint8_t Infobuffer[]={0x45,
	                                0x94, 0x13, 0x05,
	                                0x63, 0x6F, 0x6D, 0x2E, 0x71, 0x75, 0x65, 0x61, 0x6c, 0x6c, 0x79, 0x74, 0x65, 0x63, 0x68, 0x3A,
	                                0x6E, 0x66, 0x63, 0x4C, 0x2D, 0x69, 0x6E, 0x6B,
	                                0x51, 0x01, 0x26, 0x55, 0x02,
	                                0x67, 0x69, 0x74, 0x68, 0x75, 0x62, 0x2E, 0x63, 0x6F, 0x6D, 0x2F, 0x51, 0x75, 0x65, 0x61, 0x6C, 0x6C, 0x79, 0x54, 0x65, 0x63, 0x68, 0x2F, 0x4C, 0x2D, 0x69, 0x6E, 0x6B, 0x5F, 0x46, 0x69, 0x72, 0x6D, 0x77, 0x61, 0x72, 0x65, 0xFE};

		uint8_t offset = 0;
	    // Write CC File
		while(NFC04A1_NFCTAG_WriteData(NFC04A1_NFCTAG_INSTANCE,CCbuffer, offset, sizeof(CCbuffer)) != NDEF_OK );
		offset += sizeof(CCbuffer);
	    // Write NDEF Info Data
		while(NFC04A1_NFCTAG_WriteData(NFC04A1_NFCTAG_INSTANCE,Infobuffer, offset, sizeof(Infobuffer)) != NDEF_OK );
    }
}

void MX_NFC4_MAILBOX_Init(void)
{
    /* You need to present password to change static configuration */
    NFC04A1_NFCTAG_ReadI2CSecuritySession_Dyn(NFC04A1_NFCTAG_INSTANCE, &i2csso);
    if (i2csso == ST25DV_SESSION_CLOSED)
    {
        /* if I2C session is closed, present password to open session */
        passwd.MsbPasswd = 0;
        passwd.LsbPasswd = 0;
        NFC04A1_NFCTAG_PresentI2CPassword(NFC04A1_NFCTAG_INSTANCE, passwd);
    }

    /* If not activated, activate Mailbox, as long as MB is ON EEPROM is not available */
    NFC04A1_NFCTAG_ReadMBMode(NFC04A1_NFCTAG_INSTANCE, &MB_mode);
    if (MB_mode == ST25DV_DISABLE)
    {
        /* You need to present password before changing static configuration */
        NFC04A1_NFCTAG_ReadI2CSecuritySession_Dyn(NFC04A1_NFCTAG_INSTANCE, &i2csso);
        if (i2csso == ST25DV_SESSION_CLOSED)
        {
            /* if I2C session is closed, present password to open session */
            passwd.MsbPasswd = 0; /* Default value for password */
            passwd.LsbPasswd = 0; /* change it if password has been modified */
            NFC04A1_NFCTAG_PresentI2CPassword(NFC04A1_NFCTAG_INSTANCE, passwd);
        }
        NFC04A1_NFCTAG_WriteMBMode(NFC04A1_NFCTAG_INSTANCE, ST25DV_ENABLE);

        /* Close session as dynamic register doesn't need open session for modification */
        passwd.MsbPasswd = 123;
        passwd.LsbPasswd = 123;
        NFC04A1_NFCTAG_PresentI2CPassword(NFC04A1_NFCTAG_INSTANCE, passwd);
    }

    /* Enable Mailbox in dynamique register */
    NFC04A1_NFCTAG_SetMBEN_Dyn(NFC04A1_NFCTAG_INSTANCE);
    printf("\n\r\n\rMailbox is activated");

    /* Set EXTI settings for GPO Interrupt */
    NFC04A1_GPO_Init();
    /* Present configuration password */
    passwd.MsbPasswd = 0; /* Default value for password */
    passwd.LsbPasswd = 0; /* change it if password has been modified */
    NFC04A1_NFCTAG_PresentI2CPassword(NFC04A1_NFCTAG_INSTANCE, passwd);
    /* Set GPO Configuration */
    NFC04A1_NFCTAG_ConfigIT(NFC04A1_NFCTAG_INSTANCE,
                            ST25DV_GPO_ENABLE_MASK | ST25DV_GPO_RFGETMSG_MASK | ST25DV_GPO_RFPUTMSG_MASK);

}

/**
* @brief  Process of the MAILBOX application
* @retval None
*/

extern unsigned char nfcBuffer[];
int bufferIndex = 0;

void MX_NFC4_MAILBOX_Process(void)
{
    if (GPOActivated == 1)
    {
        /* Check if Mailbox is available */
        NFC04A1_NFCTAG_ReadMBCtrl_Dyn(NFC04A1_NFCTAG_INSTANCE, &mbctrldynstatus);

        if (mbctrldynstatus.RfPutMsg == 1)
        {
            /* Read length of message */
            NFC04A1_NFCTAG_ReadMBLength_Dyn(NFC04A1_NFCTAG_INSTANCE, (uint8_t *) &mblength);
            mblength += 1;

#if DEBUG
            /* Read mailbox status */
            NFC04A1_NFCTAG_ReadMBCtrl_Dyn(NFC04A1_NFCTAG_INSTANCE, &mbctrldynstatus);
            printf("\n\r\n\rCtrl MB status register value:");
            printf("\n\rHost(i2c) Missed Message  = %d", mbctrldynstatus.HostMissMsg);
            printf("\n\rRF(reader) Missed Message = %d", mbctrldynstatus.RFMissMsg);
            printf("\n\rHost(i2c) Put Message     = %d", mbctrldynstatus.HostPutMsg);
            printf("\n\rRF(reader) Put Message    = %d", mbctrldynstatus.RfPutMsg);
            printf("\n\rMailbox Enable            = %d", mbctrldynstatus.MbEnable);
#endif

            if (mblength == 3) // frame header
            {
                bufferIndex = 0;
            } else if (mblength == 200) // picture data
            {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);

                /* Read all data in Mailbox */
                NFC04A1_NFCTAG_ReadMailboxData(NFC04A1_NFCTAG_INSTANCE, nfcBuffer + bufferIndex, 0, 200);
                bufferIndex += 200;

                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
            }

            if (bufferIndex == 5000)
            {
                bufferIndex = 0;

                EpdDisFull((unsigned char *) nfcBuffer, 1);
                HAL_Delay(3000);
            }

        }

        GPOActivated = 0;
    }
}

void BSP_GPO_Callback(void)
{
    /* Prevent unused argument(s) compilation warning */
    GPOActivated = 1;
    /* This function should be implemented by the user application.
       It is called into this driver when an event on Button is triggered. */
}
