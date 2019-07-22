/*
 * Copyright (c) 2019, Nuvoton Technology Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NU_TIMER_H
#define NU_TIMER_H

#include <stdint.h>
#include <stdbool.h>
#include "pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Support one-to-many pin-perif mapping for the same pin in one pinmap
 *
 * 1. Add mx version of:
 *    - pinmap_peripheral
 *    - pinmap_merge
 *    - pinmap_pinout
 * 2. Requirement of 'PeripheralList' struct: non-NC peripherals must place
 *    in front entries; the remaining entries must fill with 'NC'.
 * 3. To enable this mapping, HAL driver must call mx version of above API
 *    when one pin can map to multiple peripherals in the same pinmap.
 */

/* Nuvoton variant of pinmap_peripheral(...) which supports one-to-many
 * pin-perif mapping for the same pinmap.
 *
 * @param   pin         pin to check for
 * @param   map         pinmap to check for
 * @param   perlist     where matched peripherals are placed. On output,
 *                      perlist->peripheral holds matched peripherals; remaining
 *                      entries are filled with NC.
 */
void nu_pinmap_peripheral_mx(PinName pin, const PinMap *map, PeripheralList *perlist);

/* Nuvoton variant of pinmap_merge(...) which supports one-to-many
 * pin-perif mapping for the same pinmap.
 *
 * @param   a_perlist   'a' peripheral list
 * @param   b_perlist   'b' peripheral list
 * @param   c_perlist   'c' peripheral list. On output, c_perlist->peripheral
 *                      holds intersection of a_perlist->peripheral and
 *                      b_perlist->peripheral; remaining entries are filled
 *                      with NC.
 */
void nu_pinmap_merge_mx(const PeripheralList *a_perlist, const PeripheralList *b_perlist, PeripheralList *c_perlist);

/* Nuvoton variant of pinmap_pinout(...) which supports one-to-many
 * pin-perif mapping for the same pinmap.
 *
 * @param   pin         pin to check for
 * @param   map         pinmap to check for
 * @param   per         extra peripheral to check for
 */
void nu_pinmap_pinout_mx(PinName pin, const PinMap *map, int per);

#ifdef __cplusplus
}
#endif

#endif
