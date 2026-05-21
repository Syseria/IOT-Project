#include "A121_Breathing_Reference.h"

void A121_I2C_Write_Byte(uint16_t reg,uint32_t data)
{
  Wire1.beginTransmission(A121_DEV_ADDR);
  Wire1.write((reg >> 8) & 0xff);
  Wire1.write(reg & 0xff);
  Wire1.write((data >> 24) & 0xff);
  Wire1.write((data >> 16) & 0xff);
  Wire1.write((data >> 8) & 0xff);
  Wire1.write(data  & 0xff);
  Wire1.endTransmission();
}

void A121_I2C_Read_Byte(uint16_t reg,uint32_t *data)
{
  Wire1.beginTransmission(A121_DEV_ADDR);
  Wire1.write((reg >> 8) & 0xff);
  Wire1.write(reg & 0xff);
  uint8_t error = Wire1.endTransmission(true);
  // Serial.printf("endTransmission: %u\n", error);

  //Read 16 bytes from the slave
  uint8_t bytesReceived = Wire1.requestFrom(A121_DEV_ADDR, 4);
  // Serial.printf("requestFrom: %u\n", bytesReceived);
  if ((bool)bytesReceived) {  //If received more than zero bytes
    uint8_t temp[bytesReceived];
    Wire1.readBytes(temp, bytesReceived);
    *data = (temp[0] << 24) | (temp[1] << 16)  | (temp[2] << 8)  | temp[3];
  }
}

void A121_Set_Start_And_End_Range(uint32_t start,uint32_t end)//The start and end point of measurement interval in millimeters.
{
  A121_I2C_Write_Byte(REG_START, start);
  A121_I2C_Write_Byte(REG_END, end);
}

void A121_Set_Num_Distances_to_Analyze(uint32_t value)
{
  A121_I2C_Write_Byte(REG_NUM_DISTANCES_TO_ANALYZE, value);
}

void A121_Set_Distance_Determination_Duration_S(uint32_t value)
{
  A121_I2C_Write_Byte(REG_DISTANCE_DETERMINATION_DURATION_S, value);
}

void A121_Set_Use_Presence_Processor(uint32_t value)
{
  A121_I2C_Write_Byte(REG_USE_PRESENCE_PROCESSOR, value);
}

void A121_Set_Lowest_Breathing_Rate(uint32_t value)
{
  A121_I2C_Write_Byte(REG_LOWEST_BREATHING_RATE, value);
}

void A121_Set_Highest_Breathing_Rate(uint32_t value)
{
  A121_I2C_Write_Byte(REG_HIGHEST_BREATHING_RATE, value);
}

void A121_Set_Time_Series_Length_S(uint32_t value)
{
  A121_I2C_Write_Byte(REG_TIME_SERIES_LENGTH_S, value);
}

void A121_Set_Frame_Rate(uint32_t value)
{
  A121_I2C_Write_Byte(REG_FRAME_RATE, value);
}

void A121_Set_Sweeps_Per_Frame(uint32_t value)
{
  A121_I2C_Write_Byte(REG_SWEEPS_PER_FRAME, value);
}

void A121_Set_Hwaas(uint32_t value)
{
  A121_I2C_Write_Byte(REG_HWAAS, value);
}

void A121_Set_Profile(uint32_t value)
{
  A121_I2C_Write_Byte(REG_PROFILE, value);
}

void A121_Set_Intra_Detection_Threshold(uint32_t value)
{
  A121_I2C_Write_Byte(REG_INTRA_DETECTION_THRESHOLD, value);
}

void A121_Set_Command(uint32_t value)
{
  A121_I2C_Write_Byte(REG_COMMAND, value);
}

//Get the RSS version.
void A121_Get_Version(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_VERSION, value);
  Serial.printf("Major: 0x%x Minor: 0x%x Patch: 0x%x\n", *value >> 16, (*value >> 8) & 0xff, *value & 0xff);
}

