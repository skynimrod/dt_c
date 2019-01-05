将 cJSON 库  重写一遍，不然编译后的动态库总是不好用

. cjson2

. 解析json过程

      typedef struct cJSON {
    	struct cJSON    *next,*prev;	/* 用来遍历数组或对象. 要不然就用函数之类的处理GetArraySize/GetArrayItem/GetObjectItem */
    	struct cJSON    *child;		    /* 一个数组或对象指针，指向下一个数组或对象 */

    	int             type;		    /* The type of the item, as above. */

    	char            *valuestring;	/* The item's string, if type==cJSON_String */
    	int             valueint;		/* The item's number, if type==cJSON_Number */
    	double          valuedouble;	/* The item's number, if type==cJSON_Number */

    	char            *string;		/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    } cJSON;


   cJSON_Parse( jsontext )  解析json 串 ，结果返回到cJSON 结构指针中.

. cJSON_Parse()  内部调用  cJSON_ParsewithOpts( jsontext, 0, 0 )

  cJSON_ParsewithOpts() 的后2个参数，const char **return_parse_end, int require_null_terminated

   return_parse_end 是返回cJSON对象链的最后一个对象. 不过不需要就设为0.

   require_null_terminated  是 表示要求最后的对象是null结束， 如果不需要也可以设为0.
  申请一个cJSON 对象, 用来存放当前处理的JSON 对象数据. 

  cJSON_ParsewithOpts() 内部调用 parse_value()

. parse_value()  是真正处理json数据的函数， 处理结果存放在cJSON 对象中。

  是嵌套处理. 处理思路很好， 遍历一遍字符串就将整个json数据处理完了.

    static const char *parse_value( cJSON *item, const char *value )
    {
    	if ( !value )						
            return 0;	/* Fail on null. */
        
    	if ( !strncmp( value, "null", 4 ) )	{
            item->type = cJSON_NULL;  
            return value + 4; 
        }
        
    	if ( !strncmp( value, "false", 5 ) ) { 
            printf("\nkey=%s, value=false!\n", item->string );
            item->type = cJSON_False; 
            return value + 5; 
        }
        
    	if ( !strncmp( value, "true", 4 ) )	{ 
            item->type = cJSON_True; 
            item->valueint = 1;	
            return value + 4; 
        }
        
    	if ( *value == '\"' )	{ 
            return parse_string( item, value ); 
        }
        
    	if ( *value == '-' || ( *value >= '0' && *value <= '9' ) )	{
            return parse_number( item, value );
        }

    	if ( *value == '[' )	{ 
            return parse_array( item, value ); 
        }
        
    	if ( *value == '{' )	{ 
            return parse_object( item, value ); 
        }
    
    	ep = value;
        return 0;	/* failure. */
    }

    有个全局指针 ep  用来记录当前处理到json字符串的位置.

    object 中的数据放在 child 对象中, 将 object 的内容视作string 来处理， 并将处理结果存放在child 对象中. 第一个处理的string  实质上是解析出来的key值， 所以valuestring 内容转存到string 后, 将valuesstring 指针清零. 

. skip() 函数

  跳过 非字母或数字的字符(ascii值小于32的字符). 主要是为了跳过空格和回车换行

  static const char *skip( const char *in ) 
    {
        while ( in && *in && ( unsigned char ) *in <= 32 ) 
            in ++; 
        
        return in;
    }
  
. parse_object()

  先通过parse_string()  获取 key 值, 如果后面不是':', 那么就返回0, 表示格式不对. 然后通过parse_value() 获取 value 值.

  如果碰到','， 那么就新创建一个cJSON对象，并链接在 next, prev 链, 

{
        "name": "Jack (\"Bee\") Nimble",
        "format":       {
                "type": "rect",
                "width":        1920,
                "height":       1080,
                "interlace":    false,
                "frame rate":   24
        }
}