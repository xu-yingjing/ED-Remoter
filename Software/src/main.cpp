#define BLINKER_WIFI
#define BLINKER_ESP_SMARTCONFIG

#include <Blinker.h>
#include <OneButton.h>

#define EDREMOTE_DBG    1               /* 串口编译调试开关 */
#define BLINKER_AUTH    "861129fc28e0"  /* Blinker设备密钥 */
#define RESET_PIN       10              /* 重置引脚 */
#define RESET_ACT       LOW             /* 重置引脚有效电平 */
#define CONNECT_LED_PIN 0               /* WiFi连接状态指示灯引脚 */
#define CONNECT_LED_ACT LOW             /* WiFi连接状态指示灯有效电平 */
#define WAIT_LED_PIN    1               /* 等待SmartConfig指示灯引脚 */
#define WAIT_LED_ACT    LOW             /* 等待SmartConfig指示灯有效电平 */
#define REMOTE_K1_PIN   7               /* 遥控器K1引脚 */
#define REMOTE_K1_ACT   HIGH            /* 遥控器K1有效电平 */
#define REMOTE_K2_PIN   6               /* 遥控器K2引脚 */
#define REMOTE_K2_ACT   HIGH            /* 遥控器K2有效电平 */
#define REMOTE_K3_PIN   5               /* 遥控器K3引脚 */
#define REMOTE_K3_ACT   HIGH            /* 遥控器K3有效电平 */
#define REMOTE_K4_PIN   4               /* 遥控器K4引脚 */
#define REMOTE_K4_ACT   HIGH            /* 遥控器K4有效电平 */

/* 重置按钮 */
static OneButton resetButton(RESET_PIN, (RESET_ACT == LOW) ? true : false);

/* Blinker按钮 */
static BlinkerButton buttonK1("btn-K1");
static BlinkerButton buttonK2("btn-K2");
static BlinkerButton buttonK3("btn-K3");
static BlinkerButton buttonK4("btn-K4");

/* 重置按钮长按回调 */
static void deviceReset(void)
{
    Blinker.reset();
}

#if (EDREMOTE_DBG != 0)
/* Blinker数据回调 */
static void blinkerDataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);
}
#endif

/* WiFi连接状态检测任务 */
static void wifiCheckTask(void *pvParameters)
{
    /* 初始化WiFi连接状态指示灯 */
    pinMode(CONNECT_LED_PIN, OUTPUT);
    digitalWrite(CONNECT_LED_PIN, !CONNECT_LED_ACT);

    /* 初始化等待SmartConfig指示灯 */
    pinMode(WAIT_LED_PIN, OUTPUT);
    digitalWrite(WAIT_LED_PIN, !WAIT_LED_ACT);

    /* 检测WiFi连接状态 */
    for (;;)
    {
        /* WiFi已连接 */
        if (WiFi.status() == WL_CONNECTED)
        {
            digitalWrite(CONNECT_LED_PIN, CONNECT_LED_ACT);
            digitalWrite(WAIT_LED_PIN, !WAIT_LED_ACT);
        }
        /* WiFi未连接 */
        else
        {
            digitalWrite(CONNECT_LED_PIN, !CONNECT_LED_ACT);
            digitalWrite(WAIT_LED_PIN, !digitalRead(WAIT_LED_PIN));
        }
        delay(200);
    }
}

/* Blinker按钮回调函数 */
static void buttonK1Callback(const String & state)
{
#if (EDREMOTE_DBG != 0)
    BLINKER_LOG("buttonK1 get button state: ", state);
#endif

    if (state == BLINKER_CMD_BUTTON_TAP)
    {
        digitalWrite(REMOTE_K1_PIN, REMOTE_K1_ACT);
        digitalWrite(CONNECT_LED_PIN, !CONNECT_LED_ACT);
        delay(250);
        digitalWrite(CONNECT_LED_PIN, !CONNECT_LED_ACT);
        delay(250);
        digitalWrite(REMOTE_K1_PIN, !REMOTE_K1_ACT);
    }
}
static void buttonK2Callback(const String & state)
{
#if (EDREMOTE_DBG != 0)
    BLINKER_LOG("buttonK2 get button state: ", state);
#endif

    if (state == BLINKER_CMD_BUTTON_TAP)
    {
        digitalWrite(REMOTE_K2_PIN, REMOTE_K2_ACT);
        digitalWrite(CONNECT_LED_PIN, !CONNECT_LED_ACT);
        delay(250);
        digitalWrite(CONNECT_LED_PIN, !CONNECT_LED_ACT);
        delay(250);
        digitalWrite(REMOTE_K2_PIN, !REMOTE_K2_ACT);
    }
}
static void buttonK3Callback(const String & state)
{
#if (EDREMOTE_DBG != 0)
    BLINKER_LOG("buttonK3 get button state: ", state);
#endif

    if (state == BLINKER_CMD_BUTTON_TAP)
    {
        digitalWrite(REMOTE_K3_PIN, REMOTE_K3_ACT);
        digitalWrite(CONNECT_LED_PIN, !CONNECT_LED_ACT);
        delay(250);
        digitalWrite(CONNECT_LED_PIN, !CONNECT_LED_ACT);
        delay(250);
        digitalWrite(REMOTE_K3_PIN, !REMOTE_K3_ACT);
    }
}
static void buttonK4Callback(const String & state)
{
#if (EDREMOTE_DBG != 0)
    BLINKER_LOG("buttonK4 get button state: ", state);
#endif

    if (state == BLINKER_CMD_BUTTON_TAP)
    {
        digitalWrite(REMOTE_K4_PIN, REMOTE_K4_ACT);
        digitalWrite(CONNECT_LED_PIN, !CONNECT_LED_ACT);
        delay(250);
        digitalWrite(CONNECT_LED_PIN, !CONNECT_LED_ACT);
        delay(250);
        digitalWrite(REMOTE_K4_PIN, !REMOTE_K4_ACT);
    }
}

void setup() {
#if (EDREMOTE_DBG != 0)
    /* 初始化Serial并开启Blinker调试模式 */
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);
#endif

    /* 初始化遥控器K1~K4 */
    pinMode(REMOTE_K1_PIN, OUTPUT);
    digitalWrite(REMOTE_K1_PIN, !REMOTE_K1_ACT);
    pinMode(REMOTE_K2_PIN, OUTPUT);
    digitalWrite(REMOTE_K2_PIN, !REMOTE_K2_ACT);
    pinMode(REMOTE_K3_PIN, OUTPUT);
    digitalWrite(REMOTE_K3_PIN, !REMOTE_K3_ACT);
    pinMode(REMOTE_K4_PIN, OUTPUT);
    digitalWrite(REMOTE_K4_PIN, !REMOTE_K4_ACT);

    /* 创建WiFi状态检测任务 */
    xTaskCreateUniversal(&wifiCheckTask, "WiFi Check Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL, ARDUINO_RUNNING_CORE);

    /* Blinker初始化 */
    Blinker.begin(BLINKER_AUTH);
#if (EDREMOTE_DBG != 0)
    /* 注册Blinker数据回调函数 */
    Blinker.attachData(blinkerDataRead);
#endif
    /* 注册重置按钮长按回调函数 */
    resetButton.attachLongPressStop(deviceReset);
    /* 注册Blinker按钮回调函数 */
    buttonK1.attach(buttonK1Callback);
    buttonK2.attach(buttonK2Callback);
    buttonK3.attach(buttonK3Callback);
    buttonK4.attach(buttonK4Callback);
}

void loop() {
    Blinker.run();
    resetButton.tick();
}
