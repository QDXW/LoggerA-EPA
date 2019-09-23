/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : libini.c
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------历史版本信息-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#include <math.h>
#include "libini.h"
#include "../Ethernet/EthernetExtern.h"
#include "../SGCC/protocol.h"

UINT8 gNumberOfLimits=0;

extern UINT16 gThirdPartyPointTableCount;         //number of point table
extern UINT16 gPointTableList[MAX_DEVICE_TYPE];

static void list_append(LIBINI *pini,
        char *key,
        char *value,
        char *section
        )
{
    ini_item *item = (ini_item *)malloc(sizeof(ini_item));

    item->section = section;
    item->key = key;
    item->value = value;
    item->next = NULL;
    if (!pini->tailer) {
        pini->header = pini->tailer = item;
    } else {
        pini->tailer->next = item;
        pini->tailer = item;
    }
    pini->length += 1;

    return;
}

static ini_item *list_search(LIBINI *pini,const char *key)
{
    ini_item *p = pini->header;
    char temp[VCHR_LEN];
    char *k = temp, *sect=temp;
    ini_item *_ret = NULL;

    if (!p) return NULL;
    strcpy(temp,key);

    /*int i;
    printf("[list_search]temp=");
    for(i=0;i<30;i++)
        printf("%c",temp[i]);
    printf("\r\n");*/

    for (; *k && '.'!=*k; k++);

    /*printf("[list_search]*k=");
    //for(i=0;i<30;i++)
        printf("%c",*k);
    printf("\r\n");

    printf("[list_search]k=");
    //for(i=0;i<30;i++)
        printf("%d",k);
    printf("\r\n");

    printf("[list_search]sect=");
    //for(i=0;i<30;i++)
        printf("%d",sect);
    printf("\r\n");*/

    if ('.'==*k && k==sect)
        sect = ++k;
    else if ('.' == *k)
        *k++ = 0;
    else
        k = sect;

    /*printf("[list_search]*k=");
    //for(i=0;i<30;i++)
        printf("%c",*k);
    printf("\r\n");

    printf("[list_search]*sect=");
    //for(i=0;i<30;i++)
        printf("%c",*sect);
    printf("\r\n");*/

    if (k!=sect && (unsigned int)(k-sect)==strlen(key)) {
    //printf("if!!!\r\n");
        while (p) {
            if (0 != strcmp(p->section,sect)) {
                p = p->next;
                continue;
            }
            if (!p->next)
                return p;
            if (0 != strcmp(p->next->section,sect))
                return p;
            p = p->next;
        }
        return _ret;
    }

    //printf("No if!!!\r\n");
    while (p) {
        if (0 != strcmp(p->key,k)) {
            p = p->next;
            continue;
        }
        if (k!=sect && 0!=strcmp(p->section,sect)) {
            p = p->next;
            continue;
        }
        if (k==sect && 0!=strcmp(p->section,
                pini->header->section)) {
            p = p->next;
            continue;
        }
        _ret = p;
        break;
    }

    return _ret;
}

LIBINI *ini_open(const char *fname)
{
    LIBINI *pini=NULL;
    FILE *fp = fopen(fname,"r");
    char buffer[LINE_LEN];
    char *p;
    char *s;
    char sect[SECT_LEN] = DEF_SECT_ROOT;
    char *section;
    char *key;
    char *value;

    if (!fp) return NULL;
    pini = (LIBINI *)malloc(sizeof(LIBINI));
    pini->fname = (char *)malloc(strlen(fname)+1);

    strcpy(pini->fname,fname);
    pini->length = 0;
    pini->header = pini->tailer = NULL;

    while (!feof(fp)) {

        if (!fgets(buffer,LINE_LEN,fp))
            break;

        for (p=buffer; ' '==*p||'\t'==*p; p++);
        if ('#'==*p || '\n'==*p || '\r'==*p) {
            continue;
        } else if ('[' == *p) {
            for (p++; ' '==*p||'\t'==*p; p++);
            for (s=p; ' '!=*p&&'\t'!=*p&&']'!=*p; p++);
            *p = 0;

            strcpy(sect,s);
        } else {

            for (; ' '==*p||'\t'==*p; p++);

            for (s=p; ' '!=*p&&'\t'!=*p&&'='!=*p&&':'!=*p; p++);
            *p = 0;

            key = (char *)malloc(strlen(s)+1);
            strcpy(key,s);

            for (p++; ' '==*p||'\t'==*p||'='==*p||':'==*p; p++);
            //printf("while 9\n");
            for (s=p; ' '!=*p&&'\t'!=*p&&'\n'!=*p&&'\r'!=*p && (p-s)<LINE_LEN; p++);
            *p = 0;
            //printf("while 10  %d\n",p-s);

            //if((p-s)>100)
            //{
                //printf("while 10=  %s\n",s);
            //}

            value = (char *)malloc(strlen(s)+1);
            strcpy(value,s);

            section = (char *)malloc(strlen(sect)+1);
            strcpy(section,sect);


            list_append(pini,key,value,section);

        }
    }
    fclose(fp);

    return pini;
}

LIBINI *ini_create(const char *fname)
{
    FILE *fp = fopen(fname,"w+");
    LIBINI *pini;

    if (!fp) return NULL;

    pini = (LIBINI *)malloc(sizeof(LIBINI));
    pini->fname = (char *)malloc(strlen(fname));
    strcpy(pini->fname,fname);
    pini->length = 0;
    pini->header = pini->tailer = NULL;

    fclose(fp);

    return pini;
}

int ini_save(LIBINI *pini)
{
    FILE *fp;
    ini_item *p;
    char *sect = NULL;
    char buffer[LINE_LEN];

    p = pini->header;
    if (!p) return 0;
    fp = fopen(pini->fname,"w+");
    if (!fp) return 0;

    while (p) {
        if (!sect || 0!=
        strcmp(p->section,sect)) {
            sect = p->section;
            buffer[0] = '[';
            strcpy(buffer+1,sect);
            strcat(buffer,"]\n");
            fputs(buffer,fp);
        }
        strcpy(buffer,p->key);
        strcat(buffer,"\t= ");
        strcat(buffer,p->value);
        strcat(buffer,"\n");
        fputs(buffer,fp);

        p = p->next;
    }

    fclose(fp);

    return 1;
}

void ini_close(LIBINI *pini)
{
    ini_item *p = pini->header;
    ini_item *temp;

    if (!p) return;
    while (p) {
        free(p->key);
        free(p->value);
        free(p->section);
        temp = p;
        p = p->next;
        free(temp);
    }

    free(pini->fname);
    free(pini);

    return;
}

char *ini_get(LIBINI *pini,const char *key,char *value)
{
    ini_item *item;

    item = list_search(pini,key);
    *value = 0;
    if (item) strcpy(value,item->value);
    /*int i;
    printf("[ini_get]value=");
    for(i=0;i<10;i++)
        printf("%c ",value[i]);
    printf("\r\n");*/
    return value;
}

int ini_get_int(LIBINI *pini,const char *key)
{
    char value[VINT_LEN];
    return atoi(ini_get(pini,key,value));
}

int ini_set(LIBINI *pini,const char *key,const char *value)
{
    ini_item *item;
    char *temp;

    item = list_search(pini,key);
    if (!item) return 0;
    temp = item->value;
    item->value = (char *)malloc(strlen(value)+1);
    strcpy(item->value,value);
    free(temp);

    return 1;
}

int ini_set_int(LIBINI *pini,const char *key,int value)
{
    char buffer[VINT_LEN];
    sprintf(buffer,"%d",value);
    return ini_set(pini,key,buffer);
}

int ini_append(LIBINI *pini,const char *key,const char *value)
{
    ini_item *item, *sear, *temp;
    char buffer[LINE_LEN];
    char *k, *v, *s, *p;

    strcpy(buffer,key);
    if (NULL == (p=strchr(buffer,'.'))) {
        if (pini->header)
            strcpy(buffer,pini->header->section);
        else
            strcpy(buffer,DEF_SECT_ROOT);
        strcat(buffer,".");
        strcat(buffer,key);
    } else if (p == buffer) {
        strcpy(buffer,pini->header->section);
        strcat(buffer,key);
    } else if ((unsigned int)(p+1-buffer) == strlen(buffer))
        return 0;
    p = strchr(buffer,'.');
    sear = list_search(pini,buffer);
    if (sear && 0==strcmp(sear->key,p+1))
        return 0;

    item = (ini_item *)malloc(sizeof(ini_item));
    k = (char *)malloc(strlen(key)+1);
    strcpy(k,p+1);
    item->key = k;
    v = (char *)malloc(strlen(value)+1);
    strcpy(v,value);
    item->value = v;

    *(p+1) = 0;
    sear = list_search(pini,buffer);
    if (sear) {
        s = (char *)malloc(strlen(sear->section)+1);
        strcpy(s,sear->section);
        temp = sear->next;
        sear->next = item;
        item->next = temp;
    } else {
        *p = 0;
        s = (char *)malloc(strlen(buffer)+1);
        strcpy(s,buffer);
        item->next = NULL;
        if (pini->header) {
            pini->tailer->next = item;
            pini->tailer = item;
        } else
            pini->header = pini->tailer = item;
    }
    item->section = s;
    pini->length += 1;

    return 1;
}

