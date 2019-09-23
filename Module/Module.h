/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : Module.h
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------历史版本信息-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#include "../interface/interfaceExtern.h"

extern UINT16 gLocationReport;                 // 0: not report yet  others: already reported

void ModulePowerSet(UINT16 nStatus);
void ModelEnable();
void ModelRFEnable(UINT16 nStatus);
void *ModuleThreadMasterChannel();
void *ModuleThreadSlaveChannel();
void UploadCertificate();
