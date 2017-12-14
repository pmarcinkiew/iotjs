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


#include <tinyara/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <tinyara/net/dns.h>
#include <netdb.h>
#include <sys/socket.h>

#include <net/lwip/ipv4/inet.h>

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
int main(void)
{
    mbedtls_printf("MBEDTLS_BIGNUM_C and/or MBEDTLS_ENTROPY_C and/or "
           "MBEDTLS_SSL_TLS_C and/or MBEDTLS_SSL_CLI_C and/or "
           "MBEDTLS_NET_C and/or MBEDTLS_RSA_C and/or "
           "MBEDTLS_CTR_DRBG_C and/or MBEDTLS_X509_CRT_PARSE_C "
           "not defined.\n");
    return(0);
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

#define SSL_DRBG_SEED "ssl_client1"
#define SSL_SERVERNAME "mbed TLS Server 1"
#define SERVER_PORT 443
#define GET_REQUEST "%s"

#define MIN(x, y) ((x) < (y) ? (x) : (y))


#include "iotjs_def.h"
#include "iotjs_module_tls.h"
#include "iotjs_objectwrap.h"


static const char dnsServer[] = "8.8.8.8";
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
          iotjs_jargs_append_error(&jargs, "Failed to open TLS socket");
        } else {
          iotjs_jargs_append_null(&jargs);
        }
        break;
      case kTlsOpRead:
        if (!result) {
          iotjs_jargs_append_error(&jargs, "Cannot read from TLS socket");
        } else {
          iotjs_jargs_append_null(&jargs);
          iotjs_jargs_append_number(&jargs, req_data->value);
        }
        break;
      case kTlsOpClose:
        if (!result) {
          iotjs_jargs_append_error(&jargs, "Cannot close TLS socket");
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

  //TODO: initialize
  //TODO: connect
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

  _this->entropy = NULL;
  _this->ctr_drbg = NULL;
  _this->cacert = NULL;
  _this->ssl_config = NULL;
  _this->ssl_ctx = NULL;
  _this->socket_fd = NULL;

  const iotjs_jval_t jconfiguration = JHANDLER_GET_ARG_IF_EXIST(0, object);
  if (jerry_value_is_null(jconfiguration)) {
    JHANDLER_THROW(TYPE, "Bad arguments - configuration should be Object");
    return;
  }

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

static void describeSSLError(int retcode) {
#ifdef MBEDTLS_ERROR_C
  if (retcode != 0) {
    char buffer[100];
    mbedtls_strerror(retcode, buffer, sizeof(buffer));
    mbedtls_printf("Last error was: %d - %s\n\n", retcode, buffer);
    mbedtls_printf("Last error was: something bad, very bad happened"
                   " during invocations of DNS server\n\n");
  }
#else
  (void)retcode;
#endif
}

static const char* resolveFirst(const char* str_address) {
  printf("\nHostname : %s [len %d]\n", str_address, strlen(str_address));
  struct hostent *shost = gethostbyname(str_address);
  if (shost == NULL || shost->h_addr_list == NULL) {
    fprintf(stderr, "gethostbyname(%s) failed\n", str_address);
    return NULL;
  } else {
    //printf("DNS results\n");
    return ip_ntoa((ip_addr_t *)shost->h_addr_list[0]);
    //printf("IP Address : %s\n", ip_ntoa((ip_addr_t *)shost->h_addr_list[0]));
  }
}

static mbedtls_entropy_context *createEntropy() {
  mbedtls_entropy_context *entropy = malloc(sizeof(mbedtls_entropy_context));
  mbedtls_entropy_init(entropy);
  return entropy;
}

static mbedtls_ctr_drbg_context *createDRBG(mbedtls_entropy_context *entropy, const char *pers) {
  mbedtls_ctr_drbg_context *ctr_drbg = malloc(sizeof(mbedtls_ctr_drbg_context));
  mbedtls_ctr_drbg_init(ctr_drbg);

  int result = mbedtls_ctr_drbg_seed(ctr_drbg, mbedtls_entropy_func, &entropy,
                                     (const unsigned char *)pers, strlen(pers));
  if (result != 0) {
    describeSSLError(result);
    mbedtls_ctr_drbg_free(ctr_drbg);
    free(ctr_drbg);
    ctr_drbg = NULL;
  }
  return ctr_drbg;
}

static mbedtls_x509_crt *createSSLCACert(const void *cas_pem, size_t cas_pem_len) {
  mbedtls_x509_crt *cacert = malloc(sizeof(mbedtls_x509_crt));
  mbedtls_x509_crt_init(cacert);
  int result = mbedtls_x509_crt_parse(
      cacert, (const unsigned char *)cas_pem, cas_pem_len);
  if (result != 0) {
    describeSSLError(result);
    mbedtls_x509_crt_free(cacert);
    free(cacert);
    cacert = NULL;
  }
  return cacert;
}

static mbedtls_ssl_config *createSSLConfig(
    mbedtls_x509_crt *cacert,
    mbedtls_ctr_drbg_context *ctr_drbg) {
  mbedtls_ssl_config *ssl_conf = malloc(sizeof(mbedtls_ssl_config));
  mbedtls_ssl_config_init(ssl_conf);
  int result = mbedtls_ssl_config_defaults(
      ssl_conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);

  if (result != 0) {
    describeSSLError(result);
    mbedtls_ssl_config_free(ssl_conf);
    free(ssl_conf);
    ssl_conf = NULL;
  } else {
    /* OPTIONAL is not optimal for security,
     * but makes interop easier in this simplified example */
    mbedtls_ssl_conf_authmode(ssl_conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(ssl_conf, cacert, NULL);
    mbedtls_ssl_conf_rng(ssl_conf, mbedtls_ctr_drbg_random, ctr_drbg);
  }

  return ssl_conf;
}

static mbedtls_ssl_context *createSSLContext(mbedtls_ssl_config *ssl_config, const char *serverName) {
  mbedtls_ssl_context *ssl_ctx = malloc(sizeof(mbedtls_ssl_context *));
  mbedtls_ssl_init(ssl_ctx);
  int result = mbedtls_ssl_setup(ssl_ctx, ssl_config);
  if (result == 0) {
    result = mbedtls_ssl_set_hostname(ssl_ctx, serverName);
  }
  if (result != 0) {
    describeSSLError(result);
    mbedtls_ssl_free(ssl_ctx);
    free(ssl_ctx);
    ssl_ctx = NULL;
  }
  return ssl_ctx;
}

static void cleanupSSLState(iotjs_tls_t* tls) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_tls_t, tls);
  if (_this->ssl_ctx && _this->socket_fd) {
    mbedtls_ssl_close_notify(_this->ssl_ctx);
  }
  if (_this->socket_fd) {
    mbedtls_net_free(_this->socket_fd);
    free(_this->socket_fd);
    _this->socket_fd = NULL;
  }
  if (_this->ssl_ctx) {
    mbedtls_ssl_free(_this->ssl_ctx);
    free(_this->ssl_ctx);
    _this->ssl_ctx = NULL;
  }
  if (_this->ssl_config) {
    mbedtls_ssl_config_free(_this->ssl_config);
    free(_this->ssl_config);
    _this->ssl_config = NULL;
  }
  if (_this->cacert) {
    mbedtls_x509_crt_free(_this->cacert);
    free(_this->cacert);
    _this->cacert = NULL;
  }
  if (_this->ctr_drbg) {
    mbedtls_ctr_drbg_free(_this->ctr_drbg);
    free(_this->ctr_drbg);
    _this->ctr_drbg = NULL;
  }
  if (_this->entropy) {
    mbedtls_entropy_free(_this->entropy);
    free(_this->entropy);
    _this->entropy = NULL;
  }
}