int ini_append_int(LIBINI *pini,const char *key,int value)
{
    char buffer[VINT_LEN];
    sprintf(buffer,"%d",value);
    return ini_append(pini,key,buffer);
}

int ini_remove(LIBINI *pini,const char *key)
{
    ini_item *item, *temp, *p;

    item = list_search(pini,key);
    if (!item) return 0;
    p = pini->header;
    if (p && !p->next) {
        if (p != item) return 0;
        free(p->section);
        free(p->key);
        free(p->value);
        free(item);
        pini->header = pini->tailer = NULL;
        pini->length -= 1;
        return 1;
    }
    while (p && p->next) {
        if (p->next == item)
            break;
        p = p->next;
    }
    if (!p || !p->next) return 0;
    temp = p->next;
    p->next = temp->next;
    free(temp->section);
    free(temp->key);
    free(temp->value);
    free(temp);
    pini->length -= 1;

    return 1;
}

void ReadConfigFile()
{
    static LIBINI *config_filefd;
    UINT8 tag_temp[50],name[50],path[50];
    UINT8 *ptype=(void *)"File%d.type",*pname=(void *)"File%d.name",*ppath=(void *)"File%d.path";
    UINT8 number_of_file=0,i,type=0;

    config_filefd=ini_open((void *)CONFIG_FILE_PATH);
    DbgPrintf("[Ini]Read config file config.ini ");
    if (!config_filefd)
    {
        DbgPrintf("fail!!!\r\n");
        return;
    }
    DbgPrintf("success!!!\r\n");

    DbgPrintf("length:%d\r\n",config_filefd->length);

    number_of_file=ini_get_int(config_filefd,"FileInfo.NumberOfFile");
    DbgPrintf("[Ini]Number of file is %d\r\n",number_of_file);

    for(i=0;i<number_of_file;i++)
    {
        //get file type
        memset(tag_temp,0,sizeof(tag_temp));
        sprintf((void *)tag_temp,(void *)ptype,i+1);
        //printf("%s\r\n",tag_temp);
        type=ini_get_int(config_filefd,(void *)tag_temp);
        DbgPrintf("[Ini]Type of file%d is %d\r\n",i+1,type);

        //get file name
        memset(tag_temp,0,sizeof(tag_temp));
        sprintf((void *)tag_temp,(void *)pname,i+1);
        //printf("%s\r\n",tag_temp);
        ini_get(config_filefd,(void *)tag_temp,(void *)name);
        DbgPrintf("[Ini]Name of file%d is %s\r\n",i+1,name);

        //get file path
        memset(tag_temp,0,sizeof(tag_temp));
        sprintf((void *)tag_temp,(void *)ppath,i+1);
        ini_get(config_filefd,(void *)tag_temp,(void *)path);
        DbgPrintf("[Ini]Path of file%d is %s\r\n",i+1,path);

        MoveFile(type,(void *)name,(void *)path);
    }
    //printf("[Ini]before ini_close\r\n");
    ini_close(config_filefd);
    //printf("[Ini]after ini_close\r\n");
    return;
}

