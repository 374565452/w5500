#include "rtc.h"

#include "debug.h"

uint8_t rtc_flag=0;
/* 秒中断标志，进入秒中断时置1，当时间被刷新之后清0 */
__IO uint32_t TimeDisplay;	

Data_Time  timer;  //定义一个时间结构体变量

uint8_t init_rtc(void)
{
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTC全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//先占优先级1位,从优先级3位
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//先占优先级0位,从优先级4位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//使能该通道中断
	NVIC_Init(&NVIC_InitStructure);		//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
																	//使能PWR和BKP外设时钟   		
	PWR_BackupAccessCmd(ENABLE);									//使能RTC和后备寄存器访问 
	
	if(BKP_ReadBackupRegister(BKP_DR1)!=0x5555)						//从指定的后备寄存器中读出数据，判断是否为第一次配置
	{	
      	//printf("时钟配置。。。\r\n");																
		BKP_DeInit();												//将外设BKP的全部寄存器重设为缺省值 	
		RCC_LSEConfig(RCC_LSE_ON);									//使能外部低速时钟 32.768KHz
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)			//检查指定的RCC标志位设置与否,等待低速晶振就绪
  		{}
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);						//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);										//使能RTC时钟  
		RTC_WaitForSynchro();										//等待RTC寄存器(RTC_CNT,RTC_ALR和RTC_PRL)与RTC APB时钟同步
		RTC_WaitForLastTask();										//等待最近一次对RTC寄存器的写操作完成
		RTC_ITConfig(RTC_IT_SEC, ENABLE);							//使能RTC秒中断
		RTC_WaitForLastTask();										//等待最近一次对RTC寄存器的写操作完成
		RTC_SetPrescaler(32767); 									//设置RTC预分频的值  RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)
		RTC_WaitForLastTask();										//等待最近一次对RTC寄存器的写操作完成
		time_set();													//时间设置	
    	BKP_WriteBackupRegister(BKP_DR1, 0x5555);					//向指定的后备寄存器中写入用户程序数据0X5555做判断标志										
	}																 	
	else															//不是第一次配置 继续计时
	{
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)			//检查指定的RCC标志位设置与否:POR/PDR复位
		{
      		//printf("上电复位。。。\r\n");
		}											
		else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)		//检查指定的RCC标志位设置与否:管脚复位
		{
      		//printf("外部复位。。。\r\n");
		}
    	//printf("不需要配置。。。\r\n");
		
		RTC_WaitForSynchro();										//等待最近一次对RTC寄存器的写操作完成
		
		RTC_ITConfig(RTC_IT_SEC, ENABLE);							//使能RTC秒中断

		RTC_WaitForLastTask();										//等待最近一次对RTC寄存器的写操作完成
	}		    				     
	time_get();														//更新时间 
	
	RCC_ClearFlag();												//清除RCC的复位标志位
	
	return 0; //ok		
}
//设置时钟
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//返回值:0,成功;其他:错误代码.
//月份数据表											 
uint8_t const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; 			//月修正数据表	  

const uint8_t mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};	//平年的月份日期表

