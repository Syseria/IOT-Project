#include "A121_Presence_Detector.h"

#ifdef A121_PRESENCE_DETECTOR

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
  printf("Major: 0x%x Minor: 0x%x Patch: 0x%x\n", *value >> 16, (*value >> 8) & 0xff, *value & 0xff);
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
  uint32_t value;
  while(DEV_Digital_Read(A121_BUSY_PIN) == 0);
  A121_Get_Application_Id(&value);
  printf("A121_Get_Application_Id: 0x%x\n", value);
  
  A121_Set_Command(RESET_MODULE);
  DEV_Delay_ms(100);

  A121_Set_Command(ENABLE_UART_LOGS);

  while(DEV_Digital_Read(A121_BUSY_PIN) == 0);

  A121_Set_Start_And_End_Range(300,2500);//mm
  A121_Set_Frame_Rate(12 * 1000);// Frame rate:12Hz
  A121_Set_Sweeps_Per_Frame(16);// Sweeps per frame:16
  if(AUTOMATIC_SUBSWEEPS)
  {
    A121_Set_Auotmatic_Subsweeps(AUTOMATIC_SUBSWEEPS);// Enable automatic subsweeps
    A121_Set_Signal_Quality(20 * 1000);// Signal quality:20
  }
  else
  {
    A121_Set_Auotmatic_Subsweeps(AUTOMATIC_SUBSWEEPS);
    A121_Set_Hwaas(32);// HWAAS:32
    if(OVERRIDE)
    {
      A121_Set_Manual_Profile(PROFILE1);// Manual profile:1
      A121_Set_Manual_Step_Length(24);// Manual step length:24
    }
  }

  if(INTRA_MOTION_DETECTION)
  {
    A121_Set_Intra_Detection_Threshold(1300);// Intra detection threshold:1.3
    A121_Set_Intra_Frame_Time_Const(150);// Intra frame time constant:0.15S
    A121_Set_Intra_Output_Time_Const(300);// Intra output time constant:0.3S
  }

  if(INTER_MOTION_DETECTION)
  {
    A121_Set_Inter_Detection_Threshold(1000);// Inter detection threshold:1.0
    A121_Set_Inter_Frame_Fast_Cutoff(6000);// Inter frame fast cutoff:6Hz
    A121_Set_Inter_Frame_Slow_Cutoff(200);  // Inter frame slow cutoff:0.2Hz
    A121_Set_Inter_Frame_Deviation_Time_Const(500); // Inter frame deviation time constant:0.5S
    A121_Set_Inter_Output_Time_Const(2000); // Inter output time constant:2S
    A121_Set_Inter_Frame_Presence_Timeout(3); // Inter frame presence timeout:3s
  }

  A121_Set_Command(APPLY_CONFIGURATION);
  while(1)
  {
    A121_Get_Detector_Status(&value);
    printf("A121_Get_Detector_Status: 0x%x\n", value);
    if((value & ALL_ERROR) == 0)
      break;
    DEV_Delay_ms(100);
  }
  
  printf("A121 init OK\n");
  
}

void A121_Get_Presence_Detector()
{
  uint32_t value;
  A121_Set_Command(START_DETECTOR);
  while(1)
  {
    A121_Get_Detector_Status(&value);
    // printf("A121_Get_Detector_Status: 0x%x\n", value);
    if((value & ALL_ERROR) == 0)
      break;
    DEV_Delay_ms(10);
  }
  A121_Get_Presence_Result(&value);

  if((value & RESULT_DETECTOR_ERROR) == 0)
  {
    if(value & PRESENCE_DETECTED)
    {
      A121_Get_Presence_Distance(&value);
      printf("Presence_Distance: %d mm\n", value);

      A121_Get_Intra_Presence_Score(&value);//快速动作幅度
      printf("Intra_Presence_Score: %d \n", value);

      A121_Get_Inter_Presence_Score(&value);//慢动作幅度
      printf("Inter_Presence_Score: %d \n", value);

      A121_Get_Presence_Actual_Frame_Rate(&value); //实际帧率
      printf("Inter_Presence_Score: %d \n\n", value);
    }
    else
    {
      if(value & PRESENCE_DETECTED_STICKY)
      {
        printf("PRESENCE_DETECTED_STICKY\n");
      }
    }
  }
  else
  {
    printf("A121 DETECTOR ERROR\n"); //出现这个错误需要复位A121 发送RESET_MODULE命令
  }
}
#endif