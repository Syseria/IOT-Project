#include "A121_Cargo_Example.h"

#ifdef A121_CARGO_EXAMPLE

//Size of the container. Valid values to write are 10U, 20U and 40U.
void A121_Set_Container_Size(uint32_t value)
{
  A121_I2C_Write_Byte(REG_CONTAINER_SIZE, value);
}

//Whether to activate utilization level measurements.The command MEASURE UTILIZATION LEVEL cannot succeed if this register if false.
void A121_Set_Activate_Utilization_Leve(uint32_t value)
{
  A121_I2C_Write_Byte(REG_ACTIVATE_UTILIZATION_LEVEL, value);
}

//Signal quality. This register is x1000 compared to the Cargo Example Application.For more information, see documentation about the Distance Detectors signal quality parameter
void A121_Set_Utilization_Signal_Quality(uint32_t value)
{
  A121_I2C_Write_Byte(REG_UTILIZATION_SIGNAL_QUALITY, value);
}

//Threshold sensitivity. This register is x1000 compared to the Cargo ExampleApplication. For more information, see documentation about the Distance Detectors threshold sensitivity parameter.
void A121_Set_Utilization_Threshold_Sens(uint32_t value)
{
  A121_I2C_Write_Byte(REG_UTILIZATION_THRESHOLD_SENS, value);
}

//Whether to activate presence measurements. The command MEASURE PRESENCE cannot succeed if this register if false.
void A121_Set_Activate_Presence(uint32_t value)
{
  A121_I2C_Write_Byte(REG_ACTIVATE_PRESENCE, value);
}

//The presence detector update rate (frame rate). This register is x1000 compared to the Cargo Example Application. For more information, see documentation about the Presence Detectors frame rate parameter.
void A121_Set_Presence_Update_Rate(uint32_t value)
{
  A121_I2C_Write_Byte(REG_PRESENCE_UPDATE_RATE, value);
}

//The number of sweeps that will be captured in each frame (measurement). For more information, see documentation about the Presence Detectors sweeps per frame parameter.
void A121_Set_Presence_Sweeps_Per_Frame(uint32_t value)
{
  A121_I2C_Write_Byte(REG_PRESENCE_SWEEPS_PER_FRAME, value);
}

//Signal quality. This register is x1000 compared to the Cargo Example Application. For more information, see documentation about the Presence Detectors signal quality parameter.
void A121_Set_Presence_Signal_Quality(uint32_t value)
{
  A121_I2C_Write_Byte(REG_PRESENCE_SIGNAL_QUALITY, value);
}

//This is the threshold for detecting slower movements between frames. This register is x1000 compared to the Cargo Example Application. For more information, see documentation about the Presence Detectors inter detection threshold parameter.
void A121_Set_Presence_Inter_Detection_Threshold(uint32_t value)
{
  A121_I2C_Write_Byte(REG_PRESENCE_INTER_THRESHOLD, value);
}

//This is the threshold for detecting faster movements between frames. This register is x1000 compared to the Cargo Example Application. For more information, see documentation about the Presence Detectors intra detection threshold parameter.
void A121_Set_Presence_Intra_Detection_Threshold(uint32_t value)
{
  A121_I2C_Write_Byte(REG_PRESENCE_INTRA_THRESHOLD, value);
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

//Get the actual update rate (frame rate) of presence during a burst.
void A121_Get_Actual_Presence_Update_Rate(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_ACTUAL_PRESENCE_UPDATE_RATE, value);
}

//Get example app status flags
void A121_Get_Application_Status(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_APPLICATION_STATUS, value);
}

//The result header for the cargo result.
void A121_Get_Result_Header(uint32_t *value) 
{
  A121_I2C_Read_Byte(REG_RESULT_HEADER, value);
}

//The distance, in millimeters, to the detection.
void A121_Get_Utilization_Distance(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_UTILIZATION_DISTANCE, value);
}

//The fill level in millimeters. Fill level is the distance from the detection to the back of the container.
void A121_Get_Utilization_Level_Mm(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_UTILIZATION_LEVEL_MM, value);
}

