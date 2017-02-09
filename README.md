# IoT.js: Platform for Internet of Things with JavaScript
[![Join the chat at https://gitter.im/Samsung/iotjs](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/Samsung/iotjs?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![License](https://img.shields.io/badge/licence-Apache%202.0-brightgreen.svg?style=flat)](LICENSE)
[![Build Status](https://travis-ci.org/Samsung/iotjs.svg?branch=master)](https://travis-ci.org/Samsung/iotjs)

You can find project details on our [project page](http://samsung.github.io/iotjs/) and [wiki](https://github.com/Samsung/iotjs/wiki).

## Quick Start
### Getting the sources

```bash
git clone https://github.com/Samsung/iotjs.git
cd iotjs
```

### How to Build

This is temporary version of Tizen Artik 10 repo. It can't be build with standard build.py script. It is modified to work correctly only with GBS environment
Compilation in GBS:

```
# gbs build -A armv7l --debug
```

It works only at root.


GBS conf

```
# cat ~/.gbs.conf
[general]
tmpdir=/var/tmp/
profile = profile.common.tizen3.0.artik
work_dir=.

[repo.tizen3.0.artik]
url = http://download.tizen.org/snapshots/tizen/common_artik/tizen-common-artik_20170116.1/repos/arm-wayland/packages/
#https://download.tizen.org/snapshots/tizen/common/latest/repos/arm-wayland/packages/

[repo.tizen.base]
url = http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/

[profile.common.tizen3.0.artik]
repos=repo.tizen3.0.artik, repo.tizen.base
```

### Installation

```
PC$sdb connect IP

PC$sdb root on
PC$sdb push file folder
PC$sdb shell

tizen# rpm -i package.rpm
```

### How to Test

```bash
tizen# /usr/bin/iotjs /root/test_console.js
```


For Additional information see [Getting Started](docs/Getting-Started.md).

## Documentation
- [Getting Started](docs/Getting-Started.md)
- [API Reference](docs/IoT.js-API-reference.md)

## License
IoT.js is Open Source software under the [Apache 2.0 license](https://www.apache.org/licenses/LICENSE-2.0). Complete license and copyright information can be found within the code.

> Copyright 2015-present Samsung Electronics Co., Ltd. and other contributors

> Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

> Copyright Node.js contributors. All rights reserved.

> Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to
 deal in the Software without restriction, including without limitation the
 rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

> The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 IN THE SOFTWARE.

> This license applies to parts of '*.js' files in '/src/js', implementing node.js
 compatible API, originating from the https://github.com/node/node repository:
