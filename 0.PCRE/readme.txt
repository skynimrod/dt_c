PCRE(Perl Compatible Regular Expressions���ĺ��壺perl���Լ���������ʽ)��һ����C���Ա�д��������ʽ�����⣬�ɷ�����.����(Philip Hazel)��д��PCRE��һ���������ĺ����⣬��Boost֮���������ʽ��С�öࡣPCREʮ�����ã�ͬʱ����Ҳ��ǿ�����ܳ�����POSIX������ʽ���һЩ�����������ʽ��[1]  ��

PCRE�İ�װ��ʹ��
��
1����ҳ��ַ��www pcre org
����pcre-7.8 tar bz2
2����ѹ����
tar xjpf pcre-7.8 tar bz2
3�����ã�
cd pcre-7.8
./configure --prefix=/usr/local/pcre-7.8 --libdir=/usr/local/lib/pcre --includedir=/usr/local/include/pcre
configure�����������䣬����μ�./configure --help���ֲ�
4�����룺
make
5����װ��
make install
6����飺
ls /usr/local ����Ƿ���pcre-7.8Ŀ¼
ls /usr/local/lib ����Ƿ���pcreĿ¼
ls /usr/local/include ����Ƿ���pcreĿ¼
7�������ļ�����cache��
����1����/etc/ld.so.conf�м��룺 /usr/local/lib/pcre��Ȼ������ldconfig
����2����/etc/ld.so.conf.d/��������һ���ļ����������е��ļ��м���ͬ�����ݣ����ļ�����Ϊ��
/usr/local/lib/pcre��Ȼ������ldconfig
8��ʹ�ã�
ʹ��pcre��дC��C++����Ȼ����롣
����C���򣬱�������Ϊ��gcc -I/usr/local/include/pcre -L/usr/local/lib/pcre -lpcre file.c
����C���򣬱�������Ϊ��gcc -I/usr/local/include/pcre -L/usr/local/lib/pcre -lpcrecpp file.cpp
Ҳ����aptֱ�Ӱ�װ��
apt-cache search pcre ����pcre
����ֻ��װpcrecpp
apt-get install libpcre++-dev ��װpcrecpp�����ļ�
apt-get install libpcre++0 ��װpcrecpp���ļ