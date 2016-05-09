#include "rtc.h"

#include "debug.h"

uint8_t rtc_flag=0;
/* ���жϱ�־���������ж�ʱ��1����ʱ�䱻ˢ��֮����0 */
__IO uint32_t TimeDisplay;	

Data_Time  timer;  //����һ��ʱ��ṹ�����

uint8_t init_rtc(void)
{
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTCȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�1λ,�����ȼ�3λ
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//��ռ���ȼ�0λ,�����ȼ�4λ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//ʹ�ܸ�ͨ���ж�
	NVIC_Init(&NVIC_InitStructure);		//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
																	//ʹ��PWR��BKP����ʱ��   		
	PWR_BackupAccessCmd(ENABLE);									//ʹ��RTC�ͺ󱸼Ĵ������� 
	
	if(BKP_ReadBackupRegister(BKP_DR1)!=0x5555)						//��ָ���ĺ󱸼Ĵ����ж������ݣ��ж��Ƿ�Ϊ��һ������
	{	
      	//printf("ʱ�����á�����\r\n");																
		BKP_DeInit();												//������BKP��ȫ���Ĵ�������Ϊȱʡֵ 	
		RCC_LSEConfig(RCC_LSE_ON);									//ʹ���ⲿ����ʱ�� 32.768KHz
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)			//���ָ����RCC��־λ�������,�ȴ����پ������
  		{}
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);						//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);										//ʹ��RTCʱ��  
		RTC_WaitForSynchro();										//�ȴ�RTC�Ĵ���(RTC_CNT,RTC_ALR��RTC_PRL)��RTC APBʱ��ͬ��
		RTC_WaitForLastTask();										//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_ITConfig(RTC_IT_SEC, ENABLE);							//ʹ��RTC���ж�
		RTC_WaitForLastTask();										//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_SetPrescaler(32767); 									//����RTCԤ��Ƶ��ֵ  RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)
		RTC_WaitForLastTask();										//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		time_set();													//ʱ������	
    	BKP_WriteBackupRegister(BKP_DR1, 0x5555);					//��ָ���ĺ󱸼Ĵ�����д���û���������0X5555���жϱ�־										
	}																 	
	else															//���ǵ�һ������ ������ʱ
	{
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)			//���ָ����RCC��־λ�������:POR/PDR��λ
		{
      		//printf("�ϵ縴λ������\r\n");
		}											
		else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)		//���ָ����RCC��־λ�������:�ܽŸ�λ
		{
      		//printf("�ⲿ��λ������\r\n");
		}
    	//printf("����Ҫ���á�����\r\n");
		
		RTC_WaitForSynchro();										//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		
		RTC_ITConfig(RTC_IT_SEC, ENABLE);							//ʹ��RTC���ж�

		RTC_WaitForLastTask();										//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	}		    				     
	time_get();														//����ʱ�� 
	
	RCC_ClearFlag();												//���RCC�ĸ�λ��־λ
	
	return 0; //ok		
}
//����ʱ��
//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//����ֵ:0,�ɹ�;����:�������.
//�·����ݱ�											 
uint8_t const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; 			//���������ݱ�	  

const uint8_t mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};	//ƽ����·����ڱ�

