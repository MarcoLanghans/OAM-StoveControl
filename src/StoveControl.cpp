#include "StoveControl.h"
#include <functional>
#include "Palazzetti.h"
#include <Arduino.h>


// Serial is used for log (baudrate=38400)
// Serial2 is used for stove communication

// RP2040 Pins
// GPIO8 (TX)   /   PIN11
// GPIO9 (RX)   /   PIN12
// GND          /   PIN38
// 3V3          /   PIN36
StoveControl::StoveControl(){}

Palazzetti myPala;

// Serial management functions
int myOpenSerial(uint32_t baudrate)
{
    Serial2.setRX(STOVE_UART_RX_PIN);
    Serial2.setTX(STOVE_UART_TX_PIN);
    Serial2.begin(baudrate,SERIAL_8N1);
    return 0;
}
void myCloseSerial()
{
    Serial2.end();
    // set TX PIN to OUTPUT HIGH to avoid stove bus blocking
    pinMode(STOVE_UART_TX_PIN, OUTPUT);
    digitalWrite(STOVE_UART_TX_PIN, HIGH);
}
int mySelectSerial(unsigned long timeout)
{
    size_t avail;
    unsigned long startmillis = millis();
    while ((avail = Serial2.available()) == 0 && (startmillis + timeout) > millis())
        ;

    return avail;
}

size_t myReadSerial(void *buf, size_t count){
    return Serial2.readBytes((char *)buf, count);

}
size_t myWriteSerial(const void *buf, size_t count)
{ 
    return Serial2.write((const uint8_t *)buf, count); 
}
int myDrainSerial()
{
    Serial2.flush(); // On ESP, Serial2.flush() is drain
    return 0;
}
int myFlushSerial()
{
    Serial2.flush();
    while (Serial2.read() != -1)
        ; // flush RX buffer
    return 0;
}
void myUSleep(unsigned long usecond) { delayMicroseconds(usecond); }

const std::string StoveControl::name(){
    return "StoveControl";
}

const std::string StoveControl::version(){
    // hides the module in the version output on the console, because the firmware version is sufficient.
    return "";
}

void StoveControl::setup(bool configured)
{
    
}

void StoveControl::setup1(bool configured)
{
  if(SetupStove())
    Serial.println("initialize OK");
  else
    Serial.println("initialize Failed");
}

void StoveControl::loop1(bool configured){
  if(configured){   
    loopStove();
  } 
}

void StoveControl::loop(bool configured)
{ 
  if(configured){
    loopStoveTemperatures(T1,T2,T3);
    loopStoveValues(STATUS, SERVICETIMEh, SERVICETIMEm, HEATTIMEh, HEATTIMEm, POWERTIMEh, POWERTIMEm, ONTIMEh, ONTIMEm, PWR, setPoint, PQT, DP_PRESS, IGN, FDR, OVERTMPERRORS, IGNERRORS);
    toggleStove(); 
    NewSetpoint();
    NewPWR();  
  }
}

bool StoveControl::SetupStove(){ 
    bool state = (myPala.initialize(myOpenSerial, myCloseSerial, mySelectSerial, myReadSerial, myWriteSerial, myDrainSerial, myFlushSerial, myUSleep) == Palazzetti::CommandResult::OK);
    return state;
}

