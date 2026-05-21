#include "A121_Presence_Detector.h"

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
    *data = ((uint32_t)temp[0] << 24) | ((uint32_t)temp[1] << 16)  | ((uint32_t)temp[2] << 8)  | (uint32_t)temp[3];
  }
}

void A121_Set_Start_And_End_Range(uint32_t start,uint32_t end)
{
  A121_I2C_Write_Byte(REG_START, start);
  A121_I2C_Write_Byte(REG_END, end);
}

void A121_Set_Sweeps_Per_Frame(uint32_t value)
{
  A121_I2C_Write_Byte(REG_SWEEPS_PER_FRAME, value);
}

void A121_Set_Inter_Frame_Presence_Timeout(uint32_t value)
{
  A121_I2C_Write_Byte(REG_INTER_FRAME_PRESENCE_TIMEOUT, value);
}

void A121_Set_Intra_Detection_Enabled(uint32_t value)
{
  A121_I2C_Write_Byte(REG_INTRA_DETECTION_ENABLED, value);
}

void A121_Set_Inter_Detection_Enabled(uint32_t value)
{
  A121_I2C_Write_Byte(REG_INTER_DETECTION_ENABLED, value);
}

void A121_Set_Frame_Rate(uint32_t value)
{
  A121_I2C_Write_Byte(REG_FRAME_RATE, value);
}

void A121_Set_Intra_Detection_Threshold(uint32_t value)
{
  A121_I2C_Write_Byte(REG_INTRA_DETECTION_THRESHOLD, value);
}

void A121_Set_Inter_Detection_Threshold(uint32_t value)
{
  A121_I2C_Write_Byte(REG_INTER_DETECTION_THRESHOLD, value);
}

void A121_Set_Inter_Frame_Deviation_Time_Const(uint32_t value)
{
  A121_I2C_Write_Byte(REG_INTER_FRAME_DEVIATION_TIME_CONST, value);
}

void A121_Set_Inter_Frame_Fast_Cutoff(uint32_t value)
{
  A121_I2C_Write_Byte(REG_INTER_FRAME_FAST_CUTOFF, value);
}

void A121_Set_Inter_Frame_Slow_Cutoff(uint32_t value)
{
  A121_I2C_Write_Byte(REG_INTER_FRAME_SLOW_CUTOFF, value);
}

void A121_Set_Intra_Frame_Time_Const(uint32_t value)
{
  A121_I2C_Write_Byte(REG_INTRA_FRAME_TIME_CONST, value);
}

void A121_Set_Intra_Output_Time_Const(uint32_t value)
{
  A121_I2C_Write_Byte(REG_INTRA_OUTPUT_TIME_CONST, value);
}

void A121_Set_Inter_Output_Time_Const(uint32_t value)
{
  A121_I2C_Write_Byte(REG_INTER_OUTPUT_TIME_CONST, value);
}

void A121_Set_Auot_Profile_Enabled(uint32_t value)
{
  A121_I2C_Write_Byte(REG_AUTO_PROFILE_ENABLED, value);
}

void A121_Set_Auot_Step_Length_Enabled(uint32_t value)
{
  A121_I2C_Write_Byte(REG_AUTO_STEP_LENGTH_ENABLED, value);
}

void A121_Set_Manual_Profile(uint32_t value)
{
  A121_I2C_Write_Byte(REG_MANUAL_PROFILE, value);
}

void A121_Set_Manual_Step_Length(uint32_t value)
{
  A121_I2C_Write_Byte(REG_MANUAL_STEP_LENGTH, value);
}

void A121_Set_Reset_Filters_On_Prepare(uint32_t value)
{
  A121_I2C_Write_Byte(REG_RESET_FILTERS_ON_PREPARE, value);
}

void A121_Set_Hwaas(uint32_t value)
{
  A121_I2C_Write_Byte(REG_HWAAS, value);
}

void A121_Set_Auotmatic_Subsweeps(uint32_t value)
{
  A121_I2C_Write_Byte(REG_AUTOMATIC_SUBSWEEPS, value);
}

void A121_Set_Signal_Quality(uint32_t value)
{
  A121_I2C_Write_Byte(REG_SIGNAL_QUALITY, value);
}

void A121_Set_Command(uint32_t value)
{
  A121_I2C_Write_Byte(REG_COMMAND, value);
}

void A121_Get_Version(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_VERSION, value);
  Serial.print("Major: 0x");
  Serial.print((*value >> 16) & 0xFF, HEX);
  Serial.print(" Minor: 0x");
  Serial.print((*value >> 8) & 0xFF, HEX);
  Serial.print(" Patch: 0x");
  Serial.println(*value & 0xFF, HEX);
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

void A121_Get_Presence_Result(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_PRESENCE_RESULT, value);
}

