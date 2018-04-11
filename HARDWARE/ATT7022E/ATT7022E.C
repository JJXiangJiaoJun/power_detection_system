#include "ATT7022E.h"
#include "math.h"
#include "rtc.h"
#include "spi.h"
#include <string.h>
#include <stdlib.h>
#include "lcd.h"
#include "led.h"
#include "delay.h"


/***********************内部结构体*********************/
enum PhaseTYPE{PhaseA,PhaseB,PhaseC};
struct Data_StructureDef Elec_Data;
struct ATT7022EREG_Typedef AmmerDATA[AmmerDATA_Count]=
{
		0x00,0X007122A0,// ATT_ID,0X7122A0
		0x0D,0X00000000,// VolA
		0x0E,0X00000000,// VolB
		0x0F,0X00000000,// VolC		
		0x2B,0X00000000,// Vol0						三 phase电压矢量和有效值
		0x10,0X00000000,// CurrA
		0x11,0X00000000,// CurrB
		0x12,0X00000000,// CurrC
		0x13,0X00000000,// Curr0					三 phase电流矢量和有效值
		0x01,0X00000000,//  PA_Power
		0x02,0X00000000,//  PB_Power
		0x03,0X00000000,//  PC_Power
		0x04,0X00000000,//  P0_Power			合 phase有功功率
		0x05,0X00000000,//  QA_Power
		0x06,0X00000000,//  QB_Power
		0x07,0X00000000,//  QC_Power
		0x08,0X00000000,//  Q0_Power			合 phase无功功率
		0x09,0X00000000,//  SA_Power
		0x0A,0X00000000,//  SB_Power
		0x0B,0X00000000,//  SC_Power
		0x0C,0X00000000,//  S0_Power			合 phase视在功率
		0x14,0X00000000,//  PowerA_Factor
		0x15,0X00000000,//  PowerB_Factor
		0x16,0X00000000,//  PowerC_Factor
		0x17,0X00000000,//  Power0_Factor	合 phase功率因数
		0x1B,0X00000000,//	INTFlag				中断标志，读后清零
		0x1C,0X00000000,//	Freq					线频率
		0x29,0X00000000,//	RmsADC7				第七路ADC输入信号的有效值
		0x2A,0X00000000,//	Temperature		温度传感器输出
		0x5C,0X00000000,//	Vref					Vref自动补偿系数
		0x40,0X00000000,//	Funda_PA_Power			A phase基波有功功率
		0x41,0X00000000,//	Funda_PB_Power			B phase基波有功功率
		0x42,0X00000000,//	Funda_PC_Power			C phase基波有功功率
		0x43,0X00000000,//	Funda_P0_Power			合 phase基波有功功率
		0x48,0X00000000,//	Funda_VolA			A phase基波电压有效值
		0x49,0X00000000,//	Funda_VolB			B phase基波电压有效值
		0x4A,0X00000000,//	Funda_VolC			C phase基波电压有效值
		0x4B,0X00000000,//	Funda_CurrA			A phase基波电流有效值
		0x4C,0X00000000,//	Funda_CurrB			B phase基波电流有效值
		0x4D,0X00000000,//	Funda_CurrC			C phase基波电流有效值
		0x57,0X00000000,//	Funda_QA_Power			A phase基波无功功率
		0x58,0X00000000,//	Funda_QB_Power			B phase基波无功功率
		0x59,0X00000000,//	Funda_QC_Power			C phase基波无功功率
		0x5A,0X00000000,//	Funda_Q0_Power			合 phase基波无功功率
		0x50,0X00000000,//	Funda_PeakVolA				A phase电压最大值
		0x51,0X00000000,//	Funda_PeakVolB			B phase电压最大值
		0x52,0X00000000,//	Funda_PeakVolC			C phase电压最大值
		0x32,0X00000000,// Sample_VolA					A phase电压ADC采样数据
		0x33,0X00000000,// Sample_VolB					B phase电压ADC采样数据
		0x34,0X00000000,// Sample_VolC					C phase电压ADC采样数据
		0x2F,0X00000000,// Sample_CurrA					A phase电流ADC采样数据
		0x30,0X00000000,// Sample_CurrB					B phase电流ADC采样数据
		0x31,0X00000000,// Sample_CurrC					C phase电流ADC采样数据									
		0x26,0X00000000,// UA_Angle							A phase电压相角
		0x27,0X00000000,// UB_Angle							B phase电压相角
		0x28,0X00000000,// UC_Angle							C phase电压相角
		0x18,0X00000000,// IA_Angle							A phase电流相角
		0x19,0X00000000,// IB_Angle							A phase电流相角
		0x1A,0X00000000// IC_Angle							A phase电流相角
};
/**********************电表寄存器**********************/
//写操作，配置电表寄存器
#define HFCONST   0X00000171	//369	//((2.592 * ATT_G * ATT_G*10000*INATTV*INATTI)/(EC*UI*VI))
struct ATT7022EREG_Typedef ATT7022E_InitStructure[REG_Count]=
{   0xD3,0X00000000,//rest
    0xC3,0X00000000,//清除校表数据
    0xc9,0X0000005A,//使能校表
//    0xC5,{0X00,0X00,0X00,0X00},//关闭同步
		0x01,0X0000B97E,//配置模式寄存器
		0x03,0X0000F884,//EMUcfg   读后清零		 0xF884   和功率有关
		0x02,0X00000200,//电流增益1  电压8 
		
