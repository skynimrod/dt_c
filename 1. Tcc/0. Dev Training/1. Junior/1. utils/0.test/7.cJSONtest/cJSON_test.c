
    #include <stdio.h>
    #include <string.h>
    #include "cJSON.h"


    /* 解析文本为JSON, 然后再将JSON 转回文本, 然后打印出来! */
    void doit( char *text )
    {
    	char    *out;
        cJSON   *json, *ptr;
	
    	json = cJSON_Parse( text );
    	if ( !json ) {
                printf( "Error before: [%s]\n", cJSON_GetErrorPtr() );
        }
    	else {
            printf("-------\n");
            //print_CJSON( json );

            ptr = cJSON_getItem( json, "format");
            printf("\n===1===\n\n");
            if ( ptr ) {
                print_CJSON( ptr );
            }
            printf("===2===\n");
            printf("-------\n");
    		out = cJSON_Print( json );
    		cJSON_Delete( json );
    		printf( "%s\n", out );
    		free( out );
    	}
    }

    void main(void)
    {
        char   buf[128];

        char   tmpbuf[128];

        memset( buf, 0, 128 );
        memset( tmpbuf, 0, 128);

        sprintf( buf, "Hello, world!" );
        sprintf( tmpbuf,  cJSON_strdup( "false" ) );

        printf( "\nbuf=%s\ntmpbuf=%s\n", buf , tmpbuf );


	    /* a bunch of json: */
	    char text1[]="{\n\"name\": \"Jack (\\\"Bee\\\") Nimble\", \n\"format\": {\"type\":       \"rect\", \n\"width\":      1920, \n\"height\":     1080, \n\"interlace\":  false,\"frame rate\": 24\n}, \"test\":\"i think so!\"\n}";	
	    char text2[]="[\"Sunday\", \"Monday\", \"Tuesday\", \"Wednesday\", \"Thursday\", \"Friday\", \"Saturday\"]";
	    char text3[]="[\n    [0, -1, 0],\n    [1, 0, 0],\n    [0, 0, 1]\n	]\n";
    	char text4[]="{\n		\"Image\": {\n			\"Width\":  800,\n			\"Height\": 600,\n			\"Title\":  \"View from 15th Floor\",\n			\"Thumbnail\": {\n				\"Url\":    \"http:/*www.example.com/image/481989943\",\n				\"Height\": 125,\n				\"Width\":  \"100\"\n			},\n			\"IDs\": [116, 943, 234, 38793]\n		}\n	}";
	    char text5[]="[\n	 {\n	 \"precision\": \"zip\",\n	 \"Latitude\":  37.7668,\n	 \"Longitude\": -122.3959,\n	 \"Address\":   \"\",\n	 \"City\":      \"SAN FRANCISCO\",\n	 \"State\":     \"CA\",\n	 \"Zip\":       \"94107\",\n	 \"Country\":   \"US\"\n	 },\n	 {\n	 \"precision\": \"zip\",\n	 \"Latitude\":  37.371991,\n	 \"Longitude\": -122.026020,\n	 \"Address\":   \"\",\n	 \"City\":      \"SUNNYVALE\",\n	 \"State\":     \"CA\",\n	 \"Zip\":       \"94085\",\n	 \"Country\":   \"US\"\n	 }\n	 ]";

	    /* Process each json textblock by parsing, then rebuilding: */
	    doit(text1);
	    doit(text2);	
	    doit(text3);
	    doit(text4);
	    doit(text5);        
    }
