# BTSsystem
*这是我们微机原理课程的大作业*，***开源为方便我们一起学习进步***

# 文件框架

![软件框架](./README/软件框架.png)



# 流程图

![整体流程图](./README/整体流程图.png)

# 一些说明

## 数码管闪烁

> 为实现自动断电保护，我们将SaveAT2402()函数放在定时器0中断服务程序中，实现了1s存放一次数据的保护功能，但是如果简单的这样操作会造成动态数码管倒计时数字的闪烁，为什么呢？按理来说是定时器1专门负责了动态数码管的刷新，在定时器0中加入功能为什么会影响到定时器1呢？注意到在SaveAT2402()函数中为了数据的正常存放，我们每次数据存入时都会加入一段5ms延时，而因为我们存放的数据过多，导致每次存放数据大概花费75ms左右时间；而我们将该函数放在定时器0的中断服务函数中，1s到，由于中断自然优先级T0>T1，所以先执行T0的中断服务程序，这会多花费75ms左右的时间再执行T1中断服务程序中的动态数码管刷新，这样就会导致动态数码管显示的闪烁。那么我们采取的解决办法是直接在定时器初始化函数中定义中断优先级T1>T0即可，这样的解决方法是建立在基本不改变我们前面的代码基础上的，是最快捷的解决办法，当然还有其他解决办法，比如将定时器0和定时器1所负责的功能互换（本质与我们所采用的解决办法相同，但是操作起来更麻烦）；还有一个解决办法就是设置一个变量标志位，每1s置为真，并在中断函数外部进行条件判断执行SaveAT2402()函数，如此就能够避免中断服务程序耗时过长。

## 温度显示乱跳

> 但是实际运行中我们发现我们的温度显示会一直跳变不稳定，这肯定是有问题的，但是我们排查了许多地方，反复检查是否封装的代码出了问题，但都不是，最后通过查阅资料发现还是因为中断[4]，我们没有考虑到单片机进入中断对单总线时序的影响，而前面我们的定时器0的中断服务程序耗时过长导致动态数码管闪烁，我们使用软件设置中断优先级解决了，但是中断服务程序时间过长还造成了单总线的时序紊乱，我们并没有考虑到，单总线时序乱了自然导致DS18B20读出来的温度值是乱的。最后的解决办法就是在单总线收发字节的地方将总中断允许关闭，等待收发完成后再打开，避免时序的混乱即可解决温度跳变不稳定的问题

参考的链接[蓝桥杯单片机DS18b20单总线测温模块常见问题解决_ds18b20读出85-CSDN博客](https://blog.csdn.net/qq_64257614/article/details/130828674)
