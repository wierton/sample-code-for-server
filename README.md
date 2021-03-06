# 项目说明

* http-server 是相对完整的最终项目，有加法和blog
* tcp-server 里面是一个简单的tcp层服务器，没有对http内容的处理
* http-static 只返回纯静态内容
* tcpstream 是对socket的stream包装
* http-server增加了多线程支持

# 编译说明

* ~~请用`g++-7`或`clang-5`，或其他支持相对完整的c++17标准的编译器~~
* ~~对于低版本编译器，请切到`g++-5`分支，`g++-5`分支已适配ubuntu自带的g++-5编译器~~
* ~~以ubuntu的默认g++-5编译器在master分支编译即可~~
* ~~编译运行命令：`cd xxx; make run`，xxx是子目录~~
* 编译：
```
sudo apt-get install cmake
mkdir build && cd build
cmake ..
make
```
* 运行：按上述命令编译完后，会在build目录下的相应子目录下生成可执行文件，在命令行中直接执行即可。


# 运行效果说明

有两个最终演示效果的地方一个是`blog`，如下图：

![](blog.png)


另一个是`trust`，是一个从`http://ncase.me/trust/`迁过来的小游戏，运行效果如下图：

![](trust.png)

加法效果是这样：

![](add.png)


# The End
