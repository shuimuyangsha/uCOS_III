例6-2 任务的挂起和恢复

实验内容：本实验是在例6-1的基础上完成的，本实验同样设计了3个任务，任务A用于创建其他任务，创建完成以后就删除掉自身，
任务B和任务C在LCD上有各自的运行区域，每隔1s他们都会切换一次各自运行区域的背景颜色，而且显示各自的运行次数，
任务B运行5次以后挂起任务C，当任务B运行10次以后重新恢复任务C，两个任务运行的过程中还要通过串口打印各自的运行次数，
当任务B挂起和恢复任务C以后也要通过串口打印提示信息。

注意：
1、 UCOSIII中以下优先级用户程序不能使用，ALIENTEK将这些优先级分配给了UCOSIII的5个系统内    部任务。
    优先级0：中断服务服务管理任务 OS_IntQTask()
    优先级1：时钟节拍任务 OS_TickTask()
    优先级2：定时任务 OS_TmrTask()
    优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
    优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()

2、 由于计算任务B和任务C运行次数的变量为u8类型，当任务B和任务C各自运行255次后任务运行次数就会清0！！
               	正点原子@ALIENTEK
               	2014-11-26
		广州市星翼电子科技有限公司
                联系电话（传真）：020-38271790
	       	购买：http://shop62103354.taobao.com
                      http://shop62057469.taobao.com
               	技术支持论坛：www.openedv.com