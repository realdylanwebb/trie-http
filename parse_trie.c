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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "./http_msg.h"
#include "./parse_trie.h"

char* HTTP_STANDARD_HEADERS[] = {
    "Access-Control-Allow-Credentials",
    "Access-Control-Allow-Headers",
    "Access-Control-Allow-Methods",
    "Access-Control-Allow-Origin",
    "Access-Control-Expose-Headers",
    "Access-Control-Max-Age",
    "Accept-Ranges",
    "Age",
    "Allow",
    "Alternate-Protocol",
    "Cache-Control",
    "Client-Date",
    "Client-Peer",
    "Client-Response-Num",
    "Connection",
    "Content-Disposition",
    "Content-Encoding",
    "Content-Language",
    "Content-Length",
    "Content-Location",
    "Content-MD5",
    "Content-Range",
    "Content-Security-Policy",
    "X-Content-Security-Policy", 
    "X-WebKit-CSP",
    "Content-Security-Policy-Report-Only",
    "Content-Type",
    "Date",
    "ETag",
    "Expires",
    "HTTP",
    "Keep-Alive",
    "Last-Modified",
    "Link",
    "Location",
    "P3P",
    "Pragma",
    "Proxy-Authenticate",
    "Proxy-Connection",
    "Refresh",
    "Retry-After",
    "Server",
    "Set-Cookie",
    "Status",
    "Strict-Transport-Security",
    "Timing-Allow-Origin",
    "Trailer",
    "Transfer-Encoding",
    "Upgrade",
    "Vary",
    "Via",
    "Warning",
    "WWW-Authenticate",
    "X-Aspnet-Version",
    "X-Content-Type-Options",
    "X-Frame-Options",
    "X-Permitted-Cross-Domain-Policies",
    "X-Pingback",
    "X-Powered-By",
    "X-Robots-Tag",
    "X-UA-Compatible",
    "X-XSS-Protection"
};


typedef struct trie_node_trace_t {
    struct trie_node_trace_t* upper;
    trie_node_t* node;
} trie_node_trace_t;

trie_node_t* get_node() {
    trie_node_t* node = (trie_node_t*)malloc(sizeof(trie_node_t));
    if (node == NULL) {
        return NULL;
    }
    node->next = calloc(sizeof(trie_node_t*), __TRIE_NODE_SIZE__);
    node->header = NULL;
    return node;
}


void free_node(trie_node_t* trie) {
    free(trie->next);
    free(trie);
    return;
}


int insert_header(trie_t* trie, http_header_key_t* key) {
    int i;
    trie_node_t* current_node;
    char* key_start;

    current_node = trie->root;
    i = 0;
    key_start = key->name;

    for (;;) {

        CHECK:
        if (current_node->next[(int)key_start[i]] == NULL) {
            if (current_node->header == NULL) {
                current_node->header = key;
                return 0;
            }

            char temp = current_node->header->name[i];
            current_node->next[(int)temp] = get_node();
            if (current_node->next[(int)temp] == NULL) {
                if (free_header_trie(trie) != 0) {
                    return -2;
                }
                return -1;
            }
            current_node->next[(int)temp]->header = current_node->header;
            current_node->header = NULL;
            goto CHECK;
        }
        current_node = current_node->next[(int)key_start[i]];
        
        i++;
        if (key_start[i] == '\0') {
            break;
        }
    }

    //reached last char, force insert
    if (current_node->header = NULL) {
        current_node->header = key;
        return 0;
    }
    
    if (current_node->header->name_s == i ) {
        return 0;
    }

    char temp = current_node->header->name[i];
    current_node->next[(int)temp] = get_node();
    current_node->next[(int)temp]->header = current_node->header;
    current_node->header = key;
    return 0;
}


//TODO FIX header.name_s OR REMOVE IT 
//SINCE HEADER STRINGS ARE '\0' TERMINATED
int init_headers_list(http_key_list_t* list, size_t num_headers) {
    http_key_list_t *current, *next;

    current = list;

    for (int i = 0; i < num_headers; i++) {
        current->next = (http_key_list_t*)malloc(sizeof(http_key_list_t));
        if (current->next == NULL) {
            goto LISTERR;
        }
        current->next->header.name = HTTP_STANDARD_HEADERS[i];
        current->next->header.name_s = sizeof(HTTP_STANDARD_HEADERS[i]);
        current = current->next;
    }

    current->next = NULL;
    return 0;

    LISTERR:
    current = list;
    do {
        next = current->next;
        free(current);
        current = next;
    } while(current != NULL);
    return -1;
}


int load_header_trie(trie_t* trie, http_key_list_t* list) {
    http_key_list_t* current;
    trie->root = get_node();
    trie->root->header = NULL;
    current = list;
    while (current->next != NULL) {
        if (insert_header(trie, &current->header) != 0) {
            if (free_header_trie(trie) != 0) {
                return -2;
            }
            return -1;
        }
        current = current->next;
    }
    return 0;
}


int free_node_recursive(trie_node_t* node) {
    int i;
    trie_node_t *current;
    trie_node_trace_t *current_trace, *deref;

    current = node;
    current_trace = (trie_node_trace_t*)malloc(sizeof(trie_node_trace_t));
    if (current_trace == NULL) {
        return -1;
    }
    current_trace->node = node;
    current_trace->upper = NULL;

    if (node == NULL) {
        return 0;
    }

    FREE_SEARCH:
    for (i = 0; i < __TRIE_NODE_SIZE__; i++) {
        if (current->next[i] != NULL) {
            trie_node_trace_t* new_trace = (trie_node_trace_t*)malloc(sizeof(trie_node_trace_t));
            if (new_trace == NULL) {
                //malloc fail
                //this one is pretty unrecoverable
                goto FREE_ERR;
            }
            current = current->next[i];
            i = 0;
            goto FREE_SEARCH;
        }

    }

    if (current == node) {
        free(current);
        return 0;
    }

    free(current);
    current = current_trace->upper->node;
    deref = current_trace->upper;
    free(current_trace);
    current_trace = deref;

    goto FREE_SEARCH;

    FREE_ERR:
    while (current_trace != NULL) {
        trie_node_trace_t* temp = current_trace->upper;
        free(current_trace);
        current_trace = temp;
    }
    return -1;
}


int free_header_trie(trie_t* trie) {
    return free_node_recursive(trie->root);
}


//required that the http message be '\0' terminated for bounds checking
int predict_header(trie_t* trie, http_header_t* header, char* s) {
    int i;
    trie_node_t* current_node;

    current_node = trie->root;
    i = 0;

    for (;;) {
        #ifdef __PREDICT_MAX_BOUND__
        if (s[i] == '\0' || i > ___PREDICT_MAX_BOUND__) {
            //EOL
            return -2;
        }
        #endif
        if (current_node->next[(int)s[i]] == NULL || s[i+1] == ':') {
            if (current_node->header == NULL) {
                //must use traditional lr parsing for this header.
                return -1;
            }
            header->key = current_node->header;
            return 0;
        }
        current_node = current_node->next[(int)s[i]];
        i++;
    }
}