void A121_Get_Presence_Distance(uint32_t *value) //Monitoring the distance at which an object exists
{
  A121_I2C_Read_Byte(REG_PRESENCE_DISTANCE, value);
}

void A121_Get_Intra_Presence_Score(uint32_t *value)//Fast movement range
{
  A121_I2C_Read_Byte(REG_INTRA_PRESENCE_SCORE, value);
}

void A121_Get_Inter_Presence_Score(uint32_t *value)//Slow-motion amplitude
{
  A121_I2C_Read_Byte(REG_INTER_PRESENCE_SCORE, value);
}

void A121_Get_Presence_Actual_Frame_Rate(uint32_t *value) //Actual frame rate
{
  A121_I2C_Read_Byte(REG_PRESENCE_ACTUAL_FRAME_RATE, value);
}

void A121_Get_Application_Id(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_APPLICATION_ID, value);
}

void A121_init()
{
  Serial.begin(115200);

  pinMode(A121_WAKEUP_PIN, OUTPUT);
  pinMode(A121_BUSY_PIN, INPUT);

  digitalWrite(A121_WAKEUP_PIN, 1);

  Wire.setClock(400000);
  Wire.begin();

  uint32_t value;
  while (digitalRead(A121_BUSY_PIN) == 0);

  A121_Get_Application_Id(&value);
  Serial.print("A121_Get_Application_Id: 0x");
  Serial.println(value, HEX);

  A121_Set_Command(RESET_MODULE);
  delay(100);

  A121_Set_Command(ENABLE_UART_LOGS);

  while (digitalRead(A121_BUSY_PIN) == 0);

  A121_Set_Start_And_End_Range(300, 2500);
  A121_Set_Frame_Rate(12 * 1000);
  A121_Set_Sweeps_Per_Frame(16);

  if (AUTOMATIC_SUBSWEEPS)
  {
    A121_Set_Auotmatic_Subsweeps(true);
    A121_Set_Signal_Quality(20 * 1000);
  }
  else
  {
    A121_Set_Auotmatic_Subsweeps(true);
    A121_Set_Hwaas(32);
    if (OVERRIDE)
    {
      A121_Set_Manual_Profile(PROFILE1);
      A121_Set_Manual_Step_Length(24);
    }
  }

  if (INTRA_MOTION_DETECTION)
  {
    A121_Set_Intra_Detection_Threshold(1300);
    A121_Set_Intra_Frame_Time_Const(150);
    A121_Set_Intra_Output_Time_Const(300);
  }

  if (INTER_MOTION_DETECTION)
  {
    A121_Set_Inter_Detection_Threshold(1000);
    A121_Set_Inter_Frame_Fast_Cutoff(6000);
    A121_Set_Inter_Frame_Slow_Cutoff(200);
    A121_Set_Inter_Frame_Deviation_Time_Const(500);
    A121_Set_Inter_Output_Time_Const(2000);
    A121_Set_Inter_Frame_Presence_Timeout(3);
  }

  A121_Set_Command(APPLY_CONFIGURATION);

  while (1)
  {
    A121_Get_Detector_Status(&value);
    Serial.print("A121_Get_Detector_Status: 0x");
    Serial.println(value, HEX);

    if ((value & ALL_ERROR) == 0)
      break;
    delay(100);
  }

  Serial.println("A121 init OK");
}


void A121_Get_Presence_Detector()
{
  uint32_t value;
  A121_Set_Command(START_DETECTOR);

  while (1)
  {
    A121_Get_Detector_Status(&value);
    // Serial.print("A121_Get_Detector_Status: 0x");
    // Serial.println(value, HEX);
    if ((value & ALL_ERROR) == 0)
      break;
    delay(10);
  }

  A121_Get_Presence_Result(&value);

  if ((value & RESULT_DETECTOR_ERROR) == 0)
  {
    if (value & PRESENCE_DETECTED)
    {
      A121_Get_Presence_Distance(&value);
      Serial.print("Presence_Distance: ");
      Serial.print(value);
      Serial.println(" mm");

      A121_Get_Intra_Presence_Score(&value); // 快速动作幅度
      Serial.print("Intra_Presence_Score: ");
      Serial.println(value);

      A121_Get_Inter_Presence_Score(&value); // 慢动作幅度
      Serial.print("Inter_Presence_Score: ");
      Serial.println(value);

      A121_Get_Presence_Actual_Frame_Rate(&value); // 实际帧率
      Serial.print("Actual_Frame_Rate: ");
      Serial.println(value);
      Serial.println();
    }
    else
    {
      if (value & PRESENCE_DETECTED_STICKY)
      {
        Serial.println("PRESENCE_DETECTED_STICKY");
      }
    }
  }
  else
  {
    Serial.println("A121 DETECTOR ERROR"); // 需要 RESET_MODULE
  }
}