static int setupTLSEnvironment(
    iotjs_tls_t* tls, const char *drbg_seed, const char *serverName) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_tls_t, tls);
  do {
    _this->entropy = createEntropy();
    if (_this->entropy == NULL) break;

    _this->ctr_drbg = createDRBG(_this->entropy, drbg_seed);
    if (_this->ctr_drbg == NULL) break;

    _this->cacert = createSSLCACert(mbedtls_test_cas_pem, mbedtls_test_cas_pem_len);
    if (_this->cacert == NULL) break;

    _this->ssl_config = createSSLConfig(_this->cacert, _this->ctr_drbg);
    if (_this->ssl_config == NULL) break;

    _this->ssl_ctx = createSSLContext(_this->ssl_config, serverName);
    if (_this->ssl_ctx == NULL)  break;

    return 0;
  } while (0);
  cleanupSSLState(tls);
  fprintf(stderr, "TLS environment setup failed");
  return -1;
}

static mbedtls_net_context *createSSLConnection(
    mbedtls_ssl_context *ssl_ctx, const char *peerIP, unsigned short port) {
  mbedtls_net_context *socket_fd = malloc(sizeof(mbedtls_net_context));
  mbedtls_net_init(socket_fd);

  char port_str[8];
  sprintf(port_str, "%d", port);
  int result = mbedtls_net_connect(
        socket_fd, peerIP, port_str, MBEDTLS_NET_PROTO_TCP);
  if (result == 0) {
    mbedtls_ssl_set_bio(
        ssl_ctx, socket_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
  } else {
    describeSSLError(result);
    mbedtls_net_free(socket_fd);
    free(socket_fd);
    socket_fd = NULL;
  }
  return socket_fd;
}

static int handshakeAndVerify(iotjs_tls_t* tls) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_tls_t, tls);

  do {
    int result = mbedtls_ssl_handshake(_this->ssl_ctx);
    if (result == MBEDTLS_ERR_SSL_WANT_READ
        || result == MBEDTLS_ERR_SSL_WANT_WRITE) {
      continue;
    }
    if (result != 0) {
      describeSSLError(result);
      return result;
    }
  } while (0);
  /* Verify the server certificate */
  /* In real life, we probably want to bail out when ret != 0 */
  uint32_t flags = mbedtls_ssl_get_verify_result(_this->ssl_ctx);
  if (flags != 0) {
    char vrfy_buf[512];
    mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
  }
  return 0;
}

