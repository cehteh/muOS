/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2016                            Christian Thäter <ct@pipapo.org>
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


#ifndef MUOS_PP_H
#define MUOS_PP_H

//TODO: DOCME

#define MUOS_PP_FIRST(h,...) h
#define MUOS_PP_SECOND(h,t,...) t
#define MUOS_PP_BOTH(h,t,...) h,t


#define MUOS_PP_INC(n, ...) MUOS_PP_INC_##n
#define MUOS_PP_INC_0 1
#define MUOS_PP_INC_1 2
#define MUOS_PP_INC_2 3
#define MUOS_PP_INC_3 4
#define MUOS_PP_INC_4 5
#define MUOS_PP_INC_5 6
#define MUOS_PP_INC_6 7
#define MUOS_PP_INC_7 8
#define MUOS_PP_INC_8 9
#define MUOS_PP_INC_9 10
#define MUOS_PP_INC_10 11
#define MUOS_PP_INC_11 12
#define MUOS_PP_INC_12 13
#define MUOS_PP_INC_13 14
#define MUOS_PP_INC_14 15
#define MUOS_PP_INC_15 16
#define MUOS_PP_INC_16

#define MUOS_PP_INDEX(n, ...) MUOS_PP_INDEX_##n(__VA_ARGS__)
#define MUOS_PP_INDEX_0(x,...) x
#define MUOS_PP_INDEX_1(_, ...) MUOS_PP_INDEX_0(__VA_ARGS__)
#define MUOS_PP_INDEX_2(_, ...) MUOS_PP_INDEX_1(__VA_ARGS__)
#define MUOS_PP_INDEX_3(_, ...) MUOS_PP_INDEX_2(__VA_ARGS__)
#define MUOS_PP_INDEX_4(_, ...) MUOS_PP_INDEX_3(__VA_ARGS__)
#define MUOS_PP_INDEX_5(_, ...) MUOS_PP_INDEX_4(__VA_ARGS__)
#define MUOS_PP_INDEX_6(_, ...) MUOS_PP_INDEX_5(__VA_ARGS__)
#define MUOS_PP_INDEX_7(_, ...) MUOS_PP_INDEX_6(__VA_ARGS__)
#define MUOS_PP_INDEX_8(_, ...) MUOS_PP_INDEX_7(__VA_ARGS__)
#define MUOS_PP_INDEX_9(_, ...) MUOS_PP_INDEX_8(__VA_ARGS__)
#define MUOS_PP_INDEX_10(_, ...) MUOS_PP_INDEX_9(__VA_ARGS__)
#define MUOS_PP_INDEX_11(_, ...) MUOS_PP_INDEX_10(__VA_ARGS__)
#define MUOS_PP_INDEX_12(_, ...) MUOS_PP_INDEX_11(__VA_ARGS__)
#define MUOS_PP_INDEX_13(_, ...) MUOS_PP_INDEX_12(__VA_ARGS__)
#define MUOS_PP_INDEX_14(_, ...) MUOS_PP_INDEX_13(__VA_ARGS__)
#define MUOS_PP_INDEX_15(_, ...) MUOS_PP_INDEX_14(__VA_ARGS__)
#define MUOS_PP_INDEX_16(_, ...) MUOS_PP_INDEX_15(__VA_ARGS__)

#define MUOS_PP_NARGS_(_, ...) MUOS_PP_INDEX(16, __VA_ARGS__)
#define MUOS_PP_NARGS(...) MUOS_PP_NARGS_(_, ## __VA_ARGS__,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define MUOS_PP_NARGS_LIST(list) MUOS_PP_NARGS(list)

#define MUOS_PP_CAT(a,b) a ## b
#define MUOS_PP_CAT2(a,b) MUOS_PP_CAT(a,b)
#define MUOS_PP_CAT3_(a,b,c) a ## b ## c
#define MUOS_PP_CAT3(a,b,c) MUOS_PP_CAT3_(a,b,c)

#define MUOS_PP_LIST_EXPAND_(...) __VA_ARGS__
#define MUOS_PP_LIST_EXPAND(list) MUOS_PP_LIST_EXPAND_ list

#define MUOS_PP_LIST_NTH0(nth,list) MUOS_PP_INDEX(nth, MUOS_PP_LIST_EXPAND_ list)

#define MUOS_PP_IS_PROBE(...) MUOS_PP_SECOND(__VA_ARGS__, 0)
#define MUOS_PP_PROBE() ~, 1

#define MUOS_PP_NOT(x) MUOS_PP_IS_PROBE(MUOS_PP_CAT(MUOS_PP_NOT_, x))
#define MUOS_PP_NOT_0 MUOS_PP_PROBE()

#define MUOS_PP_BOOL(x) MUOS_PP_NOT(MUOS_PP_NOT(x))

#define MUOS_PP_HAVE_ARGS(...) MUOS_PP_BOOL(MUOS_PP_FIRST(MUOS_PP_END __VA_ARGS__)())
#define MUOS_PP_END() 0

#define MUOS_PP_IF_(cond) MUOS_PP_IF##cond
#define MUOS_PP_IF(cond) MUOS_PP_IF_(cond)
#define MUOS_PP_IF0(...)
#define MUOS_PP_IF1(...) __VA_ARGS__

#define MUOS_PP_IF_ARGS(...) MUOS_PP_IF(MUOS_PP_HAVE_ARGS(__VA_ARGS__))


#define MUOS_PP_CODEGEN_0(fn, first, ...) fn(0, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_1(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_1(fn, first, ...) fn(1, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_2(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_2(fn, first, ...) fn(2, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_3(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_3(fn, first, ...) fn(3, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_4(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_4(fn, first, ...) fn(4, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_5(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_5(fn, first, ...) fn(5, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_6(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_6(fn, first, ...) fn(6, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_7(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_7(fn, first, ...) fn(7, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_8(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_8(fn, first, ...) fn(8, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_9(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_9(fn, first, ...) fn(9, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_10(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_10(fn, first, ...) fn(10, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_11(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_11(fn, first, ...) fn(11, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_12(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_12(fn, first, ...) fn(12, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_13(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_13(fn, first, ...) fn(13, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_14(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_14(fn, first, ...) fn(14, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_15(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_15(fn, first, ...) fn(15, first) MUOS_PP_IF_ARGS(__VA_ARGS__)(MUOS_PP_CODEGEN_16(fn, __VA_ARGS__))
#define MUOS_PP_CODEGEN_16(fn, first, ...) fn(16, first)

#define MUOS_PP_CODEGEN(fn, ...) MUOS_PP_CODEGEN_0(fn, __VA_ARGS__)

/* HOWTO

#define MUOS_STEPPER_HW HW(1,B,ORC1A,0),HW(3,B,ORC3A,0)

#define HWE(index, arg) arg[index]

   MUOS_PP_CODEGEN(HWE, MUOS_STEPPER_HW )

*/
#endif