		0x30,0X00000000,//关闭所有中断
		0x31,0X00003437,//模拟模块寄存器
		0x6D,0X0000FF00,//Vrefgain 的补偿曲线系数TCcoffA ，TCcoffB ，TCcoffC(0x6D~0x6F)
		0x6E,0X00000DB8,//Vrefgain 的补偿曲线系数TCcoffA ，TCcoffB ，TCcoffC(0x6D~0x6F)
		0x6F,0X0000D1DA,//Vrefgain 的补偿曲线系数TCcoffA ，TCcoffB ，TCcoffC(0x6D~0x6F)
		0x1E,HFCONST,					//HFCONST	// HFconst 3200  109	
		
		0x17,0X00000000,//(int)(0.99*32768)//A phase电压增益校准
		0x18,0X00000000,//(int)(0.99*32768)//B  phase电压增益校准
		0x19,0X00000000,//(int)(0.99*32768)//C  phase电压增益校准 
		
		0x1A,0X00000000,//1.918*32768//A phase电流增益 校准		
		0x1B,0X00000000,//1.918*32768//B phase电流增益 校准	
		0x1C,0X00000000,//1.908*32768//C phase电流增益 校准
		
		0x04,0X00000000,//1.442*32768//A有功率增益 校准    
		0x07,0X00000000,//1.442*32768//A无功率增益 校准
		0x0A,0X00000000,//1.442*32768//A视在功率增益 校准
		
		0x05,0X00000000,//1.437*32768//B有功率增益 校准	
		0x08,0X00000000,//1.437*32768//B无功率增益 校准
		0x0B,0X00000000,//1.437*32768//B视在功率增益 校准
		
		0x06,0X00000000,//1.437*32768//C有功率增益 校准
		0x09,0X00000000,//1.437*32768//C无功率增益 校准1.378*(pow(2,15))
		0x1C,0X00000000,//1.437*32768//C视在功率增益 校准   1.378*(pow(2,15)
		
		0x35,0X00000000,//   I/O
		0xC5,0X00000000,//打开同步
		0xc9,0X00000000,//Close the write protection
		0xC6,0X00000000,//打开计量寄存器数据
};
/*********************调用的功能函数************************/
//硬件初始化电表参数
void InitAmmeter(void)
{
  u32 ChipUniqueID[3];
	Elec_Data.Device_ID[2] = *(__IO u32*)(0X1FF0F420); 
	Elec_Data.Device_ID[1] = *(__IO u32 *)(0X1FF0F424);   
	Elec_Data.Device_ID[0] = *(__IO u32 *)(0X1FF0F428);
	
	SPI_Init(); //初始化ATT7022E的IO口及SPI 5
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);		//0
	delay_us(30) ;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);		//1
	EMU_init();    // 电表初始化
}

