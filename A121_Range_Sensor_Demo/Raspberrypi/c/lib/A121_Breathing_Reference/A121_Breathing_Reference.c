#include "A121_Breathing_Reference.h"

#ifdef A121_BREATHING_REFERENCE

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
  printf("Major: 0x%x Minor: 0x%x Patch: 0x%x\n", *value >> 16, (*value >> 8) & 0xff, *value & 0xff);
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
  uint32_t value;
  while(DEV_Digital_Read(A121_BUSY_PIN) == 0);
  A121_Get_Application_Id(&value);
  printf("A121_Get_Application_Id: 0x%x\n", value);
  
  A121_Set_Command(RESET_MODULE);
  DEV_Delay_ms(100);

  A121_Set_Command(ENABLE_UART_LOGS);

  while(DEV_Digital_Read(A121_BUSY_PIN) == 0);

  A121_Set_Start_And_End_Range(300,1000);//mm
  A121_Set_Num_Distances_to_Analyze(3); //Number of distances to analyze
  A121_Set_Distance_Determination_Duration_S(5); //Duration to determine distance
  if(USE_PRESENCE_PROCESSOR)
  {
    A121_Set_Use_Presence_Processor(USE_PRESENCE_PROCESSOR);
    A121_Set_Intra_Detection_Threshold(4000);//Intra detection threshold
    
  }
  else
    A121_Set_Use_Presence_Processor(false);
  A121_Set_Lowest_Breathing_Rate(6);//Lowest anticipated breathing rate
  A121_Set_Highest_Breathing_Rate(60);//Highest anticipated breathing rate
  A121_Set_Time_Series_Length_S(20);//Time series length
  A121_Set_Frame_Rate(20 * 1000);//Frame rate
  A121_Set_Sweeps_Per_Frame(16);//Sweeps per frame
  A121_Set_Hwaas(32);//HWAAS
  A121_Set_Profile(PROFILE3);//Profile

  A121_Set_Command(APPLY_CONFIGURATION);
  while(1)
  {
    A121_Get_App_Status(&value);
    printf("A121_Get_App_Status: 0x%x\n", value);
    if((value & ALL_ERROR) == 0)
      break;
    DEV_Delay_ms(100);
  }
  
  printf("A121 init OK\n");

  A121_Set_Command(START_APP);
  while(1)
  {
    A121_Get_App_Status(&value);
    printf("A121_Get_App_Status: 0x%x\n", value);
    if((value & ALL_ERROR) == 0)
      break;
    DEV_Delay_ms(10);
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
      printf("A121_Get_Breathing_Rate: %d Hz\n", value);
      DEV_Delay_ms(2000);
    }
    else
    {
      if(value & RESULT_READY_STICKY)
      {
        printf("PRESENCE_DETECTED_STICKY\n");
      }
      else
      {
        printf("checking...\n");
      }

    }
  }
  else
  {
    printf("A121 DETECTOR ERROR\n"); //出现这个错误需要复位A121 发送RESET_MODULE命令
  }
}
#endif