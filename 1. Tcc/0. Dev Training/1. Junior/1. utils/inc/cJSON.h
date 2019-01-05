    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    File_tools
    //
    // Description : about file operation.
    //
    // Interface :
    //        1.   IfFileExist()
    //        2.   IfOpened()
    //        3.   ifDirExist()
    //        4.   
    //
    // 外部接口:
    //        1.     cJSON_InitHooks( )
    //        
    #ifndef __CJSON_H__    // define file_tools.h Start {
    #define __CJSON_H__
    #include <ctype.h>
    
    /* cJSON Types: */
    #define cJSON_False     0
    #define cJSON_True      1
    #define cJSON_NULL      2
    #define cJSON_Number    3
    #define cJSON_String    4
    #define cJSON_Array     5
    #define cJSON_Object    6
	
    #define cJSON_IsReference   256
    #define cJSON_StringIsConst 512
    
    /* The cJSON structure: */
    typedef struct cJSON {
    	struct cJSON    *next,*prev;	/* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    	struct cJSON    *child;		    /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

    	int             type;		    /* The type of the item, as above. */

    	char            *valuestring;	/* The item's string, if type==cJSON_String */
    	int             valueint;		/* The item's number, if type==cJSON_Number */
    	double          valuedouble;	/* The item's number, if type==cJSON_Number */

    	char            *string;		/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    } cJSON;
    
    typedef struct cJSON_Hooks {
        void *(*malloc_fn)(size_t sz);
        void (*free_fn)(void *ptr);
    } cJSON_Hooks;
    
    // 1. 外部接口1, 初始化hooks
    int     pow2gt ( int x );
    char    *cJSON_Print( cJSON *item );
    char    *cJSON_GetErrorPtr( void );
    void    cJSON_Delete( cJSON * c );
    cJSON   *cJSON_Parse( const char *value );

    char    *cJSON_GetErrorPtr( void );
    
    void    print_CJSON( cJSON *item );
    cJSON   *cJSON_getItem( cJSON *item, char *key );


    // 2. 公用函数
    int cJSON_strcasecmp( char *s1, char *s2 );
    char * cJSON_strdup( const char * str );
    #endif                      //  define file_tools.h end }
