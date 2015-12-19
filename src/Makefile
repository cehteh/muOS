#
#      mµOS            - my micro OS
#
# Copyright (C)
#      2015                            Christian Thäter <ct@pipapo.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# Project configuration

PLATFORM = atmel
MCU = atmega328p
#MCU = attiny85

MAIN = example

PROGRAMMER = avrdude
#PROGRAMMER = micronucleus

AVRDUDE_BAUDRATE = 57600
AVRDUDE_PORT = /dev/ttyUSB0
#AVRDUDE_PROGRAMMER = usbasp
AVRDUDE_PROGRAMMER = arduino

# additional C Source file
SOURCES += $(wildcard *.c)

# Common CC Flags
#CCFLAGS +=

# Dependency generation Flags
#DEPFLAGS +=

# Preprocessor Flags
#CPPFLAGS +=

# Compile Flags
CFLAGS += -Wall -Wextra -Werror
#CFLAGS += -DF_CPU=16000000UL
CFLAGS += -DF_CPU=62500UL	# with clkdiv 256 for slow debuging
#CFLAGS += -DUART_BAUD=9600

# Linker flags
LDFLAGS += -Wl,--relax,--gc-sections
LDFLAGS += -Xlinker --no-fatal-warnings


#configuration:
#: detailed configuration

#:MUOS_INITFN
#: Name of the user-defined initialization function.
#: This function is pushed on the rtq (or, if not available the bgq) when starting up. It is
#: responsible for initializing the system. Interrupts are still diabled and the clock is
#: stopped and will be started after this init function returns.
MUOS_CONFIG += -DMUOS_INITFN=init


#:MUOS_EXPLICIT_INIT
#: when set to 1, then all static initialization of muos internal datastructures is disabled
#: initialization will then be done explicitly in init sections or in main before any of these
#: facilities are used
MUOS_CONFIG += -DMUOS_EXPLICIT_INIT=0
#MUOS_CONFIG += -DMUOS_EXPLICIT_INIT=1 #PLANNED: not implemented yet



#:MUOS_QUEUE_INDEX
#: the bits used for indexing queues
#: comes in 3 variants
#:  4 bits allow only small queues for up to (8-16) functions, use only for really small targets
#:  8 bits allow queues for up to 256 functions
#:  16 bits allow huge queues, use only when really required
#MUOS_CONFIG += -DMUOS_QUEUE_INDEX=4
MUOS_CONFIG += -DMUOS_QUEUE_INDEX=8
#MUOS_CONFIG += -DMUOS_QUEUE_INDEX=16

#:MUOS_ERROR_LOG_LENGTH
#: if 1 then only one error code is kept in memory
#: if greater than 1 a circular log of recent errors is kept in memory (prefer powers of 2)
#: if 0 then error storing is disabled, muos_error_set() is a nop, muos_error() fails
MUOS_CONFIG += -DMUOS_ERROR_LOG_LENGTH=0
#MUOS_CONFIG += -DMUOS_ERROR_LOG_LENGTH=1
#MUOS_CONFIG += -DMUOS_ERROR_LOG_LENGTH=8



#:MUOS_SCHED_DEPTH
#: Set the maximum depth for nested scheduler calls (muos_wait() muos_yield())
MUOS_CONFIG += -DMUOS_SCHED_DEPTH=3



#:MUOS_SCHED_SLEEP
#: Set sleep mode for the main loop if supported
MUOS_CONFIG += -DMUOS_SCHED_SLEEP=SLEEP_MODE_IDLE



#:MUOS_RTQ_LENGTH
#: How many entries the realtime queue can hold, set to 0 to disable the rtq
MUOS_CONFIG += -DMUOS_RTQ_LENGTH=32



#:MUOS_BGQ_LENGTH
#: How many entries the background queue can hold, set to 0 to disable the bgq
MUOS_CONFIG += -DMUOS_BGQ_LENGTH=32



