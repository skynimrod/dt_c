    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    pdf_tools
    //
    // Description : about pdf parse operation.
    //
    // Interface :
    //  
    #include <stdlib.h>
    #include "filemap.h"
    #include "memorymap.h"
    #include "pdf_tools.h"

    #define DLL_EXPORT __declspec(dllexport)

    void    freeAll( POSMAP * posmap_p, TRAILER * trailer_p, XREF * xref_p, PAGES *pages_p, FILEMAP * fm_p );
    void    freeALL2( PDF * pdf_p );
    
    //以下为内部调用的API
    void freeXREF( XREF * xref_p ) 
    {
        if ( xref_p->objpos_p )
            free( xref_p->objpos_p );

        free( xref_p );
    }

    void freePAGES( PAGES * pages_p ) 
    {
        if ( pages_p->leafs_p != NULL )
            free(  pages_p->leafs_p );

        free( pages_p );
    }

    void  DLL_EXPORT freeALL2( PDF * pdf_p )
    {
        printf("===================================================freeALL1()===================================================\n");
        if ( pdf_p ) {
            if ( pdf_p->posmap_p )
                free( pdf_p->posmap_p );

            if ( pdf_p->trailer_p )
                free( pdf_p->trailer_p );

            if ( pdf_p->xref_p )
                freeXREF( pdf_p->xref_p );

            if ( pdf_p->pages_p )
                freePAGES( pdf_p->pages_p );

            if ( pdf_p->fm_p )
                freeFileMap( pdf_p->fm_p );

            free( pdf_p );
        }
    }

    void  freeAll( POSMAP * posmap_p, TRAILER * trailer_p, XREF * xref_p, PAGES *pages_p, FILEMAP * fm_p )
    {
        if ( posmap_p )
            free( posmap_p );

        if ( trailer_p )
            free( trailer_p );

        freeXREF( xref_p );

        freePAGES( pages_p );

        if ( fm_p )
            freeFileMap( fm_p );
    }

