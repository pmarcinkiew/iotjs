/* Copyright 2016-present Samsung Electronics Co., Ltd. and other contributors
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

var assert = require('assert');
var dgram = require('dgram');

var socket = dgram.createSocket('udp4');
var send_count = 0;
var send_count_limit = 5;
var MESSAGE = 'Hello IoT.js';
var PORT = 41239;
var MULTICAST_ADDR = '230.255.255.250';
var SEND_INTERVAL = 1000;
var send_timeout = null;

function sendMessage() {
   if (send_count < send_count_limit) {
     console.log('sending message');
     socket.send(MESSAGE, PORT, MULTICAST_ADDR, function (err) {
       console.log('message send: ' + MESSAGE + ', to: ' + MULTICAST_ADDR
                   + ':' + PORT);
       if (err) {
         assert(false, err);
       } else {
         ++send_count;
         send_timeout = setTimeout(sendMessage, SEND_INTERVAL);
       }
     });
   } else {
     console.log('finishing test');
     socket.close();
   }
}

console.log('starting test');
send_timeout = setTimeout(sendMessage, SEND_INTERVAL);

process.on('exit', function (code) {
  console.log('exiting process');
  if (send_timeout) { // cleanup
    clearTimeout(send_timeout);
  }

  console.log('exit code: ' + code);
  console.log('send count: ' + send_count + '/' + send_count_limit);
  assert.equal(code, 0, 'program ended correctly');
  assert.equal(send_count, send_count_limit,
               'client send ' + send_count_limit + ' messages');
});