void StoveControl::loopStove(){

  uint32_t send_millisStove = 30000; //60000; 
  bool StoveCycle = false;

  StoveCycle = millis() - _Stove_last_run > send_millisStove || _Stove_last_run == 0;
  
  //toggleStove();
  if(StoveCycle){ 
    logDebugP("Get Values from stove");
    StoveBlocked = true;
    myPala.getStatus(&STATUS, &LSTATUS, &FSTATUS);
    logDebugP("myPala.GetStatus : STATUS=%d LSTATUS=%d FSTATUS=%d\r\n", STATUS, LSTATUS, FSTATUS);
    myPala.getSetPoint(&setPoint);
    //myPala.getPelletQtUsed(&PQT);
    myPala.getPower(&PWR, &FDR);
    myPala.getCounters( &IGN,  &POWERTIMEh,  &POWERTIMEm,  &HEATTIMEh,  &HEATTIMEm, &SERVICETIMEh,  &SERVICETIMEm,  &ONTIMEh, &ONTIMEm, &OVERTMPERRORS, &IGNERRORS, &PQT);
    myPala.getDPressData(&DP_TARGET, &DP_PRESS);
    

    #ifdef Serial_Debug
      logDebugP("-----------------");
      logDebugP("Debug Values uint16_t");
      logDebugP("SERVICETIMEh=%d",SERVICETIMEh);
      logDebugP("SERVICETIMEm=%d",SERVICETIMEm);
      logDebugP("POWERTIMEh=%d",POWERTIMEh);
      logDebugP("POWERTIMEm=%d",POWERTIMEm);
      logDebugP("HEATTIMEh=%d",HEATTIMEh);
      logDebugP("HEATTIMEm=%d",HEATTIMEm);
      logDebugP("ONTIMEm=%d",ONTIMEh);
      logDebugP("ONTIMEs=%d",ONTIMEm);
      logDebugP("OVERTMPERRORS=%d",OVERTMPERRORS);
      logDebugP("IGNERRORS=%d",IGNERRORS);
      logDebugP("FDR=%f",FDR);
      logDebugP("PWR=%d",PWR);
      logDebugP("PQT=%d",PQT);
      logDebugP("DP_TARGET=%d",DP_TARGET);
      logDebugP("DP_PRESS=%d",DP_PRESS);
      logDebugP("IGN=%d",IGN);   // IGN --> 389
      //Serial.println("ServiceTime"); 
      //Serial.println(SERVICETIMEm);   // 20 Minuten  
      //Serial.println(SERVICETIMEh);   // 888 Stunden 
      //Serial.println(PWR);            // 3      2
      //Serial.println(PQT);            // 65535
      logDebugP("-----------------");
    #endif

    if(STATUS >= 0){   // 0 = off , 6 On,  10 Switch Off
      myPala.getAllTemps(&T1, &T2, &T3, &T4, &T5);
      #ifdef Serial_Debug  
        logDebugP("myPala.getSetPoint : T1=%.2f\r\n", setPoint);
        logDebugP("myPala.getAllTemps : T1=%.2f T2=%.2f T3=%.2f T4=%.2f T5=%.2f\r\n", T1, T2, T3, T4, T5);
        logDebugP("myPala.GetStatus : STATUS=%d LSTATUS=%d\r\n", STATUS, LSTATUS);
      #endif
    }
    else{
      T1 = 0.0;  
      T2 = 0.0;
      T3 = 0.0;
      T4 = 0.0;
      T5 = 0.0;
      STATUS = 0;
    }
  _Stove_last_run = millis();
  StoveBlocked = false;
  } 
}

