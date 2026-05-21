#include "A121_Distance_Detector.h"

void A121_I2C_Write_Byte(uint16_t reg,uint32_t data)
{
  Wire.beginTransmission(A121_DEV_ADDR);
  Wire.write((reg >> 8) & 0xff);
  Wire.write(reg & 0xff);
  Wire.write((data >> 24) & 0xff);
  Wire.write((data >> 16) & 0xff);
  Wire.write((data >> 8) & 0xff);
  Wire.write(data  & 0xff);
  Wire.endTransmission();
}

void A121_I2C_Read_Byte(uint16_t reg,uint32_t *data)
{
  Wire.beginTransmission(A121_DEV_ADDR);
  Wire.write((reg >> 8) & 0xff);
  Wire.write(reg & 0xff);
  uint8_t error = Wire.endTransmission(true);
  // Serial.printf("endTransmission: %u\n", error);

  //Read 16 bytes from the slave
  uint8_t bytesReceived = Wire.requestFrom(A121_DEV_ADDR, 4);
  // Serial.printf("requestFrom: %u\n", bytesReceived);
  if ((bool)bytesReceived) {  //If received more than zero bytes
    uint8_t temp[bytesReceived];
    Wire.readBytes(temp, bytesReceived);
    // log_print_buf(temp, bytesReceived);
    *data = (temp[0] << 24) | (temp[1] << 16)  | (temp[2] << 8)  | temp[3];
  }
}

void A121_Set_Start_And_End_Range(uint32_t start,uint32_t end)
{
  A121_I2C_Write_Byte(REG_START, start);
  A121_I2C_Write_Byte(REG_END, end);
}

void A121_Set_Max_Step_Length(uint32_t value)
{
  A121_I2C_Write_Byte(REG_MAX_STEP_LENGTH, value);
}

void A121_Set_Close_Range_Leakage_Cancellation(uint32_t value)
{
  A121_I2C_Write_Byte(REG_CLOSE_RANGE_LEAKAGE_CANCEL, value);
}

void A121_Set_Signal_Quality(uint32_t value)
{
  A121_I2C_Write_Byte(REG_SIGNAL_QUALITY, value);
}

void A121_Set_Max_Profie(uint32_t value)
{
  A121_I2C_Write_Byte(REG_MAX_PROFILE, value);
}

void A121_Set_Threshold_Method(uint32_t value)
{
  A121_I2C_Write_Byte(REG_THRESHOLD_METHOD, value);
}

void A121_Set_Peak_Sorting(uint32_t value)
{
  A121_I2C_Write_Byte(REG_PEAK_SORTING, value);
}

void A121_Set_Num_Frames_Recorded_hreshold(uint32_t value)
{
  A121_I2C_Write_Byte(REG_NUM_FRAMES_RECORDED_THRESHOLD, value);
}

void A121_Set_Fixed_Amplitude_Threshold_Value(uint32_t value)
{
  A121_I2C_Write_Byte(REG_FIXED_AMPLITUDE_THRESHOLD_VALUE, value);
}

void A121_Set_Threshold_Sensitivity(uint32_t value)
{
  A121_I2C_Write_Byte(REG_THRESHOLD_SENSITIVITY, value);
}

void A121_Set_Reflector_Shape(uint32_t value)
{
  A121_I2C_Write_Byte(REG_REFLECTOR_SHAPE, value);
}

void A121_Set_Fixed_Strength_Threshold_Value(uint32_t value)
{
  A121_I2C_Write_Byte(REG_FIXED_STRENGTH_THRESHOLD_VALUE, value);
}

void A121_Set_Measure_On_Wakeup(uint32_t value)
{
  A121_I2C_Write_Byte(REG_MEASURE_ON_WAKEUP, value);
}

void A121_Set_Command(uint32_t value)
{
  A121_I2C_Write_Byte(REG_COMMAND, value);
}

