#include "realtime.h"
#include "rtc.h"
#include "bdgps.h"
#include "timer.h"
//返回时间函数

Time_Struct getrealtime(void)
{
	Time_Struct ttt;
	RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
	HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
	ttt.year=RTC_DateStruct.Year;
	ttt.month=RTC_DateStruct.Month;
	ttt.date=RTC_DateStruct.Date;
	ttt.hour=RTC_TimeStruct.Hours;
	ttt.minute=RTC_TimeStruct.Minutes;
	ttt.second=RTC_TimeStruct.Seconds;
//	ttt.musecond=getmusec();
	ttt.musecond=RTC_TimeStruct.SubSeconds*1000/256;
	return ttt;
}
/////设置时间函数
void setrealtime(u8 year,u8 month,u8 date,u8 hour,u8 minute,u8 second,u32 musecond)
{
	RTC_Set_Time(hour,minute,second,musecond*256/1000);
	RTC_Set_Date(year,month,date);
}