static char *createHTTPGetRequest(const char *host) {
  char *buffer;
  if (asprintf(&buffer, GET_REQUEST, host) < 0) {
    buffer = NULL;
  }
  return buffer;
}

static int writeSSLData(mbedtls_ssl_context *ssl_ctx, const void *data, size_t len) {
  int result;
  do {
    result = mbedtls_ssl_write(ssl_ctx, data, len);
    if (result == MBEDTLS_ERR_SSL_WANT_READ
        || result == MBEDTLS_ERR_SSL_WANT_WRITE) {
      continue;
    } else if (result != 0) {
      describeSSLError(result);
    }
  } while (0);
  return result;
}

static char *readAllocateSSLData(mbedtls_ssl_context *ssl_ctx, size_t max_len) {
  // Extra byte for terminator.
  // TODO: what if max_len == MAX_UINT?
  unsigned char *buffer = malloc(max_len + 1);
  if (buffer) {
    int pos = 0;
    do {
      size_t len = max_len - (size_t)pos;
      int chunk = mbedtls_ssl_read(ssl_ctx, buffer + pos, len);

      if (chunk == MBEDTLS_ERR_SSL_WANT_READ
          || chunk == MBEDTLS_ERR_SSL_WANT_WRITE) continue;
      if (chunk == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) break;
      if (chunk <= 0) {
        describeSSLError(chunk);
        pos = -1;
        break;
      }

      pos += chunk;
    } while ((size_t)pos < max_len);
    if (pos > 0) {
      buffer[pos] = 0;
    } else {
      free(buffer);
      buffer = NULL;
    }
  }
  return (char *)buffer;
}

