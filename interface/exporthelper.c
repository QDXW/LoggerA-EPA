/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : exporthelper.c
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------历史版本信息-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include "common.h"
#include "exporthelper.h"
#include "stringhelper.h"

#define EX_FRAME_SIZE 200

/**
* Start compress zip file of @day
* return 1 if success, otherwise 0
*/
int startCompressLogOfDay(int day)
{
    char cmd[256] = {0x00};

    DbgPrintf("XXX startCompressLogOfDay XXX\n");
    // prepare zip file
    if (day >= 1 && day <= 31)
    {
        // construct file name
        //my_itoa(day, filename);

        system("rm -f /mnt/flash/OAM/log/xxExportFile.tar.gz");
        usleep(5000);
        sprintf(cmd,"/mnt/flash/OAM/record/%02d.bin",day);
        if(access(cmd,R_OK)!=-1)
        {
            system("mkdir /mnt/flash/OAM/log/SouthFile");
            usleep(5000);
        }
        memset(cmd, 0, sizeof(cmd));
        usleep(5000);
        sprintf(cmd,"/mnt/flash/OAM/log/%d.bin",day);
        if(access(cmd,R_OK)!=-1)
        {
            system("mkdir /mnt/flash/OAM/log/NorthFile");
            usleep(5000);
        }
        memset(cmd, 0, sizeof(cmd));
        usleep(5000);
        // cp day.log xxExportFile
        /*strcat(cmd, "cp ");
        strcat(cmd, LogRecodeFilePath);
        strcat(cmd, filename);
        strcat(cmd, ".log ");
        strcat(cmd, "xxExportFile");*/

        sprintf(cmd,"cp /mnt/flash/OAM/log/%d.bin /mnt/flash/OAM/log/NorthFile",day);
        system(cmd);

        memset(cmd, 0, sizeof(cmd));
        usleep(5000);
        // cp day.bin xxExportFile
        /*strcat(cmd, "cp ");
        strcat(cmd, DataRecordFilePath);
        strcat(cmd, filename);
        strcat(cmd, ".bin ");
        strcat(cmd, "xxExportFile");
        system(cmd);*/
        sprintf(cmd,"cp /mnt/flash/OAM/record/%02d.bin /mnt/flash/OAM/log/SouthFile",day);
        system(cmd);
        usleep(5000);

        system("mkdir /mnt/flash/OAM/log/xxExportFile");
        usleep(5000);
        system("mv /mnt/flash/OAM/log/SouthFile /mnt/flash/OAM/log/xxExportFile");
        usleep(5000);
        system("mv /mnt/flash/OAM/log/NorthFile /mnt/flash/OAM/log/xxExportFile");
        usleep(5000);

        system("tar zcvf /mnt/flash/OAM/log/xxExportFile.tar.gz /mnt/flash/OAM/log/xxExportFile");
        usleep(10000);
        system("rm -rf /mnt/flash/OAM/log/xxExportFile");

        sleep(1);
        // compress finished
        return getLogPackageFrameNumber();
    }

    return 0;//Failed
}

/**
* Start compress zip file
* return file length(in bytes) if success, otherwise 0
*/
int StartCompressFile(char *aFilePath)
{
    char cmd[256] = {0x00};

    DbgPrintf("XXX startCompressFile: %s XXX\n", aFilePath);
    // prepare zip file
    if (strlen(aFilePath) > 5)
    {
        // construct file name
        //my_itoa(day, filename);

        system("rm -f /mnt/flash/OAM/ExportFile.tar.gz");
        usleep(5000);
        system("mkdir ExportFile");
        usleep(5000);

        // cp day.log xxExportFile
        /*strcat(cmd, "cp ");
        strcat(cmd, LogRecodeFilePath);
        strcat(cmd, filename);
        strcat(cmd, ".log ");
        strcat(cmd, "xxExportFile");*/

        sprintf(cmd,"cp %s ExportFile",aFilePath);
        system(cmd);

        memset(cmd, 0, sizeof(cmd));
        usleep(5000);
        // cp day.bin xxExportFile
        /*strcat(cmd, "cp ");
        strcat(cmd, DataRecordFilePath);
        strcat(cmd, filename);
        strcat(cmd, ".bin ");
        strcat(cmd, "xxExportFile");
        system(cmd);*/

        system("tar zcvf ExportFile.tar.gz ExportFile");
        usleep(10000);
        system("mv ExportFile.tar.gz /mnt/flash/OAM");
        system("rm -rf ExportFile");

        sleep(1);
        // compress finished
        return GetFileLength(EXPORT_FILE_PATH);
    }

    return 0;//Failed
}

/**
* get the compressed log files (.zip) frame number
* if not exists, return 0;
*/
int getLogPackageFrameNumber()
{
    struct stat statbuff;
    int size;

    if (stat("/mnt/flash/OAM/log/xxExportFile.tar.gz", &statbuff) < 0){
        size = 0;
    }else{
        size = (int)statbuff.st_size;
    }
    DbgPrintf("XXX tar size = %d XXX\n", size);
    return size % EX_FRAME_SIZE == 0 ? size / EX_FRAME_SIZE : 1 + size / EX_FRAME_SIZE;
}

/**
* get the compressed files (.zip) frame number
* if not exists, return 0;
*/
int GetFileLength(char *file_path)
{
    struct stat statbuff;
    int size;

    if (stat(file_path, &statbuff) < 0){
        size = 0;
    }else{
        size = (int)statbuff.st_size;
    }
    DbgPrintf("XXX file size = %d Bytes XXX\n", size);
    return size;
}

/**
* read next frame data and write into buff
* returns the size of current frame (bytes)
*/
int GetLogFrameData(char *buff, long frameNum)
{
    FILE *fd = NULL;
    int size = 0;

    fd = fopen("/mnt/flash/OAM/log/xxExportFile.tar.gz", "rb");
    if (fd == NULL) return 0;

    fseek(fd, frameNum * EX_FRAME_SIZE, 0);
    size = fread(buff, 1, EX_FRAME_SIZE, fd);
    fclose(fd);

    return size;
}

int GetFileData(char *aFilePath,char *buff, long frameNum, unsigned short frame_size)
{
    FILE *fd = NULL;
    int size = 0;

    fd = fopen(aFilePath, "rb");
    if (fd == NULL) return 0;

    fseek(fd, frameNum * frame_size, 0);
    size = fread(buff, 1, frame_size, fd);
    fclose(fd);

    return size;
}
