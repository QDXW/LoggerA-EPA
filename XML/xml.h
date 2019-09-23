/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : xml.h
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------历史版本信息-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#include "../interface/common.h"


//Common
#define FileHead             "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
#define FileEnter            "\r\n"
#define ProtocolRegex        "<Protocol>%d</Protocol>\r\n"
#define ProtocolHead         "<Protocol>"
#define ProtocolEnd          "</Protocol>"
#define TagBaseHead          "<Device>"
#define TagBaseEnd           "</Device>"

#define TagIDRegex           "<Tag id=\"%02d\">\r\n"
#define TagIDEnd             "</Tag>"

//TagBase Info
#define TagBaseFilePath      "/mnt/flash/OAM/xml/base.xml"
#define TagUseHead           "<Use>"
#define TagUseEnd            "</Use>"
#define TagUseRegex          "<Use>%d</Use>\r\n"
#define TagESNHead           "<ESN>"
#define TagESNEnd            "</ESN>"
#define TagESNRegex          "<ESN>%s</ESN>\r\n"
#define TagTableHead         "<Table>"
#define TagTableEnd          "</Table>"
#define TagTableRegex        "<Table>%d</Table>\r\n"
#define TagPortHead          "<Port>"
#define TagPortEnd           "</Port>"
#define TagPortRegex         "<Port>%d</Port>\r\n"
#define TagProtocolHead      "<Protocol>"
#define TagProtocolEnd       "</Protocol>"
#define TagProtocolRegex     "<Protocol>%d</Protocol>\r\n"
#define TagSoftVsersionRegex "<SoftVersion>%s</SoftVersion>\r\n"
#define TagSoftVsersionHead  "<SoftVersion>"
#define TagSoftVsersionEnd   "</SoftVersion>"


#define TagInfoFilePath      "/mnt/flash/OAM/xml/info.xml"
#define TagTypeHead          "<Type>"
#define TagTypeEnd           "</Type>"
#define TagTypeRegex         "<Type>%d</Type>\r\n"
#define TagYXHead            "<YX>"
#define TagYXEnd             "</YX>"
#define TagYXRegex           "<YX>%04X</YX>\r\n"
#define TagYCHead            "<YC>"
#define TagYCEnd             "</YC>"
#define TagYCRegex           "<YC>%04X</YC>\r\n"
#define TagYKHead            "<YK>"
#define TagYKEnd             "</YK>"
#define TagYKRegex           "<YK>%04X</YK>\r\n"
#define TagSDHead            "<SD>"
#define TagSDEnd             "</SD>"
#define TagSDRegex           "<SD>%04X</SD>\r\n"
#define TagDDHead            "<DD>"
#define TagDDEnd             "</DD>"
#define TagDDRegex           "<DD>%04X</DD>\r\n"

#define PointFilePath        "/mnt/flash/OAM/point/%d.xml"
#define PointAddrHead        "<Addr>"
#define PointAddrEnd         "</Addr>"
#define PointAddrRegex       "<Addr>%05d</Addr>\r\n"
#define PointLenHead         "<Len>"
#define PointLenEnd          "</Len>"
#define PointLenRegex        "<Len>%02d</Len>\r\n"
#define PointTypeHead        "<Type>"
#define PointTypeEnd         "</Type>"
#define PointTypeRegex       "<Type>%d</Type>\r\n"
#define PointDataTypeHead    "<DataType>"
#define PointDataTypeEnd     "</DataType>"
#define PointDataTypeRegex   "<DataType>%d</DataType>\r\n"

#define AlarmFilePath        "/mnt/flash/OAM/xml/alarm.xml"
#define AlarmAddrHead        "<Addr>"
#define AlarmAddrEnd         "</Addr>"
#define AlarmAddrRegex       "<Addr>%04X</Addr>\r\n"
#define AlarmTypeHead        "<Type>"
#define AlarmTypeEnd         "</Type>"
#define AlarmTypeRegex       "<Type>%02d</Type>\r\n"
#define AlarmOffsetHead      "<Offset>"
#define AlarmOffsetEnd       "</Offset>"
#define AlarmOffsetRegex     "<Offset>%02d</Offset>\r\n"


int fTagBasefd;
int fTypePointfd;
int fTagInfofd;

extern void ReadTagBaseFile(void);
extern void ReadTagInfoFile(void);
extern void FreshTagBaseFile(void);
extern void TagBaseFileWrite(void);
extern void TagInfoFileWrite(void);
extern void PointInfoFileWrite(void);
extern void ReadPointTableFile(void);
extern unsigned long GetFileSize(const char *path);
extern void AlarmFileWrite(void);
extern void ReadAlarmFile(void);
extern void ReloadAlarmFile(void);

