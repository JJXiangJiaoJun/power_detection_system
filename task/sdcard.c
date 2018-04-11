#include "sdcard.h"
#include "sdmmc_sdcard.h"
#include "led.h"
#include "totaltask.h"
#include "ATT7022E.h"

/****************任务控制块与任务堆栈定义**************/
//任务7
OS_TCB   SDcardTask_TCB;
__align(8) CPU_STK  SDCARD_TASK_STK[SDCARD_STK_SIZE];
/**********同步用信号量*****/
OS_SEM SDcardWrite;

/********************内部函数**************************/

/*------------------------------------------------------------
函数说明：将传送来的时间、电压、电流、数据格式化输出到SD卡中
参数说明：buffer:接收数据缓冲区  demo:传送过来的数据包   index:1/2/3分别表示A/B/C三相
返回值： 输出的数据长度
--------------------------------------------------------------*/

static u32 Format_Output(u8*buffer ,struct Data_StructureDef *demo,u8 index)        
{
   u32  length=0;
	u8   i=0;
   length  = sprintf( (char*)buffer,"20%02d-%02d-%02d    ",demo->time.year,demo->time.month,demo->time.date);
   
	length += sprintf( (char*)buffer + length, "%02d:%02d:%02d:%03d   ",\
	demo->time.hour,demo->time.minute,demo->time.second,demo->time.musecond); 
   
	length += sprintf( (char*)buffer + length, "%-16.5f", demo->frequence );
   
	length += sprintf( (char*)buffer + length, "%-16.5f", demo->Ammer_Parament[index-1].Factor );
   length += sprintf( (char*)buffer + length, "%-16.5f", demo->Ammer_Parament[index-1].Up_Vol );
   length += sprintf( (char*)buffer + length, "%-16.5f", demo->Ammer_Parament[index-1].Up_Current);
   length += sprintf( (char*)buffer + length, "%-16.5f", demo->Ammer_Parament[index-1].Up_Vol_Angle );
   length += sprintf( (char*)buffer + length, "%-16.5f", demo->Ammer_Parament[index-1].Up_Current_Angle );
   length += sprintf( (char*)buffer + length, "%-16.5f", demo->Ammer_Parament[index-1].Up_P_Power );
	length += sprintf( (char*)buffer + length, "%-16.5f", demo->Ammer_Parament[index-1].Up_Q_Power );
	length += sprintf( (char*)buffer + length, "%-16.5f", demo->Ammer_Parament[index-1].Up_S_Power );
	

	switch(index)
	{	
		case 1:
		 for(i=0;i<22;i++)
			length += sprintf( (char*)buffer + length, "%-16.5f", demo->Harm.UA_Harmonic[i] );
		 for(i=0;i<22;i++)
			length += sprintf( (char*)buffer + length, "%-16.5f", demo->Harm.IA_Harmonic[i] );
		 break;
		case 2:
		 for(i=0;i<22;i++)
			length += sprintf( (char*)buffer + length, "%-16.5f", demo->Harm.UB_Harmonic[i] );
		 for(i=0;i<22;i++)
			length += sprintf( (char*)buffer + length, "%-16.5f", demo->Harm.IB_Harmonic[i] );
		 break;
		case 3:
		 for(i=0;i<22;i++)
			length += sprintf( (char*)buffer + length, "%-16.5f", demo->Harm.UC_Harmonic[i] );
		 for(i=0;i<22;i++)
			length += sprintf( (char*)buffer + length, "%-16.5f", demo->Harm.IC_Harmonic[i] );
       break;
      default:break;		
	}
 
	length+=sprintf( (char*)buffer + length, "\r\n" );
   
	return length;
}

/*------------------------------------------------------------
函数说明：SD卡文件更新时的标题数据
参数说明：buffer:接收数据缓冲区  data:传送过来的数据包  index:1/2/3表示A/B/C三相数据
返回值： 输出的数据长度
--------------------------------------------------------------*/

