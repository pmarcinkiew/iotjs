/* Copyright 2014-2016 Samsung Electronics Co., Ltd.
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

#ifndef ECMA_COMPARISON_H
#define ECMA_COMPARISON_H

#include "ecma-globals.h"
#include "ecma-helpers.h"

/** \addtogroup ecma ECMA
 * @{
 *
 * \addtogroup ecmacomparison ECMA comparison
 * @{
 */

extern ecma_value_t ecma_op_abstract_equality_compare (ecma_value_t, ecma_value_t);
extern bool ecma_op_strict_equality_compare (ecma_value_t, ecma_value_t);
extern ecma_value_t ecma_op_abstract_relational_compare (ecma_value_t, ecma_value_t, bool);

/**
 * @}
 * @}
 */

#endif /* !ECMA_COMPARISON_H */
