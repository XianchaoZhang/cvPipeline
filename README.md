# cvPipeline - OpenCV 滤波器序列库。

## 概述

__[cvPipeline](https://github.com/XianchaoZhang/cvPipeline)__ 是使用 OpenCV 的 C++ 库。提供连接多个图像处理例程的输入/输出图像以创建一系列图像处理流程的功能。

在用 C++ 编写图像处理时，创建该图像是为了最大程度地减少重新排列处理顺序和内容所需的时间和精力。

### 常见的图像处理问题

#### 通用模型的编码样式

典型的图像处理例程通常在其输入和输出之间按顺序调用多个图像处理函数。

输入图像和处理参数传递给单个图像处理函数，处理的结果为输出图像，该图像用作下一个操作的输入图像。

#### 试验、试错以及复杂性

但是，在许多项目的早期阶段，没有建立此处理顺序，在建成之前，将重复试验和试错，例如交换处理顺序、添加或删除处理例程以及更改参数。

重复这些试验和试错会使代码变得杂乱无章，以在单个处理和处理之间保留临时输出图像。

### 图像处理器 - 打包图像处理例程

此库将独立的图像处理例程打包到名为 ImageProcessor 的类中，并实现一系列图像处理流，将此处理器的输出连接到其他处理器的输入。这使得变更或绕过图像处理顺序更容易。

#### 从输出到输入的连接

图像处理器通过向输入图像（OpenCV cv::Mat）添加处理来输出图像。

此输出连接到其他图像处理器的输入图像，形成一系列图像处理流。此连接可以在运行时动态更改（也可以反馈到自己的输入端口）。

#### 可扩展性

由于图像处理器定义为单个抽象类，因此，如果需要新的处理器，可以实现它并创建自己特定的图像处理器。

#### 基本处理器

我们已经实现一些通用和简单的图像处理器（基本处理器）。

包括相机捕获、显示图像等处理器。

#### 分层和模块化

多个图像处理器可以被视为单个图像处理器。

#### 从 XML 文件输入

由于连接是动态的，并且许多处理器支持在运行时按类名称生成，因此可以在 XML 文件中描述处理器配置。有关详细信息，请参阅示例程序。

## 使用 XML 的图像处理示例

加载 XML 并查看摄像机图像的示例程序。

### 运行方法

使用从命令行定义图像处理器的 XML 文件运行。

`$ sample sample.xml[enter]`

有关 XML 文件的内容，请参阅以下示例 XML。

### XML 示例

下面是一个 XML 文件的示例，该文件对摄像机捕获的图像执行顺序处理，并在一个窗口中显示六张图像，如中间阶段的图像。

#### 运行界面

![`$sample sample/sample.xml`](doc/img/sample.jpg)

##### 显示的内容

每个图像如下（从左上到右下为 Z 形）

1. 捕获的图像
2. 将 1 转换为灰度
3. 对 2 应用直方图均匀化和高斯模糊
4. 3 过去 18，000 帧的平均分钟数
5. 3 和 4 之间的差异
5. 5 二值化

### XML 文件内容 （sample.xml）

```
<cvImagePipeline name="testProcessor">
  <Processor class="VideoCapture" name="cap">
    <Property name="deviceIndex" value="0"/>
  </Processor>
  <Processor class="Flipper" name="fripHoriz">
    <Property name="flipDir" value="1"/>
  </Processor>
  <Processor class="ImagePoint" name="raw"/>
  <Processor class="ColorConverter" name="grayscale"/>
  <Processor class="EqualizeHist" name="equalizeHist"/>
  <Processor class="GaussianBlur" name="blur"/>
  <Processor class="DepthTo32F" name="depth32F"/>
  <Processor class="ImagePoint" name="pp"/>
  <Processor class="RunningAvg" name="background"> 
    <Property name="averageCount" value="18000"/>
  </Processor>
  <Processor class="AbsDiff" name="diff" autoBind="false">
    <Input to="src1" from="pp"/>
    <Input to="src2" from="background"/>
  </Processor>
  <Processor class="Convert" name="to8UC">
    <Property name="rtype" value="0"/>
    <Property name="alpha" value="255"/>
    <Property name="beta" value="0"/>
  </Processor>
  <Processor class="Threshold" name="binary">
    <Property name="type" value="CV_THRESH_BINARY"/>
    	<!--
	CV_THRESH_BINARY
	CV_THRESH_BINARY_INV
	CV_THRESH_TRUNC
	CV_THRESH_TOZERO
	CV_THRESH_TOZERO_INV
	-->
    <Property name="otsu" value="1"/>
    <Property name="thresh" value="50"/>
    <Property name="maxval" value="255"/>
  </Processor>

  <Processor class="FitInGrid" name="integratedImage" autoBind="false">
    <Property name="width" value="960"/>
    <Property name="height" value="480"/>
    <Property name="cols" value="3"/>
    <Property name="rows" value="2"/>
    <Property name="interleave" value="0"/>
    <Input to="0" from="raw"/>
    <Input to="1" from="grayscale"/>
    <Input to="2" from="blur"/>
    <Input to="3" from="background"/>
    <Input to="4" from="diff"/>
    <Input to="5" from="binary"/>
  </Processor>
  <Processor class="ImageWindow" name="window">
    <Property name="windowName" value="cvImagePipeline"/>
    <Property name="showFPS" value="1"/>
  </Processor>
</cvImagePipeline>
```

#### 简要说明
* 根元素是 cvImagePipeline。
* 图像处理器在处理器标记的属性中指定类和名称。
* I/O 从上到下，除非 autoBind 属性设置为 `false`
* `autoBind=false`或具有多个输入的处理器是子元素的输入元素，通过引用其他处理器进行连接。

## 构建

使用 cmake。已确认 Windows 7（32bit） Visual Studio 2010、Ubuntu 14.04 LTS 工作正常。
需要 OpenCV 2.4。 [DOWNLOADS|OpenCV](http://opencv.org/downloads.html) 可从 OpenCV 下载。

__Windows 的生成示例__
```
mkdir build
cd build
cmake -D OpenCV_DIR='path/to/opencv' -D CMAKE_INSTALL_PREFIX='C:/cvImagePipeline' ..
```

## 更多信息

### 基本处理器

以下处理器是基本处理器：

|类				|概述															|
|:---				|:--															|
| ImageProcessor ([h](include/ImageProcessor.h),[cpp](source/ImageProcessor.cpp))    | 图像处理器的基类。提供多个输入图像、单个输出图像和多个类型化属性。 |
| VideoCapture ([h](include/VideoCapture.h),[cpp](source/VideoCapture.cpp))		| 捕获摄像机或视频文件(==`cv::VideoCapture`) |
| ImageWindow ([h](include/ImageWindow.h),[cpp](source/ImageWindow.cpp))		| 屏幕显示(== `imshow`)	|
| FitInGrid ([h](include/FitInGrid.h),[cpp](source/FitInGrid.cpp))			| 将多个图像并排排列成网格的处理器。 |
| ImgProcSet ([h](include/ImgProcSet.h),[cpp](source/ImgProcSet.cpp))		| 通用图像处理器，可由任何图像处理器组成。|
| ImagePoint ([h](include/ImagePoint.h),[cpp](source/ImagePoint.cpp))		| 无处理 (== `cv::copyTo`)。按原样输出输入图像。可作为图像导出点 cv::copyTo。|

### 其他处理器。

以下类是执行示例实现的简单图像处理器。例如，在创建新处理器时，请参考它。

|类			|概述															|
|:---				|:--															|
| Convert ([h](include/Convert.h),[cpp](source/Convert.cpp)) | 格式转换 (== `cv::Mat::convertTo`)	| 
| ColorConverter ([h](include/ColorConverter.h),[cpp](source/ColorConverter.cpp))	| 通道数转换 (== `cv::cvtColor`)	|
| EqualizeHist ([h](include/EqualizeHist.h),[cpp](source/EqualizeHist.cpp))		| 直方图均衡化 (== `cv::equalizeHist`)	|
| GaussianBlur ([h](include/GaussianBlur.h),[cpp](source/GaussianBlur.cpp))		| 高斯平滑 (== `cv::GaussianBlur`)	|
| Flipper ([h](include/Flipper.h),[cpp](source/Flipper.cpp))			| 翻转。(== `cv::flip`)	|
| Resizer ([h](include/Resizer.h),[cpp](source/Resizer.cpp))			| 调整大小(== `cv::resize`)	|
| RunningAvg ([h](include/RunningAvg.h),[cpp](source/RunningAvg.cpp))		| 长期平均值(== `cv::runningAvg`)	|
| AbsDiff ([h](include/AbsDiff.h),[cpp](source/AbsDiff.cpp))			| 绝对值差(== `cv::absdiff`)	|
| SubMat ([h](include/SubMat.h),[cpp](source/SubMat.cpp))			| 差值(== `cv::sub`)	|
| Threshold ([h](include/Threshold.h),[cpp](source/Threshold.cpp))			| 二值化(== `cv::threshold`)	|
| Dilate ([h](include/Dilate.h),[cpp](source/Dilate.cpp))			| 图像膨胀(== `cv::dilate`)	|

### 关于按类名称在运行时生成动态实例

可以使用类声明中的 `DECLARE_CVFILTER` 宏和定义时的 `IMPLEMENT_CVFILTER` 宏来创建一个具有类名的实例。如果它不支持动态生成，则无法从 XML 文件生成。

