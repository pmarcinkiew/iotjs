/* Copyright 2016 Intel Corporation
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
#ifndef SETJMP_H
#define SETJMP_H

#include <stdint.h>

typedef uint64_t jmp_buf[14];

extern  int setjmp (jmp_buf env);
extern  void longjmp (jmp_buf env, int val);

#endif /* !SETJMP_H */
