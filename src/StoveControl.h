#pragma once
#include "OpenKNX.h"

class StoveControl : public OpenKNX::Module
{
  private:
    void loopStoveTemperatures(float T1,float T2 ,float T3);
    void loopStoveValues(uint16_t STATUS, uint16_t SERVICETIMEh, uint16_t SERVICETIMEm, byte PWR, float setPoint, uint16_t PQT);
    void toggleStove();
    void NewSetpoint();
    void NewPWR();
    bool SetupStove();
    void loopStove();
    float T1 = 0.0;  // Ambiant temperature
    float T2 = 0.0;  // pellet temperature
    float T3 = 0.0;  // Fumes temperature 
    float T4 = 0.0;
    float T5 = 0.0;
    float setPoint;  // target room temperature
    uint16_t PQT;
    uint16_t STATUS;    // Stove On/OFF
    uint16_t LSTATUS;   
    uint16_t FSTATUS;
    byte PWR = 5;
    float FDR;
    float SETPReturn;
    uint16_t IGN;
    uint16_t POWERTIMEh;
    uint16_t POWERTIMEm;
    uint16_t HEATTIMEh;
    uint16_t HEATTIMEm;
    uint16_t SERVICETIMEh;
    uint16_t SERVICETIMEm;
    uint16_t SERVICETIME;
    uint16_t ONTIMEh;
    uint16_t ONTIMEm;
    uint16_t OVERTMPERRORS;
    uint16_t IGNERRORS;
    uint16_t DP_TARGET; 
    uint16_t DP_PRESS;
    //bool m_input0 = 0;
    //bool m_input1 = 0;
    //bool m_input2 = 0;
    //bool m_input3 = 0;
    //unsigned long _timeout = 200;
    //unsigned long lastUpdate =-1;
    #define UPDATEINTERVAL 30000
    // Timer variables
    unsigned long lastTimeWebserver = 0;
    unsigned long timerDelayWebserver = 30000;
   
    uint32_t _input0_debounce_millis = 0; // Internal Input
    uint32_t _input0_last_send_millis = 0; // Internal Input
   
    uint32_t _temperatureT1_last_send_millis = 0;
    float _temperatureT1_last_send_value = -1000;

    uint32_t _temperatureT2_last_send_millis = 0;
    float _temperatureT2_last_send_value = -1000;

    uint32_t _temperatureT3_last_send_millis = 0;
    float _temperatureT3_last_send_value = -1000;

    uint32_t _Status_last_send_millis = 0;
    uint16_t _Status_last_send_value = -1000;

    uint32_t _setPoint_last_send_millis = 0;
    float _setPoint_last_send_value = -1000;

    uint32_t _StoveState_last_send_millis = 0;
    float _StoveState_last_send_value = 0;

    uint32_t _PQT_last_send_millis = 0;
    float _PQT_last_send_value = 0;

    uint32_t _ServiceTime_last_send_millis = 0;
    float _ServiceTime_last_send_value = 0;

    uint32_t _Stove_last_run = 0;

    uint32_t _PWR_last_send_millis = 0;
    byte _PWR_last_send_value = 0;

    bool _StovePower_last_send_value = false;
    bool StovePower = false;

    float _NewStoveSetpoint_last_send_value = 0;

   
  public:
    StoveControl();
    //void loop() override;
    void loop(bool configured) override;
    void loop1(bool configured) override;
    void setup(bool configured) override;
    void setup1(bool configured) override;
    //void setup() override;
    const std::string name() override;
    const std::string version() override;
    void processInputKo(GroupObject &ko) override;
   
};

extern StoveControl openknxStoveControlModule;