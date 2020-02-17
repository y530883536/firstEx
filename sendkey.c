#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEV_INPUT_event0 "/dev/input/event0"

//type
#define EV_ABS 0x03
#define EV_KEY 0x01
#define EV_SYN 0x00

//code
#define ABS_MT_TRACKING_ID 0x39
#define ABS_MT_POSITION_X 0x35
#define ABS_MT_POSITION_Y 0x36
#define ABS_MT_SLOT 0x2f
#define BTN_TOUCH 0x14a
#define SYN_REPORT 0

//value
#define BTN_KEY_DOWN 1
#define BTN_KEY_UP 0
//BTN_TOUCH的UP事件的ABS_MT_TRACKING_ID总是ffffffff
#define BTN_UP_TRACKING_ID 0xFFFFFFFF
//SYN_REPORT事件的值总是00000000
#define SYN_REPORT_VALUE 0

int PRESS_DOWN = 1;
int PRESS_UP = 0;
int TRACKING_END = 0xFFFFFFFF;

int g_fd = 0;

void changeToAndroidInstruct(char *originInst);

int connect_dev()
{
    int version=0;
    int g_fd = open(DEV_INPUT_event0, O_RDWR);
    if(g_fd < 0) {
        printf( "could not open %s\n", DEV_INPUT_event0);
        return 0;
    }
    if (ioctl(g_fd, EVIOCGVERSION, &version)) {
        printf( "could not get driver version \n");
        return 0;
    }
    printf("get dev!\n");
    return 1;
}

void sendEvent(int type, int code , int value)
{
    struct input_event event;
    ssize_t ret;

    memset(&event, 0, sizeof(event));
    event.type = type;
    event.code = code;
    event.value = value;
    ret = write(g_fd, &event, sizeof(event));
    if(ret < (ssize_t) sizeof(event)) {
        printf( "write event failed\n");
        return ;
    }
    return;

}

//int trackingId = 0;
/*int parseTrackingId() {
	return ++trackingId;
}*/


void swipeStart() {
    printf("start\n");
    //sendEvent(EV_ABS, ABS_MT_TRACKING_ID, parseTrackingId());
    sendEvent(EV_KEY, BTN_TOUCH, PRESS_DOWN);
    sendEvent(EV_KEY, BTN_TOOL_FINGER, PRESS_DOWN);
}

void swipeEnd() {
    printf("end\n");
    sendEvent(EV_ABS, ABS_MT_TRACKING_ID, TRACKING_END);
    sendEvent(EV_KEY, BTN_TOUCH, PRESS_UP);
    sendEvent(EV_KEY, BTN_TOOL_FINGER, PRESS_UP);
    sendEvent(EV_SYN, SYN_REPORT, 0);
}


int SWIPE_RUN_INTERVAL = 50;
void swipeRun(int x1, int y1, int x2, int y2)
{
    int xStep = (x2 - x1) / SWIPE_RUN_INTERVAL;
    int yStep = (y2 - y1) / SWIPE_RUN_INTERVAL;

    int x = x1, y = y1;
    for (int step = 0; step <= SWIPE_RUN_INTERVAL; ++step) {
        sendEvent(EV_ABS, ABS_MT_POSITION_X, x);
        sendEvent(EV_ABS, ABS_MT_POSITION_Y, y);
        sendEvent(EV_SYN, SYN_REPORT, 0);
        x += xStep;
        y += yStep;
    }
}

;
__u16 code;
__s32 value;

//触摸屏的event文件
int fd = -999;
void fuckSend( __u16 type, __u16 code , __s32 value)
{
    //int fd;
    ssize_t ret;
    int version;
    struct input_event event;

    //如果是第一次就要打开文件
    if(fd == -999) {
        fd = open("/dev/input/event0", O_RDWR);
    }
    if(fd < 0 && fd != -999) {
        printf("errno=%d\n",errno);
        char * mesg = strerror(errno);
        printf("Mesg:%s\n",mesg);
        printf("open fail\n");
        return;
    }
    if (ioctl(fd, EVIOCGVERSION, &version)) {
        printf("ioctl fail!\n");
        return;
    }
    memset(&event, 0, sizeof(event));
    event.type = type;
    event.code = code;
    event.value = value;
    ret = write(fd, &event, sizeof(event));
    if(ret < (ssize_t) sizeof(event)) {
        printf("write event failed\n");
        //close(fd);
        return ;
    }
    //close(fd);
    return ;
}

int main222(int argc, char *argv[])
{
    //三个参数的顺序分别是type,code,value
    //每个不同类型的event有自己的code代码，synEvent用到的code目前只有0（SYN_REPORT）
    printf("hello fuck222\n");
    fuckSend(EV_ABS,ABS_MT_TRACKING_ID,0x200);
    fuckSend(EV_ABS,ABS_MT_POSITION_X,0x3e5);
    fuckSend(EV_ABS,ABS_MT_POSITION_Y,0x964);
    fuckSend(EV_KEY,BTN_TOUCH,BTN_KEY_DOWN);
    //EV_ABS    ABS_X   0x3e5       这句很奇怪，事实上android没有执行这条命令，难道是识别不出来？
    //fuckSend(3,0x0,0x3e5);
    //EV_ABS    ABS_Y   0x00000964  同上，感觉这两句是标记了X和Y的坐标，但是有什么用？又不需要记录你点击的位置
    //fuckSend(3,0x1,0x00000964);
    fuckSend(EV_SYN,SYN_REPORT,SYN_REPORT_VALUE);
    fuckSend(EV_ABS,ABS_MT_TRACKING_ID,BTN_UP_TRACKING_ID);
    fuckSend(EV_KEY,BTN_TOUCH,BTN_KEY_UP);
    fuckSend(EV_SYN,SYN_REPORT,SYN_REPORT_VALUE);
}

void moveLeft(int x,int y,int trackingId) {
    printf("moveLeft\n");
    fuckSend(EV_ABS,ABS_MT_TRACKING_ID,trackingId);
    fuckSend(EV_ABS,ABS_MT_POSITION_X,x);
    fuckSend(EV_ABS,ABS_MT_POSITION_Y,y);
    fuckSend(EV_KEY,BTN_TOUCH,BTN_KEY_DOWN);
    fuckSend(EV_SYN,SYN_REPORT,SYN_REPORT_VALUE);

    for (int i = 0; i < 10; i++) {
        usleep(20000);
        x = x - 60;
        fuckSend(EV_ABS,ABS_MT_POSITION_X,x);
        fuckSend(EV_SYN,SYN_REPORT,SYN_REPORT_VALUE);
    }

    fuckSend(EV_ABS,ABS_MT_TRACKING_ID,BTN_UP_TRACKING_ID);
    fuckSend(EV_KEY,BTN_TOUCH,BTN_KEY_UP);
    fuckSend(EV_SYN,SYN_REPORT,SYN_REPORT_VALUE);

    return;
}

void moveRight(int x,int y,int trackingId) {
    printf("moveRight\n");
    fuckSend(EV_ABS,ABS_MT_TRACKING_ID,trackingId);
    fuckSend(EV_ABS,ABS_MT_POSITION_X,x);
    fuckSend(EV_ABS,ABS_MT_POSITION_Y,y);
    fuckSend(EV_KEY,BTN_TOUCH,BTN_KEY_DOWN);
    fuckSend(EV_SYN,SYN_REPORT,SYN_REPORT_VALUE);

    for (int i = 0; i < 10; i++) {
        usleep(20000);
        x = x + 60;
        fuckSend(EV_ABS,ABS_MT_POSITION_X,x);
        fuckSend(EV_SYN,SYN_REPORT,SYN_REPORT_VALUE);
    }

    fuckSend(EV_ABS,ABS_MT_TRACKING_ID,BTN_UP_TRACKING_ID);
    fuckSend(EV_KEY,BTN_TOUCH,BTN_KEY_UP);
    fuckSend(EV_SYN,SYN_REPORT,SYN_REPORT_VALUE);

    return;
}

