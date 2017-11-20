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

var Tls = require('tls');
var assert = require('assert');
var tls = new Tls();
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

test();

// read async test
function test() {
  var tls0 = tls.open(configuration, function(err) {
    console.log('TLS value');
    value = tls0.readSync();
    console.log(value);
    tls0.closeSync();
  });
}

