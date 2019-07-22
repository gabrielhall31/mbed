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

#include "cmsis_compiler.h"
#include "platform/mbed_error.h"
#include "platform/mbed_assert.h"
#include "nu_pinmap.h"

/* Check if peripheral list is valid
 *
 * @param   perlist peripheral list to check
 * @return  true if perlist is valid; otherwise false
 */
__STATIC_INLINE bool peripheral_list_is_valid(const PeripheralList *perlist)
{
    return (perlist != NULL) && ((perlist->count == 0) || (perlist->peripheral != NULL));
}

/* Check if peripheral list is all NC
 *
 * @param   perlist peripheral list to check
 * @return  true if perlist is all NC; otherwise false
 */
__STATIC_INLINE bool peripheral_list_is_all_NC(const PeripheralList *perlist)
{
    MBED_ASSERT(peripheral_list_is_valid(perlist));

    int *per_pos = (int *) perlist->peripheral;
    int *per_end = (int *) perlist->peripheral + perlist->count;
    
    if ((per_pos != per_end) && (*per_pos != NC)) {
        return false;
    } else {
        return true;
    }
}

/* Clear peripheral list range to NC 
 *
 * @param   beg start position of peripheral list (inclusive) to clear to NC
 * @param   end end position of peripheral list (exclusive) to clear to NC
 */
__STATIC_INLINE void clear_peripheral_list(int *beg, int *end)
{
    MBED_ASSERT(beg && end);

    int *pos = beg;

    while (pos != end) {
        *pos ++ = NC;
    }
}

/* Copy source peripheral list to destination peripheral list
 *
 * @param   src_perlist source peripheral list
 * @param   dst_perlist destination peripheral list
 *
 * @note    Remaining entries in destination peripheral list are filled with NC
 * @note    Extra entries in source peripheral list are skipped
 */
__STATIC_INLINE void copy_peripheral_list(PeripheralList *dst_perlist, const PeripheralList *src_perlist)
{
    MBED_ASSERT(src_perlist && dst_perlist);

    int *src_per_pos = (int *) src_perlist->peripheral;
    int *src_per_end = (int *) src_perlist->peripheral + src_perlist->count;
    int *dst_per_pos = (int *) dst_perlist->peripheral;
    int *dst_per_end = (int *) dst_perlist->peripheral + dst_perlist->count;

    while ((src_per_pos != src_per_end) &&
        (dst_per_pos != dst_per_end)) {
        *dst_per_pos ++ = *src_per_pos ++;
    }
    clear_peripheral_list(dst_per_pos, dst_per_end);
}

void nu_pinmap_peripheral_mx(PinName pin, const PinMap *map, PeripheralList *perlist)
{
    MBED_ASSERT(peripheral_list_is_valid(perlist));

    int *per_beg = (int *) perlist->peripheral;
    int *per_pos = (int *) perlist->peripheral;
    int *per_end = (int *) perlist->peripheral + perlist->count;

    if (pin == (PinName) NC) {
        clear_peripheral_list(per_pos, per_end);
        return;
    }

    while (map->pin != NC && per_pos != per_end) {
        if (map->pin == pin) {
            *per_pos ++ = map->peripheral;
        }
        map ++;
    }
    clear_peripheral_list(per_pos, per_end);

    // no mapping available
    if (peripheral_list_is_all_NC(perlist)) {
        MBED_ERROR1(MBED_MAKE_ERROR(MBED_MODULE_PLATFORM, MBED_ERROR_CODE_PINMAP_INVALID), "pinmap not found for pin", pin);
    }
}

void nu_pinmap_merge_mx(const PeripheralList *a_perlist, const PeripheralList *b_perlist, PeripheralList *c_perlist)
{
    MBED_ASSERT(peripheral_list_is_valid(a_perlist));
    MBED_ASSERT(peripheral_list_is_valid(b_perlist));
    MBED_ASSERT(peripheral_list_is_valid(c_perlist));

    int *c_per_beg = (int *) c_perlist->peripheral;
    int *c_per_pos = (int *) c_perlist->peripheral;
    int *c_per_end = (int *) c_perlist->peripheral + c_perlist->count;

    if (peripheral_list_is_all_NC(a_perlist)) {
        if (peripheral_list_is_all_NC(b_perlist)) {
            clear_peripheral_list(c_per_pos, c_per_end);
            return;
        } else {
            copy_peripheral_list(c_perlist, b_perlist);
            return;
        }
    } else {
        if (peripheral_list_is_all_NC(b_perlist)) {
            copy_peripheral_list(c_perlist, a_perlist);
            return;
        }
    }

    //int *a_per_beg = (int *) a_perlist->peripheral;
    int *a_per_pos = (int *) a_perlist->peripheral;
    int *a_per_end = (int *) a_perlist->peripheral + a_perlist->count;
    int *b_per_beg = (int *) b_perlist->peripheral;
    int *b_per_pos = (int *) b_perlist->peripheral;
    int *b_per_end = (int *) b_perlist->peripheral + b_perlist->count;

    while ((a_per_pos != a_per_end) && (*a_per_pos != NC) && (c_per_pos != c_per_end)) {
        while ((b_per_pos != b_per_end) && (*b_per_pos != NC) && (c_per_pos != c_per_end)) {
            if (*a_per_pos == *b_per_pos) {
                *c_per_pos ++ = *a_per_pos;
                break;
            }
            b_per_pos ++;
        }
        a_per_pos ++;
        b_per_pos = b_per_beg;
    }
    clear_peripheral_list(c_per_pos, c_per_end);

    // mis-match error case
    if (peripheral_list_is_all_NC(c_perlist)) {
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_PLATFORM, MBED_ERROR_CODE_PINMAP_INVALID), "pinmap mis-match");
    }
}

void nu_pinmap_pinout_mx(PinName pin, const PinMap *map, int per)
{
    if (pin == NC) {
        return;
    }

    while (map->pin != NC) {
        if (map->pin == pin && (per == NC || map->peripheral == per)) {
            pin_function(pin, map->function);

            pin_mode(pin, PullNone);
            return;
        }
        map ++;
    }
    MBED_ERROR1(MBED_MAKE_ERROR(MBED_MODULE_PLATFORM, MBED_ERROR_CODE_PINMAP_INVALID), "could not pinout", pin);
}
