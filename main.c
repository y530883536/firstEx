#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

void aaa(char *originInst) {
//void aaa(char originInst[]){
    //这个字符串数组用来保存原始命令分割之后的字符串

    //可以用originInstArray[i] != NULL来把后面没值的全部过滤掉
    /*for(int i=0;i<10;i++) {
        if(originInstArray[i] != NULL){
            printf("%s",originInstArray[i]);
            printf("%s\n","123");
        }
    }*/

    //char originInst[] = "MULTI:1:0:314:61011";

    /*char temp[100];
    for(int i=0;i<100;i++) {
        temp[i] = *(originInst + i);
        if(temp[i] == '\0') {
            break;
        }
    }

    char *originInstArray[100];
    char *buf;
    buf = temp;
    char *token;
    int index = 0;
    //strsep
    while((token = strtok(&buf,":")) != NULL) {
        originInstArray[index] = token;
        index ++;
    }
    for(int i=0;i<10;i++) {
        printf("%s\n",originInstArray[i]);
    }*/

    /*bool a = false;
    if(a) {
        printf("fuck");
    } else {
        printf("you");
    }*/

    /*bool a[] = {true,false,false,true};
    for(int i=0;i<10;i++) {
        if(a[i]) {
            printf("true");
        } else {
            printf("");
        }
    }*/

    /*int a[20] = {};
    for(int i=0;i<20;i++) {
        a[i] = 0;
    }
    for(int i=0;i<20;i++) {
        if(i%3 == 0) {
            a[i] = i;
        }
    }
    for(int i=0;i<20;i++) {
        printf("%d\n",a[i]);
    }*/

    /*int i = -7;
    printf("%d",i);*/

    /*int a[3] = {};
    for(int i=0;i<3;i++) {
        printf("%d\n",a[i]);
    }*/

    /*int i = 0x11;
    int j = (int)(i * 1.5);
    printf("%d\n",j);*/

    /*char str[] = "f";
    int i = str[0] == 'f';
    int j = str[0] == 'u';
    printf("%d\n",i);
    printf("%d\n",j);*/

    /*char msg[100] = "asdf";
    strcpy(msg,"helloWorld111");
    printf("%d",strlen(msg));
    printf("%s\n",msg);
    strcpy(msg,"fuckYou");
    printf("%d",strlen(msg));
    printf("%s\n",msg);*/

    /*char *buf = "MULTI:2:2:1114:291:203:669MULTI:2:2:1113:292:203:669MULTI:2:2:1112:293:203:669";
    //字符串的初始位置
    char *start = strstr(buf,"MULTI");
    //字符串的结束位置
    char *end = start + strlen(buf);
    while(1) {
        char msg[30] = {0};
        char *next = strstr(start+1,"MULTI");
        //如果大于0说明还有下一个指令
        if(next > 0) {
            memcpy(msg,start, next-start);
            start = next;
            printf("%s\n",msg);
        }
        //没有的话说明后面都没有指令了，当前是最后一条指令
        else{
            memcpy(msg,start,end-start);
            printf("%s\n",msg);
            break;
        }
    }*/
    /*char buf[MAX_LINE];  *//*缓冲区*//*
    FILE *fp;            *//*文件指针*//*
    int len;             *//*行字符个数*//*
    if((fp = fopen("D:/multi.log","r")) == NULL)
    {
        perror("fail to read");
        exit (1) ;
    }
    while(1) {
        if(fgets(buf,MAX_LINE,fp) == NULL){
            usleep(10000);
            continue;
        }
        if(buf[0] == 'e') {
            break;
        }
        len = strlen(buf);
        buf[len-1] = '\0';  *//*去掉换行符*//*
        printf("%s\n",buf);
    }*/


    /*int prevX[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    int prevY[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    int currX[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    int currY[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

    prevX[0] = 100;
    prevX[1] = 200;
    prevX[2] = 300;
    prevX[3] = 400;

    prevY[0] = 100;
    prevY[1] = 200;
    prevY[2] = 300;
    prevY[3] = 400;

    currX[0] = 105;
    currX[1] = 305;

    currY[0] = 105;
    currY[1] = 305;

    //输出结果是40
    //printf("%d\n", sizeof(prevX));
    //printf("%d\n", sizeof(currX));
    for(int i=0;i<10;i++) {
        //如果prevX和prevY都是-1说明这根手指没用到
        if(prevX[i] == -1 && prevY[i] == -1) {
            continue;
        }
        for(int j=0;j<10;j++) {

        }
    }*/
    printf("aassddrr");
}



int main() {
    aaa("MULTI:1:0:314:61011");
    return 0;
}
