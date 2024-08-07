/** ****************************************************************************************************
 * @file            FileX_FS.h
 * @brief           This is the Header file used to support FileX_FS.c
 * ****************************************************************************************************
 * @author original Hab Collector (habco) \n
 *
 * @version         See Main_Support.h: FIRMWARE_REV_MAJOR, FIRMWARE_REV_MINOR
 *
 * @param Development_Environment \n
 * Hardware:        STM32L4R5VGTx\n
 * IDE:             STMCubeIDE \n
 * Compiler:        GCC \n
 * Editor Settings: 1 Tab = 4 Spaces, Recommended Courier New 11
 *
 * @note            See source file for notes
 *
 *                  This is an embedded application
 *                  It will be necessary to consult the reference documents to fully understand the code
 *                  It is suggested that the documents be reviewed in the order shown.
 *                    Schematic 002-5791-00
 *                    Spokane_Application
 *                    Design Document
 *
 * @copyright       Applied Concepts, Inc
 ****************************************************************************************************** */
#ifndef INC_THREADX_FILEX_FS_H_
#define INC_THREADX_FILEX_FS_H_

#ifdef __cplusplus
extern"C" {
#endif

#include "fx_api.h"
#include <stdint.h>
#include <stdbool.h>

// DEFINES
#define FILE_TRANSFER_BUFFER_SIZE           1024U


// TYPEDEFS AND ENUMS


// FUNCTION PROTOTYPES
bool FileX_FS_FileExists(FX_MEDIA *MediaDrive, char *FileName);
UINT FileX_FS_FileCopyDriveToDrive(FX_MEDIA *DestinationMedia, char *DestinationFileName, FX_MEDIA *SourceMedia, char *SourceFileName, TX_BLOCK_POOL *BlockPool, ULONG BlockPoolBlockSize, uint32_t *TotalBytesTransfered, bool ForceOverwrite);

#ifdef __cplusplus
}
#endif
#endif /* INC_THREADX_FILEX_FS_H__ */