//The fill level in percent. Fill level is the distance from the detection to the back of the container
void A121_Get_Utilization_Level_Percent(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_UTILIZATION_LEVEL_PERCENT, value);
}

//Whether presence was detected during the 5s presence burst
void A121_Get_Presence_Detected(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_PRESENCE_DETECTED, value);
}

//Inter presence score is a measure of the amount of slow motion detected. This register contains the maximum inter presence score during the 5s presence burst.
void A121_Get_Max_Inter_Presence_Score(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_MAX_INTER_PRESENCE_SCORE, value);
}

//Intra presence score is measure of the amount of slow motion detected. This register contains the maximum intra presence score during the 5s presence burst.
void A121_Get_Max_Intra_Presence_Score(uint32_t *value)
{
  A121_I2C_Read_Byte(REG_MAX_INTRA_PRESENCE_SCORE, value);
}

//The application id register.
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

  A121_Set_Container_Size(20); // Container size: 10 20 40 
  A121_Set_Activate_Utilization_Leve(true);
  A121_Set_Presence_Update_Rate(5000);//Frame rate:5Hz
  A121_Set_Utilization_Signal_Quality(25000);//Signal quality:25
  A121_Set_Utilization_Threshold_Sens(500);//Threshold sensitivity:0.5

  if(USE_ACTIVATE_PRESENCE)
  {
    A121_Set_Activate_Presence(USE_ACTIVATE_PRESENCE);
    A121_Set_Presence_Update_Rate(6000);//Frame rate:6Hz
    A121_Set_Presence_Signal_Quality(30000);//Signal quality:30
    A121_Set_Presence_Sweeps_Per_Frame(12);//Sweeps per frame:12
    A121_Set_Presence_Inter_Detection_Threshold(2000);//Inter detection threshold:2.0 
    A121_Set_Presence_Intra_Detection_Threshold(2000);//Intra detection threshold:2.0
  }

  A121_Set_Command(APPLY_CONFIGURATION);
  while(1)
  {
    A121_Get_Application_Status(&value);
    printf("A121_Get_Application_Status: 0x%x\n", value);
    if((value & ALL_ERROR) == 0)
      break;
    DEV_Delay_ms(100);
  }
  
  printf("A121 init OK\n");
}

void A121_Get_Cargo_application()
{
  uint32_t value;

  A121_Set_Command(MEASURE_UTILIZATION_LEVEL);
  while(1)
  {
    A121_Get_Application_Status(&value);
    // printf("A121_Get_Application_Status: 0x%x\n", value);
    if((value & ALL_ERROR) == 0)
      break;
    DEV_Delay_ms(10);
  }  

  A121_Get_Result_Header(&value);
  // printf("A121_Get_Result_Header: 0x%x\n", value);
  if(value & UTILIZATION_LEVEL_VALID)
  {
    A121_Get_Utilization_Distance(&value);
    printf("A121_Get_Utilization_Distance: %d \n", value);
    A121_Get_Utilization_Level_Mm(&value);
    printf("A121_Get_Utilization_Level_Mm: %d mm\n", value);
    A121_Get_Utilization_Level_Percent(&value);
    printf("A121_Get_Utilization_Level_Percent: %d %%\n\n", value);
  }
  
  // A121_Set_Command(MEASURE_PRESENCE);
  // while(1)
  // {
  //   A121_Get_Application_Status(&value);
  //   // printf("A121_Get_Application_Status: 0x%x\n", value);
  //   if((value & ALL_ERROR) == 0)
  //     break;
  //   DEV_Delay_ms(10);
  // }

  // A121_Get_Result_Header(&value);

  // if(value & PRESENCE_VALID)
  // {
  //   A121_Get_Presence_Detected(&value);
  //   printf("A121_Get_Presence_Detected: %d \n", value);
  //   A121_Get_Max_Inter_Presence_Score(&value);
  //   printf("A121_Get_Max_Inter_Presence_Score: %d \n", value);
  //   A121_Get_Max_Intra_Presence_Score(&value);
  //   printf("A121_Get_Max_Intra_Presence_Score: %d \n\n", value);
  // }

}

#endif