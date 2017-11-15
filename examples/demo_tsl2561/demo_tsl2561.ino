//
// 获取传感器TSL2561光照值
//

#include <TSL2561.h>

TSL2561 tsl(TSL2561_ADDR);

// 传感器相关变量
uint16_t integrationTime;
double il;
uint32_t il_int;
bool autoGainOn;

// 执行控制变量
boolean operational;

//状态变量
char tsl_sta[21] = "undef";
char AutoGainDisplay[4] = "";
uint8_t error_code;
unsigned int gainDisplay;
unsigned int integrationTimeDisplay;

void setup()
{
    error_code =0;
    operational = false;
    autoGainOn = false;

    // 连接到光传感器
    if (tsl.begin()) {
        strcpy(tsl_sta,"found");
    }
    else {
        strcpy(tsl_sta,"not found");
        return;
    }

    // 设置传感器信息: gain x1 和 101ms integration time
    if(!tsl.setTiming(false,1,integrationTime))
    {
        error_code = tsl.getError();
        strcpy(tsl_sta,"setTimingError");
        return;
    }

    // 检测是否启动
    if (!tsl.setPowerUp())
    {
        error_code = tsl.getError();
        strcpy(tsl_sta,"PowerUPError");
        return;
    }

    // 设备初始化
    operational = true;
    strcpy(tsl_sta,"initOK");

}

void loop()
{
    uint16_t broadband, ir;

    // 更新曝光设置显示值
    if (tsl._gain)
        gainDisplay = 16;
    else
        gainDisplay = 1;

    if (autoGainOn)
        strcpy(AutoGainDisplay,"yes");
    else
        strcpy(AutoGainDisplay,"no");

    integrationTimeDisplay = integrationTime & 0x0000FFFF;

    if (operational)
    {
        // device operational, update status vars
        strcpy(tsl_sta,"OK");

        // 得到传感器未处理的值
        if(!tsl.getData(broadband,ir,autoGainOn))
        {
            error_code = tsl.getError();
            strcpy(tsl_sta,"saturated?");
            operational = false;
        }



        // 计算得到单位为lux的光照值
        if(!tsl.getLux(integrationTime,broadband,ir,il))
        {
            error_code = tsl.getError();
            strcpy(tsl_sta,"getLuxError");
            operational = false;
        }

        // 得到光照整数值
        if(!tsl.getLuxInt(broadband,ir,il_int))
        {
            error_code = tsl.getError();
            strcpy(tsl_sta,"getLuxIntError");
            operational = false;
        }

    }
    else
        // 设备出错
    {
        strcpy(tsl_sta,"OperationError");
        il = -1.0;
        // 尝试修复
        // 关掉电源
        tsl.setPowerDown();
        delay(100);
        // 重新初始化传感器
        if (tsl.begin())
        {
            // 上电启动
            tsl.setPowerUp();
            // 重新配置
            tsl.setTiming(tsl._gain,1,integrationTime);
            // 恢复到正常状态
            operational = true;
        }
    }

    delay(1000);
}