void StoveControl::loopStoveTemperatures(float T1,float T2 ,float T3){

  // Ambient temperatur
  if(!isnan(T1)){
    
    uint8_t send_cycle1 = ParamSEN_Temperature1SendCycle;
    uint32_t send_millis1 = send_cycle1 * 60000;
    bool sendnow1 = false;

    if(send_cycle1){
      sendnow1 = millis() - _temperatureT1_last_send_millis > send_millis1 || _temperatureT1_last_send_millis == 0;
    }

    if(!sendnow1){
      float SendTresh1 = ParamSEN_Temperature1SendChangeAmount;
      if(SendTresh1 != 0){
        float current_temp_diffT1 = T1 - _temperatureT1_last_send_value;
        sendnow1 = current_temp_diffT1 >= SendTresh1 || 0 - current_temp_diffT1 >= SendTresh1;
        if(sendnow1){
          logDebugP("Send temperatur T1 KO Diff: %f", current_temp_diffT1);
        }
      }
    }

    if(sendnow1){
      KoSEN_AmbientTemperature.value(T1, DPT_Value_Temp);
      logDebugP("Send temperatur T1 KO: %f", T1 );
      _temperatureT1_last_send_millis = millis();
      _temperatureT1_last_send_value = T1;
    }
    else{
      KoSEN_AmbientTemperature.valueNoSend(T1, DPT_Value_Temp);
    }

  }

  // Pellet temperature
  if(!isnan(T2)){
    uint8_t send_cycle2 = ParamSEN_Temperature2SendCycle;
    uint32_t send_millis2 = send_cycle2 * 60000;
    bool sendnow2 = false;

    if(send_cycle2){
      sendnow2 = millis() - _temperatureT2_last_send_millis > send_millis2 || _temperatureT2_last_send_millis == 0;
    }

    if(!sendnow2){
      float SendTresh2 = ParamSEN_Temperature2SendChangeAmount;
      if(SendTresh2 != 0){
        float current_temp_diffT2 = T2 - _temperatureT2_last_send_value;
        sendnow2 = current_temp_diffT2 >= SendTresh2 || 0 - current_temp_diffT2 >= SendTresh2;
        if(sendnow2){
          logDebugP("Send temperatur T2 KO Diff: %f", current_temp_diffT2);
        }
      }
    }

    if(sendnow2){
        KoSEN_PelletsTemperature.value(T2, DPT_Value_Temp);
        logDebugP("Send temperatur T2 KO: %f", T2 );
        _temperatureT2_last_send_millis = millis();
        _temperatureT2_last_send_value = T2;
    }
    else{
        KoSEN_PelletsTemperature.valueNoSend(T2, DPT_Value_Temp);
    }

  }

  // Fumes temperature
  if(!isnan(T3)){
    uint8_t send_cycle3 = ParamSEN_Temperature3SendCycle;
    uint32_t send_millis3 = send_cycle3 * 60000;
    bool sendnow3 = false;

    if(send_cycle3){
      sendnow3 = millis() - _temperatureT3_last_send_millis > send_millis3 || _temperatureT3_last_send_millis == 0;
    }

    if(!sendnow3){
      float SendTresh3 = ParamSEN_Temperature3SendChangeAmount;
      if(SendTresh3 != 0){
        float current_temp_diffT3 = T3 - _temperatureT3_last_send_value;
        sendnow3 = current_temp_diffT3 >= SendTresh3 || 0 - current_temp_diffT3 >= SendTresh3;
        if(sendnow3){
          logDebugP("Send temperatur T3 KO Diff: %f", current_temp_diffT3);
        }
      }
    }

    if(sendnow3){
      KoSEN_FumesTemperatur.value(T3, DPT_Value_Temp);
      logDebugP("Send temperatur T3 KO: %f", T3 );
      _temperatureT3_last_send_millis = millis();
      _temperatureT3_last_send_value = T3;
    }
    else{
      KoSEN_FumesTemperatur.valueNoSend(T3, DPT_Value_Temp);
    }

  }
}