static u32 Title_Output(u8*buffer ,struct Data_StructureDef *data,u8 index)        
{
   u32  length=0;
	
	length  = sprintf( (char*)buffer,"ID: %d%d%d\r\n",data->Device_ID[0],data->Device_ID[1],data->Device_ID[2]);
   length += sprintf( (char*)buffer + length, "Date          "); 
   length += sprintf( (char*)buffer + length, "Time        ");
   length += sprintf( (char*)buffer + length, "  Frequence    ");
	
	switch(index)
 {
	
	case 1:		
   
	length += sprintf( (char*)buffer + length, "    A:Factor    ");
   length += sprintf( (char*)buffer + length, "      A:Vol     ");
   length += sprintf( (char*)buffer + length, "    A:Current   ");
	length += sprintf( (char*)buffer + length, "   A:Vol_Angle  ");
	length += sprintf( (char*)buffer + length, " A:Current_Angle");
	length += sprintf( (char*)buffer + length, "    A:P_Power   ");
	length += sprintf( (char*)buffer + length, "    A:Q_Power   ");
	length += sprintf( (char*)buffer + length, "    A:S_Power   ");
   
	
	length += sprintf( (char*)buffer + length, "    UA:Har[1]   ");
  	length += sprintf( (char*)buffer + length, "    UA:Har[2]   ");	
	length += sprintf( (char*)buffer + length, "    UA:Har[3]   ");
  	length += sprintf( (char*)buffer + length, "    UA:Har[4]   ");
	length += sprintf( (char*)buffer + length, "    UA:Har[5]   ");
  	length += sprintf( (char*)buffer + length, "    UA:Har[6]   ");	
	length += sprintf( (char*)buffer + length, "    UA:Har[7]   ");
  	length += sprintf( (char*)buffer + length, "    UA:Har[8]   ");		
	length += sprintf( (char*)buffer + length, "    UA:Har[9]   ");
  	length += sprintf( (char*)buffer + length, "    UA:Har[10]  ");	
	length += sprintf( (char*)buffer + length, "    UA:Har[11]  ");
  	length += sprintf( (char*)buffer + length, "    UA:Har[12]  ");		
	length += sprintf( (char*)buffer + length, "    UA:Har[13]  ");
  	length += sprintf( (char*)buffer + length, "    UA:Har[14]  ");	
	length += sprintf( (char*)buffer + length, "    UA:Har[15]  ");
  	length += sprintf( (char*)buffer + length, "    UA:Har[16]  ");	
	length += sprintf( (char*)buffer + length, "    UA:Har[17]  ");
  	length += sprintf( (char*)buffer + length, "    UA:Har[18]  ");	
	length += sprintf( (char*)buffer + length, "    UA:Har[19]  ");
  	length += sprintf( (char*)buffer + length, "    UA:Har[20]  ");	
	length += sprintf( (char*)buffer + length, "    UA:Har[21]  ");
  	length += sprintf( (char*)buffer + length, "    UA:Har[22]  ");
	
	length += sprintf( (char*)buffer + length, "    IA:Har[1]   ");
  	length += sprintf( (char*)buffer + length, "    IA:Har[2]   ");	
	length += sprintf( (char*)buffer + length, "    IA:Har[3]   ");
  	length += sprintf( (char*)buffer + length, "    IA:Har[4]   ");
	length += sprintf( (char*)buffer + length, "    IA:Har[5]   ");
  	length += sprintf( (char*)buffer + length, "    IA:Har[6]   ");	
	length += sprintf( (char*)buffer + length, "    IA:Har[7]   ");
  	length += sprintf( (char*)buffer + length, "    IA:Har[8]   ");		
	length += sprintf( (char*)buffer + length, "    IA:Har[9]   ");
  	length += sprintf( (char*)buffer + length, "    IA:Har[10]  ");	
	length += sprintf( (char*)buffer + length, "    IA:Har[11]  ");
  	length += sprintf( (char*)buffer + length, "    IA:Har[12]  ");		
	length += sprintf( (char*)buffer + length, "    IA:Har[13]  ");
  	length += sprintf( (char*)buffer + length, "    IA:Har[14]  ");	
	length += sprintf( (char*)buffer + length, "    IA:Har[15]  ");
  	length += sprintf( (char*)buffer + length, "    IA:Har[16]  ");	
	length += sprintf( (char*)buffer + length, "    IA:Har[17]  ");
  	length += sprintf( (char*)buffer + length, "    IA:Har[18]  ");	
	length += sprintf( (char*)buffer + length, "    IA:Har[19]  ");
  	length += sprintf( (char*)buffer + length, "    IA:Har[20]  ");	
	length += sprintf( (char*)buffer + length, "    IA:Har[21]  ");
  	length += sprintf( (char*)buffer + length, "    IA:Har[22]  \r\n");
   break;

  case 2:
	length += sprintf( (char*)buffer + length, "    B:Factor    ");
   length += sprintf( (char*)buffer + length, "      B:Vol     ");
   length += sprintf( (char*)buffer + length, "    B:Current   ");
	length += sprintf( (char*)buffer + length, "   B:Vol_Angle  ");
	length += sprintf( (char*)buffer + length, " B:Current_Angle");
	length += sprintf( (char*)buffer + length, "    B:P_Power   ");
	length += sprintf( (char*)buffer + length, "    B:Q_Power   ");
	length += sprintf( (char*)buffer + length, "    B:S_Power   ");
  
   
   length += sprintf( (char*)buffer + length, "    UB:Har[1]   ");
  	length += sprintf( (char*)buffer + length, "    UB:Har[2]   ");	
	length += sprintf( (char*)buffer + length, "    UB:Har[3]   ");
  	length += sprintf( (char*)buffer + length, "    UB:Har[4]   ");
	length += sprintf( (char*)buffer + length, "    UB:Har[5]   ");
  	length += sprintf( (char*)buffer + length, "    UB:Har[6]   ");	
	length += sprintf( (char*)buffer + length, "    UB:Har[7]   ");
  	length += sprintf( (char*)buffer + length, "    UB:Har[8]   ");		
	length += sprintf( (char*)buffer + length, "    UB:Har[9]   ");
  	length += sprintf( (char*)buffer + length, "    UB:Har[10]  ");	
	length += sprintf( (char*)buffer + length, "    UB:Har[11]  ");
  	length += sprintf( (char*)buffer + length, "    UB:Har[12]  ");		
	length += sprintf( (char*)buffer + length, "    UB:Har[13]  ");
  	length += sprintf( (char*)buffer + length, "    UB:Har[14]  ");	
	length += sprintf( (char*)buffer + length, "    UB:Har[15]  ");
  	length += sprintf( (char*)buffer + length, "    UB:Har[16]  ");	
	length += sprintf( (char*)buffer + length, "    UB:Har[17]  ");
  	length += sprintf( (char*)buffer + length, "    UB:Har[18]  ");	
	length += sprintf( (char*)buffer + length, "    UB:Har[19]  ");
  	length += sprintf( (char*)buffer + length, "    UB:Har[20]  ");	
	length += sprintf( (char*)buffer + length, "    UB:Har[21]  ");
  	length += sprintf( (char*)buffer + length, "    UB:Har[22]  ");
	
	length += sprintf( (char*)buffer + length, "    IB:Har[1]   ");
  	length += sprintf( (char*)buffer + length, "    IB:Har[2]   ");	
	length += sprintf( (char*)buffer + length, "    IB:Har[3]   ");
  	length += sprintf( (char*)buffer + length, "    IB:Har[4]   ");
	length += sprintf( (char*)buffer + length, "    IB:Har[5]   ");
  	length += sprintf( (char*)buffer + length, "    IB:Har[6]   ");	
	length += sprintf( (char*)buffer + length, "    IB:Har[7]   ");
  	length += sprintf( (char*)buffer + length, "    IB:Har[8]   ");		
	length += sprintf( (char*)buffer + length, "    IB:Har[9]   ");
  	length += sprintf( (char*)buffer + length, "    IB:Har[10]  ");	
	length += sprintf( (char*)buffer + length, "    IB:Har[11]  ");
  	length += sprintf( (char*)buffer + length, "    IB:Har[12]  ");		
	length += sprintf( (char*)buffer + length, "    IB:Har[13]  ");
  	length += sprintf( (char*)buffer + length, "    IB:Har[14]  ");	
	length += sprintf( (char*)buffer + length, "    IB:Har[15]  ");
  	length += sprintf( (char*)buffer + length, "    IB:Har[16]  ");	
	length += sprintf( (char*)buffer + length, "    IB:Har[17]  ");
  	length += sprintf( (char*)buffer + length, "    IB:Har[18]  ");	
	length += sprintf( (char*)buffer + length, "    IB:Har[19]  ");
  	length += sprintf( (char*)buffer + length, "    IB:Har[20]  ");	
	length += sprintf( (char*)buffer + length, "    IB:Har[21]  ");
  	length += sprintf( (char*)buffer + length, "    IB:Har[22]  \r\n");
   
	break;
	
  case 3:
	length += sprintf( (char*)buffer + length, "    C:Factor    ");
   length += sprintf( (char*)buffer + length, "      C:Vol     ");
   length += sprintf( (char*)buffer + length, "    C:Current   ");
	length += sprintf( (char*)buffer + length, "   C:Vol_Angle  ");
	length += sprintf( (char*)buffer + length, " C:Current_Angle");
	length += sprintf( (char*)buffer + length, "    C:P_Power   ");
	length += sprintf( (char*)buffer + length, "    C:Q_Power   ");
	length += sprintf( (char*)buffer + length, "    C:S_Power   ");
  
   length += sprintf( (char*)buffer + length, "    UC:Har[1]   ");
  	length += sprintf( (char*)buffer + length, "    UC:Har[2]   ");	
	length += sprintf( (char*)buffer + length, "    UC:Har[3]   ");
  	length += sprintf( (char*)buffer + length, "    UC:Har[4]   ");
	length += sprintf( (char*)buffer + length, "    UC:Har[5]   ");
  	length += sprintf( (char*)buffer + length, "    UC:Har[6]   ");	
	length += sprintf( (char*)buffer + length, "    UC:Har[7]   ");
  	length += sprintf( (char*)buffer + length, "    UC:Har[8]   ");		
	length += sprintf( (char*)buffer + length, "    UC:Har[9]   ");
  	length += sprintf( (char*)buffer + length, "    UC:Har[10]  ");	
	length += sprintf( (char*)buffer + length, "    UC:Har[11]  ");
  	length += sprintf( (char*)buffer + length, "    UC:Har[12]  ");		
	length += sprintf( (char*)buffer + length, "    UC:Har[13]  ");
  	length += sprintf( (char*)buffer + length, "    UC:Har[14]  ");	
	length += sprintf( (char*)buffer + length, "    UC:Har[15]  ");
  	length += sprintf( (char*)buffer + length, "    UC:Har[16]  ");	
	length += sprintf( (char*)buffer + length, "    UC:Har[17]  ");
  	length += sprintf( (char*)buffer + length, "    UC:Har[18]  ");	
	length += sprintf( (char*)buffer + length, "    UC:Har[19]  ");
  	length += sprintf( (char*)buffer + length, "    UC:Har[20]  ");	
	length += sprintf( (char*)buffer + length, "    UC:Har[21]  ");
  	length += sprintf( (char*)buffer + length, "    UC:Har[22]  ");
	
	length += sprintf( (char*)buffer + length, "    IC:Har[1]   ");
  	length += sprintf( (char*)buffer + length, "    IC:Har[2]   ");	
	length += sprintf( (char*)buffer + length, "    IC:Har[3]   ");
  	length += sprintf( (char*)buffer + length, "    IC:Har[4]   ");
	length += sprintf( (char*)buffer + length, "    IC:Har[5]   ");
  	length += sprintf( (char*)buffer + length, "    IC:Har[6]   ");	
	length += sprintf( (char*)buffer + length, "    IC:Har[7]   ");
  	length += sprintf( (char*)buffer + length, "    IC:Har[8]   ");		
	length += sprintf( (char*)buffer + length, "    IC:Har[9]   ");
  	length += sprintf( (char*)buffer + length, "    IC:Har[10]  ");	
	length += sprintf( (char*)buffer + length, "    IC:Har[11]  ");
  	length += sprintf( (char*)buffer + length, "    IC:Har[12]  ");		
	length += sprintf( (char*)buffer + length, "    IC:Har[13]  ");
  	length += sprintf( (char*)buffer + length, "    IC:Har[14]  ");	
	length += sprintf( (char*)buffer + length, "    IC:Har[15]  ");
  	length += sprintf( (char*)buffer + length, "    IC:Har[16]  ");	
	length += sprintf( (char*)buffer + length, "    IC:Har[17]  ");
  	length += sprintf( (char*)buffer + length, "    IC:Har[18]  ");	
	length += sprintf( (char*)buffer + length, "    IC:Har[19]  ");
  	length += sprintf( (char*)buffer + length, "    IC:Har[20]  ");	
	length += sprintf( (char*)buffer + length, "    IC:Har[21]  ");
  	length += sprintf( (char*)buffer + length, "    IC:Har[22]  \r\n");
   break;	
  
  default:break;
  }
	
		
   return length;
}

