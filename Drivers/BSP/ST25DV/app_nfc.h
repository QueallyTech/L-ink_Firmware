#ifndef __APP_NFC_H
#define __APP_NFC_H

#ifdef __cplusplus
 extern "C" {
#endif
 
void MX_NFC_Init(void);
void MX_NFC_Process(void);
void MX_NFC4_MAILBOX_Init(void);
void MX_NFC4_NDEF_URI_Init(void);
void MX_NFC4_MAILBOX_Process(void);

#ifdef __cplusplus
}
#endif
#endif  
 
