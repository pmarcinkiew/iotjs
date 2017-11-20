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
var receive_count = 0;
var receive_count_limit = 2;
var MESSAGE = 'Hello IoT.js';
var PORT = 41239;
var MULTICAST_ADDR = '230.255.255.250';
var TIMEOUT_TTL = 5000;
var timeout = null;

console.log('starting test');
socket.on('message', function (data) {
  console.log('got message: ' + data);

  assert.equal(data, MESSAGE, 'proper message received');
  ++receive_count;

  if (receive_count === receive_count_limit) { // receive only two messages
    console.log('max message count, dropping membership');
    socket.dropMembership(MULTICAST_ADDR);
  }
});

socket.on('error', function (err) {
  assert(false, err);
});

socket.bind(PORT, function (err) {
  if (err) {
    assert(false, err);
  } else {
    console.log('socket bound, adding membership');
    socket.addMembership(MULTICAST_ADDR);

    timeout = setTimeout(function () {
      console.log('test ttl reached');
      socket.close();
    }, TIMEOUT_TTL);
  }
});

process.on('exit', function(code) {
  console.log('exiting process');
  if (timeout) { // clean up
    clearTimeout(timeout);
  }

  console.log('exit code: ' + code);
  console.log('receive count: ' + receive_count + '/' + receive_count_limit);
  assert.equal(code, 0, 'program ended correctly');
  assert.equal(receive_count, receive_count_limit,
               'server received only ' + receive_count_limit + ' times');
});