/*************************SPI协议 READ_Att7022*************************************************/
u32 READ_Att7022(u8 Address)
{
	u8 dwData[3];
	u32 DwData=0;
	SPI_Transmit(&Address,1);
	SPI_Receive(dwData,3);
	DwData=(dwData[0]<<16)|(dwData[1]<<8)|(dwData[2]);
  return DwData;
}

/*********************SPI协议 Wr_Dat*************************/
void Write_Att7022(u8 Address, u32 DwData)
{
			u8 dwData[3];
			dwData[0]=(DwData>>16)&0xff;
			dwData[1]=(DwData>>8)&0xff;
			dwData[2]=(DwData>>0)&0xff;
			Address=(Address&0x7f)|(1<<7);
			SPI_Transmit(&Address,1);
			SPI_Transmit(dwData,3);
}
////读取电表芯片寄存器数据
u32 ReadRegData(u8 DATAType)
{
	u32 data;
	data=READ_Att7022(AmmerDATA[DATAType].REGAddr);	
	return data;
}

//检测ATT7022E是否存在或正常
//返回值:1，成功;0，失败	
u8 ATT7022E_Check(void)
{
	u32 buf1,buf2;
	buf1=READ_Att7022(0x00);	
	buf2=READ_Att7022(0x5D);	
	if(buf2==0X7022E0)return 1;
	if(buf1==0X7122A0)return 1;//检测ATT7022E正确
	return 0;		 //检步ATT7022E失败
}	 	 
		   
//配置ATT7022E校表参数
void EMU_init(void)
{
	u8 t=0;
	Write_Att7022(0xC9, 0x00005A);		//校表数据写使能
	//参数配置	
	Write_Att7022(0x01, 0xB97E);	//模式配置
	Write_Att7022(0x03, 0xFC04);	//EMU单元配置
	Write_Att7022(0x31, 0x3437);	//模拟模块
	Write_Att7022(0x70, 0x010A);	//算法控制
	Write_Att7022(0x02, 0x0000);	//ADC增益配置，电压*1，电流*1
	Write_Att7022(0x30, 0x0003);	//中断使能	开启数据刷新	
//	Write_Att7022(0x1E, 0xB97E);	//HFCONST		
//																//369	//((2.592 * ATT_G * ATT_G*10000*INATTV*INATTI)/(EC*UI*VI))

	Write_Att7022(0x17, 65460);	//A phase电压有效值增益
	Write_Att7022(0x18, 65410);	//B phase电压有效值增益
	Write_Att7022(0x19, 65350);	//C phase电压有效值增益，降下10倍
	Write_Att7022(0x1A, 65032);	//A phase电流有效值增益
	Write_Att7022(0x1B, 65062);	//B phase电流有效值增益
	Write_Att7022(0x1C, 65042);	//C phase电流有效值增益
	

	Write_Att7022(0xC9, 0x00005B);		//校表数据写关闭
	delay_us(500);			
}