void A121_Get_Version(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_VERSION, value);
  Serial.printf("Major: 0x%x Minor: 0x%x Patch: 0x%x\n", *value >> 16, (*value >> 8) & 0xff, *value & 0xff);
}

void A121_Get_Protocol_Status(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_PROTOCOL_STATUS, value);
}

void A121_Get_Measure_Counter(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_MEASURE_COUNTER, value);
}

void A121_Get_Detector_Status(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_DETECTOR_STATUS, value);
}

void A121_Get_Distance_Result(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_DISTANCE_RESULT, value);
}

void A121_Get_Measure_On_Wakeup(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_MEASURE_ON_WAKEUP, value);
}

void A121_Get_Application_Id(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_APPLICATION_ID, value);
}

void A121_init()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  pinMode(A121_WAKEUP_PIN,OUTPUT);
  pinMode(A121_BUSY_PIN,INPUT);

  digitalWrite(A121_WAKEUP_PIN,1);

  Wire.setPins(A121_SDA_PIN,A121_SCL_PIN);
  Wire.setClock(400 * 1000);
  Wire.begin();

  uint32_t value;
  while(digitalRead(A121_BUSY_PIN) == 0);
  A121_Get_Application_Id(&value);
  Serial.printf("A121_Get_Application_Id: 0x%x\n", value);
  
  A121_Set_Command(RESET_MODULE);
  delay(100);

  A121_Set_Command(ENABLE_UART_LOGS);

  while(digitalRead(A121_BUSY_PIN) == 0);

  A121_Set_Start_And_End_Range(250,3000);
  A121_Set_Max_Step_Length(0);
  A121_Set_Close_Range_Leakage_Cancellation(false);
  A121_Set_Signal_Quality(15000);
  A121_Set_Max_Profie(PROFILE5);
  A121_Set_Threshold_Method(CFAR);
  A121_Set_Peak_Sorting(STRONGEST);
  A121_Set_Num_Frames_Recorded_hreshold(100);
  A121_Set_Fixed_Amplitude_Threshold_Value(100000);
  A121_Set_Threshold_Sensitivity(500);
  A121_Set_Reflector_Shape(GENERIC);
  A121_Set_Fixed_Strength_Threshold_Value(0);
  A121_Set_Command(APPLY_CONFIG_AND_CALIBRATE);
  while(1)
  {
    A121_Get_Detector_Status(&value);
    Serial.printf("A121_Get_Detector_Status: 0x%x\n", value);
    if((value & ALL_ERROR) == 0)
      break;
    delay(100);
  }
  
  Serial.printf("A121 init OK\n");
  
}

void A121_Get_Distance_mm()
{
  uint32_t value;
  A121_Set_Command(MEASURE_DISTANCE);
  while(1)
  {
    A121_Get_Detector_Status(&value);
    if((value & ALL_ERROR) == 0)
      break;
    delay(10);
  }
  A121_Get_Distance_Result(&value);

  int num = (value & NUM_DISTANCES);
  if((value & MEASURE_DISTANCE_ERROR) == 0)
  {
    if((value & CALIBRATION_NEEDED) == 0)
    {
      if(num != 0)
      {
        for(int i = 0;i < num;i++)
        {
          A121_I2C_Read_Byte(REG_PEAK0_DISTANCE + i, &value);
          Serial.printf("num : %d \nMEASURE_DISTANCE: %d mm\n", i, value);
          A121_I2C_Read_Byte(REG_PEAK0_STRENGTH + i, &value);
          Serial.printf("REG_PEAK0_STRENGTH: %0.2f dB\n\n", (int)value / 1000.0);
        }
      }
      else
        Serial.printf("A121 NUM DISTANCES = 0\n");
    }
    else
    {
      Serial.printf("A121 CALIBRATION NEEDED\n");
    }
  }
  else
  {
    Serial.printf("A121 MEASURE DISTANCE ERROR\n");
  }
}