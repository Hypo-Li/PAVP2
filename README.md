PAVP2(Post-processing Audio Video Player 2)

后处理音视频播放器2(第一版太简陋了)

利用shader对视频的每一帧图像进行处理，从而达到各种渲染播放的效果，像是一个离线版的shaderToy，不过目前仍有很多变量没有添加进去。。。

用户可以自行编写shader，也可以尝试shader目录中提供的测试shader，大部分抄自shaderToy。它可以展示类似以下的画面：

原画面：

![show0](.\res\show\show0.png)

卡通效果（虽然看起来有种油画的感觉……

![show1](.\res\show\show1.png)

铅笔的彩绘

![show2](.\res\show\show2.png)



目前这个播放器仍然存在着一些bug，起初我也想好好设计的不过后来还是越写越乱，最后索性破罐子破摔了hhh……所以代码还是有一点乱，不过最重要的是好玩就行了。