#PLANNED: MUOS_QUEUE_ARGTAG
# : How functions with arguments are tagged in queues, currently only the 'negate is implemened
# : Works with mpu's with up to 64k flash
#MUOS_CONFIG += -DMUOS_QUEUE_ARGTAG=NEGATE
##endif



#:MUOS_SPRIQ_TYPE
#: Type used for the 'priorities' of the small priority queue.
#: This priority queue uses a 'sliding window'. The rtpq uses this.
#: Scheduler needs to be called at least half of the range provided by this type.
MUOS_CONFIG += -DMUOS_SPRIQ_TYPE=MUOS_CLOCK_SHORT_TYPE
#MUOS_CONFIG += -DMUOS_SPRIQ_TYPE=uint8_t
#MUOS_CONFIG += -DMUOS_SPRIQ_TYPE=uint16_t
#MUOS_CONFIG += -DMUOS_SPRIQ_TYPE=uint32_t



#:MUOS_SPRIQ_INDEX
#: Type to keep track of the size of the spriq.
#: uint8_t for up to 255 entries, uint16_t for up to 65k entries.
#: choosing uint8_t saves only one byte ram, usually not worth it
#MUOS_CONFIG += -DMUOS_SPRIQ_INDEX=uint8_t
MUOS_CONFIG += -DMUOS_SPRIQ_INDEX=uint16_t



#:MUOS_LPRIQ_TYPE
#: type used for the 'priorities' if the priority queue
MUOS_CONFIG += -DMUOS_LPRIQ_TYPE=MUOS_CLOCK_TYPE
#MUOS_CONFIG += -DMUOS_LPRIQ_TYPE=uint8_t
#MUOS_CONFIG += -DMUOS_LPRIQ_TYPE=uint16_t
#MUOS_CONFIG += -DMUOS_LPRIQ_TYPE=uint32_t



#:MUOS_LPRIQ_INDEX
#: Type to keep track of the size of the lpriq.
#MUOS_CONFIG += -DMUOS_SPRIQ_INDEX=uint8_t
MUOS_CONFIG += -DMUOS_LPRIQ_INDEX=uint16_t


#:MUOS_CLPQ_LENGTH
#: How many entriex the scheduling Queue can hold, set to 0 to disable the clpq
MUOS_CONFIG += -DMUOS_CLPQ_LENGTH=32



#:MUOS_CLOCK_HW
#: Which Hardware timer to use.
#: This is a hardware dependent config. usually a number or a simple name
#: timer hardware setup is tightly bound to the hardware capabilities and only barely
#: abstracted, check for the respective hardware implementation about possible settings.
MUOS_CONFIG += -DMUOS_CLOCK_HW=0



#:MUOS_CLOCK_HW_PRESCALER
#: Prescaler from some hardware defined master clock
MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=1024


#:MUOS_CLOCK_HW_COMPAREMATCH
#: The Compare/Match unit of the Timer to use
#: This is a hardware dependent config. usually a number or a simple name
MUOS_CONFIG += -DMUOS_CLOCK_HW_COMPAREMATCH=A



#:MUOS_CLOCK_TYPE
#: MµOS clock configuration above the hardware
#: The type used for the overflow counter of the clock.
#: together with the harware register of the clock this gives the full time.
#MUOS_CONFIG += -DMUOS_CLOCK_TYPE=uint16_t
#MUOS_CONFIG += -DMUOS_CLOCK_TYPE=uint32_t
MUOS_CONFIG += -DMUOS_CLOCK_TYPE=uint64_t



#:MUOS_CLOCK_SHORT_TYPE
#: The type used for shorter timespans.
#MUOS_CONFIG += -DMUOS_CLOCK_SHORT_TYPE=uint8_t
MUOS_CONFIG += -DMUOS_CLOCK_SHORT_TYPE=uint16_t
#MUOS_CONFIG += -DMUOS_CLOCK_SHORT_TYPE=uint32_t
#MUOS_CONFIG += -DMUOS_CLOCK_SHORT_TYPE=uint64_t


# now load the main makefile
include muos/muos.mk