/*------------------------------------------------------------
函数说明：将时间不同的数据存入不同的TXT文档中，文件名为时间标签
参数说明：timeyear:接收时间(年) timemon:接收时间(月) timeday:接收时间(日)
			*timebuf:文件名保存缓冲区指针  length:要写入数据长度  
			*buffer:要写入数据的指针       index:1/2/3表示A/B/C三相
返回值： 无
--------------------------------------------------------------*/

static void sdcard_output(u8 timeyear,u8 timemon,u8 timeday,u8 *timebuf,u32 length,u8 *buffer,u8 index)
{
    u8 res;
	OS_ERR err;
	//文件名格式化，文件名为时间标签
	printlcd("out_sprintf");
	OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
   switch(index) 
	{
		case 1:sprintf((char*)timebuf,"0:/A_PHASE/A20%02d-%02d-%02d.txt",timeyear,timemon,timeday);break;
		
		case 2:sprintf((char*)timebuf,"0:/B_PHASE/B20%02d-%02d-%02d.txt",timeyear,timemon,timeday);break;
		
		case 3:sprintf((char*)timebuf,"0:/C_PHASE/C20%02d-%02d-%02d.txt",timeyear,timemon,timeday);break;
		
		default:break;
	}
	//打开该文件
	printlcd("openfile");
	OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
	OSSchedLock(&err);
	res=f_open(file,(const TCHAR*)timebuf,FA_OPEN_EXISTING|FA_WRITE);
	 if(res!=0)
	  //打开文件错误
	   {
	     printlcd("can not open this file");
	   }
	  else 
	  //成功打开文件
	  {
		 f_stat((const TCHAR*)timebuf,&fileinfo);	  
		  //文件指针移动到文件尾后写入
	    f_lseek(file,fileinfo.fsize);
		 res=f_write(file,buffer,length,&bw);	
      		
	  }
		OSSchedUnlock(&err);
		printlcd("close");
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
		//关闭文件
	    f_close(file);	

}

