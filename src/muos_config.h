/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2015                            Christian Thäter <ct@pipapo.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MUOS_CONFIG_H
#define MUOS_CONFIG_H

// the bits used for indexing queues
// comes in 3 variants
//  4 bits allow only small queues for up to (8-16) functions, use only for really small targets
//  8 bits allow queues for up to (128-256) functions, this is the default
//  16 bits allow huge queues, use only when really required
#ifndef MUOS_QUEUE_INDEX
//#define MUOS_QUEUE_INDEX 4
#define MUOS_QUEUE_INDEX 8
//#define MUOS_QUEUE_INDEX 16
#endif



// How functions with arguments are tagged in queues, currently only the negate is implemened
// Works with mpu's with up to 32k flash
#ifndef MUOS_QUEUE_ARGTAG
#define MUOS_QUEUE_ARGTAG ARGTAG_NEGATE
// LSB tagging works with up to 64k flash but needs function alignment on even addresses
//#define MUOS_QUEUE_ARGTAG ARGTAG_LSB //unimplemented due gcc bug
#endif

#endif
