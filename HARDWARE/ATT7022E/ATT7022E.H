#ifndef _AT7022E_H
#define _AT7022E_H
#include "sys.h"
#include "GetFFT.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEK STM32F7开发板
//IIC驱动代码	   
//正点原子@ALIENTEK
//创建日期:2017/5/23
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) ZIWEI CHEN 2017-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
#define REG_Count 40
#define AmmerDATA_Count 80
#define DATALEN		4
struct ATT7022EREG_Typedef
{
    uint8_t     REGAddr;
    uint32_t    REGDada; 
};
struct Ammer_Parament_StructureDef
{
		float Factor;
		float Up_Vol;
		float Up_Current;
		float Up_Vol_Angle;
		float Up_Current_Angle;		
		float Up_P_Power;
		float Up_Q_Power; 
		float Up_S_Power; 	
};
struct TimeTypeDef
{
	u8 year;
	u8 month;
	u8 date;
	u8 hour;
	u8 minute;
	u8 second;
	u32 musecond;
}; 
struct Data_StructureDef
{
		u32 Device_ID[3];
		float frequence;
		struct Ammer_Parament_StructureDef Ammer_Parament[3];
		struct TimeTypeDef time;
		Harmonic_StructureDef Harm;
};

//用于读操作，读取各电参量值
enum DATADef{	ATT_ID,
							Reg_VolA,Reg_VolB,Reg_VolC,Reg_Vol0,// 相电压
							Reg_CurrA,Reg_CurrB,Reg_CurrC,Reg_Curr0,//相电流
							Reg_PA_Power,Reg_PB_Power,Reg_PC_Power,Reg_P0_Power,//相有功功率
							Reg_QA_Power,Reg_QB_Power,Reg_QC_Power,Reg_Q0_Power,//相无功功率
							Reg_SA_Power,Reg_SB_Power,Reg_SC_Power,Reg_S0_Power,//相视在功率
							Reg_PowerA_Factor,Reg_PowerB_Factor,Reg_PowerC_Factor,Reg_Power0_Factor,//相功率因数
							Reg_INTFlag,Reg_Freq,Reg_RmsADC7,Reg_Temperature,Reg_Vref,							 //特殊寄存器
							Reg_Funda_PA_Power,Reg_Funda_PB_Power,Reg_Funda_PC_Power,Reg_Funda_P0_Power,//基波有功功率
							Reg_Funda_VolA,Reg_Funda_VolB,Reg_Funda_VolC,//基波电压有效值
							Reg_Funda_CurrA,Reg_Funda_CurrB,Reg_Funda_CurrC,//基波电流有效值
							Reg_Funda_QA_Power,Reg_Funda_QB_Power,Reg_Funda_QC_Power,Reg_Funda_Q0_Power,//基波无功功率
							Reg_Funda_PeakVolA,Reg_Funda_PeakVolB,Reg_Funda_PeakVolC,//电压最大值
							Reg_Sample_VolA,Reg_Sample_VolB,Reg_Sample_VolC,//电压ADC采样数据
							Reg_Sample_CurrA,Reg_Sample_CurrB,Reg_Sample_CurrC,//电流ADC采样数据				
							Reg_UA_Ang,Reg_UB_Ang,Reg_UC_Ang,Reg_IA_Ang,Reg_IB_Ang,Reg_IC_Ang
}; 

extern struct Data_StructureDef Elec_Data;
extern struct ATT7022EREG_Typedef AmmerDATA[AmmerDATA_Count];
extern struct ATT7022EREG_Typedef ATT7022E_InitStructure[REG_Count];

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define RDSPIData   HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)//读数据




//底层功能函数
void InitAmmeter(void);
u32 READ_Att7022(u8 Address);
void Write_Att7022(u8 Address, u32 dwData);
u32 ReadRegData(u8 DATAType);
u8 ATT7022E_Check(void);	//检测ATT7022E是否存在
void EMU_init(void);	//调用Write_Att7022()函数，将所有可写寄存器全部初始化一遍
u8 ReadAmmeterData(void);
u8 ReadSend_ALL_AmmeterData(void);

#endif