//放大或缩小图片，type（0放大，1缩小）
void zoom(int type,int firstX,int firstY,int firstTrackingId,int firstMtSlotId,int secondX,int secondY,int secondTrackingId,int secondMtSlotId) {
    fuckSend(EV_ABS,ABS_MT_SLOT,firstMtSlotId);
    fuckSend(EV_ABS,ABS_MT_TRACKING_ID,firstTrackingId);
    fuckSend(EV_ABS,ABS_MT_POSITION_X,firstX);
    fuckSend(EV_ABS,ABS_MT_POSITION_Y,firstY);
    fuckSend(EV_KEY,BTN_TOUCH,BTN_KEY_DOWN);
    fuckSend(EV_SYN,SYN_REPORT,SYN_REPORT_VALUE);

    //这组指令和上面的指令唯一的区别就是没有key down这个指令
    fuckSend(EV_ABS,ABS_MT_SLOT,secondMtSlotId);
    fuckSend(EV_ABS,ABS_MT_TRACKING_ID,secondTrackingId);
    fuckSend(EV_ABS,ABS_MT_POSITION_X,secondX);
    fuckSend(EV_ABS,ABS_MT_POSITION_Y,secondY);
    fuckSend(EV_SYN,SYN_REPORT,SYN_REPORT_VALUE);

    //x轴每次移动25，y轴每次移动50，间隔为0.2s
    for(int i=0;i<5;i++) {
        usleep(20000);
        if(type == 0) {
            firstX = firstX + 25;
            firstY = firstY - 50;
            secondX = secondX - 25;
            secondY = secondY + 50;
        } else{
            firstX = firstX - 25;
            firstY = firstY + 50;
            secondX = secondX + 25;
            secondY = secondY - 50;
        }
        fuckSend(EV_ABS,ABS_MT_SLOT,firstMtSlotId);
        fuckSend(EV_ABS,ABS_MT_POSITION_X,firstX);
        fuckSend(EV_ABS,ABS_MT_POSITION_Y,firstY);
        fuckSend(EV_ABS,ABS_MT_SLOT,secondMtSlotId);
        fuckSend(EV_ABS,ABS_MT_POSITION_X,secondX);
        fuckSend(EV_ABS,ABS_MT_POSITION_Y,secondY);
        fuckSend(EV_SYN,SYN_REPORT,SYN_REPORT_VALUE);
    }

    //BTN_UP这个事件所有参数都是常量，所以和坐标是无关的（鬼才关心你最后手指抬起的坐标是在哪里）
    fuckSend(EV_ABS,ABS_MT_TRACKING_ID,BTN_UP_TRACKING_ID);
    fuckSend(EV_KEY,BTN_TOUCH,BTN_KEY_UP);
    fuckSend(EV_SYN,SYN_REPORT,SYN_REPORT_VALUE);
    return;
}