/*------------------------------------------------------------
函数说明：创建一个新文件，文件名为时间标签
参数说明：timeyear:接收时间(年) timemon:接收时间(月) timeday:接收时间(日)
			*timebuf:文件名保存缓冲区指针  length:要写入数据长度  
			*buffer:要写入数据的指针   index:索引1/2/3分别表示ABC三相数据
返回值： 无
--------------------------------------------------------------*/

static void creat_newfile(u8 timeyear,u8 timemon,u8 timeday,u8 *timebuf,u32 length,u8 *buffer,u8 index)
{
   u8 res;
	//文件名格式化，文件名为时间标签
	u32 title_length = 0;
   switch(index) 
	{
   
	case 1:sprintf((char*)timebuf,"0:/A_PHASE/A20%02d-%02d-%02d.txt",timeyear,timemon,timeday);break;
	
	case 2:sprintf((char*)timebuf,"0:/B_PHASE/B20%02d-%02d-%02d.txt",timeyear,timemon,timeday);break;

	case 3:sprintf((char*)timebuf,"0:/C_PHASE/C20%02d-%02d-%02d.txt",timeyear,timemon,timeday);break;
	
	default:break; 
  }
	res=f_open(file,(const TCHAR*)timebuf,FA_OPEN_ALWAYS|FA_WRITE);
	if(res!=0)
  //打开文件错误
	{
	  printlcd("file create error!");
	}
  else 
  //成功打开文件
  {
	 res=f_write(file,buffer,length,&bw);
      		
  }
	//关闭文件
   f_close(file);	

}
/********************内部函数**************************/


/********************外部函数**************************/

//SD卡初始化函数
char sdcard_task_init(void *p_arg)
{
	OS_ERR err;

	
	
	OSTaskCreate((OS_TCB 	* )&SDcardTask_TCB,		//任务控制块
				 (CPU_CHAR	* )"sdcard task", 		//任务名字
                 (OS_TASK_PTR )sdcard_task, 	    //任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )SDCARD_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&SDCARD_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)SDCARD_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)SDCARD_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )SDCARD_TASK_Q_NUM,	//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //任务选项,为了保险起见，所有任务都保存浮点寄存器的值
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
		OSSemCreate(&SDcardWrite,"SDCard",0,&err);						 
	if(err != OS_ERR_NONE)
		return 0;//失败返回0
	return 1;//成功返回1
}
extern struct Data_StructureDef Elec_Data;
//SD卡写入数据任务
u8 buf_[1024];
char tim_[64];
void sdcard_task(void *p_arg)
{
   //数据包接收区
   struct Data_StructureDef *datapack;
	datapack = &Elec_Data;
	OS_ERR err;
		//SD卡初始化函数,返回值为0时，表示返回正常
	while(SD_Init())                              
    {
	  printlcd("SDcard error!");		
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
	}
	printlcd("SDcard check success");
	//挂载SD卡 
	f_mount(fs[0],"0:",1); 				
   //时间判断缓冲区
   u8  year=0;
   u8  month=0;
   u8  day =0;	
   //文件系统返回值 0：正常   其他：错误
   u8 res=0;
   //数据接收缓冲区
   u8 *buf;
   //时间判断缓冲区
   u8 *timebuf;
   //接收到数据长度
   u32  length=0;
	//更新标志
	u8 update_flag = 0;
	
	u8 select = 0;
   OS_MSG_SIZE size;
   
   p_arg=p_arg;
	
	f_mkdir("0:/A_PHASE");
	f_mkdir("0:/B_PHASE");
	f_mkdir("0:/C_PHASE");
	
	//为缓冲区申请内存
	//buf=(u8*)mymalloc(SRAMEX,1024);
	//timebuf=(u8*)mymalloc(SRAMIN,64);
	buf = buf_;
	timebuf = tim_;
   while(1)
 {
     //请求消息DATAPACK_Msg
//	datapack=OSTaskQPend ((OS_TICK                )0,
//							    (OS_OPT                 )OS_OPT_PEND_BLOCKING,
//							    (OS_MSG_SIZE*           )&size,
//							    (CPU_TS*                )0,
//							    (OS_ERR*                )&err);
	 printlcd("SDCard Ready to Recieve");
		//	OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
	 OSSemPend(&SDcardWrite,0,OS_OPT_PEND_BLOCKING,0,&err);
	 printlcd("Start Write");
		//	OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
	   //若日期不同则更新文件
	   if(day!=datapack->time.date)
	   {
		   year=datapack->time.year;
		   month=datapack->time.month;
		   day=datapack->time.date;
			//置位更新标志
			update_flag = 1;
	   }
			printlcd("time malloc");
			//OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
	   
	   //为缓冲区申请内存
	  // buf=(u8*)mymalloc(SRAMEX,1024);
	   //为时间缓冲区申请内存
	   
		//若要更新,则输出新文件
		 printlcd("update file");
			//OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
		if(update_flag==1)
		{
        update_flag = 0;
			for(select=1;select<=3;select++)
			{ 
				//__disable_irq();
				//CPU_SR_ALLOC();
			//OS_CRITICAL_ENTER();
				//INTX_DISABLE();   
				length = Title_Output(buf ,datapack,select);
				creat_newfile(year,month,day,timebuf,length,buf,select);
				//INTX_ENABLE();   
				//OS_CRITICAL_EXIT();	
				//__enable_irq();				
			}			
		}
	   //格式化输出到缓冲区
	   printlcd("output");
			//OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
//		__disable_irq(); 
		//OSSchedLock(&err);
     for(select=1;select<=3;select++)
		{
			printlcd("format%d",select);
			//OSTimeDlyHMSM(0,0,0,400,OS_OPT_TIME_HMSM_STRICT,&err);
			
			
			//__disable_irq();
			//CPU_SR_ALLOC();
			//OS_CRITICAL_ENTER();
			//INTX_DISABLE();   
			length=Format_Output(buf ,datapack,select); 
			//INTX_ENABLE(); 
			//__enable_irq();
       
      printlcd("write to sdcard%d",select);
			//OSTimeDlyHMSM(0,0,0,400,OS_OPT_TIME_HMSM_STRICT,&err);
			
			
			//__disable_irq();
			//INTX_DISABLE();   
       sdcard_output(year,month,day,timebuf,length,buf,select); 
			//INTX_ENABLE(); 
			//OS_CRITICAL_EXIT();	
			//__enable_irq();
		}
	//	OSSchedUnlock(&err);
//		__enable_irq();

      //释放申请的内存	 
	   //myfree(SRAMEX,buf);
		//格式化输出到缓冲区
	   printlcd("free mem");
		//	OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
	 //  myfree(SRAMIN,timebuf);
	   length=0;
		LED0_Toggle;
	
   }
}

char sdcard_task_deinit(void* p_arg)
{
	OS_ERR err;
	OSTaskDel((OS_TCB*)&SDcardTask_TCB,&err);
	if(err != OS_ERR_NONE)
		return 0;//失败返回0
	return 1;//成功返回1
}




