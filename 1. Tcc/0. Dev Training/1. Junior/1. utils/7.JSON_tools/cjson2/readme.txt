�� cJSON ��  ��дһ�飬��Ȼ�����Ķ�̬�����ǲ�����

. cjson2

. ����json����

      typedef struct cJSON {
    	struct cJSON    *next,*prev;	/* ����������������. Ҫ��Ȼ���ú���֮��Ĵ���GetArraySize/GetArrayItem/GetObjectItem */
    	struct cJSON    *child;		    /* һ����������ָ�룬ָ����һ���������� */

    	int             type;		    /* The type of the item, as above. */

    	char            *valuestring;	/* The item's string, if type==cJSON_String */
    	int             valueint;		/* The item's number, if type==cJSON_Number */
    	double          valuedouble;	/* The item's number, if type==cJSON_Number */

    	char            *string;		/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    } cJSON;


   cJSON_Parse( jsontext )  ����json �� ��������ص�cJSON �ṹָ����.

. cJSON_Parse()  �ڲ�����  cJSON_ParsewithOpts( jsontext, 0, 0 )

  cJSON_ParsewithOpts() �ĺ�2��������const char **return_parse_end, int require_null_terminated

   return_parse_end �Ƿ���cJSON�����������һ������. ��������Ҫ����Ϊ0.

   require_null_terminated  �� ��ʾҪ�����Ķ�����null������ �������ҪҲ������Ϊ0.
  ����һ��cJSON ����, ������ŵ�ǰ�����JSON ��������. 

  cJSON_ParsewithOpts() �ڲ����� parse_value()

. parse_value()  ����������json���ݵĺ����� �����������cJSON �����С�

  ��Ƕ�״���. ����˼·�ܺã� ����һ���ַ����ͽ�����json���ݴ�������.

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

    �и�ȫ��ָ�� ep  ������¼��ǰ����json�ַ�����λ��.

    object �е����ݷ��� child ������, �� object ����������string ������ ���������������child ������. ��һ�������string  ʵ�����ǽ���������keyֵ�� ����valuestring ����ת�浽string ��, ��valuesstring ָ������. 

. skip() ����

  ���� ����ĸ�����ֵ��ַ�(asciiֵС��32���ַ�). ��Ҫ��Ϊ�������ո�ͻس�����

  static const char *skip( const char *in ) 
    {
        while ( in && *in && ( unsigned char ) *in <= 32 ) 
            in ++; 
        
        return in;
    }
  
. parse_object()

  ��ͨ��parse_string()  ��ȡ key ֵ, ������治��':', ��ô�ͷ���0, ��ʾ��ʽ����. Ȼ��ͨ��parse_value() ��ȡ value ֵ.

  �������','�� ��ô���´���һ��cJSON���󣬲������� next, prev ��, 

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