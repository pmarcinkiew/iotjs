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

#include <peripheral-io.h>

#include "iotjs_systemio-linux.h"
#include "modules/iotjs_module_gpio.h"

bool iotjs_gpio_write(iotjs_gpio_t* gpio, bool value) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_gpio_t, gpio);
  peripheral_gpio_h* _gpio = (peripheral_gpio_h*)_this->_private;
  int retVal = peripheral_gpio_write(_gpio, value);
  return PERIPHERAL_ERROR_NONE == retVal;
}


int iotjs_gpio_read(iotjs_gpio_t* gpio) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_gpio_t, gpio);
  peripheral_gpio_h* _gpio = (peripheral_gpio_h*)_this->_private;
  int value;
  int retVal = peripheral_gpio_read(_gpio, value);
  if (PERIPHERAL_ERROR_NONE == retVal) {
    return value;
  } else {
    return 0;
  }
}


bool iotjs_gpio_close(iotjs_gpio_t* gpio) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_gpio_t, gpio);
  peripheral_gpio_h* _gpio = (peripheral_gpio_h*)_this->_private;
  peripheral_gpio_close(_gpio);
  free(_gpio);
  return true;
}


void iotjs_gpio_open_worker(uv_work_t* work_req) {
  GPIO_WORKER_INIT;
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_gpio_t, gpio);

  _this->_private = malloc(sizeof(peripheral_gpio_h));
  peripheral_gpio_h* _gpio = (peripheral_gpio_h*)_this->_private;
  int retVal = peripheral_gpio_open(_this->pin, _gpio);
  if (retVal != PERIPHERAL_ERROR_NONE) {
    req_data->result = false;
    return;
  }
  peripheral_gpio_direction_e _direction;
  if (_this->direction == kGpioDirectionIn) {
    _direction = PERIPHERAL_GPIO_DIRECTION_IN;
  } else {
    _direction = PERIPHERAL_GPIO_DIRECTION_OUT;
  }
  retVal = peripheral_gpio_set_direction(_gpio, _direction);
  if (retVal != PERIPHERAL_ERROR_NONE) {
    req_data->result = false;
    return;
  }
  // Mode is not supported by Peripheral API for Tizen
  peripheral_gpio_edge_e _edge;
  switch(_this->edge) {
    case kGpioEdgeNone:
      _edge = PERIPHERAL_GPIO_EDGE_NONE;
    break;
    case kGpioEdgeRising:
      _edge = PERIPHERAL_GPIO_EDGE_RISING;
    break;
    case kGpioEdgeFalling:
      _edge = PERIPHERAL_GPIO_EDGE_FALLING;
    break;
    case kGpioEdgeBoth:
      _edge = PERIPHERAL_GPIO_EDGE_BOTH;
    break;
    default:
      _edge = PERIPHERAL_GPIO_EDGE_BOTH;
  }
  retVal = peripheral_gpio_set_edge_mode(_gpio, _edge);
  if (retVal != PERIPHERAL_ERROR_NONE) {
    req_data->result = false;
    return;
  }
  req_data->result = true;
}
