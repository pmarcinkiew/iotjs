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

var port = 41237;
var broadcast_address = '255.255.255.255';
var interval = 100;

var msg_count = 0, msg_count2 = 0, msg_count3 = 0, send_count = 0;

var msg = 'Hello IoT.js';
console.log("create sockets 1");
var socket = dgram.createSocket({type: 'udp4', reuseAddr: true});
console.log("create sockets 2");
var socket2 = dgram.createSocket({type: 'udp4', reuseAddr: true});
console.log("create sockets 3");
var socket3 = dgram.createSocket({type: 'udp4', reuseAddr: true});
console.log("init end");

socket.on('error', function(err) {
  console.log("error sock1 " + err);
  assert.fail(true, false, err);
  socket.close();
});

socket2.on('error', function(err) {
  console.log("error sock2 " + err);
  assert.fail(true, false, err);
  socket2.close();
});

socket3.on('error', function(err) {
  console.log("error sock3 " + err);
  assert.fail(true, false, err);
  socket3.close();
});

socket.on('message', function(data, rinfo) {
  console.log('socket got data : ' + data);
  msg_count++;
  if (msg_count == 3) {
    console.log("close socket 1 msg count 3");
    socket.close();
  }
});

socket2.on('message', function(data, rinfo) {
  console.log('socket2 got data : ' + data);
  msg_count2++;
  if (msg_count2 == 3) {
    console.log("close socket 2 msg count 3");
    socket2.close();
  }
});

socket3.on('message', function(data, rinfo) {
  console.log('socket3 got data : ' + data);
  msg_count3++;
  if (msg_count3 == 3) {
    console.log("close socket 3 msg count 3");
    socket3.close();
  }
});

console.log('bind socket 1 port: ' + port);
socket.bind(port, function() {
  console.log('bind 1 success for socket port: ' + port);
  socket.setBroadcast(true);
  var timer = setInterval(function () {
    console.log('send interval: ' + send_count);
    send_count++;
    socket.setBroadcast(true);
    socket.send(msg, port, broadcast_address);
    if (send_count == 3) {
      console.log('clear interval: ' + send_count);
      clearInterval(timer);
    }
  }, interval);
});

console.log('bind socket 2 port: ' + port);
socket2.bind(port);

console.log('bind socket 3 port: ' + port);
socket3.bind(port);

process.on('exit', function(code) {
  console.log('process exit code: ' + code);
  assert.equal(code, 0);
  console.log('process exit msg_count: ' + msg_count);
  assert.equal(msg_count, 3);
  console.log('process exit msg_count2: ' + msg_count2);
  assert.equal(msg_count2, 3);
  console.log('process exit msg_count3: ' + msg_count3);
  assert.equal(msg_count3, 3);
  console.log('process exit send_count: ' + send_count);
  assert.equal(send_count, 3);
});
