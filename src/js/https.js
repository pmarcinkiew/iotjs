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
    var console = require('console');

    function ClientRequest(options, cb) {
        var res = {
          statusCode: 200,
          on: function(str, callback) {
            console.log('res.on(' + str + ')');
          }
        };
        cb.call(this, res);
    };
    ClientRequest.prototype.write = function(data) {
      console.log(data);
    };
    ClientRequest.prototype.end = function(data) {
      console.log('end function');
    };

    exports.ClientRequest = ClientRequest;

    exports.request = function(options, cb) {
      return new ClientRequest(options, cb);
    };
} else {
    var client = require('https_client');

    var ClientRequest = exports.ClientRequest = client.ClientRequest;

    exports.request = function(options, cb) {
      return new ClientRequest(options, cb);
    };

    exports.METHODS = client.METHODS;

    exports.get = function(options, cb) {
      var req = exports.request(options, cb);
      req.end();
      return req;
    };
}
