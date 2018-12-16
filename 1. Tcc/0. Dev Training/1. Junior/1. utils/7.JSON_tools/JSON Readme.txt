参见:

    https://blog.csdn.net/xy913741894/article/details/65445192

    https://sourceforge.net/projects/cjson/files/latest/download    cJson 库

. JSON ( JavaScript Object Notation,  JS 对象标记) 

  是一种轻量级的数据交换格式. 它基于 ECMAScript( w3c 制定的js 规范) 的一个子集, 采用完全独立于编程语言的文本格式来存储和表示数据. 

. JSON 语法

  JSON  语法规则

      在JS语言中, 一切都是对象. 因此, 任何支持的类型都可以通过JSON 来表示, 例如  字符串、数字、对象、数组等. 但是 对象和数组是比较特殊且常用的两种类型:

     1. 对象表示为键值对

     2. 数据由逗号分隔

     3. 花括号保存对象

     4. 方括号保存数组

 JSON 键/值对

     JSON 键值对 用来保存JS对象的一种方式, 和JS对象的写法大同小异, 键值对组合中的键名 写在前面 并用双引号"" 包裹, 使用冒号: 分隔, 然后紧接着 值:

     { "firstName": "Json"}

     这很容易理解, 等价于这条JavaScript 语句:

     { firstName : "Json" }

JSON 与 JS 对象的关系

    很多人搞不清楚 JSON 和 Js 对象的关系，甚至连谁是谁都不清楚。其实，可以这么理解：

    JSON 是 JS 对象的字符串表示法，它使用文本表示一个 JS 对象的信息，本质是一个字符串。

    如
	
       var obj = {a: 'Hello', b: 'World'}; //这是一个对象，注意键名也是可以使用引号包裹的

       var json = '{"a": "Hello", "b": "World"}'; //这是一个 JSON 字符串，本质是一个字符串

JSON 和 JS 对象互转
  
     要实现从对象转换为 JSON 字符串，使用 JSON.stringify() 方法：
	
          var json = JSON.stringify({a: 'Hello', b: 'World'}); //结果是 '{"a": "Hello", "b": "World"}'

     要实现从 JSON 转换为对象，使用 JSON.parse() 方法：
	
          var obj = JSON.parse('{"a": "Hello", "b": "World"}'); //结果是 {a: 'Hello', b: 'World'}


基础示例

    简单地说，JSON 可以将 JavaScript 对象中表示的一组数据转换为字符串，然后就可以在网络或者程序之间轻松地传递这个字符串，并在需要的时候将它还原为各编程语言所支持的数据格式，例如在 PHP 中，可以将 JSON 还原为数组或者一个基本对象。在用到AJAX时，如果需要用到数组传值，这时就需要用JSON将数组转化为字符串。


    表示对象

        JSON最常用的格式是对象的 键值对。例如下面这样：
	
         {"firstName": "Brett", "lastName": "McLaughlin"}
    表示数组

          和普通的 JS 数组一样，JSON 表示数组的方式也是使用方括号 []。
      {
 
          "people":[
         
               {
                    "firstName": "Brett",
                    "lastName":"McLaughlin"
               },
               {
                    "firstName": "Jason",
                    "lastName":"Hunter"
               }
         ]
     }

    这不难理解。在这个示例中，只有一个名为 people的变量，值是包含两个条目的数组，每个条目是一个人的记录，其中包含名和姓。上面的示例演示如何用括号将记录组合成一个值。当然，可以使用相同的语法表示更过多的值（每个值包含多个记录）。

    在处理 JSON 格式的数据时，没有需要遵守的预定义的约束。所以，在同样的数据结构中，可以改变表示数据的方式，也可以使用不同方式表示同一事物。
如前面所说，除了对象和数组，你也可以简单地使用字符串或者数字等来存储简单的数据，但这样并没有多大意义。