# HID-Consumer-Keypad
<img src="Documents\assert\dino_keypad.jpg" alt="dino_keypad" style="zoom: 25%;" />

HID Consumer keypad, base on CH582.

目前已经满足个人使用需求， 后续几乎没有可能更新了。

本工程可以看作一个例程缝合怪。



# 功能

旋钮调节音量，按下静音，

上一曲 暂停 下一曲。

长按下一曲可以点亮网易云音乐的红心。



# 硬件部分

PCB 设计由嘉立创EDA Pro 版本完成。



# 软件部分

## USB HID 与 BLE HID 

已经完成。



## 低功耗部分

未完成，如果想要较好地完成这个功能，原理图上需要加上5V的插入检测。



## 其他

振子体验不好，后续如果继续填坑会删除这个部分。

WS2812灯控部分驱动写完了，应用层暂时不想写了。(CH58x的 SPI DMA 传输完成中断无法清除， ~~体验不是很好~~)。



# 结构部分

由于是第一次设计结构，bug有点多，如果后续做了新的外壳会更新。
