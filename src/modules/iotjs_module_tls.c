/* Copyright 2017-present Samsung Electronics Co., Ltd. and other contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#include <stdlib.h>
#define mbedtls_time       time
#define mbedtls_time_t     time_t
#define mbedtls_fprintf    fprintf
#define mbedtls_printf     printf
#endif

#if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_ENTROPY_C) ||  \
    !defined(MBEDTLS_SSL_TLS_C) || !defined(MBEDTLS_SSL_CLI_C) || \
    !defined(MBEDTLS_NET_C) || !defined(MBEDTLS_RSA_C) ||         \
    !defined(MBEDTLS_CERTS_C) || !defined(MBEDTLS_PEM_PARSE_C) || \
    !defined(MBEDTLS_CTR_DRBG_C) || !defined(MBEDTLS_X509_CRT_PARSE_C)
int main( void )
{
    mbedtls_printf("MBEDTLS_BIGNUM_C and/or MBEDTLS_ENTROPY_C and/or "
           "MBEDTLS_SSL_TLS_C and/or MBEDTLS_SSL_CLI_C and/or "
           "MBEDTLS_NET_C and/or MBEDTLS_RSA_C and/or "
           "MBEDTLS_CTR_DRBG_C and/or MBEDTLS_X509_CRT_PARSE_C "
           "not defined.\n");
    return( 0 );
}
#else

#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#include <string.h>

#define SERVER_PORT "443"
#define GET_REQUEST "%s"


#include "iotjs_def.h"
#include "iotjs_module_tls.h"
#include "iotjs_objectwrap.h"


static JNativeInfoType this_module_native_info = {.free_cb = NULL };


static iotjs_tls_t* iotjs_tls_instance_from_jval(iotjs_jval_t jtls);


static iotjs_tls_t* iotjs_tls_create(const iotjs_jval_t jtls) {
  iotjs_tls_t* tls = IOTJS_ALLOC(iotjs_tls_t);
  IOTJS_VALIDATED_STRUCT_CONSTRUCTOR(iotjs_tls_t, tls);
  iotjs_jobjectwrap_initialize(&_this->jobjectwrap, jtls,
                               &this_module_native_info);

  return tls;
}


static void iotjs_tls_destroy(iotjs_tls_t* tls) {
  IOTJS_VALIDATED_STRUCT_DESTRUCTOR(iotjs_tls_t, tls);
  iotjs_jobjectwrap_destroy(&_this->jobjectwrap);
#if defined(__linux__)
  iotjs_string_destroy(&_this->device);
#endif
  IOTJS_RELEASE(tls);
}


#define THIS iotjs_tls_reqwrap_t* tls_reqwrap


static iotjs_tls_reqwrap_t* iotjs_tls_reqwrap_create(
    const iotjs_jval_t jcallback, iotjs_tls_t* tls, TlsOp op) {
  iotjs_tls_reqwrap_t* tls_reqwrap = IOTJS_ALLOC(iotjs_tls_reqwrap_t);
  IOTJS_VALIDATED_STRUCT_CONSTRUCTOR(iotjs_tls_reqwrap_t, tls_reqwrap);

  iotjs_reqwrap_initialize(&_this->reqwrap, jcallback, (uv_req_t*)&_this->req);

  _this->req_data.op = op;
  _this->tls_instance = tls;
  return tls_reqwrap;
}

static void iotjs_tls_reqwrap_destroy(THIS) {
  IOTJS_VALIDATED_STRUCT_DESTRUCTOR(iotjs_tls_reqwrap_t, tls_reqwrap);
  iotjs_reqwrap_destroy(&_this->reqwrap);
  IOTJS_RELEASE(tls_reqwrap);
}


static void iotjs_tls_reqwrap_dispatched(THIS) {
  IOTJS_VALIDATABLE_STRUCT_METHOD_VALIDATE(iotjs_tls_reqwrap_t, tls_reqwrap);
  iotjs_tls_reqwrap_destroy(tls_reqwrap);
}


static uv_work_t* iotjs_tls_reqwrap_req(THIS) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_tls_reqwrap_t, tls_reqwrap);
  return &_this->req;
}


static iotjs_jval_t iotjs_tls_reqwrap_jcallback(THIS) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_tls_reqwrap_t, tls_reqwrap);
  return iotjs_reqwrap_jcallback(&_this->reqwrap);
}


static iotjs_tls_t* iotjs_tls_instance_from_jval(iotjs_jval_t jtls) {
  uintptr_t handle = iotjs_jval_get_object_native_handle(jtls);
  return (iotjs_tls_t*)handle;
}


iotjs_tls_reqwrap_t* iotjs_tls_reqwrap_from_request(uv_work_t* req) {
  return (iotjs_tls_reqwrap_t*)(iotjs_reqwrap_from_request((uv_req_t*)req));
}


iotjs_tls_reqdata_t* iotjs_tls_reqwrap_data(THIS) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_tls_reqwrap_t, tls_reqwrap);
  return &_this->req_data;
}


iotjs_tls_t* iotjs_tls_instance_from_reqwrap(THIS) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_tls_reqwrap_t, tls_reqwrap);
  return _this->tls_instance;
}


#undef THIS


static void iotjs_tls_after_work(uv_work_t* work_req, int status) {
  iotjs_tls_reqwrap_t* req_wrap = iotjs_tls_reqwrap_from_request(work_req);
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_tls_reqwrap_t, req_wrap);

  iotjs_tls_reqdata_t* req_data = &_this->req_data;
  iotjs_jargs_t jargs = iotjs_jargs_create(2);
  bool result = req_data->result;

  if (status) {
    iotjs_jval_t error = iotjs_jval_create_error("System error");
    iotjs_jargs_append_jval(&jargs, error);
    jerry_release_value(error);
  } else {
    switch (req_data->op) {
      case kTlsOpOpen:
        if (!result) {
          iotjs_jargs_append_error(&jargs, "Failed to open TLS device");
        } else {
          iotjs_jargs_append_null(&jargs);
        }
        break;
      case kTlsOpRead:
        if (!result) {
          iotjs_jargs_append_error(&jargs, "Cannot read from TLS device");
        } else {
          iotjs_jargs_append_null(&jargs);
          iotjs_jargs_append_number(&jargs, req_data->value);
        }
        break;
      case kTlsOpClose:
        if (!result) {
          iotjs_jargs_append_error(&jargs, "Cannot close TLS device");
        } else {
          iotjs_jargs_append_null(&jargs);
        }
        break;
      default: {
        IOTJS_ASSERT(!"Unreachable");
        break;
      }
    }
  }

  const iotjs_jval_t jcallback = iotjs_tls_reqwrap_jcallback(req_wrap);
  iotjs_make_callback(jcallback, jerry_create_undefined(), &jargs);

  iotjs_jargs_destroy(&jargs);

  iotjs_tls_reqwrap_dispatched(req_wrap);

  if (req_data->op == kTlsOpClose) {
    iotjs_tls_destroy(_this->tls_instance);
  }
}

void iotjs_tls_open_worker(uv_work_t* work_req) {
  TLS_WORKER_INIT;
  req_data->result = true;
}


#define TLS_ASYNC(call, this, jcallback, op)                                   \
  do {                                                                         \
    uv_loop_t* loop = iotjs_environment_loop(iotjs_environment_get());         \
    iotjs_tls_reqwrap_t* req_wrap =                                            \
        iotjs_tls_reqwrap_create(jcallback, this, op);                         \
    uv_work_t* req = iotjs_tls_reqwrap_req(req_wrap);                          \
    uv_queue_work(loop, req, iotjs_tls_##call##_worker, iotjs_tls_after_work); \
  } while (0)

JHANDLER_FUNCTION(TlsConstructor) {
  DJHANDLER_CHECK_THIS(object);

  // Create TLS object
  const iotjs_jval_t jtls = JHANDLER_GET_THIS(object);
  iotjs_tls_t* tls = iotjs_tls_create(jtls);
  IOTJS_ASSERT(tls == iotjs_tls_instance_from_jval(jtls));
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_tls_t, tls);

  const iotjs_jval_t jconfiguration = JHANDLER_GET_ARG_IF_EXIST(0, object);
  if (jerry_value_is_null(jconfiguration)) {
    JHANDLER_THROW(TYPE, "Bad arguments - configuration should be Object");
    return;
  }

#if defined(__linux__)
  DJHANDLER_GET_REQUIRED_CONF_VALUE(jconfiguration, _this->device,
                                    IOTJS_MAGIC_STRING_DEVICE, string);
#elif defined(__NUTTX__) || defined(__TIZENRT__)
  DJHANDLER_GET_REQUIRED_CONF_VALUE(jconfiguration, _this->pin,
                                    IOTJS_MAGIC_STRING_PIN, number);
#endif

  if (iotjs_jhandler_get_arg_length(jhandler) > 1) {
    const iotjs_jval_t jcallback = iotjs_jhandler_get_arg(jhandler, 1);
    if (jerry_value_is_function(jcallback)) {
      TLS_ASYNC(open, tls, jcallback, kTlsOpOpen);
    } else {
      JHANDLER_THROW(TYPE, "Bad arguments - callback should be Function");
      return;
    }
  } else {
    iotjs_jval_t jdummycallback =
        iotjs_jval_create_function(&iotjs_jval_dummy_function);
    TLS_ASYNC(open, tls, jdummycallback, kTlsOpOpen);
    jerry_release_value(jdummycallback);
  }
}

#define MIN(x, y) ((x) < (y) ? (x) : (y))


JHANDLER_FUNCTION(ReadSync) {
  JHANDLER_DECLARE_THIS_PTR(tls, tls);

  // IP address, host, path, data, bearer
  DJHANDLER_CHECK_ARGS(2, string, string);
  iotjs_string_t address = JHANDLER_GET_ARG(0, string);
  iotjs_string_t host = JHANDLER_GET_ARG(1, string);
  const char* str_address = iotjs_string_data(&address);
  const char* str_host = iotjs_string_data(&host);


/************** from here *****************************************************/


    int ret;
    size_t len;
    mbedtls_net_context server_fd;
    uint32_t flags;
    unsigned char buf[1024];
    const char *pers = "ssl_client1";

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;

    /*
     * 0. Initialize the RNG and the session data
     */
    mbedtls_net_init( &server_fd );
    mbedtls_ssl_init( &ssl );
    mbedtls_ssl_config_init( &conf );
    mbedtls_x509_crt_init( &cacert );
    mbedtls_ctr_drbg_init( &ctr_drbg );


    mbedtls_entropy_init( &entropy );
    if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                               (const unsigned char *) pers,
                               strlen( pers ) ) ) != 0 )
    {
        goto exit;
    }


    /*
     * 0. Initialize certificates
     */

    ret = mbedtls_x509_crt_parse( &cacert, (const unsigned char *) mbedtls_test_cas_pem,
                          mbedtls_test_cas_pem_len );
    if( ret < 0 )
    {
        goto exit;
    }


    /*
     * 1. Start the connection
     */

    if( ( ret = mbedtls_net_connect( &server_fd, str_address,
                                         SERVER_PORT, MBEDTLS_NET_PROTO_TCP ) ) != 0 )
    {
        goto exit;
    }


    /*
     * 2. Setup stuff
     */

    if( ( ret = mbedtls_ssl_config_defaults( &conf,
                    MBEDTLS_SSL_IS_CLIENT,
                    MBEDTLS_SSL_TRANSPORT_STREAM,
                    MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
    {
        goto exit;
    }


    /* OPTIONAL is not optimal for security,
     * but makes interop easier in this simplified example */
    mbedtls_ssl_conf_authmode( &conf, MBEDTLS_SSL_VERIFY_OPTIONAL );
    mbedtls_ssl_conf_ca_chain( &conf, &cacert, NULL );
    mbedtls_ssl_conf_rng( &conf, mbedtls_ctr_drbg_random, &ctr_drbg );

    if( ( ret = mbedtls_ssl_setup( &ssl, &conf ) ) != 0 )
    {
        goto exit;
    }

    if( ( ret = mbedtls_ssl_set_hostname( &ssl, "mbed TLS Server 1" ) ) != 0 )
    {
        goto exit;
    }

    mbedtls_ssl_set_bio( &ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL );

    /*
     * 4. Handshake
     */

    while( ( ret = mbedtls_ssl_handshake( &ssl ) ) != 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            goto exit;
        }
    }


    /*
     * 5. Verify the server certificate
     */

    /* In real life, we probably want to bail out when ret != 0 */
    if( ( flags = mbedtls_ssl_get_verify_result( &ssl ) ) != 0 )
    {
        char vrfy_buf[512];


        mbedtls_x509_crt_verify_info( vrfy_buf, sizeof( vrfy_buf ), "  ! ", flags );

    }

    /*
     * 3. Write the GET request
     */

    len = (size_t)(snprintf( (char *) buf, sizeof(buf), GET_REQUEST,
                            str_host));

    while( ( ret = mbedtls_ssl_write( &ssl, buf, len ) ) <= 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            goto exit;
        }
    }

    len = (size_t)ret;

    /*
     * 7. Read the HTTP response
     */

    char buffer[1024];
    size_t position = 0;

    do
    {
        len = sizeof( buf ) - 1;
        memset( buf, 0, sizeof( buf ) );
        ret = mbedtls_ssl_read( &ssl, buf, len );

        if( ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE )
            continue;

        if( ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY )
            break;

        if( ret < 0 )
        {
            break;
        }

        if( ret == 0 )
        {
            break;
        }

        len = (size_t)ret;
        memcpy(buffer + position, buf, MIN(len, sizeof(buffer) - position - 1));
        position = MIN(position + len, sizeof(buffer) - 1);
    }
    while( 1 );
    buffer[position] = '\0';

    mbedtls_ssl_close_notify( &ssl );

exit:

#ifdef MBEDTLS_ERROR_C
    if( ret != 0 )
    {
        char error_buf[100];
        mbedtls_strerror( ret, error_buf, 100 );
        mbedtls_printf("Last error was: %d - %s\n\n", ret, error_buf );
    }
#endif

    mbedtls_net_free( &server_fd );

    mbedtls_x509_crt_free( &cacert );
    mbedtls_ssl_free( &ssl );
    mbedtls_ssl_config_free( &conf );
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );


#endif /* MBEDTLS_BIGNUM_C && MBEDTLS_ENTROPY_C && MBEDTLS_SSL_TLS_C &&
          MBEDTLS_SSL_CLI_C && MBEDTLS_NET_C && MBEDTLS_RSA_C &&
          MBEDTLS_CERTS_C && MBEDTLS_PEM_PARSE_C && MBEDTLS_CTR_DRBG_C &&
          MBEDTLS_X509_CRT_PARSE_C */
/************** to here *******************************************************/


  int32_t value = 1;// ret;
  if (value < 0) {
    JHANDLER_THROW(COMMON, "TLS Read Error");
  } else {
    //iotjs_jhandler_return_number(jhandler, value);
    iotjs_jhandler_return_string_raw(jhandler, buffer);
  }
}

JHANDLER_FUNCTION(CloseSync) {
  JHANDLER_DECLARE_THIS_PTR(tls, tls);

  iotjs_jhandler_return_null(jhandler);
}

iotjs_jval_t InitTls() {
  iotjs_jval_t jtls = iotjs_jval_create_object();
  iotjs_jval_t jtlsConstructor =
      iotjs_jval_create_function_with_dispatch(TlsConstructor);
  iotjs_jval_set_property_jval(jtls, IOTJS_MAGIC_STRING_TLS, jtlsConstructor);

  iotjs_jval_t jprototype = iotjs_jval_create_object();
  iotjs_jval_set_method(jprototype, IOTJS_MAGIC_STRING_READSYNC, ReadSync);
  iotjs_jval_set_method(jprototype, IOTJS_MAGIC_STRING_CLOSESYNC, CloseSync);
  iotjs_jval_set_property_jval(jtlsConstructor, IOTJS_MAGIC_STRING_PROTOTYPE,
                               jprototype);

  jerry_release_value(jprototype);
  jerry_release_value(jtlsConstructor);

  return jtls;
}
