#include "sdcard.h"
#include "sdmmc_sdcard.h"
#include "led.h"
#include "totaltask.h"
#include "ATT7022E.h"

/****************������ƿ��������ջ����**************/
//����7
OS_TCB   SDcardTask_TCB;
__align(8) CPU_STK  SDCARD_TASK_STK[SDCARD_STK_SIZE];
/**********ͬ�����ź���*****/
OS_SEM SDcardWrite;

/********************�ڲ�����**************************/

/*------------------------------------------------------------
����˵��������������ʱ�䡢��ѹ�����������ݸ�ʽ�������SD����
����˵����buffer:�������ݻ�����  demo:���͹��������ݰ�   index:1/2/3�ֱ��ʾA/B/C����
����ֵ�� ��������ݳ���
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
����˵����SD���ļ�����ʱ�ı�������
����˵����buffer:�������ݻ�����  data:���͹��������ݰ�  index:1/2/3��ʾA/B/C��������
����ֵ�� ��������ݳ���
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
����˵������ʱ�䲻ͬ�����ݴ��벻ͬ��TXT�ĵ��У��ļ���Ϊʱ���ǩ
����˵����timeyear:����ʱ��(��) timemon:����ʱ��(��) timeday:����ʱ��(��)
			*timebuf:�ļ������滺����ָ��  length:Ҫд�����ݳ���  
			*buffer:Ҫд�����ݵ�ָ��       index:1/2/3��ʾA/B/C����
����ֵ�� ��
--------------------------------------------------------------*/

static void sdcard_output(u8 timeyear,u8 timemon,u8 timeday,u8 *timebuf,u32 length,u8 *buffer,u8 index)
{
    u8 res;
	OS_ERR err;
	//�ļ�����ʽ�����ļ���Ϊʱ���ǩ
	printlcd("out_sprintf");
	OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
   switch(index) 
	{
		case 1:sprintf((char*)timebuf,"0:/A_PHASE/A20%02d-%02d-%02d.txt",timeyear,timemon,timeday);break;
		
		case 2:sprintf((char*)timebuf,"0:/B_PHASE/B20%02d-%02d-%02d.txt",timeyear,timemon,timeday);break;
		
		case 3:sprintf((char*)timebuf,"0:/C_PHASE/C20%02d-%02d-%02d.txt",timeyear,timemon,timeday);break;
		
		default:break;
	}
	//�򿪸��ļ�
	printlcd("openfile");
	OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
	OSSchedLock(&err);
	res=f_open(file,(const TCHAR*)timebuf,FA_OPEN_EXISTING|FA_WRITE);
	 if(res!=0)
	  //���ļ�����
	   {
	     printlcd("can not open this file");
	   }
	  else 
	  //�ɹ����ļ�
	  {
		 f_stat((const TCHAR*)timebuf,&fileinfo);	  
		  //�ļ�ָ���ƶ����ļ�β��д��
	    f_lseek(file,fileinfo.fsize);
		 res=f_write(file,buffer,length,&bw);	
      		
	  }
		OSSchedUnlock(&err);
		printlcd("close");
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
		//�ر��ļ�
	    f_close(file);	

}

/*------------------------------------------------------------
����˵��������һ�����ļ����ļ���Ϊʱ���ǩ
����˵����timeyear:����ʱ��(��) timemon:����ʱ��(��) timeday:����ʱ��(��)
			*timebuf:�ļ������滺����ָ��  length:Ҫд�����ݳ���  
			*buffer:Ҫд�����ݵ�ָ��   index:����1/2/3�ֱ��ʾABC��������
����ֵ�� ��
--------------------------------------------------------------*/

static void creat_newfile(u8 timeyear,u8 timemon,u8 timeday,u8 *timebuf,u32 length,u8 *buffer,u8 index)
{
   u8 res;
	//�ļ�����ʽ�����ļ���Ϊʱ���ǩ
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
  //���ļ�����
	{
	  printlcd("file create error!");
	}
  else 
  //�ɹ����ļ�
  {
	 res=f_write(file,buffer,length,&bw);
      		
  }
	//�ر��ļ�
   f_close(file);	

}
/********************�ڲ�����**************************/


/********************�ⲿ����**************************/

//SD����ʼ������
char sdcard_task_init(void *p_arg)
{
	OS_ERR err;

	
	
	OSTaskCreate((OS_TCB 	* )&SDcardTask_TCB,		//������ƿ�
				 (CPU_CHAR	* )"sdcard task", 		//��������
                 (OS_TASK_PTR )sdcard_task, 	    //������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )SDCARD_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&SDCARD_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)SDCARD_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)SDCARD_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )SDCARD_TASK_Q_NUM,	//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
		OSSemCreate(&SDcardWrite,"SDCard",0,&err);						 
	if(err != OS_ERR_NONE)
		return 0;//ʧ�ܷ���0
	return 1;//�ɹ�����1
}
extern struct Data_StructureDef Elec_Data;
//SD��д����������
u8 buf_[1024];
char tim_[64];
void sdcard_task(void *p_arg)
{
   //���ݰ�������
   struct Data_StructureDef *datapack;
	datapack = &Elec_Data;
	OS_ERR err;
		//SD����ʼ������,����ֵΪ0ʱ����ʾ��������
	while(SD_Init())                              
    {
	  printlcd("SDcard error!");		
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
	}
	printlcd("SDcard check success");
	//����SD�� 
	f_mount(fs[0],"0:",1); 				
   //ʱ���жϻ�����
   u8  year=0;
   u8  month=0;
   u8  day =0;	
   //�ļ�ϵͳ����ֵ 0������   ����������
   u8 res=0;
   //���ݽ��ջ�����
   u8 *buf;
   //ʱ���жϻ�����
   u8 *timebuf;
   //���յ����ݳ���
   u32  length=0;
	//���±�־
	u8 update_flag = 0;
	
	u8 select = 0;
   OS_MSG_SIZE size;
   
   p_arg=p_arg;
	
	f_mkdir("0:/A_PHASE");
	f_mkdir("0:/B_PHASE");
	f_mkdir("0:/C_PHASE");
	
	//Ϊ�����������ڴ�
	//buf=(u8*)mymalloc(SRAMEX,1024);
	//timebuf=(u8*)mymalloc(SRAMIN,64);
	buf = buf_;
	timebuf = tim_;
   while(1)
 {
     //������ϢDATAPACK_Msg
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
	   //�����ڲ�ͬ������ļ�
	   if(day!=datapack->time.date)
	   {
		   year=datapack->time.year;
		   month=datapack->time.month;
		   day=datapack->time.date;
			//��λ���±�־
			update_flag = 1;
	   }
			printlcd("time malloc");
			//OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
	   
	   //Ϊ�����������ڴ�
	  // buf=(u8*)mymalloc(SRAMEX,1024);
	   //Ϊʱ�仺���������ڴ�
	   
		//��Ҫ����,��������ļ�
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
	   //��ʽ�������������
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

      //�ͷ�������ڴ�	 
	   //myfree(SRAMEX,buf);
		//��ʽ�������������
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
		return 0;//ʧ�ܷ���0
	return 1;//�ɹ�����1
}




