#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

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
}

//最多同时识别10根手指
int prevSlotIdArray[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int prevArrayX[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int prevArrayY[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
//记录上次指令的手指数量
int lastTouchNum = 0;

//清楚数组内容（将数组里的所有值重新赋值成-1）,目前是写死数组的长度为10
void clearArray(int array[]) {
    for(int i=0;i<10;i++) {
        array[i] = -1;
    }
}

//输出数组内容
void printArray(char *name,int array[]) {
    printf("%s:",name);
    for(int i=0;i<10;i++) {
        printf("%d,",array[i]);
    }
    printf("\n");
}

//触屏类型：0（按下）  1（抬起）  2（移动）
void bbb(int xPointArray[],int yPointArray[],int touchNum,int type) {
    int currSlotIdArray[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    int currArrayX[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    int currArrayY[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    //按下事件
    if(type == 0) {
        for(int i=0;i<touchNum;i++) {
            prevSlotIdArray[i] = i;
            prevArrayX[i] = xPointArray[i];
            prevArrayY[i] = yPointArray[i];
            currSlotIdArray[i] = i;
            currArrayX[i] = xPointArray[i];
            currArrayY[i] = yPointArray[i];
        }
    }
    //抬起事件
    if(type == 1) {
        for(int i=0;i<10;i++) {
            prevSlotIdArray[i] = -1;
            prevArrayX[i] = -1;
            prevArrayY[i] = -1;
        }
    }
    //移动事件
    if(type == 2) {
        //本次的手指触摸数量<=上次的手指触摸数量
        if(touchNum <= lastTouchNum) {
            //最外层循环是本次的指令坐标
            for(int i=0;i<touchNum;i++) {
                int isFirst = 1;
                int lastDifference = -1;
                int surviveSlotId = -1;
                //这一层的循环是上次的指令坐标
                for(int j=0;j<10;j++) {
                    if(prevSlotIdArray[j] == -1) {
                        continue;
                    }
                    int xDifference = abs(prevArrayX[j] - xPointArray[i]);
                    int yDifference = abs(prevArrayY[j] - yPointArray[i]);
                    int sumDifference = xDifference + yDifference;
                    if(isFirst == 1 || sumDifference <= lastDifference) {
                        lastDifference = sumDifference;
                        surviveSlotId = prevSlotIdArray[j];
                        isFirst = 0;
                    }
                }
                currSlotIdArray[surviveSlotId] = prevSlotIdArray[surviveSlotId];
                currArrayX[surviveSlotId] = xPointArray[i];
                currArrayY[surviveSlotId] = yPointArray[i];
            }
            for(int i=0;i<10;i++) {
                //上次指令有坐标，这次指令没有坐标，那么就是手指离开了
                if(prevSlotIdArray[i] != -1 && currSlotIdArray[i] == -1) {
                    //todo 这里到了具体的逻辑是进行离开了的手指事件触发
                    printf("手指%d,%d离开了",prevArrayX[i],prevArrayY[i]);
                }
            }
            //最后把记录本次的手指指令
            for(int i=0;i<10;i++) {
                prevSlotIdArray[i] = currSlotIdArray[i];
                prevArrayX[i] = currArrayX[i];
                prevArrayY[i] = currArrayY[i];
            }
        }
        //本次的手指触摸数量>上次的手指触摸数量
        if(touchNum > lastTouchNum) {
            int isFirst = 1;
            int lastDifference = -1;
            for(int i=0;i<10;i++) {
               if(prevSlotIdArray[i] == -1) {
                   continue;
               }
               currSlotIdArray[i] = prevSlotIdArray[i];
               for(int j=0;j<touchNum;j++) {
                   int xDifference = abs(prevArrayX[i] - xPointArray[j]);
                   int yDifference = abs(prevArrayY[i] - yPointArray[j]);
                   int sumDifference = xDifference + yDifference;
                   if(isFirst == 1 || sumDifference <= lastDifference) {
                       lastDifference = sumDifference;
                       currArrayX[i] = xPointArray[j];
                       currArrayY[i] = yPointArray[j];
                       isFirst = 0;
                   }
               }
            }
            for(int k=0;k<10;k++) {
                if(currSlotIdArray[k] == -1) {
                    continue;
                }
                int currX = currArrayX[k];
                int currY = currArrayY[k];
                for(int l=0;l<touchNum;l++) {
                    if(xPointArray[l] == currX && yPointArray[l] == currY) {
                        xPointArray[l] = -1;
                        yPointArray[l] = -1;
                        break;
                    }
                }
            }
            printArray("新增加的手指坐标x:",xPointArray);
            printArray("新增加的手指坐标y:",yPointArray);
            for(int m=0;m<touchNum;m++) {
                if(xPointArray[m] == -1 && yPointArray[m] == -1) {
                    continue;
                }
                for(int n=0;n<10;n++) {
                    if(currSlotIdArray[n] == -1) {
                        //todo 这里是新增加手指的事件了
                        currSlotIdArray[n] = n;
                        currArrayX[n] = xPointArray[m];
                        currArrayY[n] = yPointArray[m];
                        break;
                    }
                }
            }
            //最后把记录本次的手指指令
            for(int i=0;i<10;i++) {
                prevSlotIdArray[i] = currSlotIdArray[i];
                prevArrayX[i] = currArrayX[i];
                prevArrayY[i] = currArrayY[i];
            }
        }
    }
    //记录这次的touchNum
    lastTouchNum = touchNum;

    //输出三个数组里的值
    printArray("prevSlotIdArray:",prevSlotIdArray);
    printArray("currArrayX:",currArrayX);
    printArray("currArrayY:",currArrayY);
    printf("\n\n");
}

void testInstruction(){
    int xPointArray[10] = {};
    int yPointArray[10] = {};

    //bbb(100,100,-1,-1,-1,-1);
    clearArray(xPointArray);
    clearArray(yPointArray);
    xPointArray[0] = 100;
    yPointArray[0] = 101;
    bbb(xPointArray,yPointArray,1,0);

    //bbb(101,101,-1,-1,-1,-1);
    clearArray(xPointArray);
    clearArray(yPointArray);
    xPointArray[0] = 101;
    yPointArray[0] = 102;
    bbb(xPointArray,yPointArray,1,2);

    //bbb(102,102,200,200,-1,-1);
    clearArray(xPointArray);
    clearArray(yPointArray);
    xPointArray[0] = 102;
    yPointArray[0] = 103;
    xPointArray[1] = 200;
    yPointArray[1] = 201;
    bbb(xPointArray,yPointArray,2,2);

    //bbb(103,103,201,201,-1,-1);
    clearArray(xPointArray);
    clearArray(yPointArray);
    xPointArray[0] = 103;
    yPointArray[0] = 104;
    xPointArray[1] = 201;
    yPointArray[1] = 202;
    bbb(xPointArray,yPointArray,2,2);

    //bbb(104,104,202,202,300,300);
    clearArray(xPointArray);
    clearArray(yPointArray);
    xPointArray[0] = 104;
    yPointArray[0] = 105;
    xPointArray[1] = 202;
    yPointArray[1] = 203;
    xPointArray[2] = 300;
    yPointArray[2] = 301;
    bbb(xPointArray,yPointArray,3,2);

    //bbb(105,105,203,203,301,301);
    clearArray(xPointArray);
    clearArray(yPointArray);
    xPointArray[0] = 105;
    yPointArray[0] = 106;
    xPointArray[1] = 203;
    yPointArray[1] = 204;
    xPointArray[2] = 301;
    yPointArray[2] = 302;
    bbb(xPointArray,yPointArray,3,2);

    //bbb(105,105,302,302,-1,-1);
    clearArray(xPointArray);
    clearArray(yPointArray);
    xPointArray[0] = 105;
    yPointArray[0] = 106;
    xPointArray[1] = 302;
    yPointArray[1] = 303;
    bbb(xPointArray,yPointArray,2,2);

    //bbb(303,303,-1,-1,-1,-1);
    clearArray(xPointArray);
    clearArray(yPointArray);
    xPointArray[0] = 303;
    yPointArray[0] = 304;
    bbb(xPointArray,yPointArray,1,2);

    //bbb(555,555,303,303,-1,-1);
    clearArray(xPointArray);
    clearArray(yPointArray);
    xPointArray[0] = 555;
    yPointArray[0] = 556;
    xPointArray[1] = 303;
    yPointArray[1] = 304;
    bbb(xPointArray,yPointArray,2,2);

    //bbb(556,556,304,304,888,888);
    clearArray(xPointArray);
    clearArray(yPointArray);
    xPointArray[0] = 556;
    yPointArray[0] = 557;
    xPointArray[1] = 304;
    yPointArray[1] = 305;
    xPointArray[2] = 888;
    yPointArray[2] = 889;
    bbb(xPointArray,yPointArray,3,2);
    
    clearArray(xPointArray);
    clearArray(yPointArray);
    bbb(xPointArray,yPointArray,0,1);
}

int main() {
    printf("sad\n");
    testInstruction();

    /*int test[10] = {};
    printArray("test1",test);
    clearArray(test);
    printArray("test2",test);*/

    /*int a = 11;
    int b = -12;
    printf("%d\n",abs(a));
    printf("%d\n",abs(b));
    printf("%d\n",b);*/

    return 0;
}
