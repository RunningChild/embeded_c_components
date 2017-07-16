# 软件万年历实现

应用系统实现诸如linux date万年历的功能，一般情况下，嵌入式系统，硬件上做法一般有如下几种：

第一种，搭载一颗永久不断电的时钟芯片（系统断电可由纽扣电池供电）；

第二种，片上有一直在运行的硬件万年历的外设（系统断电可由纽扣电池供电）；

第三种，片上有一个一直在运行的硬件定时器的外设（系统断电可由纽扣电池供电）；



### 第一种具体做法

1）系统开机时刻，从时钟芯片上读取当前硬件万年历时间，一般能取到year-month-day hour:minute:second，有时还有时区。

2）计算开机时刻的时间戳SecondCountRTC（从2000-01-01 00:00:00到该硬件万年历时间走过的秒数）。详见接口`set_system_clock()`。

3）之后，运行一个周期性1秒的软件定时器，在定时器中断里，将SecondCountRTC加1。

4）应用程序，每次要获取当前时刻的软件万年历时间，只需调用一个负责将当前时刻的时间戳（从2000-01-01 00:00:00到此刻的走过的秒数）转为软件万年历时间的接口`get_wall_clock_time()`即可。


详见`calendar.c`实现。


#### note
1）应用程序，如果要获取当前时刻的标准时间戳（从1970-01-01 00:00:00到此刻的走过的秒数），则调用`get_curr_timestamp_int()`和`linux_mktime()`

详见`datetime.c`实现。

### 第二种具体做法
直接操作硬件寄存器即可

### 第三种具体做法
1）重新实现接口`set_system_clock()`

2）重新实现接口`get_wall_clock_time()`

详见`calendar_enhance.c`实现。

3) 外部需要定义以上两个接口实现所需要的其他接口。`clock_gettime()`和`clock_settime()`

以下是具体的代码举例：

在这里讲述的硬件定时器，需要具备如下能力：芯片在外部断电期间，能够通过该外挂纽扣电池继续给该硬件定时器和备份域供电。

以EFM32GG系列为例，该硬件定时器为burtc。


重点关注硬件定时器几个参数：

1）每秒走过的计数值`COUNTS_PER_SEC`

2）计数最大值`RTC_COUNTER_MASK`

通过以上参数，能够获取1个溢出周期内走过的秒数：

    /* Set overflow interval based on counter width and frequency */
    overflow_interval  =  ((uint64_t)UINT32_MAX+1) / COUNTS_PER_SEC; /* in seconds */
    overflow_interval_r = ((uint64_t)UINT32_MAX+1) % COUNTS_PER_SEC; /* division remainder */

在整个实现过程，需要保存 万年历起始时间，和 计数溢出次数，到备份域。



	//wallClockTimeBase:万年历起始时间戳
	//wallClockOverflowCnt:硬件定时器计数溢出次数
	extern volatile uint32_t  wallClockTimeBase;
	extern volatile uint32_t  wallClockOverflowCnt;	
	
	//对外函数接口
	/***************************************************************************//**
	 * @brief wallclock_set_timebase
	 ******************************************************************************/
	__STATIC_INLINE void wallclock_set_timebase(uint32_t timebase)
	{
	    wallClockTimeBase = timebase;
	}
	
	/***************************************************************************//**
	 * @brief wallclock_get_timebase
	 ******************************************************************************/
	__STATIC_INLINE uint32_t wallclock_get_timebase(void)
	{
	    return wallClockTimeBase;
	}
	
	/***************************************************************************//**
	 * @brief wallclock_set_overflowcnt
	 ******************************************************************************/
	__STATIC_INLINE void wallclock_set_overflowcnt(uint32_t overflowcnt)
	{
	    wallClockOverflowCnt = overflowcnt;
	}
	
	/***************************************************************************//**
	 * @brief wallclock_get_overflowcnt
	 ******************************************************************************/
	__STATIC_INLINE uint32_t wallclock_get_overflowcnt(void)
	{
	    return wallClockOverflowCnt;
	}



以EFM32GG系列为例，片上有一个一直在运行的硬件定时器的外设burtc

	//RTC_DIVIDER：外设时钟分频比
	//RTC_CLOCK：外设时钟频率
	#define BURTC_PRESCALING                        (RTC_DIVIDER)
	#define COUNTS_PER_SEC                          (RTC_CLOCK/BURTC_PRESCALING)
	
	static uint32_t overflow_interval;
	static uint32_t overflow_interval_r;
	
	/*****************************************************************************
	 * @brief  Backup CALENDAR to retention registers
	 *   RET[0].REG : number of BURTC overflows
	 *   RET[1].REG : epoch offset
	 ******************************************************************************/
	static void clock_backup(void)
	{
		uint32_t burtcOverflowCounter = wallclock_get_overflowcnt();
		uint32_t burtcStartTime = wallclock_get_timebase();
		
		/* Write overflow counter to retention memory */
		BURTC_RetRegSet(0, burtcOverflowCounter);
		
		/* Write local epoch offset to retention memory */
		BURTC_RetRegSet(1, burtcStartTime);
	}
	
	/******************************************************************************
	 * @brief Returns the current system time
	 *
	 * @param timer
	 *   If not a null pointer, time is copied to this
	 *
	 * @return
	 *   Current system time. Should, but does not, return -1 if system time is not available
	 *
	 *****************************************************************************/
	uint32_t clock_gettime(void)
	{
	    uint32_t burtcOverflowCounter = wallclock_get_overflowcnt();
	
	    /* Add the time offset */
	    uint32_t t = wallclock_get_timebase();
	
	    /* Add time based on number of counter overflows*/
	    t += burtcOverflowCounter * overflow_interval;
	
	    /* Correct if overflow interval is not an integer*/
	    if ( overflow_interval_r != 0 )
	    {
	        t += burtcOverflowCounter * overflow_interval_r / COUNTS_PER_SEC;
	    }
	
	    /* Add the number of seconds for BURTC */
	    t += (BURTC_CounterGet() / COUNTS_PER_SEC);
	
	    return t;
	}
	
	/*****************************************************************************
	 * @brief Set the epoch offset
	 ******************************************************************************/
	void clock_settime(const uint32_t set_timestamp)
	{
	    uint32_t burtcOverflowCounter = wallclock_get_overflowcnt();
	
	    //这里，不能改变burtc的cnt值
	    uint32_t burtcStartTime = set_timestamp;
	    burtcStartTime -= (BURTC_CounterGet() / COUNTS_PER_SEC);
	    burtcStartTime -= burtcOverflowCounter * overflow_interval;
	    if ( overflow_interval_r != 0 )
	    {
	        burtcStartTime -= burtcOverflowCounter * overflow_interval_r / COUNTS_PER_SEC;
	    }
	
	    //更新开始时间戳，并保存到备份域
	    wallclock_set_timebase(burtcStartTime);
	    LOG(LEVEL_DEBUG, "settime TimeBase=%d", wallclock_get_timebase());
	    clock_backup();
	}
	
	/*****************************************************************************
	 * @brief hal_burtc_over_flow_irq_process
	 * @该接口需要在burtc定时器溢出中断中被执行
	 ******************************************************************************/
	void hal_burtc_over_flow_irq_process(void)
	{
	    uint32_t overflowcnt = wallclock_get_overflowcnt();
	
	    //更新溢出值
	    wallclock_set_overflowcnt(overflowcnt++);
	
	    //保存到备份域
	    clock_backup();
	}