//最大支持十指操作
//上个指令的手指x坐标
int lastFingerArrayX[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
//上个指令的手指y坐标
int lastFingerArrayY[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int trackingId = 0;
//上个指令的手指触摸数
int lastTouchNum = 0;
//上一个有操作的slotId
int lastOperateSlotId = -1;

//坐标转换的时候要考虑x轴和y轴转换的情况
//Y轴的坐标变成x轴的坐标，x轴的坐标变成720-Y轴的坐标
//正常的屏幕分辨率都是1920*1080，但是模拟器是1080*720的，所以x坐标和y坐标都要*1.5

//一根手指按下事件，i是slotId，第一根手指是0，第二根手指是1，依此类推
void fingerDown(int i,int xAxis,int yAxis) {
    if(lastOperateSlotId != i) {
        fuckSend(EV_ABS,ABS_MT_SLOT,i);
        lastOperateSlotId = i;
    }
    fuckSend(EV_ABS,ABS_MT_TRACKING_ID,trackingId);
    //trackingId每次使用完要加一
    trackingId ++;
    lastFingerArrayX[i] = xAxis;
    lastFingerArrayY[i] = yAxis;
    fuckSend(EV_ABS,ABS_MT_POSITION_X,xAxis);
    fuckSend(EV_ABS,ABS_MT_POSITION_Y,yAxis);
    //第一个要触发DOWN事件
    if(i == 0) {
        fuckSend(EV_KEY,BTN_TOUCH,BTN_KEY_DOWN);
    }
    fuckSend(EV_SYN,SYN_REPORT,SYN_REPORT_VALUE);
}

//所有手指抬起事件（没办法做一根手指抬起事件）
void fingerUp() {
    for(int i=0;i<lastTouchNum;i++) {
        if(lastOperateSlotId != i) {
            fuckSend(EV_ABS,ABS_MT_SLOT,i);
            lastOperateSlotId = -1;
        }
        fuckSend(EV_ABS,ABS_MT_TRACKING_ID,BTN_UP_TRACKING_ID);
        //最后一个要触发UP事件
        if(i == lastTouchNum-1) {
            fuckSend(EV_KEY,BTN_TOUCH,BTN_KEY_UP);
        }
    }
    fuckSend(EV_SYN,SYN_REPORT,SYN_REPORT_VALUE);
    //把x轴坐标，y轴坐标和slotId复位
    for(int i=0;i<10;i++) {
        lastFingerArrayX[i] = -1;
        lastFingerArrayY[i] = -1;
    }
    lastTouchNum = 0;
    lastOperateSlotId = -1;
}

void changeToAndroidInstruct(char *originInst){
    //200000us是200ms也就是0.2s
    usleep(20000);

    char temp[100];
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
    while((token = strsep(&buf,":")) != NULL) {
        originInstArray[index] = token;
        index ++;
    }
    //触屏数量
    int touchNum = atoi(originInstArray[1]);
    //触屏类型：0（按下）  1（抬起）  2（移动）
    int touchType = atoi(originInstArray[2]);
    //按下事件
    if(touchType == 0) {
        for(int i=0;i<touchNum;i++) {
            //int xAxis = (int)(atoi(originInstArray[i*2+3]) * 1.5);
            //int yAxis = (int)(atoi(originInstArray[i*2+4]) * 1.5);
            int xAxis = (int)((720-atoi(originInstArray[i*2+4])) * 1.5);
            int yAxis = (int)(atoi(originInstArray[i*2+3]) * 1.5);
            fingerDown(i,xAxis,yAxis);
        }
        lastTouchNum = touchNum;
    }
    //抬起事件
    if(touchType == 1) {
        fingerUp();
    }
    //移动事件
    if(touchType == 2) {
        //如果当前指令的触屏数量不等于上个指令的触屏数量，重新初始化一次
        if(touchNum >= lastTouchNum) {
            //手指增加的情况，原来的手指继续移动
            for(int i=0;i<lastTouchNum;i++) {
                //int xAxis = (int)(atoi(originInstArray[i*2+3]) * 1.5);
                //int yAxis = (int)(atoi(originInstArray[i*2+4]) * 1.5);
                int xAxis = (int)((720-atoi(originInstArray[i*2+4])) * 1.5);
                int yAxis = (int)(atoi(originInstArray[i*2+3]) * 1.5);
                int lastFingerX = lastFingerArrayX[i];
                int lastFingerY = lastFingerArrayY[i];
                //如果和上次手指位置是一样的，不需要触发任何事件
                if(lastFingerX == xAxis && lastFingerY == yAxis) {
                    continue;
                }
                //如果上次操作的slotId不是当前slotId，要加上一句切换slotId的指令
                if(lastOperateSlotId != i) {
                    fuckSend(EV_ABS,ABS_MT_SLOT,i);
                    lastOperateSlotId = i;
                }
                if(lastFingerX != xAxis) {
                    fuckSend(EV_ABS,ABS_MT_POSITION_X,xAxis);
                }
                if(lastFingerY != yAxis) {
                    fuckSend(EV_ABS,ABS_MT_POSITION_Y,yAxis);
                }
            }
            //新增加的手指逻辑
            for(int i=lastTouchNum;i<touchNum;i++){
                //int xAxis = (int)(atoi(originInstArray[i*2+3]) * 1.5);
                //int yAxis = (int)(atoi(originInstArray[i*2+4]) * 1.5);
                int xAxis = (int)((720-atoi(originInstArray[i*2+4])) * 1.5);
                int yAxis = (int)(atoi(originInstArray[i*2+3]) * 1.5);
                fuckSend(EV_ABS,ABS_MT_SLOT,i);
                lastOperateSlotId = i;
                fuckSend(EV_ABS,ABS_MT_TRACKING_ID,trackingId);
                trackingId ++;
                lastFingerArrayX[i] = xAxis;
                lastFingerArrayY[i] = yAxis;
                fuckSend(EV_ABS,ABS_MT_POSITION_X,xAxis);
                fuckSend(EV_ABS,ABS_MT_POSITION_Y,yAxis);
            }
            fuckSend(EV_SYN,SYN_REPORT,SYN_REPORT_VALUE);
        }
            //如果抬起了手指，就要触发一次手指抬起和触摸事件
        else{
            fingerUp();
            for(int i=0;i<touchNum;i++) {
                //int xAxis = (int)(atoi(originInstArray[i*2+3]) * 1.5);
                //int yAxis = (int)(atoi(originInstArray[i*2+4]) * 1.5);
                int xAxis = (int)((720-atoi(originInstArray[i*2+4])) * 1.5);
                int yAxis = (int)(atoi(originInstArray[i*2+3]) * 1.5);
                fingerDown(i,xAxis,yAxis);
            }
        }
        //记录一下触屏数量
        lastTouchNum = touchNum;
    }
    //如果trackingId太大的话，把trackingId归0
    if(trackingId >= 0xfff) {
        trackingId = 0;
    }
    return;
}

int main(int argc, char *argv[])
{
    /*int trackingId = 0x280;
    for(int i=0;i<5;i++) {
        moveLeft(0x3ed,0x09c,trackingId);
        sleep(1);
        trackingId ++;
        moveRight(0x03b,0x09c,trackingId);
        sleep(1);
    }*/

    /*int trackingId = 0x130;
    for(int i=0;i<5;i++) {
        zoom(0,0x333,0x201,trackingId,0,0x19b,0x4e2,trackingId+1,1);
        trackingId = trackingId + 2;
        sleep(1);
        zoom(1,0x3a8,0x112,trackingId,0,0x0bb,0x6c6,trackingId+1,1);
        trackingId = trackingId + 2;
        sleep(1);
    }*/
    changeToAndroidInstruct("MULTI:1:0:1000:276");
    changeToAndroidInstruct("MULTI:2:2:1000:276:203:541");
    changeToAndroidInstruct("MULTI:2:2:1004:276:203:541");
    changeToAndroidInstruct("MULTI:2:2:1011:275:203:541");
    changeToAndroidInstruct("MULTI:2:2:1011:275:207:541");
    changeToAndroidInstruct("MULTI:2:2:1021:275:207:541");
    changeToAndroidInstruct("MULTI:2:2:1029:275:207:541");
    changeToAndroidInstruct("MULTI:2:2:1029:275:211:541");
    changeToAndroidInstruct("MULTI:2:2:1038:275:211:541");
    changeToAndroidInstruct("MULTI:2:2:1048:275:211:541");
    changeToAndroidInstruct("MULTI:2:2:1048:275:215:541");
    changeToAndroidInstruct("MULTI:2:2:1051:275:215:541");
    changeToAndroidInstruct("MULTI:2:2:1056:276:215:541");
    changeToAndroidInstruct("MULTI:2:2:1056:276:227:541");
    changeToAndroidInstruct("MULTI:2:2:1060:276:227:541");
    changeToAndroidInstruct("MULTI:2:2:1063:276:227:541");
    changeToAndroidInstruct("MULTI:2:2:1063:276:239:541");
    changeToAndroidInstruct("MULTI:2:2:1063:276:251:541");
    changeToAndroidInstruct("MULTI:2:2:1063:276:263:541");
    changeToAndroidInstruct("MULTI:2:2:1063:276:275:541");
    changeToAndroidInstruct("MULTI:2:2:1064:276:275:541");
    changeToAndroidInstruct("MULTI:2:2:1064:276:287:541");
    changeToAndroidInstruct("MULTI:2:2:1065:276:287:541");
    changeToAndroidInstruct("MULTI:2:2:1067:276:287:541");
    changeToAndroidInstruct("MULTI:2:2:1071:276:287:541");
    changeToAndroidInstruct("MULTI:2:2:1071:276:287:553");
    changeToAndroidInstruct("MULTI:2:2:1074:276:287:553");
    changeToAndroidInstruct("MULTI:2:2:1078:276:287:553");
    changeToAndroidInstruct("MULTI:2:2:1078:276:287:565");
    changeToAndroidInstruct("MULTI:2:2:1083:276:287:565");
    changeToAndroidInstruct("MULTI:2:2:1086:276:287:565");
    changeToAndroidInstruct("MULTI:2:2:1086:276:288:577");
    changeToAndroidInstruct("MULTI:2:2:1090:276:288:577");
    changeToAndroidInstruct("MULTI:2:2:1090:276:289:589");
    changeToAndroidInstruct("MULTI:2:2:1091:276:289:589");
    changeToAndroidInstruct("MULTI:2:2:1094:276:289:589");
    changeToAndroidInstruct("MULTI:2:2:1096:276:289:589");
    changeToAndroidInstruct("MULTI:2:2:1096:276:290:601");
    changeToAndroidInstruct("MULTI:2:2:1098:275:290:601");
    changeToAndroidInstruct("MULTI:2:2:1098:275:291:613");
    changeToAndroidInstruct("MULTI:2:2:1101:275:291:613");
    changeToAndroidInstruct("MULTI:2:2:1101:275:292:625");
    changeToAndroidInstruct("MULTI:2:2:1104:275:292:625");
    changeToAndroidInstruct("MULTI:2:2:1106:274:292:625");
    changeToAndroidInstruct("MULTI:2:2:1106:274:293:631");
    changeToAndroidInstruct("MULTI:2:2:1110:274:293:631");
    changeToAndroidInstruct("MULTI:2:2:1114:273:293:631");
    changeToAndroidInstruct("MULTI:2:2:1118:273:293:631");
    changeToAndroidInstruct("MULTI:2:2:1121:272:293:631");
    changeToAndroidInstruct("MULTI:2:2:1124:272:293:631");
    changeToAndroidInstruct("MULTI:2:2:1128:272:293:631");
    changeToAndroidInstruct("MULTI:2:2:1130:272:293:631");
    changeToAndroidInstruct("MULTI:2:2:1133:272:293:631");
    changeToAndroidInstruct("MULTI:2:2:1134:272:293:631");
    changeToAndroidInstruct("MULTI:2:2:1136:272:293:631");
    changeToAndroidInstruct("MULTI:2:2:1139:272:293:631");
    changeToAndroidInstruct("MULTI:2:2:1143:272:293:631");
    changeToAndroidInstruct("MULTI:2:2:1145:272:293:631");
    changeToAndroidInstruct("MULTI:2:2:1152:272:293:631");
    changeToAndroidInstruct("MULTI:2:2:1156:272:293:631");
    changeToAndroidInstruct("MULTI:2:2:1162:272:293:631");
    changeToAndroidInstruct("MULTI:2:2:1167:272:293:631");
    changeToAndroidInstruct("MULTI:2:2:1173:272:293:631");
    changeToAndroidInstruct("MULTI:2:2:1173:272:281:635");
    changeToAndroidInstruct("MULTI:2:2:1177:272:281:635");
    changeToAndroidInstruct("MULTI:2:2:1179:272:281:635");
    changeToAndroidInstruct("MULTI:2:2:1179:272:269:640");
    changeToAndroidInstruct("MULTI:2:2:1183:272:269:640");
    changeToAndroidInstruct("MULTI:2:2:1183:272:257:645");
    changeToAndroidInstruct("MULTI:2:2:1187:272:257:645");
    changeToAndroidInstruct("MULTI:2:2:1192:272:257:645");
    changeToAndroidInstruct("MULTI:2:2:1196:272:257:645");
    changeToAndroidInstruct("MULTI:2:2:1196:272:245:650");
    changeToAndroidInstruct("MULTI:2:2:1200:271:245:650");
    changeToAndroidInstruct("MULTI:2:2:1204:271:245:650");
    changeToAndroidInstruct("MULTI:2:2:1204:271:233:655");
    changeToAndroidInstruct("MULTI:2:2:1209:269:233:655");
    changeToAndroidInstruct("MULTI:2:2:1215:268:233:655");
    changeToAndroidInstruct("MULTI:2:2:1215:268:221:660");
    changeToAndroidInstruct("MULTI:2:2:1220:268:221:660");
    changeToAndroidInstruct("MULTI:2:2:1225:267:221:660");
    changeToAndroidInstruct("MULTI:2:2:1225:267:209:665");
    changeToAndroidInstruct("MULTI:2:2:1231:266:209:665");
    changeToAndroidInstruct("MULTI:2:2:1235:266:209:665");
    changeToAndroidInstruct("MULTI:2:2:1235:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1238:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1243:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1246:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1250:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1254:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1261:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1263:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1268:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1271:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1275:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1278:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1283:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1287:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1293:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1298:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1304:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1308:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1312:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1317:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1321:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1323:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1327:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1331:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1335:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1340:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1344:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1348:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1351:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1353:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1355:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1357:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1358:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1361:266:203:669");
    changeToAndroidInstruct("MULTI:2:2:1361:266:214:664");
    changeToAndroidInstruct("MULTI:2:2:1365:266:214:664");
    changeToAndroidInstruct("MULTI:2:2:1369:268:214:664");
    changeToAndroidInstruct("MULTI:2:2:1369:268:225:659");
    changeToAndroidInstruct("MULTI:2:2:1373:268:225:659");
    changeToAndroidInstruct("MULTI:2:2:1373:268:233:649");
    changeToAndroidInstruct("MULTI:2:2:1375:269:233:649");
    changeToAndroidInstruct("MULTI:2:2:1378:270:233:649");
    changeToAndroidInstruct("MULTI:2:2:1378:270:241:639");
    changeToAndroidInstruct("MULTI:2:2:1382:273:241:639");
    changeToAndroidInstruct("MULTI:2:2:1384:274:241:639");
    changeToAndroidInstruct("MULTI:2:2:1384:274:249:629");
    changeToAndroidInstruct("MULTI:2:2:1388:277:249:629");
    changeToAndroidInstruct("MULTI:2:2:1393:281:249:629");
    changeToAndroidInstruct("MULTI:2:2:1393:281:257:619");
    changeToAndroidInstruct("MULTI:2:2:1397:283:257:619");
    changeToAndroidInstruct("MULTI:2:2:1402:286:257:619");
    changeToAndroidInstruct("MULTI:2:2:1402:286:265:609");
    changeToAndroidInstruct("MULTI:2:2:1405:287:265:609");
    changeToAndroidInstruct("MULTI:2:2:1408:290:265:609");
    changeToAndroidInstruct("MULTI:2:2:1408:290:273:599");
    changeToAndroidInstruct("MULTI:2:2:1409:290:273:599");
    changeToAndroidInstruct("MULTI:2:2:1411:292:273:599");
    changeToAndroidInstruct("MULTI:2:2:1411:292:282:589");
    changeToAndroidInstruct("MULTI:2:2:1414:294:282:589");
    changeToAndroidInstruct("MULTI:2:2:1415:295:282:589");
    changeToAndroidInstruct("MULTI:2:2:1415:295:291:580");
    changeToAndroidInstruct("MULTI:2:2:1417:296:291:580");
    changeToAndroidInstruct("MULTI:2:2:1417:296:300:571");
    changeToAndroidInstruct("MULTI:2:2:1419:296:300:571");
    changeToAndroidInstruct("MULTI:2:2:1419:296:309:562");
    changeToAndroidInstruct("MULTI:2:2:1419:296:318:553");
    changeToAndroidInstruct("MULTI:2:2:1420:297:318:553");
    changeToAndroidInstruct("MULTI:2:2:1420:297:327:544");
    changeToAndroidInstruct("MULTI:2:2:1421:297:327:544");
    changeToAndroidInstruct("MULTI:2:2:1421:297:331:541");
    changeToAndroidInstruct("MULTI:2:2:1423:300:331:541");
    changeToAndroidInstruct("MULTI:2:2:1423:302:331:541");
    changeToAndroidInstruct("MULTI:2:2:1424:304:331:541");
    changeToAndroidInstruct("MULTI:2:2:1425:307:331:541");
    changeToAndroidInstruct("MULTI:2:2:1425:307:326:529");
    changeToAndroidInstruct("MULTI:2:2:1425:308:326:529");
    changeToAndroidInstruct("MULTI:2:2:1425:308:321:517");
    changeToAndroidInstruct("MULTI:2:2:1425:311:321:517");
    changeToAndroidInstruct("MULTI:2:2:1426:312:321:517");
    changeToAndroidInstruct("MULTI:2:2:1426:315:321:517");
    changeToAndroidInstruct("MULTI:2:2:1426:315:316:505");
    changeToAndroidInstruct("MULTI:2:2:1426:318:316:505");
    changeToAndroidInstruct("MULTI:2:2:1426:318:311:493");
    changeToAndroidInstruct("MULTI:2:2:1426:320:311:493");
    changeToAndroidInstruct("MULTI:2:2:1426:323:311:493");
    changeToAndroidInstruct("MULTI:2:2:1426:326:311:493");
    changeToAndroidInstruct("MULTI:2:2:1426:326:306:481");
    changeToAndroidInstruct("MULTI:2:2:1426:327:306:481");
    changeToAndroidInstruct("MULTI:2:2:1425:331:306:481");
    changeToAndroidInstruct("MULTI:2:2:1425:331:295:473");
    changeToAndroidInstruct("MULTI:2:2:1421:335:295:473");
    changeToAndroidInstruct("MULTI:2:2:1421:335:284:466");
    changeToAndroidInstruct("MULTI:2:2:1416:340:284:466");
    changeToAndroidInstruct("MULTI:2:2:1412:344:284:466");
    changeToAndroidInstruct("MULTI:2:2:1412:344:273:458");
    changeToAndroidInstruct("MULTI:2:2:1410:346:273:458");
    changeToAndroidInstruct("MULTI:2:2:1406:350:273:458");
    changeToAndroidInstruct("MULTI:2:2:1406:350:262:451");
    changeToAndroidInstruct("MULTI:2:2:1403:351:262:451");
    changeToAndroidInstruct("MULTI:2:2:1396:355:262:451");
    changeToAndroidInstruct("MULTI:2:2:1396:355:251:444");
    changeToAndroidInstruct("MULTI:2:2:1391:358:251:444");
    changeToAndroidInstruct("MULTI:2:2:1386:359:251:444");
    changeToAndroidInstruct("MULTI:2:2:1386:359:240:437");
    changeToAndroidInstruct("MULTI:2:2:1381:360:240:437");
    changeToAndroidInstruct("MULTI:2:2:1378:360:240:437");
    changeToAndroidInstruct("MULTI:2:2:1375:360:240:437");
    changeToAndroidInstruct("MULTI:2:2:1375:360:229:430");
    changeToAndroidInstruct("MULTI:2:2:1372:360:229:430");
    changeToAndroidInstruct("MULTI:2:2:1372:360:218:422");
    changeToAndroidInstruct("MULTI:2:2:1368:360:218:422");
    changeToAndroidInstruct("MULTI:2:2:1363:360:218:422");
    changeToAndroidInstruct("MULTI:2:2:1363:360:207:415");
    changeToAndroidInstruct("MULTI:2:2:1358:360:207:415");
    changeToAndroidInstruct("MULTI:2:2:1350:360:207:415");
    changeToAndroidInstruct("MULTI:2:2:1350:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1345:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1336:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1331:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1325:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1322:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1318:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1314:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1310:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1305:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1301:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1297:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1294:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1292:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1290:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1288:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1287:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1286:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1283:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1281:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1280:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1278:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1276:360:203:413");
    changeToAndroidInstruct("MULTI:2:2:1276:360:203:425");
    changeToAndroidInstruct("MULTI:2:2:1276:360:193:433");
    changeToAndroidInstruct("MULTI:2:2:1280:363:193:433");
    changeToAndroidInstruct("MULTI:2:2:1280:363:183:441");
    changeToAndroidInstruct("MULTI:2:2:1291:364:183:441");
    changeToAndroidInstruct("MULTI:2:2:1297:365:183:441");
    changeToAndroidInstruct("MULTI:2:2:1297:365:173:449");
    changeToAndroidInstruct("MULTI:2:2:1305:365:173:449");
    changeToAndroidInstruct("MULTI:2:2:1309:365:173:449");
    changeToAndroidInstruct("MULTI:2:2:1309:365:163:457");
    changeToAndroidInstruct("MULTI:2:2:1313:365:163:457");
    changeToAndroidInstruct("MULTI:2:2:1316:365:163:457");
    changeToAndroidInstruct("MULTI:2:2:1319:365:163:457");
    changeToAndroidInstruct("MULTI:2:2:1319:365:153:465");
    changeToAndroidInstruct("MULTI:2:2:1322:365:153:465");
    changeToAndroidInstruct("MULTI:2:2:1322:365:143:473");
    changeToAndroidInstruct("MULTI:2:2:1327:365:143:473");
    changeToAndroidInstruct("MULTI:2:2:1333:365:143:473");
    changeToAndroidInstruct("MULTI:2:2:1333:365:133:482");
    changeToAndroidInstruct("MULTI:2:2:1340:365:133:482");
    changeToAndroidInstruct("MULTI:2:2:1346:365:133:482");
    changeToAndroidInstruct("MULTI:2:2:1346:365:124:491");
    changeToAndroidInstruct("MULTI:2:2:1354:365:124:491");
    changeToAndroidInstruct("MULTI:2:2:1358:365:124:491");
    changeToAndroidInstruct("MULTI:2:2:1358:365:115:500");
    changeToAndroidInstruct("MULTI:2:2:1361:365:115:500");
    changeToAndroidInstruct("MULTI:2:2:1364:365:115:500");
    changeToAndroidInstruct("MULTI:2:2:1364:365:106:509");
    changeToAndroidInstruct("MULTI:2:2:1365:365:106:509");
    changeToAndroidInstruct("MULTI:2:2:1365:365:118:513");
    changeToAndroidInstruct("MULTI:2:2:1366:365:118:513");
    changeToAndroidInstruct("MULTI:2:2:1367:365:118:513");
    changeToAndroidInstruct("MULTI:2:2:1367:365:130:517");
    changeToAndroidInstruct("MULTI:2:2:1367:365:142:520");
    changeToAndroidInstruct("MULTI:2:2:1368:364:142:520");
    changeToAndroidInstruct("MULTI:2:2:1369:364:142:520");
    changeToAndroidInstruct("MULTI:2:2:1372:363:142:520");
    changeToAndroidInstruct("MULTI:2:2:1372:363:154:524");
    changeToAndroidInstruct("MULTI:2:2:1374:362:154:524");
    changeToAndroidInstruct("MULTI:2:2:1374:362:166:528");
    changeToAndroidInstruct("MULTI:2:2:1378:359:166:528");
    changeToAndroidInstruct("MULTI:2:2:1383:358:166:528");
    changeToAndroidInstruct("MULTI:2:2:1383:358:178:532");
    changeToAndroidInstruct("MULTI:2:2:1388:355:178:532");
    changeToAndroidInstruct("MULTI:2:2:1395:353:178:532");
    changeToAndroidInstruct("MULTI:2:2:1395:353:190:536");
    changeToAndroidInstruct("MULTI:2:2:1400:350:190:536");
    changeToAndroidInstruct("MULTI:2:2:1404:348:190:536");
    changeToAndroidInstruct("MULTI:2:2:1404:348:201:540");
    changeToAndroidInstruct("MULTI:2:2:1407:346:201:540");
    changeToAndroidInstruct("MULTI:2:2:1410:346:201:540");
    changeToAndroidInstruct("MULTI:2:2:1410:346:203:541");
    changeToAndroidInstruct("MULTI:2:2:1413:345:203:541");
    changeToAndroidInstruct("MULTI:2:2:1417:344:203:541");
    changeToAndroidInstruct("MULTI:2:2:1420:343:203:541");
    changeToAndroidInstruct("MULTI:2:2:1423:342:203:541");
    changeToAndroidInstruct("MULTI:2:2:1423:342:203:553");
    changeToAndroidInstruct("MULTI:2:2:1426:341:203:553");
    changeToAndroidInstruct("MULTI:2:2:1428:340:203:553");
    changeToAndroidInstruct("MULTI:2:2:1428:340:203:565");
    changeToAndroidInstruct("MULTI:2:2:1431:338:203:565");
    changeToAndroidInstruct("MULTI:2:2:1433:338:203:565");
    changeToAndroidInstruct("MULTI:2:2:1433:338:203:577");
    changeToAndroidInstruct("MULTI:2:2:1434:338:203:577");
    changeToAndroidInstruct("MULTI:2:2:1437:337:203:577");
    changeToAndroidInstruct("MULTI:2:2:1437:337:203:589");
    changeToAndroidInstruct("MULTI:2:2:1440:337:203:589");
    changeToAndroidInstruct("MULTI:2:2:1444:337:203:589");
    changeToAndroidInstruct("MULTI:2:2:1444:337:203:601");
    changeToAndroidInstruct("MULTI:2:2:1448:336:203:601");
    changeToAndroidInstruct("MULTI:2:2:1453:336:203:601");
    changeToAndroidInstruct("MULTI:2:2:1453:336:203:613");
    changeToAndroidInstruct("MULTI:2:2:1457:336:203:613");
    changeToAndroidInstruct("MULTI:2:2:1460:336:203:613");
    changeToAndroidInstruct("MULTI:2:2:1460:336:203:625");
    changeToAndroidInstruct("MULTI:2:2:1465:334:203:625");
    changeToAndroidInstruct("MULTI:2:2:1465:334:203:637");
    changeToAndroidInstruct("MULTI:2:2:1467:334:203:637");
    changeToAndroidInstruct("MULTI:2:2:1468:334:203:637");
    changeToAndroidInstruct("MULTI:2:2:1468:334:203:649");
    changeToAndroidInstruct("MULTI:2:2:1471:332:203:649");
    changeToAndroidInstruct("MULTI:2:2:1472:332:203:649");
    changeToAndroidInstruct("MULTI:2:2:1472:332:203:661");
    changeToAndroidInstruct("MULTI:2:2:1478:332:203:661");
    changeToAndroidInstruct("MULTI:2:2:1482:332:203:661");
    changeToAndroidInstruct("MULTI:2:2:1482:332:203:669");
    changeToAndroidInstruct("MULTI:2:2:1491:332:203:669");
    changeToAndroidInstruct("MULTI:2:2:1500:332:203:669");
    changeToAndroidInstruct("MULTI:2:2:1508:332:203:669");
    changeToAndroidInstruct("MULTI:2:2:1516:332:203:669");
    changeToAndroidInstruct("MULTI:2:2:1516:332:191:664");
    changeToAndroidInstruct("MULTI:2:2:1523:332:191:664");
    changeToAndroidInstruct("MULTI:2:2:1527:332:191:664");
    changeToAndroidInstruct("MULTI:2:2:1527:332:202:668");
    changeToAndroidInstruct("MULTI:2:2:1529:332:202:668");
    changeToAndroidInstruct("MULTI:2:2:1530:332:202:668");
    changeToAndroidInstruct("MULTI:2:2:1530:332:203:669");
    changeToAndroidInstruct("MULTI:2:2:1530:332:214:664");
    changeToAndroidInstruct("MULTI:2:2:1530:332:225:659");
    changeToAndroidInstruct("MULTI:2:2:1533:330:225:659");
    changeToAndroidInstruct("MULTI:2:2:1534:329:225:659");
    changeToAndroidInstruct("MULTI:2:2:1534:329:236:654");
    changeToAndroidInstruct("MULTI:2:2:1535:328:236:654");
    changeToAndroidInstruct("MULTI:2:2:1535:328:247:649");
    changeToAndroidInstruct("MULTI:2:2:1537:326:247:649");
    changeToAndroidInstruct("MULTI:2:2:1538:325:247:649");
    changeToAndroidInstruct("MULTI:2:2:1538:325:258:644");
    changeToAndroidInstruct("MULTI:2:2:1539:323:258:644");
    changeToAndroidInstruct("MULTI:2:2:1539:323:265:633");
    changeToAndroidInstruct("MULTI:2:2:1539:323:272:622");
    changeToAndroidInstruct("MULTI:2:2:1540:322:272:622");
    changeToAndroidInstruct("MULTI:2:2:1540:322:279:611");
    changeToAndroidInstruct("MULTI:2:2:1540:321:279:611");
    changeToAndroidInstruct("MULTI:2:2:1541:320:279:611");
    changeToAndroidInstruct("MULTI:2:2:1541:320:286:600");
    changeToAndroidInstruct("MULTI:2:2:1541:320:293:589");
    changeToAndroidInstruct("MULTI:2:2:1542:317:293:589");
    changeToAndroidInstruct("MULTI:2:2:1542:316:293:589");
    changeToAndroidInstruct("MULTI:2:2:1542:316:300:578");
    changeToAndroidInstruct("MULTI:2:2:1544:313:300:578");
    changeToAndroidInstruct("MULTI:2:2:1544:312:300:578");
    changeToAndroidInstruct("MULTI:2:2:1544:312:308:568");
    changeToAndroidInstruct("MULTI:2:2:1544:310:308:568");
    changeToAndroidInstruct("MULTI:2:2:1544:309:308:568");
    changeToAndroidInstruct("MULTI:2:2:1544:309:316:558");
    changeToAndroidInstruct("MULTI:2:2:1544:307:316:558");
    changeToAndroidInstruct("MULTI:2:2:1544:307:324:548");
    changeToAndroidInstruct("MULTI:2:2:1544:305:324:548");
    changeToAndroidInstruct("MULTI:2:2:1544:304:324:548");
    changeToAndroidInstruct("MULTI:2:2:1544:304:331:541");
    changeToAndroidInstruct("MULTI:2:2:1544:303:331:541");
    changeToAndroidInstruct("MULTI:2:2:1544:301:331:541");
    changeToAndroidInstruct("MULTI:2:2:1544:298:331:541");
    changeToAndroidInstruct("MULTI:2:2:1544:297:331:541");
    changeToAndroidInstruct("MULTI:2:2:1544:296:331:541");
    changeToAndroidInstruct("MULTI:2:2:1544:295:331:541");
    changeToAndroidInstruct("MULTI:2:2:1544:294:331:541");
    changeToAndroidInstruct("MULTI:2:2:1544:293:331:541");
    changeToAndroidInstruct("MULTI:2:2:1544:293:326:529");
    changeToAndroidInstruct("MULTI:2:2:1544:292:326:529");
    changeToAndroidInstruct("MULTI:2:2:1544:292:316:520");
    changeToAndroidInstruct("MULTI:2:2:1544:292:306:511");
    changeToAndroidInstruct("MULTI:2:2:1543:291:306:511");
    changeToAndroidInstruct("MULTI:2:2:1542:290:306:511");
    changeToAndroidInstruct("MULTI:2:2:1542:290:296:502");
    changeToAndroidInstruct("MULTI:2:2:1539:288:296:502");
    changeToAndroidInstruct("MULTI:2:2:1537:286:296:502");
    changeToAndroidInstruct("MULTI:2:2:1532:284:296:502");
    changeToAndroidInstruct("MULTI:2:2:1532:284:286:493");
    changeToAndroidInstruct("MULTI:2:2:1526:281:286:493");
    changeToAndroidInstruct("MULTI:2:2:1521:279:286:493");
    changeToAndroidInstruct("MULTI:2:2:1521:279:276:484");
    changeToAndroidInstruct("MULTI:2:2:1517:277:276:484");
    changeToAndroidInstruct("MULTI:2:2:1517:277:266:475");
    changeToAndroidInstruct("MULTI:2:2:1513:276:266:475");
    changeToAndroidInstruct("MULTI:2:2:1511:276:266:475");
    changeToAndroidInstruct("MULTI:2:2:1511:276:256:466");
    changeToAndroidInstruct("MULTI:2:2:1509:274:256:466");
    changeToAndroidInstruct("MULTI:2:2:1507:273:256:466");
    changeToAndroidInstruct("MULTI:2:2:1507:273:246:456");
    changeToAndroidInstruct("MULTI:2:2:1506:272:246:456");
    changeToAndroidInstruct("MULTI:2:2:1506:272:236:446");
    changeToAndroidInstruct("MULTI:2:2:1503:271:236:446");
    changeToAndroidInstruct("MULTI:2:2:1503:271:226:436");
    changeToAndroidInstruct("MULTI:2:2:1502:271:226:436");
    changeToAndroidInstruct("MULTI:2:2:1502:271:216:426");
    changeToAndroidInstruct("MULTI:2:2:1501:271:216:426");
    changeToAndroidInstruct("MULTI:2:2:1501:271:206:416");
    changeToAndroidInstruct("MULTI:2:2:1501:271:203:413");
    changeToAndroidInstruct("MULTI:2:2:1500:271:203:413");
    changeToAndroidInstruct("MULTI:2:2:1500:270:203:413");
    changeToAndroidInstruct("MULTI:2:2:1500:270:203:425");
    changeToAndroidInstruct("MULTI:2:2:1497:270:203:425");
    changeToAndroidInstruct("MULTI:2:2:1497:270:203:437");
    changeToAndroidInstruct("MULTI:2:2:1496:270:203:437");
    changeToAndroidInstruct("MULTI:2:2:1494:270:203:437");
    changeToAndroidInstruct("MULTI:2:2:1494:270:203:449");
    changeToAndroidInstruct("MULTI:2:2:1492:270:203:449");
    changeToAndroidInstruct("MULTI:2:2:1491:270:203:449");
    changeToAndroidInstruct("MULTI:2:2:1491:270:203:461");
    changeToAndroidInstruct("MULTI:2:2:1490:270:203:461");
    changeToAndroidInstruct("MULTI:2:2:1490:270:203:473");
    changeToAndroidInstruct("MULTI:2:2:1490:270:203:485");
    changeToAndroidInstruct("MULTI:2:2:1488:270:203:485");
    changeToAndroidInstruct("MULTI:2:2:1487:270:203:485");
    changeToAndroidInstruct("MULTI:2:2:1487:270:214:490");
    changeToAndroidInstruct("MULTI:2:2:1485:272:214:490");
    changeToAndroidInstruct("MULTI:2:2:1485:272:225:495");
    changeToAndroidInstruct("MULTI:2:2:1484:273:225:495");
    changeToAndroidInstruct("MULTI:2:2:1484:273:236:500");
    changeToAndroidInstruct("MULTI:2:2:1482:275:236:500");
    changeToAndroidInstruct("MULTI:2:2:1481:276:236:500");
    changeToAndroidInstruct("MULTI:2:2:1481:276:247:505");
    changeToAndroidInstruct("MULTI:2:2:1480:277:247:505");
    changeToAndroidInstruct("MULTI:2:2:1479:278:247:505");
    changeToAndroidInstruct("MULTI:2:2:1479:278:258:510");
    changeToAndroidInstruct("MULTI:2:2:1478:279:258:510");
    changeToAndroidInstruct("MULTI:2:2:1476:281:258:510");
    changeToAndroidInstruct("MULTI:2:2:1476:281:269:515");
    changeToAndroidInstruct("MULTI:2:2:1475:283:269:515");
    changeToAndroidInstruct("MULTI:2:2:1472:285:269:515");
    changeToAndroidInstruct("MULTI:2:2:1472:285:280:519");
    changeToAndroidInstruct("MULTI:2:2:1470:288:280:519");
    changeToAndroidInstruct("MULTI:2:2:1466:291:280:519");
    changeToAndroidInstruct("MULTI:2:2:1466:291:281:531");
    changeToAndroidInstruct("MULTI:2:2:1464:294:281:531");
    changeToAndroidInstruct("MULTI:2:2:1463:295:281:531");
    changeToAndroidInstruct("MULTI:2:2:1463:295:282:543");
    changeToAndroidInstruct("MULTI:2:2:1459:299:282:543");
    changeToAndroidInstruct("MULTI:2:2:1457:301:282:543");
    changeToAndroidInstruct("MULTI:2:2:1457:301:283:555");
    changeToAndroidInstruct("MULTI:2:2:1455:302:283:555");
    changeToAndroidInstruct("MULTI:2:2:1455:304:283:555");
    changeToAndroidInstruct("MULTI:2:2:1455:304:275:565");
    changeToAndroidInstruct("MULTI:2:2:1453:305:275:565");
    changeToAndroidInstruct("MULTI:2:2:1452:306:275:565");
    changeToAndroidInstruct("MULTI:2:2:1452:306:267:575");
    changeToAndroidInstruct("MULTI:2:2:1451:308:267:575");
    changeToAndroidInstruct("MULTI:2:2:1450:309:267:575");
    changeToAndroidInstruct("MULTI:2:2:1450:309:259:585");
    changeToAndroidInstruct("MULTI:2:2:1448:310:259:585");
    changeToAndroidInstruct("MULTI:2:2:1448:310:251:595");
    changeToAndroidInstruct("MULTI:2:2:1445:311:251:595");
    changeToAndroidInstruct("MULTI:2:2:1445:311:244:605");
    changeToAndroidInstruct("MULTI:2:2:1443:311:244:605");
    changeToAndroidInstruct("MULTI:2:2:1439:311:244:605");
    changeToAndroidInstruct("MULTI:2:2:1439:311:237:615");
    changeToAndroidInstruct("MULTI:2:2:1435:311:237:615");
    changeToAndroidInstruct("MULTI:2:2:1435:311:230:625");
    changeToAndroidInstruct("MULTI:2:2:1435:311:223:635");
    changeToAndroidInstruct("MULTI:2:2:1435:311:216:646");
    changeToAndroidInstruct("MULTI:2:2:1435:311:209:657");
    changeToAndroidInstruct("MULTI:2:2:1435:311:203:668");
    changeToAndroidInstruct("MULTI:2:2:1435:311:212:658");
    changeToAndroidInstruct("MULTI:2:2:1434:311:212:658");
    changeToAndroidInstruct("MULTI:2:2:1432:311:212:658");
    changeToAndroidInstruct("MULTI:2:2:1432:311:221:649");
    changeToAndroidInstruct("MULTI:2:2:1431:311:221:649");
    changeToAndroidInstruct("MULTI:2:2:1431:311:230:640");
    changeToAndroidInstruct("MULTI:2:2:1429:311:230:640");
    changeToAndroidInstruct("MULTI:2:2:1427:310:230:640");
    changeToAndroidInstruct("MULTI:2:2:1427:310:239:631");
    changeToAndroidInstruct("MULTI:2:2:1424:309:239:631");
    changeToAndroidInstruct("MULTI:2:2:1421:307:239:631");
    changeToAndroidInstruct("MULTI:2:2:1418:306:239:631");
    changeToAndroidInstruct("MULTI:2:2:1418:306:251:631");
    changeToAndroidInstruct("MULTI:2:2:1414:305:251:631");
    changeToAndroidInstruct("MULTI:2:2:1411:304:251:631");
    changeToAndroidInstruct("MULTI:2:2:1411:304:240:638");
    changeToAndroidInstruct("MULTI:2:2:1408:303:240:638");
    changeToAndroidInstruct("MULTI:2:2:1408:303:230:646");
    changeToAndroidInstruct("MULTI:2:2:1403:303:230:646");
    changeToAndroidInstruct("MULTI:2:2:1399:302:230:646");
    changeToAndroidInstruct("MULTI:2:2:1395:301:230:646");
    changeToAndroidInstruct("MULTI:2:2:1395:301:217:644");
    changeToAndroidInstruct("MULTI:2:2:1392:301:217:644");
    changeToAndroidInstruct("MULTI:2:2:1392:301:204:642");
    changeToAndroidInstruct("MULTI:2:2:1390:299:204:642");
    changeToAndroidInstruct("MULTI:2:2:1388:299:204:642");
    changeToAndroidInstruct("MULTI:2:2:1388:299:191:640");
    changeToAndroidInstruct("MULTI:2:2:1386:299:191:640");
    changeToAndroidInstruct("MULTI:2:2:1383:298:191:640");
    changeToAndroidInstruct("MULTI:2:2:1383:298:178:638");
    changeToAndroidInstruct("MULTI:2:2:1383:297:178:638");
    changeToAndroidInstruct("MULTI:2:2:1380:297:178:638");
    changeToAndroidInstruct("MULTI:2:2:1380:297:165:636");
    changeToAndroidInstruct("MULTI:2:2:1379:296:165:636");
    changeToAndroidInstruct("MULTI:2:2:1377:295:165:636");
    changeToAndroidInstruct("MULTI:2:2:1377:295:152:634");
    changeToAndroidInstruct("MULTI:2:2:1376:295:152:634");
    changeToAndroidInstruct("MULTI:2:2:1375:295:152:634");
    changeToAndroidInstruct("MULTI:2:2:1375:295:139:633");
    changeToAndroidInstruct("MULTI:2:2:1374:295:139:633");
    changeToAndroidInstruct("MULTI:2:2:1374:294:139:633");
    changeToAndroidInstruct("MULTI:2:2:1374:294:126:632");
    changeToAndroidInstruct("MULTI:2:2:1373:294:126:632");
    changeToAndroidInstruct("MULTI:2:2:1369:292:126:632");
    changeToAndroidInstruct("MULTI:2:2:1369:292:137:637");
    changeToAndroidInstruct("MULTI:2:2:1365:292:137:637");
    changeToAndroidInstruct("MULTI:2:2:1359:291:137:637");
    changeToAndroidInstruct("MULTI:2:2:1359:291:149:636");
    changeToAndroidInstruct("MULTI:2:2:1351:290:149:636");
    changeToAndroidInstruct("MULTI:2:2:1342:287:149:636");
    changeToAndroidInstruct("MULTI:2:2:1342:287:161:635");
    changeToAndroidInstruct("MULTI:2:2:1335:286:161:635");
    changeToAndroidInstruct("MULTI:2:2:1328:285:161:635");
    changeToAndroidInstruct("MULTI:2:2:1328:285:173:634");
    changeToAndroidInstruct("MULTI:2:2:1324:285:173:634");
    changeToAndroidInstruct("MULTI:2:2:1321:285:173:634");
    changeToAndroidInstruct("MULTI:2:2:1321:285:185:633");
    changeToAndroidInstruct("MULTI:2:2:1318:284:185:633");
    changeToAndroidInstruct("MULTI:2:2:1318:284:197:632");
    changeToAndroidInstruct("MULTI:2:2:1316:283:197:632");
    changeToAndroidInstruct("MULTI:2:2:1314:283:197:632");
    changeToAndroidInstruct("MULTI:2:2:1314:283:209:631");
    changeToAndroidInstruct("MULTI:2:2:1311:283:209:631");
    changeToAndroidInstruct("MULTI:2:2:1307:282:209:631");
    changeToAndroidInstruct("MULTI:2:2:1307:282:221:631");
    changeToAndroidInstruct("MULTI:2:2:1302:282:221:631");
    changeToAndroidInstruct("MULTI:2:2:1297:281:221:631");
    changeToAndroidInstruct("MULTI:2:2:1292:280:221:631");
    changeToAndroidInstruct("MULTI:2:2:1292:280:233:631");
    changeToAndroidInstruct("MULTI:2:2:1288:278:233:631");
    changeToAndroidInstruct("MULTI:2:2:1284:278:233:631");
    changeToAndroidInstruct("MULTI:2:2:1284:278:245:631");
    changeToAndroidInstruct("MULTI:2:2:1282:278:245:631");
    changeToAndroidInstruct("MULTI:2:2:1282:278:257:631");
    changeToAndroidInstruct("MULTI:2:2:1279:277:257:631");
    changeToAndroidInstruct("MULTI:2:2:1276:277:257:631");
    changeToAndroidInstruct("MULTI:2:2:1276:277:246:638");
    changeToAndroidInstruct("MULTI:2:2:1274:277:246:638");
    changeToAndroidInstruct("MULTI:2:2:1272:276:246:638");
    changeToAndroidInstruct("MULTI:2:2:1272:276:235:645");
    changeToAndroidInstruct("MULTI:2:2:1271:276:235:645");
    changeToAndroidInstruct("MULTI:2:2:1269:276:235:645");
    changeToAndroidInstruct("MULTI:2:2:1269:276:224:652");
    changeToAndroidInstruct("MULTI:2:2:1268:276:224:652");
    changeToAndroidInstruct("MULTI:2:2:1266:276:224:652");
    changeToAndroidInstruct("MULTI:2:2:1266:276:214:660");
    changeToAndroidInstruct("MULTI:2:2:1264:276:214:660");
    changeToAndroidInstruct("MULTI:2:2:1261:276:214:660");
    changeToAndroidInstruct("MULTI:2:2:1261:276:204:668");
    changeToAndroidInstruct("MULTI:2:2:1257:276:204:668");
    changeToAndroidInstruct("MULTI:2:2:1253:276:204:668");
    changeToAndroidInstruct("MULTI:2:2:1253:276:203:669");
    changeToAndroidInstruct("MULTI:2:2:1249:276:203:669");
    changeToAndroidInstruct("MULTI:2:2:1243:274:203:669");
    changeToAndroidInstruct("MULTI:2:2:1243:274:191:664");
    changeToAndroidInstruct("MULTI:2:2:1238:273:191:664");
    changeToAndroidInstruct("MULTI:2:2:1235:273:191:664");
    changeToAndroidInstruct("MULTI:2:2:1235:273:179:659");
    changeToAndroidInstruct("MULTI:2:2:1232:273:179:659");
    changeToAndroidInstruct("MULTI:2:2:1231:273:179:659");
    changeToAndroidInstruct("MULTI:2:2:1231:273:167:654");
    changeToAndroidInstruct("MULTI:2:2:1229:273:167:654");
    changeToAndroidInstruct("MULTI:2:2:1227:273:167:654");
    changeToAndroidInstruct("MULTI:2:2:1227:273:155:649");
    changeToAndroidInstruct("MULTI:2:2:1225:273:155:649");
    changeToAndroidInstruct("MULTI:2:2:1222:272:155:649");
    changeToAndroidInstruct("MULTI:2:2:1222:272:143:644");
    changeToAndroidInstruct("MULTI:2:2:1220:272:143:644");
    changeToAndroidInstruct("MULTI:2:2:1217:272:143:644");
    changeToAndroidInstruct("MULTI:2:2:1217:272:154:648");
    changeToAndroidInstruct("MULTI:2:2:1213:272:154:648");
    changeToAndroidInstruct("MULTI:2:2:1208:272:154:648");
    changeToAndroidInstruct("MULTI:2:2:1208:272:165:653");
    changeToAndroidInstruct("MULTI:2:2:1205:272:165:653");
    changeToAndroidInstruct("MULTI:2:2:1202:272:165:653");
    changeToAndroidInstruct("MULTI:2:2:1202:272:177:650");
    changeToAndroidInstruct("MULTI:2:2:1198:272:177:650");
    changeToAndroidInstruct("MULTI:2:2:1196:272:177:650");
    changeToAndroidInstruct("MULTI:2:2:1196:272:189:647");
    changeToAndroidInstruct("MULTI:2:2:1192:272:189:647");
    changeToAndroidInstruct("MULTI:2:2:1189:272:189:647");
    changeToAndroidInstruct("MULTI:2:2:1189:272:201:645");
    changeToAndroidInstruct("MULTI:2:2:1185:272:201:645");
    changeToAndroidInstruct("MULTI:2:2:1185:272:213:643");
    changeToAndroidInstruct("MULTI:2:2:1181:272:213:643");
    changeToAndroidInstruct("MULTI:2:2:1175:272:213:643");
    changeToAndroidInstruct("MULTI:2:2:1175:272:225:641");
    changeToAndroidInstruct("MULTI:2:2:1173:272:225:641");
    changeToAndroidInstruct("MULTI:2:2:1170:273:225:641");
    changeToAndroidInstruct("MULTI:2:2:1170:273:237:639");
    changeToAndroidInstruct("MULTI:2:2:1169:273:237:639");
    changeToAndroidInstruct("MULTI:2:2:1168:273:237:639");
    changeToAndroidInstruct("MULTI:2:2:1168:273:249:637");
    changeToAndroidInstruct("MULTI:2:2:1165:274:249:637");
    changeToAndroidInstruct("MULTI:2:2:1164:274:249:637");
    changeToAndroidInstruct("MULTI:2:2:1164:274:238:644");
    changeToAndroidInstruct("MULTI:2:2:1161:275:238:644");
    changeToAndroidInstruct("MULTI:2:2:1159:275:238:644");
    changeToAndroidInstruct("MULTI:2:2:1159:275:227:651");
    changeToAndroidInstruct("MULTI:2:2:1158:275:227:651");
    changeToAndroidInstruct("MULTI:2:2:1155:277:227:651");
    changeToAndroidInstruct("MULTI:2:2:1155:277:216:658");
    changeToAndroidInstruct("MULTI:2:2:1153:278:216:658");
    changeToAndroidInstruct("MULTI:2:2:1152:278:216:658");
    changeToAndroidInstruct("MULTI:2:2:1152:278:206:666");
    changeToAndroidInstruct("MULTI:2:2:1150:279:206:666");
    changeToAndroidInstruct("MULTI:2:2:1149:280:206:666");
    changeToAndroidInstruct("MULTI:2:2:1149:280:203:669");
    changeToAndroidInstruct("MULTI:2:2:1148:281:203:669");
    changeToAndroidInstruct("MULTI:2:2:1146:281:203:669");
    changeToAndroidInstruct("MULTI:2:2:1144:282:203:669");
    changeToAndroidInstruct("MULTI:2:2:1143:282:203:669");
    changeToAndroidInstruct("MULTI:2:2:1136:285:203:669");
    changeToAndroidInstruct("MULTI:2:2:1134:286:203:669");
    changeToAndroidInstruct("MULTI:2:2:1130:286:203:669");
    changeToAndroidInstruct("MULTI:2:2:1126:287:203:669");
    changeToAndroidInstruct("MULTI:2:2:1122:289:203:669");
    changeToAndroidInstruct("MULTI:2:2:1119:290:203:669");
    changeToAndroidInstruct("MULTI:2:2:1117:291:203:669");
    changeToAndroidInstruct("MULTI:2:2:1115:291:203:669");
    changeToAndroidInstruct("MULTI:2:2:1114:291:203:669");
    changeToAndroidInstruct("MULTI:2:2:1112:291:203:669");
    changeToAndroidInstruct("MULTI:2:2:1111:291:203:669");
    changeToAndroidInstruct("MULTI:2:2:1111:291:214:664");
    changeToAndroidInstruct("MULTI:2:2:1109:291:214:664");
    changeToAndroidInstruct("MULTI:2:2:1107:291:214:664");
    changeToAndroidInstruct("MULTI:2:2:1107:291:225:659");
    changeToAndroidInstruct("MULTI:2:2:1105:293:225:659");
    changeToAndroidInstruct("MULTI:2:2:1105:293:236:654");
    changeToAndroidInstruct("MULTI:2:2:1104:293:236:654");
    changeToAndroidInstruct("MULTI:2:2:1104:293:247:649");
    changeToAndroidInstruct("MULTI:2:2:1103:293:247:649");
    changeToAndroidInstruct("MULTI:2:2:1103:293:258:644");
    changeToAndroidInstruct("MULTI:2:2:1102:293:258:644");
    changeToAndroidInstruct("MULTI:2:2:1099:294:258:644");
    changeToAndroidInstruct("MULTI:2:2:1099:294:269:639");
    changeToAndroidInstruct("MULTI:2:2:1096:294:269:639");
    changeToAndroidInstruct("MULTI:2:2:1092:295:269:639");
    changeToAndroidInstruct("MULTI:2:2:1092:295:275:628");
    changeToAndroidInstruct("MULTI:2:2:1088:295:275:628");
    changeToAndroidInstruct("MULTI:2:2:1085:297:275:628");
    changeToAndroidInstruct("MULTI:2:2:1085:297:281:617");
    changeToAndroidInstruct("MULTI:2:2:1083:297:281:617");
    changeToAndroidInstruct("MULTI:2:2:1082:297:281:617");
    changeToAndroidInstruct("MULTI:2:2:1082:297:288:606");
    changeToAndroidInstruct("MULTI:2:2:1082:297:295:595");
    changeToAndroidInstruct("MULTI:2:2:1082:297:302:584");
    changeToAndroidInstruct("MULTI:2:2:1082:297:309:573");
    changeToAndroidInstruct("MULTI:2:2:1082:297:316:562");
    changeToAndroidInstruct("MULTI:1:2:1082:297");
    changeToAndroidInstruct("MULTI:1:2:1081:297");
    changeToAndroidInstruct("MULTI:0:1");
    changeToAndroidInstruct("MULTI:1:0:994:222");
    changeToAndroidInstruct("MULTI:0:1");




    return 0;
}

/*
struct input_event {
//WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS 
  struct timeval time;
  __u16 type;
  __u16 code;
  __s32 value;
// WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS 
};
*/

int main22(int argc, char *argv[])
{
    int fd;
    ssize_t ret;
    int version;
    struct input_event event;

    if(argc != 5) {
        fprintf(stderr, "use: %s device type code value\n", argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDWR);
    if(fd < 0) {
        fprintf(stderr, "could not open %s, %s\n", argv[optind], strerror(errno));
        return 1;
    }
    if (ioctl(fd, EVIOCGVERSION, &version)) {
        fprintf(stderr, "could not get driver version for %s, %s\n", argv[optind], strerror(errno));
        return 1;
    }
    memset(&event, 0, sizeof(event));
    event.type = atoi(argv[2]);
    event.code = atoi(argv[3]);
    event.value = atoi(argv[4]);
    ret = write(fd, &event, sizeof(event));
    if(ret < (ssize_t) sizeof(event)) {
        fprintf(stderr, "write event failed, %s\n", strerror(errno));
        return -1;
    }
    return 0;
}