UINT8 ReadThirdPartyFile()
{
    static LIBINI *third_partyfd;
    UINT8  i,j,sum=0,device_addr=0;
    UINT16 port=0,YC_point_num=0,YX_point_num=0,station_id=0;
    UINT32 modbus_addr=0,_104_addr=0;
    UINT8  tag_temp[50]={0},ip[20]={0};
    UINT8  *YC_addr=(void *)"YC%d.Addr%d",*YC_modbus=(void *)"YC%d.Modbus%d",*YX_addr=(void *)"YX%d.Addr",*YX_modbus=(void *)"YX%d.Modbus";

    memset(tag_temp,0,sizeof(tag_temp));
    sprintf((void *)tag_temp,THIRD_PARTY_FILE_PATH);
    third_partyfd=NULL;
    third_partyfd=ini_open((void *)tag_temp);
    DbgPrintf("[Ini]Read third party file config.ini ");
    if (!third_partyfd)
    {
        DbgPrintf("fail!!!\r\n");
        return 0;
    }
    DbgPrintf("success!!!\r\n");

    DbgPrintf("length:%d\r\n",third_partyfd->length);

    ini_get(third_partyfd,(void *)"Connection.IP",(void *)ip);
    DbgPrintf("[Ini]IP is %s\r\n",ip);
    memcpy((void *)gThirdPartyStationInfo.nIP,(void *)ip,20);

    port=ini_get_int(third_partyfd,(void *)"Connection.Port");
    DbgPrintf("[Ini]Port is %d\r\n",port);
    gThirdPartyStationInfo.nPort=port;

    station_id=ini_get_int(third_partyfd,(void *)"Connection.StationID");
    DbgPrintf("[Ini]Station ID is %d\r\n",station_id);
    gThirdPartyStationInfo.nStationID=station_id;

    YC_point_num=ini_get_int(third_partyfd,(void *)"PointTable.YCPointNumber");
    DbgPrintf("[Ini]YCPointNumber is %d\r\n",YC_point_num);
    gYCPointSumTemp=YC_point_num;

    YX_point_num=ini_get_int(third_partyfd,(void *)"PointTable.YXPointNumber");
    DbgPrintf("[Ini]YXPointNumber is %d\r\n",YX_point_num);
    gYXPointSumTemp=YX_point_num;

    for(i=0;i<YC_point_num;i++)
    {
        //get 104 address
        memset(tag_temp,0,sizeof(tag_temp));
        sprintf((void *)tag_temp,(void *)"YC%d.104Addr",i+1);
        //printf("%s\r\n",tag_temp);
        _104_addr=ini_get_int(third_partyfd,(void *)tag_temp);
        //from decimal to hex
        /*_104_addr=_104_addr_temp % 10;
        for(j=0;j<4;j++)
        {
            _104_addr+=_104_addr_temp % (int)(pow(10,j+2)) / (int)(pow(10,j+1)) * (int)(pow(16,j+1));
        }*/
        DbgPrintf("[Ini]104 address of YC%d is %d\r\n",i+1,_104_addr);

        //get sum
        memset(tag_temp,0,sizeof(tag_temp));
        sprintf((void *)tag_temp,(void *)"YC%d.Sum",i+1);
        //printf("%s\r\n",tag_temp);
        sum=ini_get_int(third_partyfd,(void *)tag_temp);
        DbgPrintf("[Ini]Sum of YC%d is %d\r\n",i+1,sum);

        SinglePointAdd(Type_104_YC,_104_addr,sum);

        for(j=0;j<sum;j++)
        {
            //get device address
            memset(tag_temp,0,sizeof(tag_temp));
            sprintf((void *)tag_temp,(void *)YC_addr,i+1,j+1);
            device_addr=ini_get_int(third_partyfd,(void *)tag_temp);
            DbgPrintf("[Ini]Device address%d of YC%d is %d\r\n",j+1,i+1,device_addr);

            //get modbus address
            memset(tag_temp,0,sizeof(tag_temp));
            sprintf((void *)tag_temp,(void *)YC_modbus,i+1,j+1);
            modbus_addr=ini_get_int(third_partyfd,(void *)tag_temp);
            DbgPrintf("[Ini]Modbus address%d of YC%d is %d\r\n",j+1,i+1,modbus_addr);

            SubPointAdd(device_addr,modbus_addr,j);
        }
    }

    for(i=0;i<YX_point_num;i++)
    {
        //get 104 address
        memset(tag_temp,0,sizeof(tag_temp));
        sprintf((void *)tag_temp,(void *)"YX%d.104Addr",i+1);
        //printf("%s\r\n",tag_temp);
        _104_addr=ini_get_int(third_partyfd,(void *)tag_temp);
        //from decimal to hex
        /*_104_addr=_104_addr_temp % 10;
        for(j=0;j<4;j++)
        {
            _104_addr+=_104_addr_temp % (int)(pow(10,j+2)) / (int)(pow(10,j+1)) * (int)(pow(16,j+1));
        }*/
        DbgPrintf("[Ini]104 address of YX%d is %d\r\n",i+1,_104_addr);

        //default sum for YX point is 1
        sum=1;
        SinglePointAdd(Type_104_YX,_104_addr,sum);

        //get device address
        memset(tag_temp,0,sizeof(tag_temp));
        sprintf((void *)tag_temp,(void *)YX_addr,i+1);
        device_addr=ini_get_int(third_partyfd,(void *)tag_temp);
        DbgPrintf("[Ini]Device address of YX%d is %d\r\n",i+1,device_addr);

        //get modbus address
        memset(tag_temp,0,sizeof(tag_temp));
        sprintf((void *)tag_temp,(void *)YX_modbus,i+1);
        modbus_addr=ini_get_int(third_partyfd,(void *)tag_temp);
        DbgPrintf("[Ini]Modbus address of YX%d is %d\r\n",i+1,modbus_addr);

        SubPointAdd(device_addr,modbus_addr,0);
    }
    ini_close(third_partyfd);
    return 1;
}