uint8_t time_update(uint16_t syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec)
{
	uint16_t t;
	uint32_t seccount=0;
	if(syear<1970||syear>2099)return 1;	   
	for(t=1970;t<syear;t++)											//把所有年份的秒钟相加
		{
		if(is_leap_year(t))seccount+=31622400;						//闰年的秒钟数
		else seccount+=31536000;			  						//平年的秒钟数
		}
	smon-=1;
	for(t=0;t<smon;t++)	   											//把前面月份的秒钟数相加
		{
		seccount+=(uint32_t)mon_table[t]*86400;						//月份秒钟数相加
		if(is_leap_year(syear)&&t==1)seccount+=86400;				//闰年2月份增加一天的秒钟数	   
		}
	seccount+=(uint32_t)(sday-1)*86400;								//把前面日期的秒钟数相加 
	seccount+=(uint32_t)hour*3600;									//小时秒钟数
	seccount+=(uint32_t)min*60;	 									//分钟秒钟数
	seccount+=sec;													//最后的秒钟加上去
											    
	RTC_WaitForLastTask();											//等待最近一次对RTC寄存器的写操作完成
	RTC_SetCounter(seccount);										//设置RTC计数器的值
	RTC_WaitForLastTask();											//等待最近一次对RTC寄存器的写操作完成  	
	return 0;		
}
//得到当前的时间
//返回值:0,成功;其他:错误代码.
uint8_t time_get(void)
{
	static uint16_t daycnt=0;
	uint32_t timecount=0; 
	uint32_t temp=0;
	uint16_t temp1=0;	  
	 
	timecount = RTC_GetCounter();									//获得 RTC 计数器值(秒钟数)			 
	temp=timecount/86400;   //得到天数(秒钟数对应的)
	if(daycnt!=temp)//超过一天了
	{	  
		daycnt=temp;
		temp1=1970;	//从1970年开始
		while(temp>=365)
		{				 
			if(is_leap_year(temp1))//是闰年
			{
				if(temp>=366)temp-=366;//闰年的秒钟数
				else {temp1++;break;}  
			}
			else temp-=365;	  //平年 
			temp1++;  
		}   
		timer.w_year=temp1;//得到年份
		temp1=0;
		while(temp>=28)//超过了一个月
		{
			if(is_leap_year(timer.w_year)&&temp1==1)//当年是不是闰年/2月份
			{
				if(temp>=29)temp-=29;//闰年的秒钟数
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
				else break;
			}
			temp1++;  
		}
		timer.w_month=temp1+1;//得到月份
		timer.w_date=temp+1;  //得到日期 
	}
	temp=timecount%86400;     //得到秒钟数   	   
	timer.hour=temp/3600;     //小时
	timer.min=(temp%3600)/60; //分钟	
	timer.sec=(temp%3600)%60; //秒钟
	//timer.week=rtc_get_week(timer.w_year,timer.w_month,timer.w_date);//获取星期 
debug_format("Time: %0.4d年%0.2d月%0.2d日 %0.2d:%0.2d:%0.2d\r\n",timer.w_year,timer.w_month,timer.w_date,timer.hour,timer.min,timer.sec);  
	return 0;
}
//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//输入参数：公历年月日 
//返回值：星期号																						 
uint8_t rtc_get_week(uint16_t year,uint8_t month,uint8_t day)
{	
	uint16_t temp2;
	uint8_t yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 	 
	if (yearH>19)yearL+=100;// 如果为21世纪,年份数加100 
  
	temp2=yearL+yearL/4;  	// 所过闰年数只算1900年之后的
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
} 
//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//输入:年份
//输出:该年份是不是闰年.1,是.0,不是
uint8_t is_leap_year(uint16_t year)
{			  
	if(year%4==0) //必须能被4整除
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//如果以00结尾,还要能被400整除 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}
/*****************************************************************************
** 函数名称: RTC_IRQHandler
** 功能描述: RTC中断服务函数
				每秒触发一次  
** 作  　者: Dream
** 日　  期: 2011年6月20日
*****************************************************************************/
void RTC_IRQHandler(void)
{							    
	if(RTC_GetITStatus(RTC_IT_SEC))			//秒钟中断
	{							
		//time_get();							//更新时间 	
			rtc_flag=1;
	}
	if(RTC_GetITStatus(RTC_IT_ALR))			//闹钟中断
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);	//清闹钟中断		  								 				
	}
	RTC_ClearITPendingBit(RTC_IT_SEC);		//清除溢出，秒钟中断标志		  								 
	RTC_WaitForLastTask();					//等待RTC寄存器操作完成
}
/*****************************************************************************
** 函数名称: Time_Set
** 功能描述: 通过键盘设置时间
					包括年月日时分秒
** 作  　者: Dream
** 日　  期: 2011年6月20日
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
** 函数名称: Usart_Scanf
** 功能描述: 键盘输入函数
					包括年月日时间
** 作  　者: Dream
** 日　  期: 2011年6月20日
*****************************************************************************/

