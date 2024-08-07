/** ****************************************************************************************************
 * @file            FileX_FS.c
 * @brief           Higher level functions that work with lower level FileX API calls
 * ****************************************************************************************************
 * @author original Hab Collector (habco)\n
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
 ********************************************************************************************************/
#include "FileX_FS.h"
//#include "fx_stm32_levelx_nand_driver.h"
#include <stdlib.h>


/*******************************************************************************************************
* @brief Determine if a file is present on disk
*
* @author original: Hab Collector \n
*
* @note: The FileX must be previously initialized
* @note: The Media drive must be previously opened
*
* @param MediaDrive: Handle to the Drive Media
* @param FilePathName: name of file to look for includes path if any
*
* @return True if file found
*
* STEP 1: Open File if OK file found
********************************************************************************************************/
bool FileX_FS_FileExists(FX_MEDIA *MediaDrive, char *FilePathName)
{
    UINT FileX_Status;
    FX_FILE FileHandle;
    bool FilePresentStatus = false;

    // STEP 1: Open File if OK file found
    FileX_Status = fx_file_open(MediaDrive, &FileHandle, FilePathName, FX_OPEN_FOR_READ);
    if (FileX_Status == FX_SUCCESS)
    {
        fx_file_close(&FileHandle);
        FilePresentStatus = true;
    }

    return(FilePresentStatus);

} // END OF FileX_FS_FileExists



/*******************************************************************************************************
* @brief Copies a file from one drive (media) to another drive (media) with an option to overwrite if the
* file is pre-exsisting on the destination media.
*
* @author original: Hab Collector \n
*
* @note: The FileX must be previously initialized
* @note: Both Media drives must be previously opened
*
* @param DestinationMedia: Handle to the Destination Drive Media
* @param DestinationFileName: File name of destination
* @param SourceMedia: Handle to the Source Drive Media
* @param SourceFileName: File name of source
* @param BlockPool: Block pool from which memory will be allocated
* @param BlockPoolBlockSize: The size of the block that can be allocated from the block pool
* @param TotalBytesTransfered: The total bytes transferred is returned by reference
* @param ForceOverwrite: Force (or not) overwrite if the destination file is already present
*
* @return See FileX FX return status for file read and write operations.  OK is FX_SUCCESS
*
* STEP 1: Verify Drives have been assigned with simple test
* STEP 2: Open the source file
* STEP 3: Create the destination file - check for overwrite if file exist
* STEP 4: Open the destination file
* STEP 5: Allocate memory buffer for file read write transfer
* STEP 6: Copy the file contents from Source To Destination until all bytes copied
* STEP 7: Free file handle and block resources
********************************************************************************************************/
UINT FileX_FS_FileCopyDriveToDrive(FX_MEDIA *DestinationMedia, char *DestinationFileName, FX_MEDIA *SourceMedia, char *SourceFileName, TX_BLOCK_POOL *BlockPool, ULONG BlockPoolBlockSize, uint32_t *TotalBytesTransfered, bool ForceOverwrite)
{
    FX_FILE DestinationFileHandle;
    FX_FILE SourceFileHandle;
    UINT FileX_Status;

    // STEP 1: Verify Drives have been assigned with simple test
    *TotalBytesTransfered = 0;
    if ((DestinationMedia == NULL) || (SourceMedia == NULL))
        return(FX_PTR_ERROR);

    // STEP 2: Open the source file
    FileX_Status = fx_file_open(SourceMedia, &SourceFileHandle, SourceFileName, FX_OPEN_FOR_READ);
    if (FileX_Status != FX_SUCCESS)
        return(FileX_Status);
    FileX_Status = fx_file_seek(&SourceFileHandle, 0);

    // STEP 3: Create the destination file - check for overwrite if file exist
    FileX_Status = fx_file_create(DestinationMedia, DestinationFileName);
    if (FileX_Status == FX_ALREADY_CREATED)
    {
        if (!ForceOverwrite)
        {
            fx_file_close(&SourceFileHandle);
            return(FileX_Status);
        }
        fx_file_delete(DestinationMedia, DestinationFileName);
        FileX_Status = fx_file_create(DestinationMedia, DestinationFileName);
        if (FileX_Status != FX_SUCCESS)
        {
            fx_file_close(&SourceFileHandle);
            return(FileX_Status);
        }
    }

    // STEP 4: Open the destination file
    FileX_Status = fx_file_open(DestinationMedia, &DestinationFileHandle, DestinationFileName, FX_OPEN_FOR_WRITE);
    if (FileX_Status != FX_SUCCESS)
    {
        fx_file_close(&SourceFileHandle);
        fx_file_close(&DestinationFileHandle);
        return(FileX_Status);
    }

    // STEP 5: Allocate memory buffer for file read write transfer from the block pool
    uint8_t *FileBuffer;
    if (tx_block_allocate(BlockPool, (VOID **)&FileBuffer, TX_NO_WAIT) != TX_SUCCESS)
    {
        fx_file_close(&SourceFileHandle);
        fx_file_close(&DestinationFileHandle);
        return(FX_PTR_ERROR);
    }

    // STEP 6: Copy the file contents from Source To Destination until all bytes copied
    ULONG BytesRead;
    do
    {
        FileX_Status = fx_file_read(&SourceFileHandle, FileBuffer, BlockPoolBlockSize, &BytesRead);
        if ((FileX_Status == FX_SUCCESS) && BytesRead)
        {
            FileX_Status = fx_file_write(&DestinationFileHandle, FileBuffer, BytesRead);
            if (FileX_Status != FX_SUCCESS)
                break;
            else
                *TotalBytesTransfered += BytesRead;
        }
    } while ((FileX_Status == FX_SUCCESS) && (BytesRead >= BlockPoolBlockSize));

    // STEP 7: Free file handle and block resources
    fx_file_close(&SourceFileHandle);
    fx_file_close(&DestinationFileHandle);
    tx_block_release(FileBuffer);
    fx_media_flush(DestinationMedia);

    return(FileX_Status);

} // END OF FileX_FS_FileCopyDriveToDrive






