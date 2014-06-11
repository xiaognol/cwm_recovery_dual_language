   cwm_bootable_recovery_cn 6.0.4.9
==============================
适配：ZJL@安智ATX团队 机型：C8813Q
感谢：ATX龙腾-大星星 的6047汉化代码
<br /><br /><br />当BoardConfig.mk中定义了recovery的字体且为中文字体时，自动编译为中文版，否则编译为英文版<br />
例如：

    BOARD_USE_CUSTOM_RECOVERY_FONT := \"fontcn28_15x40.h\"  
此时编译将使用graphics_cn.c，且recovery界面显示为中文。

    BOARD_USE_CUSTOM_RECOVERY_FONT := \"roboto_15x24.h\"
此时编译将使用原版graphics.c，且recovery界面显示为英文。

<br /><br />制作中参考使用了几位大神/团队提供的代码，特此感谢：
<br />CyanogenMod : https://github.com/CyanogenMod/android_bootable_recovery
<br />suky : https://github.com/suky/TWRP_cn
<br />tenfar : https://github.com/tenfar/android_bootable_recovery_cn
<br />xiaolu : https://github.com/xiaolu/android_bootable_recovery