void StoveControl::loopStoveValues(uint16_t STATUS, uint16_t SERVICETIMEh, uint16_t SERVICETIMEm, uint16_t HEATTIMEh, uint16_t HEATTIMEm, uint16_t POWERTIMEh, uint16_t POWERTIMEm, uint16_t ONTIMEh, uint16_t ONTIMEm, byte PWR, float setPoint, uint16_t PQT, uint16_t DP_PRESS, uint16_t IGN, float FDR, uint16_t OVERTMPERRORS, uint16_t IGNERRORS){

  // State
  if(!isnan(STATUS)){
    uint8_t send_cycleStoveState = ParamSEN_StoveStateSendCycle;
    uint32_t send_millisStoveState = send_cycleStoveState * 60000; 
    bool sendnowSToveState = false;
    uint16_t _STATUS = 0;
    
    if(send_cycleStoveState){
      sendnowSToveState = millis() - _StoveState_last_send_millis > send_millisStoveState || _StoveState_last_send_millis == 0;
    }
  
    if(STATUS > 0){_STATUS = 1;}
    if(sendnowSToveState){
        KoSEN_StoveState.value(_STATUS, DPT_State);
        logDebugP("Send stove state KO: %d", _STATUS);
        _StoveState_last_send_millis = millis();
        _StoveState_last_send_value = _STATUS;
    }
    else{
      KoSEN_StoveState.valueNoSend(_STATUS, DPT_State);}
  }
  
  // ServiceTime
  // Umrechnung auf Minuten
  SERVICETIME = SERVICETIMEh; 
  if(!isnan(SERVICETIME)){

    uint8_t send_cycleServiceTime = ParamSEN_MaintenanceTimerSendCycle;
    uint32_t send_millisServiceTime = send_cycleServiceTime * 60000; 
    bool sendnowServiceTime = false;

    if(send_cycleServiceTime){
      sendnowServiceTime = millis() - _ServiceTime_last_send_millis > send_millisServiceTime || _ServiceTime_last_send_millis == 0;
    }
    
    if(sendnowServiceTime){
        KoSEN_ServiceTime.value(SERVICETIME, DPT_TimePeriodHrs);
        logDebugP("Send service time KO: %d", SERVICETIME );
        _ServiceTime_last_send_millis = millis();
        _ServiceTime_last_send_value = SERVICETIME;
    }
    else{KoSEN_ServiceTime.valueNoSend(SERVICETIME, DPT_TimePeriodHrs);}
  }

  // POWERTIME
  // Umrechnung auf Minuten
  POWERTIME = POWERTIMEh; 
  if(!isnan(POWERTIME)){

    uint8_t send_cyclePowerTime = ParamSEN_PowerTimerSendCycle;
    uint32_t send_millisPowerTime = send_cyclePowerTime * 60000; 
    bool sendnowPowerTime = false;

    if(send_cyclePowerTime){
      sendnowPowerTime = millis() - _PowerTime_last_send_millis > send_millisPowerTime || _PowerTime_last_send_millis == 0;
    }
    
    if(sendnowPowerTime){
      KoSEN_PowerTime.value(POWERTIME, DPT_TimePeriodHrs);
        logDebugP("Send power time KO: %d", POWERTIME );
        _PowerTime_last_send_millis = millis();
        _PowerTime_last_send_value = POWERTIME;
    }
    else{KoSEN_PowerTime.valueNoSend(POWERTIME, DPT_TimePeriodHrs);}
  }

  // HEATTIME
  // Umrechnung auf Minuten
  HEATTIME = HEATTIMEh;
  if(!isnan(HEATTIME)){

    uint8_t send_cycleHeatTime = ParamSEN_HeatTimerSendCycle;
    uint32_t send_millisHeatTime = send_cycleHeatTime * 60000; 
    bool sendnowHeatTime = false;

    if(send_cycleHeatTime){
      sendnowHeatTime = millis() - _HeatTime_last_send_millis > send_millisHeatTime || _HeatTime_last_send_millis == 0;
    }
    
    if(sendnowHeatTime){
      KoSEN_HeatTime.value(HEATTIME, DPT_TimePeriodHrs);
        logDebugP("Send heat time KO: %d", HEATTIME );
        _HeatTime_last_send_millis = millis();
        _HeatTime_last_send_value = HEATTIME;
    }
    else{KoSEN_HeatTime.valueNoSend(HEATTIME, DPT_TimePeriodHrs);}
  }

  // ONTIMEh
  // Umrechnung auf Minuten
  ONTIME = ONTIMEh; 
  if(!isnan(ONTIME)){

    uint8_t send_cycleOnTime = ParamSEN_OnTimerSendCycle;
    uint32_t send_millisOnTime = send_cycleOnTime * 60000; 
    bool sendnowOnTime = false;

    if(send_cycleOnTime){
      sendnowOnTime = millis() - _OnTime_last_send_millis > send_millisOnTime || _OnTime_last_send_millis == 0;
    }
    
    if(sendnowOnTime){
      KoSEN_OnTime.value(ONTIME, DPT_TimePeriodMin);
        logDebugP("Send on time KO: %d", ONTIME );
        _OnTime_last_send_millis = millis();
        _OnTime_last_send_value = ONTIME;
    }
    else{KoSEN_OnTime.valueNoSend(ONTIME, DPT_TimePeriodMin);}
  }

  // Pellets used
  if(!isnan(PQT)){

    uint8_t send_cyclePQT = ParamSEN_PelletConsumptionSendCycle;
    uint32_t send_millisPQT = send_cyclePQT * 60000; 
    bool sendnowPQT = false;

    if(send_cyclePQT){
      sendnowPQT = millis() - _PQT_last_send_millis > send_millisPQT || _PQT_last_send_millis == 0;
    }
    
    if(sendnowPQT){
        KoSEN_PelletsUsed.value(PQT, DPT_Value_Mass);
        logDebugP("Send pellets used KO: %f", PQT );
        _PQT_last_send_millis = millis();
        _PQT_last_send_value = PQT;
    }
    else{KoSEN_PelletsUsed.valueNoSend(PQT, DPT_Value_Mass);}
  }

  // Feeder used
  if(!isnan(FDR)){

    uint8_t send_cycleFDR = ParamSEN_FeederSendCycle;
    uint32_t send_millisFDR = send_cycleFDR * 60000; 
    bool sendnowFDR = false;

    if(send_cycleFDR){
      sendnowFDR = millis() - _FDR_last_send_millis > send_millisFDR || _FDR_last_send_millis == 0;
    }
    
    if(sendnowFDR){
      KoSEN_Feeder.value(FDR, DPT_Value_Volume_Flow);
        logDebugP("Send feeder KO: %f", FDR );
        _FDR_last_send_millis = millis();
        _FDR_last_send_value = FDR;
    }
    else{KoSEN_Feeder.valueNoSend(FDR, DPT_Value_Volume_Flow);}
  }

  // overtemperature errors
  if(!isnan(OVERTMPERRORS)){

    uint8_t send_cycleOVERTMPERRORS = ParamSEN_OverheatErrorsSendCycle;
    uint32_t send_millisOVERTMPERRORS = send_cycleOVERTMPERRORS * 60000; 
    bool sendnowOVERTMPERRORS = false;

    if(send_cycleOVERTMPERRORS){
      sendnowOVERTMPERRORS = millis() - _OVERTMPERRORS_last_send_millis > send_millisOVERTMPERRORS || _OVERTMPERRORS_last_send_millis == 0;
    }
    
    if(sendnowOVERTMPERRORS){
      KoSEN_OverheatErrors.value(OVERTMPERRORS, DPT_Value_4_Count);
        logDebugP("Send overtemperature errors KO: %d", OVERTMPERRORS );
        _OVERTMPERRORS_last_send_millis = millis();
        _OVERTMPERRORS_last_send_value = OVERTMPERRORS;
    }
    else{KoSEN_OverheatErrors.valueNoSend(OVERTMPERRORS, DPT_Value_4_Count);}
  }

  // ignition errors
  if(!isnan(IGNERRORS)){

    uint8_t send_cycleIGNERRORS = ParamSEN_IgnitionErrorsSendCycle;
    uint32_t send_millisIGNERRORS = send_cycleIGNERRORS * 60000; 
    bool sendnowIGNERRORS = false;

    if(send_cycleIGNERRORS){
      sendnowIGNERRORS = millis() - _IGNERRORS_last_send_millis > send_millisIGNERRORS || _IGNERRORS_last_send_millis == 0;
    }
    
    if(sendnowIGNERRORS){
        KoSEN_IgnitionsErrors.value(IGNERRORS, DPT_Value_4_Count);
        logDebugP("Send ignition errors KO: %d", IGNERRORS );
        _IGNERRORS_last_send_millis = millis();
        _IGNERRORS_last_send_value = IGNERRORS;
    }
    else{KoSEN_IgnitionsErrors.valueNoSend(IGNERRORS, DPT_Value_4_Count);}
  }

  // current Setpoint
  if(!isnan(setPoint)){

    uint8_t send_cycleSetpoint = ParamSEN_SetpointAmbienteSendCycle;
    uint32_t send_millisSetpoint = send_cycleSetpoint * 60000; 
    bool sendnowSetPoint = false;

    if(send_cycleSetpoint){
      sendnowSetPoint = millis() - _setPoint_last_send_millis > send_millisSetpoint || _setPoint_last_send_millis == 0;
    }
    
    if(sendnowSetPoint){
        KoSEN_CurrentSetpointAmbientTemperature.value(setPoint, DPT_Value_Temp);
        logDebugP("Send setPoint KO: %f", setPoint );
        _setPoint_last_send_millis = millis();
        _setPoint_last_send_value = setPoint;
    }
    else{KoSEN_CurrentSetpointAmbientTemperature.valueNoSend(setPoint, DPT_Value_Temp);}
  }
  
  // current Powerlevel
  if(!isnan(PWR)){

    uint8_t send_cyclePWR = ParamSEN_StoveLevelSendCycle;
    uint32_t send_millisPWR = send_cyclePWR * 60000; 
    bool sendnowPWR = false;

    if(send_cyclePWR){
      sendnowPWR = millis() - _PWR_last_send_millis > send_millisPWR || _PWR_last_send_millis == 0;
    }
    
    if(sendnowPWR){
        KoSEN_CurrentPowerLevel.value(PWR, DPT_DecimalFactor);
        logDebugP("Send power level KO: %d", PWR );
        _PWR_last_send_millis = millis();
        _PWR_last_send_value = PWR;
    }
    else{KoSEN_CurrentPowerLevel.valueNoSend(PWR, DPT_DecimalFactor);}
  }

  // pressure
  if(!isnan(DP_PRESS)){

    uint8_t send_cycleDP = ParamSEN_PressureSendCycle;
    uint32_t send_millisDP = send_cycleDP * 60000; 
    bool sendnowDP = false;

    // Set DP_PRESS to zero if stove is off
    if(STATUS < 1){DP_PRESS = 0;}

    if(send_cycleDP){
      sendnowDP = millis() - _DP_last_send_millis > send_millisDP || _DP_last_send_millis == 0;
    }
    
    if(sendnowDP){
        KoSEN_Pressure.value(DP_PRESS, DPT_Value_Pres);
        logDebugP("Send pressure KO: %d", DP_PRESS );
        _DP_last_send_millis = millis();
        _DP_last_send_value = DP_PRESS;
    }
    else{KoSEN_Pressure.valueNoSend(DP_PRESS, DPT_Value_Pres);}
  }

  // ignitions 
  if(!isnan(IGN)){

    uint8_t send_cycleIGN = ParamSEN_IgnitionsSendCycle;
    uint32_t send_millisIGN = send_cycleIGN * 60000; 
    bool sendnowIGN = false;
    
    if(send_cycleIGN){
      sendnowIGN = millis() - _IGN_last_send_millis > send_millisIGN || _IGN_last_send_millis == 0;
    }
    
    if(sendnowIGN){
        KoSEN_Ignitions.value(IGN, DPT_Value_4_Count);
        logDebugP("Send ignitions KO: %d", IGN );
        _IGN_last_send_millis = millis();
        _IGN_last_send_value = IGN;
    }
    else{KoSEN_Ignitions.valueNoSend(IGN, DPT_Value_4_Count);}
  }

}

