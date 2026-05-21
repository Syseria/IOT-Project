
#include "A121_Distance_Detector.h"

#ifdef A121_DISTANCE_DETECTOR
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
  uint32_t value;
  while(DEV_Digital_Read(A121_BUSY_PIN) == 0);
  A121_Get_Application_Id(&value);
  printf("A121_Get_Application_Id: 0x%x\n", value);
  
  A121_Set_Command(RESET_MODULE);
  DEV_Delay_ms(100);

  A121_Set_Command(ENABLE_UART_LOGS);

  while(DEV_Digital_Read(A121_BUSY_PIN) == 0);

  A121_Set_Start_And_End_Range(250,3000);
  A121_Set_Max_Step_Length(0);//Max step length
  A121_Set_Close_Range_Leakage_Cancellation(false);//Close range leakage cancellation
  A121_Set_Signal_Quality(15000);//Signal quality:15
  A121_Set_Max_Profie(PROFILE5);//Max profile:5
  A121_Set_Threshold_Method(CFAR);//Threshold method:CFAR
  A121_Set_Peak_Sorting(STRONGEST);//Peak sorting:STRONGEST
  A121_Set_Num_Frames_Recorded_hreshold(100);//Number of frames recorded threshold:100
  A121_Set_Fixed_Amplitude_Threshold_Value(100000);//Fixed amplitude threshold value:100
  A121_Set_Threshold_Sensitivity(500);//Threshold sensitivity:0.5
  A121_Set_Reflector_Shape(GENERIC);//Reflector shape:GENERIC
  A121_Set_Fixed_Strength_Threshold_Value(0);//Fixed strength threshold value:0
  
  A121_Set_Command(APPLY_CONFIG_AND_CALIBRATE);
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

void A121_Get_Distance_mm()
{
  uint32_t value;
  A121_Set_Command(MEASURE_DISTANCE);
  while(1)
  {
    A121_Get_Detector_Status(&value);
    // printf("A121_Get_Detector_Status: 0x%x\n", value);
    if((value & ALL_ERROR) == 0)
      break;
    DEV_Delay_ms(10);
  }
  A121_Get_Distance_Result(&value);
  // printf("A121_Get_Distance_Result: 0x%x\n", value);
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
          printf("num : %d \nMEASURE_DISTANCE: %d mm\n", i, value);
          A121_I2C_Read_Byte(REG_PEAK0_STRENGTH + i, &value);
          printf("REG_PEAK0_STRENGTH: %0.2f dB\n\n", (int)value / 1000.0);
        }
      }
      else
        printf("A121 NUM DISTANCES = 0\n");
    }
    else
    {
      printf("A121 CALIBRATION NEEDED\n");
    }
  }
  else
  {
    printf("A121 MEASURE DISTANCE ERROR\n");
  }
}
#endif