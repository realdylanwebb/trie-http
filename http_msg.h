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

#ifndef __WH_HTTP_MSG__
#define __WH_HTTP_MSG__

#include <sys/types.h>

typedef enum {
    H_GET,
    H_HEAD,
    H_POST,
    H_PUT,
    H_DELETE,
    H_CONNECT,
    H_OPTIONS,
    H_TRACE,
    H_PATCH
} method_t;

typedef struct http_header_key_t {
    char *name;
    size_t name_s;
} http_header_key_t;

typedef struct http_header_t {
    http_header_key_t* key;
    char* value;
    size_t value_s;
} http_header_t;

typedef struct http_req_t {
    method_t method;
    char* host_start;
    size_t host_s;
    char* path_start;
    size_t path_s;
    char* version_start;
    size_t version_s;
    http_header_t* headers_start;
    size_t headers_s;
    char* body_start;
    size_t body_s;
} http_req_t;


typedef struct http_res_t {
    char* version_start;
    size_t version_s;
    u_int16_t status;
    char* reason_start;
    size_t reason_s;
    http_header_t* headers_start;
    size_t headers_s;
    char* body_start;
    size_t body_s;
} http_res_t;

#endif