void PrintThirdPartyList(void)
{
    struct sThirdPartySinglePoint *pSinglePointPoint;
    int i=1,j;

    i=1;
    pSinglePointPoint=gThirdPartySinglePointHead;
    while(pSinglePointPoint!=NULL)
    {
        DbgPrintf("[Third Party List]Type of Single Point%d is %d\r\n",i,pSinglePointPoint->nType);
        DbgPrintf("[Third Party List]104 Addr of Single Point%d is %d\r\n",i,pSinglePointPoint->n104Addr);
        DbgPrintf("[Third Party List]Number Of SubPoint of Single Point%d is %d\r\n",i,pSinglePointPoint->nNumberOfSubPoint);

        for(j=0;j<pSinglePointPoint->nNumberOfSubPoint;j++)
        {
            DbgPrintf("[Third Party List]addr of SubPoint%d is %d\r\n",j,pSinglePointPoint->pSubPoint[j].nAddr);
            DbgPrintf("[Third Party List]modbus of SubPoint%d is %d\r\n",j,pSinglePointPoint->pSubPoint[j].nModbus);
        }

        pSinglePointPoint=pSinglePointPoint->pNext;
        i++;
        DbgPrintf("\r\n");
    }
}

UINT8 ReadSouthernAlarmFile()
{
    static LIBINI *southern_alarmfd;
    UINT8 tag_temp[50];
    UINT8 *ptype=(void *)"Limits.%d";
    UINT8 i,index=0;
    UINT16 limit=0;

    southern_alarmfd=ini_open((void *)SOUTH_ALARM_PATH);
    DbgPrintf("[Ini]Read config file alarm.ini ");
    if (!southern_alarmfd)
    {
        DbgPrintf("fail!!!\r\n");
        return 0;
    }
    DbgPrintf("success!!!\r\n");

    DbgPrintf("length:%d\r\n",southern_alarmfd->length);

    gNumberOfLimits=ini_get_int(southern_alarmfd,"Alarm.NumberOfLimits");
    DbgPrintf("[Ini]Number of limits is %d\r\n",gNumberOfLimits);
    gSouthernAlarm=(struct sSouthernAlarm *)malloc(gNumberOfLimits * sizeof(struct sSouthernAlarm));

    for(i=0;i<gNumberOfLimits*2;i++)
    {
        //get limit
        memset(tag_temp,0,sizeof(tag_temp));
        sprintf((void *)tag_temp,(void *)ptype,i+1);
        //printf("%s\r\n",tag_temp);
        limit=ini_get_int(southern_alarmfd,(void *)tag_temp);

        index=i/2;
        if(i%2==0)
        {
            gSouthernAlarm[index].upper_limit=limit;
            DbgPrintf("[Ini]upper limit of data%d is %d\r\n",index+1,limit);
        }
        else
        {
            gSouthernAlarm[index].lower_limit=limit;
            DbgPrintf("[Ini]lower limit of data%d is %d\r\n",index+1,limit);
        }        
    }
    //printf("[Ini]before ini_close\r\n");
    ini_close(southern_alarmfd);
    //printf("[Ini]after ini_close\r\n");
    return 1;
}

void PrintfLimits()
{
    UINT8 i;
    
    for(i=0;i<gNumberOfLimits;i++)
    {
        DbgPrintf("[PrintfLimits]gSouthernAlarm[%d].upper_limit = %d\r\n",i,gSouthernAlarm[i].upper_limit);
        DbgPrintf("[PrintfLimits]gSouthernAlarm[%d].lower_limit = %d\r\n",i,gSouthernAlarm[i].lower_limit);
    }
}
