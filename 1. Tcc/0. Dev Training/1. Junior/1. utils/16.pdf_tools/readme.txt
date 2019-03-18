. 2019.03.18

  关于子列与子行的修正有问题， 还是需要改为原来的方式. 因为不同行的子列可能有相同的母列， 如果计算所有子列的maxlen 之和就错了！！！！

  fixCols(), fixRows() 更改为  fillColSon(), fillRowSon(), 并删除其中修正 maxlen, maxlines 的部分代码

  增加  fixColMaxlen(), fixRowMaxlines()， 并且放在fill_c_list() 之后才能运行，因为必须要访问c_list.