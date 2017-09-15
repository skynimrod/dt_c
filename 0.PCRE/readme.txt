PCRE(Perl Compatible Regular Expressions中文含义：perl语言兼容正则表达式)是一个用C语言编写的正则表达式函数库，由菲利普.海泽(Philip Hazel)编写。PCRE是一个轻量级的函数库，比Boost之类的正则表达式库小得多。PCRE十分易用，同时功能也很强大，性能超过了POSIX正则表达式库和一些经典的正则表达式库[1]  。

PCRE的安装及使用
。
1、主页地址：www pcre org
下载pcre-7.8 tar bz2
2、解压缩：
tar xjpf pcre-7.8 tar bz2
3、配置：
cd pcre-7.8
./configure --prefix=/usr/local/pcre-7.8 --libdir=/usr/local/lib/pcre --includedir=/usr/local/include/pcre
configure有许多参数可配，具体参见./configure --help及手册
4、编译：
make
5、安装：
make install
6、检查：
ls /usr/local 检查是否有pcre-7.8目录
ls /usr/local/lib 检查是否有pcre目录
ls /usr/local/include 检查是否有pcre目录
7、将库文件导入cache：
方法1：在/etc/ld.so.conf中加入： /usr/local/lib/pcre，然后运行ldconfig
方法2：在/etc/ld.so.conf.d/下新生成一个文件（或在其中的文件中加入同样内容），文件内容为：
/usr/local/lib/pcre，然后运行ldconfig
8、使用：
使用pcre编写C或C++程序，然后编译。
对于C程序，编译命令为：gcc -I/usr/local/include/pcre -L/usr/local/lib/pcre -lpcre file.c
对于C程序，编译命令为：gcc -I/usr/local/include/pcre -L/usr/local/lib/pcre -lpcrecpp file.cpp
也可用apt直接安装：
apt-cache search pcre 查找pcre
下面只安装pcrecpp
apt-get install libpcre++-dev 安装pcrecpp开发文件
apt-get install libpcre++0 安装pcrecpp库文件