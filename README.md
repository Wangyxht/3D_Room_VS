# 3DRoom
 
该项目基于NUAA 计算机图形学课程3DRoom作业创建,源代码与GitHub项目网址提交给老师，不建议直接使用本代码验收，后果自负。

## 作业要求：
### Introduction
This programming assignment will introduce you to the OpenGL graphics programming interface. In this programming assignment, you will be creating different 3D objects to model interesting shapes. The objective of this assignment is to apply your understanding of the computer graphics theories and give you an introduction to the OpenGL programming library.

Your goal is to design a 3D room, which consists of five planes, i.e., the left, right, and back walls, the ceiling and the floor, at least five different geometric primitives, such as the cone, sphere, cylinder, cubes, etc. And you should be able to apply arbitrary transformations-- translation, rotation, scaling-to them. The user should be able to use the keyboard (and/or the mouse) to translate, rotate, and scale the object. After you have implemented all the basic functionality, we expect you to construct an interesting scene with your program. In order to make your scene more realistic, you may use the perspective projection instead of orthographic projection. You can also use different primitives to construct a complex object in the room (furniture or electrical equipment).
### Implementation Details. 
In this assignment package, we have provided you with a template program (i.e., submit.cpp) that includes the necessary functions you are going to use and callback functions in the GLUT interface toolkit. Please use this template as the basis for your implementation. There is also a file (i.e., readme.txt) including the keyboard usage of the demo program (i.e., demo.exe) for the users. You may design your own function to process the keyboard events, but you should also submit a file like this to specify the keyboard (and/or mouse) events you designed in your program. Otherwise, the mark for related items will be deducted.
All programs should meet reasonable programming standards: header comment, in-line comments, good modularity, clear printout, efficiency.
Constraints:
1. Draw at least five geometric primitives in the 3D space bounded by the five planes;
2. Ensure that the objects are in good lighting condition;
3. Create at least five keyboard or mouse events;
4. Design diverse objects transformations, such as rotation, translating, scaling;
5. Use perspective projection to draw the scene;
6. Set interesting material properties to different objects.
No-constraints:
You are free to add objects, move them, organize them, deal with their material attributes, and whatever you with to make your scene interesting.
### Grading Scheme
Your assignment will be graded by the following marking scheme:
#### Basic (80%)
-	Planes (the left, right, and back walls, the ceiling and the floor)  10%
-	At least five different geometric primitives                  20%
-	At least five keyboard events (mouse event is optional)        15%
-	Object transformation animation (rotation, translating, scaling)  15%
-	General lighting control (different material properties setting)   20%
#### Bonus (Up to 20%)
-	Well-organized room                                       5%
-	Complex meaningful object constructed by different primitives      5%
-	Additional light (with different properties, on/off or transformation)  10%
-	Other creativities                                           10%

Total                                                       100%
- Note: No grade will be given if the program is incomplete.
### Guidelines to submit programming assignments
1)	You are suggested to write your programs on Windows, since there will be enough technical support. If you develop the program in other platforms, make sure your program can be compiled and executed on Windows as the program will only be tested on this platform.
2)	Modify the provided submit.cpp, and provide all your code in this file. No more additional .cpp or .h files are allowed. Type your full name and student ID in submit.cpp. Missing such essential information will lead to mark deduction.
3)	Zip the source code file (i.e., submit.cpp), and the executable file (i.e., submit.exe), and the readme file (i.e., readme.txt) in a .zip or .rar file. Name it with your own username and StuID (e.g., 学号+张三.zip). That is, there should be exactly three files in your submitted package.
4)	Pass your .zip file to me by QQ (Mingqiang Wei), I will answer you if I receive it. 
5)	The score will be zero if you copy. I will ask you WHY if two or more similar codes are found.

## 使用库及环境
#### OpenGL
- glad: 加载opengl指针函数
- glfw3: 窗体加载与管理
- glm: OpenGL数学库，用于实现向量、矩阵变换等

#### stb_image
- 用于加载纹理与模型

#### Assimp
- 用于导入第三方模型文件

#### 环境
- Visual Studio 2022（转移自之前Clion+Mingw的项目）

## 项目结构
- bin: 内含.exe与资源文件、dll文件, shader文件夹含有着色器代码，model文件夹含有模型、texture文件夹含有纹理贴图
- include: 包含库相关
- src: 源代码
- lib: 链接库相关

使用CMAKE构建，详见cmakeLists.txt
## 交互
- 鼠标移动：视角转换
- WASD: 世界位置移动（与一般游戏相同）
- L: 开启/关闭台灯
- F: 开启/关闭手电筒
- 0: 光源玩具停止运动
- 1: 光源玩具做匀速圆周运动（参数方程实现）
- 2: 光源玩具做水滴形运动（参数方程实现）
- 3: 光源做心形线运动（参数方程实现）
- -: 风扇减速
- +: 风扇加速
- E: 芙卡洛斯视角转向摄像机位置
- T: 开启/关闭电视机
- P: 枕头位置调整
- ←(LEFT): 符华模型转速向反方向增加
- →(RIGHT): 符华模型转速向正方向增加
- ↓(DOWN):主灯调亮
- ↑(UP):主灯调暗或关闭

## 参考资料
[1] OpenGL Learn CN https://learnopengl-cn.github.io/
