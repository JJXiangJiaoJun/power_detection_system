#include "ATT7022E.h"
#include "GetFFT.h"
#include "math.h"
#include "arm_math.h"
#include "spi.h"
#include <string.h>
#include <stdlib.h>
#include "lcd.h"
#include "delay.h"

#define FFT_LENGTH		64 		//FFT长度，64
//struct Harmonic_StructureDef Harmonic;
float GAIN[22]={
1.00000000000000,
1.00000000000000,
1.00362187060665,
1.00969162604172,
1.01825901332331,
1.02939520355364,
1.04319331488342,
1.05977378492696,
1.07927443401769,
1.10187021533519,
1.12776405100967,
1.15718387269867,
1.19042304659018,
1.22777858879375,
1.26962990669109,
1.31639900106378,
1.36856044411429,
1.42671649331348,
1.49149037623292,
1.56363705732262,
1.64398947583697,
1.73366802372421
};	
 
 
void getFFT(Harmonic_StructureDef *Harmonic)
{
    arm_cfft_radix4_instance_f32 scfft;
 	u8 key;
	float time; 
	u8 buf[50];
 //64个点
	float FFT_UA_INData[FFT_LENGTH*2];
	float FFT_UA_OUTData[FFT_LENGTH];
	float FFT_UB_INData[FFT_LENGTH*2];
	float FFT_UB_OUTData[FFT_LENGTH];
	float FFT_UC_INData[FFT_LENGTH*2];
	float FFT_UC_OUTData[FFT_LENGTH];
	float FFT_IA_INData[FFT_LENGTH*2];
	float FFT_IA_OUTData[FFT_LENGTH];
	float FFT_IB_INData[FFT_LENGTH*2];
	float FFT_IB_OUTData[FFT_LENGTH];
	float FFT_IC_INData[FFT_LENGTH*2];
	float FFT_IC_OUTData[FFT_LENGTH];
	float FFT_IN_INData[FFT_LENGTH*2];
	float FFT_IN_OUTData[FFT_LENGTH];
	
	u8 t,i,temp;
	u32 tt=0;
	arm_cfft_radix4_init_f32(&scfft,FFT_LENGTH,0,1);//初始化scfft结构体，设定FFT相关参数

	Write_Att7022(0xC9, 0x00005A);		//校表数据写使能
	Write_Att7022(0xC5, 0x000002);		//自动方式  启动同步数据缓存
//		Write_Att7022(0xC9, 0x00005B);		//校表数据写使能
	
	delay_ms(50);	//等待缓冲数据
	
	t=0;tt=0;i=0;
	for(;tt<7*FFT_LENGTH;tt++)
	{
		temp=READ_Att7022(0X7F);
		switch(t)
		{//虚部全部为0
			case 0:	
//							if(temp>262144)
//								FFT_UA_INData[2*i]=(float)temp-16777216;
//							else
								FFT_UA_INData[2*i]=(float)temp;
							FFT_UA_INData[2*i+1]=0.0;break;
			case 1:	
//							if(temp>262144)
//								FFT_UB_INData[2*i]=(float)temp-16777216;
//							else
								FFT_UB_INData[2*i]=(float)temp;
							FFT_UB_INData[2*i+1]=0.0;break;
			case 2:	
//							if(temp>262144)
//								FFT_UC_INData[2*i]=(float)temp-16777216;
//							else
								FFT_UC_INData[2*i]=(float)temp;
							FFT_UC_INData[2*i+1]=0.0;break;
			case 3:	
//							if(temp>262144)
//								FFT_IA_INData[2*i]=(float)temp-16777216;
//							else
								FFT_IA_INData[2*i]=(float)temp;
							FFT_IA_INData[2*i+1]=0.0;break;
			case 4:	
//							if(temp>262144)
//								FFT_IB_INData[2*i]=(float)temp-16777216;
//							else
								FFT_IB_INData[2*i]=(float)temp;
							FFT_IB_INData[2*i+1]=0.0;break;
			case 5:	
//							if(temp>262144)
//								FFT_IC_INData[2*i]=(float)temp-16777216;
//							else
								FFT_IC_INData[2*i]=(float)temp;
							FFT_IC_INData[2*i+1]=0.0;break;
			case 6:
//							if(temp>262144)
//								FFT_IN_INData[2*i]=(float)temp-16777216;
//							else
								FFT_IN_INData[2*i]=(float)temp;
							FFT_IN_INData[2*i+1]=0.0;break;
			default:break;
		}
		t++;
		if(t==7)
		{
			t=0;
			i++;
		}
	}
	for(i=0;i<FFT_LENGTH;i++)
	{
			FFT_UA_INData[2*i]=(float)100+    10*arm_sin_f32(2*PI*i/FFT_LENGTH*50)+
																30*arm_sin_f32(2*PI*i*4/FFT_LENGTH*50)+
																50*arm_cos_f32(2*PI*i*8/FFT_LENGTH*50);
			FFT_IN_INData[2*i+1]=0.0;
		}
//		Write_Att7022(0xC9, 0x00005A);		//校表数据写使能
	Write_Att7022(0xC5, 0x000000);		//自动方式  关闭同步数据缓存
		Write_Att7022(0xC9, 0x00005B);		//校表数据写使能		
	
	arm_cfft_radix4_f32(&scfft,FFT_UA_INData);	//FFT计算（基4）
	arm_cmplx_mag_f32(FFT_UA_INData,FFT_UA_OUTData,FFT_LENGTH);	//把运算结果复数求模得幅值 
	for(i=0;i<22;i++)
	{
		Harmonic->UA_Harmonic[i]=FFT_UA_OUTData[i];
//		Harmonic->UA_Harmonic[i]=FFT_UA_OUTData[i] * GAIN[i];
	}
	
	arm_cfft_radix4_f32(&scfft,FFT_UB_INData);	//FFT计算（基4）
	arm_cmplx_mag_f32(FFT_UB_INData,FFT_UB_OUTData,FFT_LENGTH);	//把运算结果复数求模得幅值 
	for(i=0;i<22;i++)
	{
		Harmonic->UB_Harmonic[i]=FFT_UB_OUTData[i] ;
//		Harmonic->UB_Harmonic[i]=FFT_UB_OUTData[i] * GAIN[i];
	}
	
	arm_cfft_radix4_f32(&scfft,FFT_UC_INData);	//FFT计算（基4）
	arm_cmplx_mag_f32(FFT_UC_INData,FFT_UC_OUTData,FFT_LENGTH);	//把运算结果复数求模得幅值 
	for(i=0;i<22;i++)
	{
		Harmonic->UC_Harmonic[i]=FFT_UC_OUTData[i] ;
//		Harmonic->UC_Harmonic[i]=FFT_UC_OUTData[i] * GAIN[i];
	}

	arm_cfft_radix4_f32(&scfft,FFT_IA_INData);	//FFT计算（基4）
	arm_cmplx_mag_f32(FFT_IA_INData,FFT_IA_OUTData,FFT_LENGTH);	//把运算结果复数求模得幅值 
	for(i=0;i<22;i++)
	{
		Harmonic->IA_Harmonic[i]=FFT_IA_OUTData[i];
//		Harmonic->IA_Harmonic[i]=FFT_IA_OUTData[i] * GAIN[i];
	}

	arm_cfft_radix4_f32(&scfft,FFT_IB_INData);	//FFT计算（基4）
	arm_cmplx_mag_f32(FFT_IB_INData,FFT_IB_OUTData,FFT_LENGTH);	//把运算结果复数求模得幅值 
	for(i=0;i<22;i++)
	{
//				Harmonic->IB_Harmonic[i]=FFT_IB_OUTData[i] * GAIN[i];
		Harmonic->IB_Harmonic[i]=FFT_IB_OUTData[i] ;
	}

	arm_cfft_radix4_f32(&scfft,FFT_IC_INData);	//FFT计算（基4）
	arm_cmplx_mag_f32(FFT_IC_INData,FFT_IC_OUTData,FFT_LENGTH);	//把运算结果复数求模得幅值 
	for(i=0;i<22;i++)
	{
		Harmonic->IC_Harmonic[i]=FFT_IC_OUTData[i] ;
//		Harmonic->IC_Harmonic[i]=FFT_IC_OUTData[i] * GAIN[i];
	}	
}