/**********************校表数据************************/
#define ATT_G  1.163
#define INATTV 	72			      // 模拟扩大8倍         
#define INATTI 	5			        //互感器感应电压 扩大1000倍  
#define EC     3200 					//电表表常数
#define UI	   220 					  //额定电压
#define VI	   0.0478
#define U_K   4.15946  
#define I_K   0.088388  
#define P_K   0.367646//U_K*I_K			2.592*pow(10,10)	/(HFCONST *EC *pow(2,23))  0.01225 //0.965595/HFCONST//0.0025745
u16 Huganqibeilv = 1;                             //互感器倍率
/******************************************************/
//获取所有电参数
//返回值:1，成功;0，失败	
u8 ReadAmmeterData(void)
{
		//设置一些临时参量
		RTC_TimeTypeDef RTC_TimeStruct;
		RTC_DateTypeDef RTC_DateStruct;
		u8 tbuf[40],t;	
		float frequence;
		u32 freq;
		float VoltageA,VoltageA_Angle,CurrentA,CurrentA_Angle = 0,PPA_Power,QQA_Power,SSA_Power,FactorA;	 //A  phase电压有效值,电流有效值	有功功,无功功率,视在功率
		u32 VolA,VolA_Ang,CurrA,CurrA_Ang,PA_Power,QA_Power,SA_Power,FactA; // A   phase电压,电流,有功功,无功功率,视在功率
		float VoltageB,VoltageB_Angle,CurrentB=0,CurrentB_Angle,PPB_Power,QQB_Power,SSB_Power,FactorB;	 //B  phase电流有效值	
		u32 VolB = 0,VolB_Ang,CurrB = 0,CurrB_Ang,PB_Power = 0,QB_Power = 0,SB_Power = 0,FactB; // B   phase电压,电流,有功功,无功功率,视在功率
		float VoltageC,VoltageC_Angle,CurrentC = 0,CurrentC_Angle,PPC_Power = 0,QQC_Power = 0,SSC_Power = 0,FactorC;	 //C  phase电流有效值	
		u32 VolC = 0,VolC_Ang,CurrC = 0,CurrC_Ang,PC_Power = 0,QC_Power = 0,SC_Power = 0,FactC; // C   phase电压,电流,有功功,无功功率,视在功率
			//检测电表
		if(ATT7022E_Check()==0)
		{
			EMU_init();    	// 电表初始化
			if(ATT7022E_Check()==0)
				return 0;			//读取数据失败
		}	
		
	HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
	Elec_Data.time.hour=RTC_TimeStruct.Hours;
	Elec_Data.time.minute=RTC_TimeStruct.Minutes;
	Elec_Data.time.second=RTC_TimeStruct.Seconds;
	Elec_Data.time.musecond=RTC_TimeStruct.SubSeconds*1000/256;
	Elec_Data.time.month=RTC_DateStruct.Month;
	Elec_Data.time.date=RTC_DateStruct.Date;
	Elec_Data.time.year=RTC_DateStruct.Year;

	freq = ReadRegData(Reg_Freq); 
	frequence = (float)(freq/8192.0)	; 
	Elec_Data.frequence=frequence;	
		
/*************读取寄存器内的值******************************/	  //
		VolA = ReadRegData(Reg_VolA);  //Vp2 A电压 ,
	  CurrA = ReadRegData(Reg_CurrA);   //Vp5 C电压  Vp1 A电压
	  PA_Power = ReadRegData(Reg_PA_Power);  //A phase有功功率
	  QA_Power = ReadRegData(Reg_QA_Power);  //A phase无功功率
	  SA_Power = ReadRegData(Reg_SA_Power);  //A phase视在功率
	  FactA = ReadRegData(Reg_PowerA_Factor); //A phase功率因数
		VolA_Ang = ReadRegData(Reg_UA_Ang);				//A phase电压相角
		CurrA_Ang = ReadRegData(Reg_IA_Ang);				//A phase电流相角
		
		VolB = ReadRegData(Reg_VolB);  //Vp4 B电压 ,
	  CurrB = ReadRegData(Reg_CurrB);   //Vp5 C电压  Vp1 A电压	  VP3 B
	  PB_Power = ReadRegData(Reg_PB_Power);  //A phase有功功率
	  QB_Power = ReadRegData(Reg_QB_Power);  //A phase无功功率
	  SB_Power = ReadRegData(Reg_SB_Power);  //A phase视在功率
		FactB = ReadRegData(Reg_PowerB_Factor); //A phase功率因数		
		VolB_Ang = ReadRegData(Reg_UB_Ang);				//A phase电压相角
		CurrB_Ang = ReadRegData(Reg_IB_Ang);				//A phase电流相角
				
	  VolC = ReadRegData(Reg_VolC);  //Vp4 C电压 ,
	  CurrC = ReadRegData(Reg_CurrC);  //Vp5 C电压  Vp1 A电压	  VP3 B
	  PC_Power = ReadRegData(Reg_PC_Power);  //C phase有功功率
	  QC_Power = ReadRegData(Reg_QC_Power);  //C phase无功功率
	  FactC = ReadRegData(Reg_PowerC_Factor); //C phase功率因数	
		VolC_Ang = ReadRegData(Reg_UC_Ang);				//A phase电压相角
		CurrC_Ang = ReadRegData(Reg_IC_Ang);				//A phase电流相角
				
		getFFT(&Elec_Data.Harm);				
/*************数据类型转换*****************************/	  //			

		VoltageA = (float)(VolA/8192.0*U_K)	;   //电压有效值   	????							
		VoltageB = (float)(VolB/8192.0*U_K)	;   //电压有效值   	????							
		VoltageC = (float)(VolC/8192.0*U_K)	;   //电压有效值   	????							
	  
		CurrentA = ((float)CurrA/8192.0)*I_K;   //A phase电流有效值
		CurrentB = ((float)CurrB/8192.0)*I_K;   //A phase电流有效值
		CurrentC = ((float)CurrC/8192.0)*I_K;   //A phase电流有效值
	  
		if(PA_Power > 8388608) //         A phase有功功率
	     PPA_Power = ((16777216.0-(PA_Power))*P_K);//	
	  else 
	    PPA_Power =  (PA_Power)*P_K;			
		if(PB_Power > 8388608) //      
	     PPB_Power = ((16777216.0-(PB_Power))*P_K);//	
	  else 
	    PPB_Power =  (PB_Power)*P_K;				
		if(PC_Power > 8388608) //   
	     PPC_Power = ((16777216.0-(PC_Power))*P_K);//	
	  else 
	    PPC_Power =  (PC_Power)*P_K;		 
			
	  if (QA_Power > 8388608)	 //         A phase无功功率
			QQA_Power = ((16777216.0-QA_Power)*P_K);
		else 
			QQA_Power = (QA_Power*P_K);
	  if (QB_Power > 8388608)	 //
			QQB_Power = ((16777216.0-QB_Power)*P_K);
		else 
			QQB_Power = (QB_Power*P_K);	  
		if (QC_Power > 8388608)	 //
			QQC_Power = ((16777216.0-QC_Power)*P_K);
		else 
			QQC_Power = (QC_Power*P_K);
				
		if (SA_Power> 8388608)	//         A phase视在功功率
			SSA_Power = ((16777216.0-SA_Power)*P_K);
		else 
			SSA_Power = (SA_Power*P_K);				
		if (SB_Power> 8388608)	//     
			SSB_Power = ((16777216.0-SB_Power)*P_K);
		else 
			SSB_Power = (SB_Power*P_K);				
		if (SC_Power> 8388608)	//     
			SSC_Power = ((16777216.0-SC_Power)*P_K);
		else 
			SSC_Power = (SC_Power*P_K);		
		

	  if(FactA > 8388608)//         A phase功率因数
	  	FactorA =(float) 	((FactA-16777216.0)/8388608.0);
	  else 
			FactorA = (float) (FactA /8388608.0);		
	  if(FactB > 8388608)
	  	FactorB =(float) 	((FactB-16777216.0)/8388608.0);
	  else 
			FactorB = (float) (FactB /8388608.0);		
		if(FactC > 8388608)
	  	FactorC =(float) 	((FactC-16777216.0)/8388608.0);
	  else 
			FactorC = (float) (FactC /8388608.0);
				
//		if(VolA_Ang > 1048576)
//			VoltageA_Angle = (float)((VolA_Ang-16777216.0)/1048576.0*180);
//		else
			VoltageA_Angle = (float)(VolA_Ang/1048576.0*180.0);
//		if(VolB_Ang > 1048576)
//			VoltageB_Angle = (float)((VolB_Ang-16777216.0)/1048576.0*180);
//		else
			VoltageB_Angle = (float)(VolB_Ang/1048576.0*180.0);
//		if(VolC_Ang > 1048576)
//			VoltageC_Angle = (float)((VolC_Ang-16777216.0)/1048576.0*180);
//		else
			VoltageC_Angle = (float)(VolC_Ang/1048576.0*180.0);

		if(CurrA > 1048576)
			CurrentA_Angle = (float)((CurrA-16777216.0)/1048576.0*180);
		else
			CurrentA_Angle = (float)(CurrA/1048576.0*180.0);
		if(CurrB > 1048576)
			CurrentB_Angle = (float)((CurrB-16777216.0)/1048576.0*180);
		else
			CurrentB_Angle = (float)(CurrB/1048576.0*180.0);
		if(CurrC > 1048576)
			CurrentC_Angle = (float)((CurrC-16777216.0)/1048576.0*180);
		else
			CurrentC_Angle = (float)(CurrC/1048576.0*180.0);

//		if(CurrentA_Angle>0)
//			CurrentA_Angle=CurrentA_Angle-360.0;
//		VoltageC_Angle=-VoltageA_Angle;
//		VoltageB_Angle=VoltageC_Angle-VoltageB_Angle;
//		CurrentA_Angle=VoltageC_Angle-CurrentA_Angle;
//		CurrentB_Angle=VoltageC_Angle+CurrentB_Angle-360.0;
//		CurrentC_Angle=VoltageC_Angle+CurrentC_Angle;
//		VoltageA_Angle=0;
	/**************上传数据******************************/		
		Elec_Data.Ammer_Parament[PhaseA].Up_Vol=VoltageA;
		Elec_Data.Ammer_Parament[PhaseA].Up_Current=CurrentA;
		Elec_Data.Ammer_Parament[PhaseA].Up_Vol_Angle=VoltageA_Angle;	
		Elec_Data.Ammer_Parament[PhaseA].Up_Current_Angle=CurrentA_Angle;	
		Elec_Data.Ammer_Parament[PhaseA].Up_P_Power=PPA_Power;
		Elec_Data.Ammer_Parament[PhaseA].Up_Q_Power=QQA_Power;	
		Elec_Data.Ammer_Parament[PhaseA].Up_S_Power=SSA_Power;	
		Elec_Data.Ammer_Parament[PhaseA].Factor=FactorA;

		Elec_Data.Ammer_Parament[PhaseB].Up_Vol=VoltageB;
		Elec_Data.Ammer_Parament[PhaseB].Up_Current=CurrentB;
		Elec_Data.Ammer_Parament[PhaseB].Up_Vol_Angle=VoltageB_Angle;	
		Elec_Data.Ammer_Parament[PhaseB].Up_Current_Angle=CurrentB_Angle;	
		Elec_Data.Ammer_Parament[PhaseB].Up_P_Power=PPB_Power;
		Elec_Data.Ammer_Parament[PhaseB].Up_Q_Power=QQB_Power;	
		Elec_Data.Ammer_Parament[PhaseB].Up_S_Power=SSB_Power;	
		Elec_Data.Ammer_Parament[PhaseB].Factor=FactorB;

	  Elec_Data.Ammer_Parament[PhaseC].Up_Vol=VoltageC;
		Elec_Data.Ammer_Parament[PhaseC].Up_Current=CurrentC;
		Elec_Data.Ammer_Parament[PhaseC].Up_Vol_Angle=VoltageC_Angle;	
		Elec_Data.Ammer_Parament[PhaseC].Up_Current_Angle=CurrentC_Angle;	
		Elec_Data.Ammer_Parament[PhaseC].Up_P_Power=PPC_Power;
		Elec_Data.Ammer_Parament[PhaseC].Up_Q_Power=QQC_Power;	
		Elec_Data.Ammer_Parament[PhaseC].Up_S_Power=SSC_Power;	
		Elec_Data.Ammer_Parament[PhaseC].Factor=FactorC;
		
		return 1;	//读取数据成功
}
///******************************************************/
///**************************************************************/
u8 ReadSend_ALL_AmmeterData(void)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	u8 tbuf[40],t;
	u32 DATA[4];
	float data[4],temp;
	
	HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
	
	printf("\r\nTime:%02d:%02d:%02d:%03d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds,RTC_TimeStruct.SubSeconds*1000/256);		
	printf("\r\nDate:20%02d-%02d-%02d",RTC_DateStruct.Year,RTC_DateStruct.Month,RTC_DateStruct.Date);		
	printf("\r\nWeek:%d",RTC_DateStruct.WeekDay);		
	
	printf("\r\n电压有效值数据:\t");	
	DATA[0] = ReadRegData(Reg_VolA);  
	DATA[1] = ReadRegData(Reg_VolB);  
	DATA[2] = ReadRegData(Reg_VolC);
	DATA[3] = ReadRegData(Reg_Vol0);
	data[0] = (float)(DATA[0]/8192.0)*U_K	; 
	data[1] = (float)(DATA[1]/8192.0)*U_K	; 	
	data[2] = (float)(DATA[2]/8192.0)*U_K	; 
	data[3] = (float)(DATA[3]/8192.0)*U_K	; 
	data[3] =2*data[3];
	printf("A phase: %f\tB phase: %f\tC phase: %f\t0 phase: %f",data[0],data[1],data[2],data[3]);	
	
	printf("\r\n电流有效值数据:\t");	
	DATA[0] = ReadRegData(Reg_CurrA);  
	DATA[1] = ReadRegData(Reg_CurrB);  
	DATA[2] = ReadRegData(Reg_CurrC);
	DATA[3] = ReadRegData(Reg_Curr0);
	for(t=0;t<4;t++)
	{
		data[t] = (float)(DATA[t]/8192.0)*I_K	; 
	}	
	data[3] =2*data[3];
	printf("A phase: %f\tB phase: %f\tC phase: %f\t0 phase: %f",data[0],data[1],data[2],data[3]);	
	
	printf("\r\n有功功率数据:\t");	
	DATA[0] = ReadRegData(Reg_PA_Power);  
	DATA[1] = ReadRegData(Reg_PB_Power);  
	DATA[2] = ReadRegData(Reg_PC_Power);
	DATA[3] = ReadRegData(Reg_P0_Power);
	for(t=0;t<4;t++)
	{
		if(DATA[t] > 8388608)  
		 data[t] = (float)((16777216 - DATA[t] )*P_K);
		else 
		 data[t] = (float) (DATA[t]*P_K);
	}
	data[3] =2*data[3];
	printf("A phase: %f\tB phase: %f\tC phase: %f\t0 phase: %f",data[0],data[1],data[2],data[3]);	
	
	printf("\r\n无功功率数据:\t");	
	DATA[0] = ReadRegData(Reg_QA_Power);  
	DATA[1] = ReadRegData(Reg_QB_Power);  
	DATA[2] = ReadRegData(Reg_QC_Power);
	DATA[3] = ReadRegData(Reg_Q0_Power);
	for(t=0;t<4;t++)
	{
		if(DATA[t] > 8388608)  
		 data[t] = (float)((16777216 - DATA[t] )*P_K);
		else 
		 data[t] = (float) (DATA[t]*P_K);
	}
	data[3] =2*data[3];
	printf("A phase: %f\tB phase: %f\tC phase: %f\t0 phase: %f",data[0],data[1],data[2],data[3]);	

	printf("\r\n视在功率数据:\t");	
	DATA[0] = ReadRegData(Reg_SA_Power);  
	DATA[1] = ReadRegData(Reg_SB_Power);  
	DATA[2] = ReadRegData(Reg_SC_Power);
	DATA[3] = ReadRegData(Reg_S0_Power);
	for(t=0;t<4;t++)
	{
		if(DATA[t] > 8388608)  
		 data[t] = (float)((16777216 - DATA[t] )*P_K);
		else 
		 data[t] = (float) (DATA[t]*P_K);
	}
	data[3] =2*data[3];
	printf("A phase: %f\tB phase: %f\tC phase: %f\t0 phase: %f",data[0],data[1],data[2],data[3]);	

	printf("\r\n功率因数数据:\t");	
	DATA[0] = ReadRegData(Reg_PowerA_Factor);  
	DATA[1] = ReadRegData(Reg_PowerB_Factor);  
	DATA[2] = ReadRegData(Reg_PowerC_Factor);
	DATA[3] = ReadRegData(Reg_Power0_Factor);
	for(t=0;t<4;t++)
	{
		if(DATA[t] > 8388608)  
		 data[t] = (float)((16777216 - DATA[t] )/8388608);
		else 
		 data[t] = (float) (DATA[t]/8388608);
	}
	printf("A phase: %f\tB phase: %f\tC phase: %f\t0 phase: %f",data[0],data[1],data[2],data[3]);	

	
	printf("\r\n电压相角\r\n");
	DATA[0] = ReadRegData(Reg_UA_Ang);  
	DATA[1] = ReadRegData(Reg_UB_Ang);  
	DATA[2] = ReadRegData(Reg_UC_Ang);
	for(t=0;t<3;t++)
	{
//		if(DATA[t] >= 1048576)  
//		 data[t] = (float)(DATA[t]-16777216.0)/1048576.0*180;
//		else 
		 data[t] = (float) (DATA[t]/1048576.0*180.0);
	}
//	data[2] = -data[0];
//	data[1] = data[2]-data[1];		
//	data[0] =0;
//	temp=data[2];
		printf("A phase: %f\tB phase: %f\tC phase: %f",data[0],data[1],data[2]);	

	printf("\r\n电流相角\r\n");
	DATA[0] = ReadRegData(Reg_IA_Ang);  
	DATA[1] = ReadRegData(Reg_IB_Ang);  
	DATA[2] = ReadRegData(Reg_IC_Ang);
	for(t=0;t<3;t++)
	{
		if(DATA[t] >= 1048576)  
		 data[t] = (float)(DATA[t]-16777216.0)/1048576.0*180;
		else 
		 data[t] = (float) (DATA[t]/1048576.0*180.0);
	}
	if(data[0]>0)
		data[0]=data[0]-360;
	
//	data[0] = temp-data[0];
//	data[1] = temp+data[1]-360;
//	data[2] = temp+data[2];
	printf("A phase: %f\tB phase: %f\tC phase: %f",data[0],data[1],data[2]);		
	
	
	printf("\r\n基波有功功率数据:\t");	
	DATA[0] = ReadRegData(Reg_Funda_PA_Power);  
	DATA[1] = ReadRegData(Reg_Funda_PB_Power);  
	DATA[2] = ReadRegData(Reg_Funda_PC_Power);
	DATA[3] = ReadRegData(Reg_Funda_P0_Power);
	for(t=0;t<4;t++)
	{
		if(DATA[t] > 8388608)  
		 data[t] = (float)((16777216 - DATA[t] )*P_K);
		else 
		 data[t] = (float) (DATA[t]*P_K);
	}
	data[3] =2*data[3];
	printf("A phase: %f\tB phase: %f\tC phase: %f\t0 phase: %f",data[0],data[1],data[2],data[3]);	
		
	printf("\r\n基波无功功率数据:\t");	
	DATA[0] = ReadRegData(Reg_Funda_QA_Power);  
	DATA[1] = ReadRegData(Reg_Funda_QB_Power);  
	DATA[2] = ReadRegData(Reg_Funda_QC_Power);
	DATA[3] = ReadRegData(Reg_Funda_Q0_Power);
	for(t=0;t<4;t++)
	{
		if(DATA[t] > 8388608)  
		 data[t] = (float)((16777216 - DATA[t] )*P_K);
		else 
		 data[t] = (float) (DATA[t]*P_K);
	}
	data[3] =2*data[3];
	printf("A phase: %f\tB phase: %f\tC phase: %f\t0 phase: %f",data[0],data[1],data[2],data[3]);	
		
	printf("\r\n基波电压有效值数据:\t");	
	DATA[0] = ReadRegData(Reg_Funda_VolA);  
	DATA[1] = ReadRegData(Reg_Funda_VolB);  
	DATA[2] = ReadRegData(Reg_Funda_VolC);
	for(t=0;t<3;t++)
	{
		data[t] = (float)(DATA[t]/8192.0)	; 
	}
	printf("A phase: %f\tB phase: %f\tC phase: %f\t",data[0],data[1],data[2]);	

	printf("\r\n基波电流有效值数据:\t");	
	DATA[0] = ReadRegData(Reg_Funda_CurrA);  
	DATA[1] = ReadRegData(Reg_Funda_CurrB);  
	DATA[2] = ReadRegData(Reg_Funda_CurrC);
	for(t=0;t<3;t++)
	{
		data[t] = (float)(DATA[t]/8192.0)	; 
	}
	printf("A phase: %f\tB phase: %f\tC phase: %f\t",data[0],data[1],data[2]);	

	printf("\r\n电压最大值数据:\t");	
	DATA[0] = ReadRegData(Reg_Funda_PeakVolA);  
	DATA[1] = ReadRegData(Reg_Funda_PeakVolB);  
	DATA[2] = ReadRegData(Reg_Funda_PeakVolC);
	for(t=0;t<3;t++)
	{
		data[t] = (float)(DATA[t]/8192.0)	; 
	}
	printf("A phase: %f\tB phase: %f\tC phase: %f\t",data[0],data[1],data[2]);	
		return 1;	//读取数据成功	
}