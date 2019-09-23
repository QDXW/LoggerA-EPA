/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : libini.h
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------历史版本信息-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#ifndef ALAN_LIBINI_H
#define ALAN_LIBINI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../interface/interfaceExtern.h"

#define LINE_LEN 128    // INI文件行长度
#define SECT_LEN 64 // 节点名称长度
#define VINT_LEN 64 // 整型数值长度
#define VCHR_LEN 64 // 字符型值长度

#define DEF_SECT_ROOT "General" //默认根节点名称

//Third Party Device Info
#define THIRD_PARTY_FILE_PATH "/mnt/flash/OAM/thirdparty/config.ini"
#define CONFIG_FILE_PATH      "/mnt/flash/OAM/filetemp/config.ini"
#define SOUTH_ALARM_PATH      "/mnt/flash/OAM/southalarm/alarm.ini"

typedef struct ini_item_ {
    char            *key;
    char            *value;
    char            *section;
    struct ini_item_    *next;

} ini_item;

typedef struct {
    char            *fname;
    struct ini_item_    *header;
    struct ini_item_    *tailer;
    int          length;
} LIBINI;

/********************************************************
 * ini_open : 解析INI文件到缓冲区
 *        返回值为INI文件操作指针
 * @fname   : 要操作的INI文件名
*/
LIBINI *ini_open(const char *fname);

/********************************************************
 * ini_create   : 创建一个新的INI文件
 *        返回值为INI文件操作指针
 * @fname   : 要创建的INI文件名
*/
LIBINI *ini_create(const char *fname);

/********************************************************
 * ini_save : 将缓冲区中的INI文件内容保存到文件
 *        返回值为0保存成功，否则为保存失败
 * @pini    : 要保存的INI文件指针
*/
int ini_save(LIBINI *pini);

/********************************************************
 * ini_close    : 释放INI文件占用的内存资源
 *        无返回值
 * @pini    : 要释放的INI文件指针
*/
void ini_close(LIBINI *pini);

/********************************************************
 * ini_get  : 取得指定键的键值
 *        返回值为保存键值的字符串指针
 * @pini    : INI文件指针
 * @key     : 键名字符串指针
 * @value   : 用于保存返回值的字符串指针
*/
char *ini_get(LIBINI *pini,const char *key,char *value);

/********************************************************
 * ini_get_int  : 取得指定键的整型键值
 *        返回值整型键值
 * @pini    : INI文件指针
 * @key     : 键名字符串指针
*/
int ini_get_int(LIBINI *pini,const char *key);

/********************************************************
 * ini_set  : 设置指定键的键值
 *        返回值为1设置成功，0设置失败
 * @pini    : INI文件指针
 * @key     : 键名字符串指针
 * @value   : 用于保存要设置的值的字符串指针
*/
int ini_set(LIBINI *pini,const char *key,const char *value);

/********************************************************
 * ini_set_int  : 用整型值设置指定键的键值
 *        返回值为1设置成功，0设置失败
 * @pini    : INI文件指针
 * @key     : 键名字符串指针
 * @value   : 用于保存要设置的值的字符串指针
*/
int ini_set_int(LIBINI *pini,const char *key,int value);

/********************************************************
 * ini_append   : 添加新键
 *        返回值为1设置成功，0设置失败
 * @pini    : INI文件指针
 * @key     : 键名字符串指针
 * @value   : 键值字符串指针
*/
int ini_append(LIBINI *pini,const char *key,const char *value);

/********************************************************
 * ini_append_int:用整型数值来添加新键
 *        返回值为1设置成功，0设置失败
 * @pini    : INI文件指针
 * @key     : 键名字符串指针
 * @value   : 整型键值
*/
int ini_append_int(LIBINI *pini,const char *key,int value);

/********************************************************
 * ini_remove   : 移除指定键
 *        返回值为1设置成功，0设置失败
 * @pini    : INI文件指针
 * @key     : 键名字符串指针
*/
int ini_remove(LIBINI *pini,const char *key);

/********************************************************
 * ReadConfigFile   : 读取配置文件
*/
void ReadConfigFile();

/********************************************************
 * ReadThirdPartyInfo   : 读取三方平台信息
 *        返回值为1是成功，0是失败
*/
UINT8 ReadThirdPartyFile();

/********************************************************
 * ReadThirdPartyInfo   : 打印三方点表
*/
void PrintThirdPartyList(void);

/********************************************************
 * ReadThirdPartyInfo   : 读取南向告警信息
 *        返回值为1是成功，0是失败
*/
UINT8 ReadSouthernAlarmFile();

/********************************************************
 * ReadThirdPartyInfo   : 打印告警阈值数组
*/
void PrintfLimits();

#endif//ALAN_LIBINI_H