JHANDLER_FUNCTION(RequestSync) {
  JHANDLER_DECLARE_THIS_PTR(tls, tls);
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_tls_t, tls);
  DJHANDLER_CHECK_ARGS(2, string, string);

  // IP address, host, path, data, bearer
  iotjs_string_t address = JHANDLER_GET_ARG(0, string);
  iotjs_string_t host = JHANDLER_GET_ARG(1, string);
  const char* str_address = iotjs_string_data(&address);
  const char* str_host = iotjs_string_data(&host);
  const char* hostIpAddr = resolveFirst(str_address);

  int result = -1;
  do {
    result = setupTLSEnvironment(tls, SSL_DRBG_SEED, SSL_SERVERNAME);
    if (result != 0) {
      describeSSLError(result);
      break;
    }

    result = -1;
    _this->socket_fd = createSSLConnection(_this->ssl_ctx, hostIpAddr, SERVER_PORT);
    if (_this->socket_fd == NULL) {
      break;
    }

    if (handshakeAndVerify(tls) != 0) {
      break;
    }

    char *request = createHTTPGetRequest(str_host);
    if (request == NULL) {
      break;
    }

    if (writeSSLData(_this->ssl_ctx, request, strlen(request)) != 0) {
      break;
    }
    free(request);

    char *response = readAllocateSSLData(_this->ssl_ctx, 1024);
    if (response) {
      // Success, do stuff
      result = 0;
      iotjs_jhandler_return_string_raw(jhandler, response);
      free(response);
    }
  } while (0);
  cleanupSSLState(tls);
#endif /* MBEDTLS_BIGNUM_C && MBEDTLS_ENTROPY_C && MBEDTLS_SSL_TLS_C &&
          MBEDTLS_SSL_CLI_C && MBEDTLS_NET_C && MBEDTLS_RSA_C &&
          MBEDTLS_CERTS_C && MBEDTLS_PEM_PARSE_C && MBEDTLS_CTR_DRBG_C &&
          MBEDTLS_X509_CRT_PARSE_C */
  if (result < 0) {
    JHANDLER_THROW(COMMON, "TLS Read Error");
  }
}

static void addDNSServer() {
//	if (argc == 3 && argv[2] != NULL) {
//#ifdef CONFIG_NETDB_DNSSERVER_IPv4
  printf("dnsclient : dns_add_nameserver : %s\n", dnsServer);
  struct sockaddr_in dns;
  dns.sin_family = AF_INET;
  dns.sin_port = htons(53);
  dns.sin_addr.s_addr = inet_addr(dnsServer);
  dns_add_nameserver((FAR struct sockaddr *)&dns, sizeof(struct sockaddr_in));
//#endif
//#ifdef CONFIG_NETDB_DNSSERVER_BY_DHCP
  printf("dnsclient : dns server address is set by DHCP\n");
//#endif
//	}
}


JHANDLER_FUNCTION(CloseSync) {
  JHANDLER_DECLARE_THIS_PTR(tls, tls);
  // TODO: move disconnect here maybe?
  // TODO: then destruct (now it's done within RequestSync)

  iotjs_jhandler_return_null(jhandler);
}

iotjs_jval_t InitTls() {
  iotjs_jval_t jtls = iotjs_jval_create_object();
  iotjs_jval_t jtlsConstructor =
      iotjs_jval_create_function_with_dispatch(TlsConstructor);
  iotjs_jval_set_property_jval(jtls, IOTJS_MAGIC_STRING_TLS, jtlsConstructor);

  iotjs_jval_t jprototype = iotjs_jval_create_object();
  iotjs_jval_set_method(jprototype, IOTJS_MAGIC_STRING_REQUESTSYNC, RequestSync);
  iotjs_jval_set_method(jprototype, IOTJS_MAGIC_STRING_CLOSESYNC, CloseSync);
  iotjs_jval_set_property_jval(jtlsConstructor, IOTJS_MAGIC_STRING_PROTOTYPE,
                               jprototype);

  jerry_release_value(jprototype);
  jerry_release_value(jtlsConstructor);

  addDNSServer();

  return jtls;
}