void StoveControl::toggleStove( ){
 
  StovePower = KoSEN_Stove.value(DPT_Switch);
  if(StoveBlocked == false){
    //logDebugP("Current Stove Power: %f", StovePower );
    if(_StovePower_last_send_value != StovePower){
        // STATUS = 0 stove is off
        // STATUS = 3 or 4 stove is heating up
        // STATUS = 6 stove is on
        // STATUS = 10 stove is in shutdown process
      if(StovePower == 1 && STATUS == 0){       
        #ifdef Serial_Debug
        logDebugP("-----------------");
        logDebugP("Power on Stove");
        logDebugP("-----------------");
        #endif
        myPala.switchOn(&STATUS, &LSTATUS, &FSTATUS);
      }
      if(StovePower == 0 && STATUS == 6){
        #ifdef Serial_Debug
        logDebugP("-----------------");
        logDebugP("Power off Stove");
        logDebugP("-----------------");
        #endif
        myPala.switchOff(&STATUS, &LSTATUS, &FSTATUS);
      }
      _StovePower_last_send_value = StovePower;
    }
  }
}

void StoveControl::NewSetpoint( ){

  float NewSetPoint = KoSEN_SetpointAmbientTemperature.value(DPT_Value_Temp);
  if(StoveBlocked == false){
    if(!isnan(NewSetPoint)){
      if(_NewStoveSetpoint_last_send_value != NewSetPoint){
        if(NewSetPoint > setPoint || NewSetPoint < setPoint){
          #ifdef Serial_Debug
            logDebugP("New setpoint: %f", NewSetPoint);
          #endif
          // Set new Setpoint Value
          myPala.setSetpoint(NewSetPoint, &SETPReturn);
        }
        _NewStoveSetpoint_last_send_value = NewSetPoint;
      }
    }
  }
}

void StoveControl::NewPWR(){
  
  uint16_t NewPowerlevel = KoSEN_SetPointPowerLevel.value(DPT_DecimalFactor);
  if(StoveBlocked == false){
    if(NewPowerlevel > 5){NewPowerlevel = 5;}
    if(!isnan(NewPowerlevel)){
      if(_NewPowerlevel_last_send_value != NewPowerlevel){
        if(NewPowerlevel > PWR || NewPowerlevel < PWR){
          #ifdef Serial_Debug
            logDebugP("New power level : %d", NewPowerlevel);
          #endif
          // Set new powerlevel
          myPala.setPower(NewPowerlevel);
          
        }
        _NewPowerlevel_last_send_value = NewPowerlevel;
      }
    }
  }
  //no yet implemented  
  //myPala.setPower()
  //setPower(byte powerLevel, byte *PWRReturn, bool *isF2LReturnValid, uint16_t *F2LReturn, uint16_t (*FANLMINMAXReturn)[6]
}

void StoveControl::processInputKo(GroupObject& ko){
  //logDebugP("processInputKo GA%04X", ko.asap());
  //logHexDebugP(ko.valueRef(), ko.valueSize()); 
}

StoveControl openknxStoveControlModule;