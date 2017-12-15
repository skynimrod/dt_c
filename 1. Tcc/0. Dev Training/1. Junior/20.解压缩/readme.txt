使用 lzlib 库. C语言写的.

   http://www.ietf.org/rfc/rfc1951.txt

   http://www.nongnu.org/lzip/lzip.html

   http://blog.csdn.net/peilw625311/article/details/7440978       Deflate 相关的说明

   http://blog.csdn.net/blues1021/article/details/45165777   关于 rfc1051比较好的文章

   http://blog.csdn.net/abcjennifer/article/details/8020695     Huffman 树实现代码

   https://baike.baidu.com/item/%E5%93%88%E5%A4%AB%E6%9B%BC%E7%BC%96%E7%A0%81/1719730?fr=aladdin   百度百科中也有huffman的实现代码

   http://blog.csdn.net/blues1021/article/details/45165777  下面有很多关于deflate的文章

. 构造哈夫曼树

    构造哈夫曼树非常简单，将所有的节点放到一个队列中，用一个节点替换两个频率最低的节点，新节点的频率就是这两个节点的频率之和。这样，新节点就是两个被替换节点的父节点了。如此循环，直到队列中只剩一个节点（树根）。

解压缩

    解压缩比构造哈夫曼树要简单的多，将输入缓冲区中的每个编码用对应的ASCII码逐个替换就可以了。只要记住，这里的输入缓冲区是一个包含每个ASCII值的编码的位流。因此，为了用ASCII值替换编码，我们必须用位流搜索哈夫曼树，直到发现一个叶节点，然后将它的ASCII值添加到输出缓冲区中：