uint8_t time_update(uint16_t syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec)
{
	uint16_t t;
	uint32_t seccount=0;
	if(syear<1970||syear>2099)return 1;	   
	for(t=1970;t<syear;t++)											//��������ݵ��������
		{
		if(is_leap_year(t))seccount+=31622400;						//�����������
		else seccount+=31536000;			  						//ƽ���������
		}
	smon-=1;
	for(t=0;t<smon;t++)	   											//��ǰ���·ݵ����������
		{
		seccount+=(uint32_t)mon_table[t]*86400;						//�·����������
		if(is_leap_year(syear)&&t==1)seccount+=86400;				//����2�·�����һ���������	   
		}
	seccount+=(uint32_t)(sday-1)*86400;								//��ǰ�����ڵ���������� 
	seccount+=(uint32_t)hour*3600;									//Сʱ������
	seccount+=(uint32_t)min*60;	 									//����������
	seccount+=sec;													//�������Ӽ���ȥ
											    
	RTC_WaitForLastTask();											//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	RTC_SetCounter(seccount);										//����RTC��������ֵ
	RTC_WaitForLastTask();											//�ȴ����һ�ζ�RTC�Ĵ�����д�������  	
	return 0;		
}
//�õ���ǰ��ʱ��
//����ֵ:0,�ɹ�;����:�������.
uint8_t time_get(void)
{
	static uint16_t daycnt=0;
	uint32_t timecount=0; 
	uint32_t temp=0;
	uint16_t temp1=0;	  
	 
	timecount = RTC_GetCounter();									//��� RTC ������ֵ(������)			 
	temp=timecount/86400;   //�õ�����(��������Ӧ��)
	if(daycnt!=temp)//����һ����
	{	  
		daycnt=temp;
		temp1=1970;	//��1970�꿪ʼ
		while(temp>=365)
		{				 
			if(is_leap_year(temp1))//������
			{
				if(temp>=366)temp-=366;//�����������
				else {temp1++;break;}  
			}
			else temp-=365;	  //ƽ�� 
			temp1++;  
		}   
		timer.w_year=temp1;//�õ����
		temp1=0;
		while(temp>=28)//������һ����
		{
			if(is_leap_year(timer.w_year)&&temp1==1)//�����ǲ�������/2�·�
			{
				if(temp>=29)temp-=29;//�����������
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
				else break;
			}
			temp1++;  
		}
		timer.w_month=temp1+1;//�õ��·�
		timer.w_date=temp+1;  //�õ����� 
	}
	temp=timecount%86400;     //�õ�������   	   
	timer.hour=temp/3600;     //Сʱ
	timer.min=(temp%3600)/60; //����	
	timer.sec=(temp%3600)%60; //����
	//timer.week=rtc_get_week(timer.w_year,timer.w_month,timer.w_date);//��ȡ���� 
debug_format("Time: %0.4d��%0.2d��%0.2d�� %0.2d:%0.2d:%0.2d\r\n",timer.w_year,timer.w_month,timer.w_date,timer.hour,timer.min,timer.sec);  
	return 0;
}
//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//������������������� 
//����ֵ�����ں�																						 
uint8_t rtc_get_week(uint16_t year,uint8_t month,uint8_t day)
{	
	uint16_t temp2;
	uint8_t yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 	 
	if (yearH>19)yearL+=100;// ���Ϊ21����,�������100 
  
	temp2=yearL+yearL/4;  	// ����������ֻ��1900��֮���
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
} 
//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//����:���
//���:������ǲ�������.1,��.0,����
uint8_t is_leap_year(uint16_t year)
{			  
	if(year%4==0) //�����ܱ�4����
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400���� 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}
/*****************************************************************************
** ��������: RTC_IRQHandler
** ��������: RTC�жϷ�����
				ÿ�봥��һ��  
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
*****************************************************************************/
void RTC_IRQHandler(void)
{							    
	if(RTC_GetITStatus(RTC_IT_SEC))			//�����ж�
	{							
		//time_get();							//����ʱ�� 	
			rtc_flag=1;
	}
	if(RTC_GetITStatus(RTC_IT_ALR))			//�����ж�
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);	//�������ж�		  								 				
	}
	RTC_ClearITPendingBit(RTC_IT_SEC);		//�������������жϱ�־		  								 
	RTC_WaitForLastTask();					//�ȴ�RTC�Ĵ����������
}
/*****************************************************************************
** ��������: Time_Set
** ��������: ͨ����������ʱ��
					����������ʱ����
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
*****************************************************************************/
void time_set(void)
{
	uint16_t Data[6];	
	Data[0]=2015;
	Data[1]=9;
	Data[2]=23;
	Data[3]=19;
	Data[4]=10;
	Data[5]=28;
	time_update(Data[0],Data[1],Data[2],Data[3],Data[4],Data[5]);
}
/*****************************************************************************
** ��������: Usart_Scanf
** ��������: �������뺯��
					����������ʱ��
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
*****************************************************************************/

