    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    deflate
    //
    // Description : about deflate.
    //
    // Interface :
    //        1.   IfFileExist()
    //        2.   IfOpened()
    //        3.   ifDirExist()
    //        4.   
    //
    #ifndef __DELFATE_H__    // define deflate.h Start {
    #define __DEFLATE_H__
    
    #define LENGTH_CODES 29                     /* number of length codes, not counting the special END_BLOCK code */

    #define LITERALS  256                       /* number of literal bytes 0..255 */

    #define L_CODES (LITERALS+1+LENGTH_CODES)   /* number of Literal or Length codes, including the END_BLOCK code */

    #define D_CODES   30                        /* number of distance codes */

    #define BL_CODES  19                        /* number of codes used to transfer the bit lengths */

    #define HEAP_SIZE (2*L_CODES+1)             /* maximum heap size */

    #define MAX_BITS 15                         /* All codes must not exceed MAX_BITS bits */

    #define Buf_size 16                         /* size of bit buffer in bi_buf */

    #define INIT_STATE    42    /* zlib header -> BUSY_STATE */
    #ifdef GZIP
    #  define GZIP_STATE  57    /* gzip header -> BUSY_STATE | EXTRA_STATE */
    #endif
    #define EXTRA_STATE   69    /* gzip extra block -> NAME_STATE */
    #define NAME_STATE    73    /* gzip file name -> COMMENT_STATE */
    #define COMMENT_STATE 91    /* gzip comment -> HCRC_STATE */
    #define HCRC_STATE   103    /* gzip header CRC -> BUSY_STATE */
    #define BUSY_STATE   113    /* deflate -> FINISH_STATE */
    #define FINISH_STATE 666    /* stream complete */
    /* Stream status */
    
    typedef unsigned short ush;

    /* 描述单值及其编码串的数据结构    Data structure describing a single value and its code string. */
    typedef struct ct_data_s {
        union {
            ush  freq;       /* 频率计数 frequency count */
            ush  code;       /* 位串bit string */
        } fc;
        union {
            ush  dad;        /* 霍夫曼树中的父节点 father node in Huffman tree */
            ush  len;        /* 位串的  长度  length of bit string */
        } dl;
    } far ct_data;
    
    #define Freq fc.freq
    #define Code fc.code
    #define Dad  dl.dad
    #define Len  dl.len
    
    typedef struct static_tree_desc_s  static_tree_desc;
    
    typedef struct tree_desc_s {
        ct_data *dyn_tree;           /* 动态树   the dynamic tree */
        int     max_code;            /* 没有零出现的最长编码    largest code with non zero frequency */
        const static_tree_desc *stat_desc;  /* the corresponding static tree */
    } far tree_desc;
   
    typedef ush Pos;
    typedef Pos FAR Posf;
    typedef unsigned IPos;
    
    /* A Pos is an index in the character window. We use short instead of int to
     * save space in the various tables. IPos is used only for parameter passing.
     */
typedef voidpf (*alloc_func) OF((voidpf opaque, uInt items, uInt size));
typedef void   (*free_func)  OF((voidpf opaque, voidpf address));
    
typedef struct z_stream_s {
    z_const Bytef *next_in;     /* next input byte */
    uInt     avail_in;  /* number of bytes available at next_in */
    uLong    total_in;  /* total number of input bytes read so far */

    Bytef    *next_out; /* next output byte will go here */
    uInt     avail_out; /* remaining free space at next_out */
    uLong    total_out; /* total number of bytes output so far */

    z_const char *msg;  /* last error message, NULL if no error */
    struct internal_state FAR *state; /* not visible by applications */

    alloc_func zalloc;  /* used to allocate the internal state */
    free_func  zfree;   /* used to free the internal state */
    voidpf     opaque;  /* private data object passed to zalloc and zfree */

    int     data_type;  /* best guess about the data type: binary or text
                           for deflate, or the decoding state for inflate */
    uLong   adler;      /* Adler-32 or CRC-32 value of the uncompressed data */
    uLong   reserved;   /* reserved for future use */
} z_stream;