//Get protocol error flags
void A121_Get_Protocol_Status(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_PROTOCOL_STATUS, value);
}

//Get the measure counter, the number of measurements performed since restart.
void A121_Get_Measure_Counter(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_MEASURE_COUNTER, value);
}

//Get application status flags.
void A121_Get_App_Status(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_APP_STATUS, value);
}

//The result from the breathing reference application.
void A121_Get_Breathing_Result(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_BREATHING_RESULT, value);
}

//The breathing rate. 0 if no breathing rate available. Note: This value is a factor 1000 larger than the RSS value.
void A121_Get_Breathing_Rate(uint32_t *value) 
{
  A121_I2C_Read_Byte(REG_BREATHING_RATE, value);
}

//The current state of the application
void A121_Get_App_State(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_APP_STATE, value);
}

void A121_Get_Application_Id(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_APPLICATION_ID, value);
}

void A121_init()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  // pinMode(17,OUTPUT);
  pinMode(A121_BUSY_PIN,INPUT);

  // digitalWrite(17,0);

  Wire1.setSDA(A121_SDA_PIN);
  Wire1.setSCL(A121_SCL_PIN);
  Wire1.setClock(400 * 1000);
  Wire1.begin();

  uint32_t value;
  while(digitalRead(A121_BUSY_PIN) == 0);
  A121_Get_Application_Id(&value);
  Serial.printf("A121_Get_Application_Id: 0x%x\n", value);
  
  A121_Set_Command(RESET_MODULE);
  delay(100);

  A121_Set_Command(ENABLE_UART_LOGS);

  while(digitalRead(A121_BUSY_PIN) == 0);

  A121_Set_Start_And_End_Range(300,1000);
  A121_Set_Num_Distances_to_Analyze(3);
  A121_Set_Distance_Determination_Duration_S(5);
  if(USE_PRESENCE_PROCESSOR)
  {
    A121_Set_Use_Presence_Processor(USE_PRESENCE_PROCESSOR);
    A121_Set_Intra_Detection_Threshold(4000);
    
  }
  else
    A121_Set_Use_Presence_Processor(false);
  A121_Set_Lowest_Breathing_Rate(6);
  A121_Set_Highest_Breathing_Rate(60);
  A121_Set_Time_Series_Length_S(20);
  A121_Set_Frame_Rate(20 * 1000);
  A121_Set_Sweeps_Per_Frame(16);
  A121_Set_Hwaas(32);
  A121_Set_Profile(PROFILE3);

  A121_Set_Command(APPLY_CONFIGURATION);
  while(1)
  {
    A121_Get_App_Status(&value);
    Serial.printf("A121_Get_App_Status: 0x%x\n", value);
    if((value & ALL_ERROR) == 0)
      break;
    delay(100);
  }
  
  Serial.printf("A121 init OK\n");

  A121_Set_Command(START_APP);
  while(1)
  {
    A121_Get_App_Status(&value);
    Serial.printf("A121_Get_App_Status: 0x%x\n", value);
    if((value & ALL_ERROR) == 0)
      break;
    delay(10);
  }
}

void A121_Get_Breathing_application()
{
  uint32_t value;
  
  A121_Get_Breathing_Result(&value);

  if((value & APP_ERROR) == 0)
  {
    if(value & RESULT_READY)
    {
      A121_Get_Breathing_Rate(&value);
      Serial.printf("A121_Get_Breathing_Rate: %d Hz\n", value);
      delay(2000);
    }
    else
    {
      if(value & RESULT_READY_STICKY)
      {
        Serial.printf("PRESENCE_DETECTED_STICKY\n");
      }
      else
        Serial.printf("checking...\n");
    }
  }
  else
  {
    Serial.printf("A121 DETECTOR ERROR\n"); //出现这个错误需要复位A121 发送RESET_MODULE命令
  }
}