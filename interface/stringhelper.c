/**
* TA: a simple ascii string to integer convert function (only support decimal string)
* @str ascii number string
*
* return: integer number
*/
int my_atoi(char *str)
{
    char    *p = str;
    int  sign = 1;
    int  ret  = 0;

    switch (*p)
    {
        case '+':{
            p++;
        }break;
        case '-':{
            p++;
            sign = -1;
        }break;
        default:break;
    }

    for (; *p != '\0'; p++)
    {
        if (*p >= '0' && *p <= '9')
        {
            ret = ret * 10 + (*p - '0');
        }
    }

    return sign > 0 ? ret : -ret;
}

void my_itoa(int num, char *result)
{
    char tmp[100] = {0x00};
    char *p = result;
    int   sign = 1, index = 0;

    if (num == 0){
        tmp[0] = '0';
		index++;
    }
    else if (num < 0){
        sign = -1;
        num = -num;
    }

    for (; num != 0; index++){
        tmp[index] = num % 10 + '0';
        num /= 10;
    }

    index--;

    if (sign == -1){
        *p++ = '-';
    }

    while (index >= 0){
        *p++ = tmp[index--];
    }

    *p = '\0';
}