typedef z_stream FAR *z_streamp;    typedef struct internal_state {
        z_streamp strm;      /* pointer back to this zlib stream */
        int   status;        /* as the name implies */
        Bytef *pending_buf;  /* output still pending */
        ulg   pending_buf_size; /* size of pending_buf */
        Bytef *pending_out;  /* next pending byte to output to the stream */
        ulg   pending;       /* nb of bytes in the pending buffer */
        int   wrap;          /* bit 0 true for zlib, bit 1 true for gzip */
        gz_headerp  gzhead;  /* gzip header information to write */
        ulg   gzindex;       /* where in extra, name, or comment */
        Byte  method;        /* can only be DEFLATED */
        int   last_flush;    /* value of flush param for previous deflate call */
    
                    /* used by deflate.c: */
    
        uInt  w_size;        /* LZ77 window size (32K by default) */
        uInt  w_bits;        /* log2(w_size)  (8..16) */
        uInt  w_mask;        /* w_size - 1 */
    
        Bytef *window;
        /* Sliding window. Input bytes are read into the second half of the window,
         * and move to the first half later to keep a dictionary of at least wSize
         * bytes. With this organization, matches are limited to a distance of
         * wSize-MAX_MATCH bytes, but this ensures that IO is always
         * performed with a length multiple of the block size. Also, it limits
         * the window size to 64K, which is quite useful on MSDOS.
         * To do: use the user input buffer as sliding window.
         */
    
        ulg window_size;
        /* Actual size of window: 2*wSize, except when the user input buffer
         * is directly used as sliding window.
         */
    
        Posf *prev;
        /* Link to older string with same hash index. To limit the size of this
         * array to 64K, this link is maintained only for the last 32K strings.
         * An index in this array is thus a window index modulo 32K.
         */
    
        Posf *head; /* Heads of the hash chains or NIL. */
    
        uInt  ins_h;          /* hash index of string to be inserted */
        uInt  hash_size;      /* number of elements in hash table */
        uInt  hash_bits;      /* log2(hash_size) */
        uInt  hash_mask;      /* hash_size-1 */
    
        uInt  hash_shift;
        /* Number of bits by which ins_h must be shifted at each input
         * step. It must be such that after MIN_MATCH steps, the oldest
         * byte no longer takes part in the hash key, that is:
         *   hash_shift * MIN_MATCH >= hash_bits
         */
    
        long block_start;
        /* Window position at the beginning of the current output block. Gets
         * negative when the window is moved backwards.
         */
    
        uInt match_length;           /* length of best match */
        IPos prev_match;             /* previous match */
        int match_available;         /* set if previous match exists */
        uInt strstart;               /* start of string to insert */
        uInt match_start;            /* start of matching string */
        uInt lookahead;              /* number of valid bytes ahead in window */
    
        uInt prev_length;
        /* Length of the best match at previous step. Matches not greater than this
         * are discarded. This is used in the lazy match evaluation.
         */
    
        uInt max_chain_length;
        /* To speed up deflation, hash chains are never searched beyond this
         * length.  A higher limit improves compression ratio but degrades the
         * speed.
         */
    
        uInt max_lazy_match;
        /* Attempt to find a better match only when the current match is strictly
         * smaller than this value. This mechanism is used only for compression
         * levels >= 4.
         */
    #   define max_insert_length  max_lazy_match
        /* Insert new strings in the hash table only if the match length is not
         * greater than this length. This saves time but degrades compression.
         * max_insert_length is used only for compression levels <= 3.
         */
    
        int level;    /* compression level (1..9) */
        int strategy; /* favor or force Huffman coding*/
    
        uInt good_match;
        /* Use a faster search when the previous match is longer than this */
    
        int nice_match; /* Stop searching when current match exceeds this */
    
                    /* used by trees.c: */
        /* Didn't use ct_data typedef below to suppress compiler warning */
        struct ct_data_s dyn_ltree[HEAP_SIZE];   /* literal and length tree */
        struct ct_data_s dyn_dtree[2*D_CODES+1]; /* distance tree */
        struct ct_data_s bl_tree[2*BL_CODES+1];  /* Huffman tree for bit lengths */
    
        struct tree_desc_s l_desc;               /* desc. for literal tree */
        struct tree_desc_s d_desc;               /* desc. for distance tree */
        struct tree_desc_s bl_desc;              /* desc. for bit length tree */
    
        ush bl_count[MAX_BITS+1];
        /* number of codes at each bit length for an optimal tree */
    
        int heap[2*L_CODES+1];      /* heap used to build the Huffman trees */
        int heap_len;               /* number of elements in the heap */
        int heap_max;               /* element of largest frequency */
        /* The sons of heap[n] are heap[2*n] and heap[2*n+1]. heap[0] is not used.
         * The same heap array is used to build all trees.
         */
    
        uch depth[2*L_CODES+1];
        /* Depth of each subtree used as tie breaker for trees of equal frequency
         */
    
        uchf *l_buf;          /* buffer for literals or lengths */
    
        uInt  lit_bufsize;
        /* Size of match buffer for literals/lengths.  There are 4 reasons for
         * limiting lit_bufsize to 64K:
         *   - frequencies can be kept in 16 bit counters
         *   - if compression is not successful for the first block, all input
         *     data is still in the window so we can still emit a stored block even
         *     when input comes from standard input.  (This can also be done for
         *     all blocks if lit_bufsize is not greater than 32K.)
         *   - if compression is not successful for a file smaller than 64K, we can
         *     even emit a stored file instead of a stored block (saving 5 bytes).
         *     This is applicable only for zip (not gzip or zlib).
         *   - creating new Huffman trees less frequently may not provide fast
         *     adaptation to changes in the input data statistics. (Take for
         *     example a binary file with poorly compressible code followed by
         *     a highly compressible string table.) Smaller buffer sizes give
         *     fast adaptation but have of course the overhead of transmitting
         *     trees more frequently.
         *   - I can't count above 4
         */
    
        uInt last_lit;      /* running index in l_buf */
    
        ushf *d_buf;
        /* Buffer for distances. To simplify the code, d_buf and l_buf have
         * the same number of elements. To use different lengths, an extra flag
         * array would be necessary.
         */
    
        ulg opt_len;        /* bit length of current block with optimal trees */
        ulg static_len;     /* bit length of current block with static trees */
        uInt matches;       /* number of string matches in current block */
        uInt insert;        /* bytes at end of window left to insert */
    
    #ifdef ZLIB_DEBUG
        ulg compressed_len; /* total bit length of compressed file mod 2^32 */
        ulg bits_sent;      /* bit length of compressed data sent mod 2^32 */
    #endif
    
        ush bi_buf;
        /* Output buffer. bits are inserted starting at the bottom (least
         * significant bits).
         */
        int bi_valid;
        /* Number of valid bits in bi_buf.  All bits above the last valid bit
         * are always zero.
         */
    
        ulg high_water;
        /* High water mark offset in window for initialized bytes -- bytes above
         * this are set to zero in order to avoid memory check warnings when
         * longest match routines access bytes past the input.  This is then
         * updated to the new high water mark.
         */
    
    } far deflate_state;
    #endif
