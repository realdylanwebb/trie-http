/* Copyright (c) 2020 Dylan Webb. All rights reserved.
*  
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*  
*  1. Redistributions of source code must retain the above copyright notice, this list of conditions
*  and the following disclaimer.
*  
*  2. Redistributions in binary form must reproduce the above copyright notice, 
*  this list of conditions and the following disclaimer in the documentation
*  and/or other materials provided with the distribution.
*  
*  3. Neither the name of the copyright holder nor the names of its contributors may
*  be used to endorse or promote products derived from this
*  software without specific prior written permission.
*  
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
*  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
*  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR 
*  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
*  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
*  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
*  POSSIBILITY OF SUCH DAMAGE. */

#ifndef __WH_PARSE_TRIE__
#define __WH_PARSE_TRIE__

#include <sys/types.h>
#include "./http_msg.h"

#define __TRIE_NODE_SIZE__ 256

typedef struct http_key_list_t {
    struct http_key_list_t* next;
    http_header_key_t header;
} http_key_list_t;

typedef struct trie_node_t {
    struct trie_node_t** next;
    http_header_key_t* header;
} trie_node_t;

typedef struct trie_t {
    trie_node_t* root;
} trie_t;


/* Created a linked list of http_header_key_t
*  starting at list.
*  returns 0 on success.
*  returns -1 on fail. */
int init_headers_list(http_key_list_t* list, size_t num_headers);


/* Loads the standard HTTP headers and common
*  proprietary headers from list into the trie 
*  pointed to by trie.
*  returns 0 on success.
*  returns -1 on fail.
*  returns -2 on a fail where the underlying
*  structures could not be freed. */
int load_header_trie(trie_t* trie, http_key_list_t* list);


/* Free the header trie pointed at by trie,
*  frees all children nodes as well.
*  returns 0 on success.
*  -1 if the trie could not be freed. */
int free_header_trie(trie_t* trie);


/* Predict the header pointed to by s.
*  returns 0 on success.
*  -1 if the header is not a standard header.
*  -2 if the request is malformed so that an EOL is reached. 
*  Define __PREDICT_MAX_BOUND__ to define a max bound for prediction
*  this shouldn't be necessary due to the length of standard headers, but
*  maybe a really malformed message could segfault. */
int predict_header(trie_t* trie, http_header_t* header, char* s);

#endif
