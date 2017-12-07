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
if (process.platform === 'tizenrt') {
  var Tls = require('tls');
  var assert = require('assert');
  var tls = new Tls();
  //test();
  // read async test
  function test(self) {
    var configuration = {};
    if (process.platform === 'linux') {
      configuration.device =
        '/sys/devices/12d10000.tls/iio:device0/in_voltage0_raw';
    } else if (process.platform === 'nuttx') {
      configuration.pin = require('stm32f4dis').pin.TLS1_3;
    } else if (process.platform === 'tizenrt') {
      configuration.pin = 0;
    } else {
      assert.fail();
    }
    setTimeout(function() {
      var tls0 = tls.open(configuration, function (err) {
          var options = self._options,
          http = options.method + " " + options.path + " HTTP/1.1\r\n" +
          "HOST: " + options.hostname + "\r\n";
          for (key in options.headers) {
            http += key + ": " + options.headers[key] + "\r\n";
          }
          http += "\r\n" +
                  self._data +
                  "\r\n" +
                  "\r\n";
          //FIXME: DNS is still not working correctly for Tizen RT
          var value = tls0.readSync(options.hostname, //IP
                                    http);
          value = (value.split("\r\n\r\n"))[1];
          var data = tls0.closeSync();
          data = "DATA";
          //data is string
          self._callbacks.data(value);
          self._callbacks.end();
      });
    });
  }
  function ClientRequest(options, cb) {
    var self = this,
    res = {
            statusCode: 200,
            on: function (str, callback) {
                self._callbacks[str] = callback;
            }
    };
    this._options = options;
    this._callbacks = {};
    setTimeout(function() {
        cb.call(this, res);
    });
    this._data = "";
    this._options = options;
  };
  ClientRequest.prototype.write = function (data) {
    this._data = data;
  };
  ClientRequest.prototype.end = function (data) {
    test(this);
  };
  exports.ClientRequest = ClientRequest;
  exports.request = function (options, cb) {
    return new ClientRequest(options, cb);
  };
} else { // if (process.platform === 'tizenrt')
  var client = require('https_client');
  var ClientRequest = exports.ClientRequest = client.ClientRequest;
  exports.request = function (options, cb) {
    return new ClientRequest(options, cb);
  };
  exports.METHODS = client.METHODS;
  exports.get = function (options, cb) {
    var req = exports.request(options, cb);
    req.end();
    return req;
  };
}

