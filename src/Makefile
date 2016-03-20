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

PLATFORM = atmel/avr
#MCU = atmega328p
MCU = attiny85

MAIN = example

#PROGRAMMER = avrdude
PROGRAMMER = micronucleus

AVRDUDE_BAUDRATE = 57600
AVRDUDE_PORT = $(wildcard /dev/ttyUSB?)
#AVRDUDE_PROGRAMMER = usbasp
AVRDUDE_PROGRAMMER = arduino

# additional C Source file
SOURCES += $(wildcard *.c)

# Common CC Flags
CCFLAGS += -g
#CCFLAGS += -DF_CPU=62500UL
#CCFLAGS += -DF_CPU=125000UL
#CCFLAGS += -DF_CPU=250000UL
#CCFLAGS += -DF_CPU=500000UL
#CCFLAGS += -DF_CPU=1000000UL
#CCFLAGS += -DF_CPU=2000000UL
#CCFLAGS += -DF_CPU=4000000UL
#CCFLAGS += -DF_CPU=8000000UL
CCFLAGS += -DF_CPU=16000000UL

# Dependency generation Flags
#DEPFLAGS +=

# Preprocessor Flags
#CPPFLAGS +=

# Compile Flags
CFLAGS += -Wall -Wextra -Werror


# Linker flags
LDFLAGS += -Wl,--relax,--gc-sections
LDFLAGS += -Xlinker --no-fatal-warnings


#configuration:
#: detailed configuration



#:MUOS_DEBUG_BUSY
#: Uses a GPIO to indicate when the cpu is busy,
#: otherwise, while sleeping the GPIO is turned off.
#FIXME: currently hardcoded GPIO
#MUOS_CONFIG += -DMUOS_DEBUG_BUSY=1


#:MUOS_DEBUG_INTR
#: Uses a GPIO to indicate when the cpu is handling interrupts,
#: otherwise, the GPIO is turned off.
#FIXME: currently hardcoded GPIO
#MUOS_CONFIG += -DMUOS_DEBUG_INTR=1


#:MUOS_DEBUG_ERROR
#: Uses a GPIO to indicate when an error got set,
#: otherwise, the GPIO is turned off.
#FIXME: currently hardcoded GPIO
#MUOS_CONFIG += -DMUOS_DEBUG_ERROR=1


#:MUOS_INITFN
#: Name of the user-defined initialization function.
#: This function is pushed on the rtq (or, if not available the bgq) when starting up. It is
#: responsible for initializing the system. Interrupts are still diabled and the clock is
#: stopped and will be started after this init function returns.
MUOS_CONFIG += -DMUOS_INITFN=init



#:MUOS_ERRORFN
#: Name of the user-defined function which is called when am error is pending.
#: Errors can happen synchronusly and be handled right where they are happen or asynchronously
#: from interrupts. If any undhandled error stays pending, this function will be called
#: synchronously to handle it at very high priority (before any queues)
MUOS_CONFIG += -DMUOS_ERRORFN=error



#:MUOS_EXPLICIT_INIT
#: when set to 1, then all static initialization of muos internal datastructures is disabled
#: initialization will then be done explicitly in init sections or in main before any of these
#: facilities are used
MUOS_CONFIG += -DMUOS_EXPLICIT_INIT=0
#MUOS_CONFIG += -DMUOS_EXPLICIT_INIT=1 #PLANNED: not implemented yet



#:MUOS_SCHED_DEPTH
#: Set the maximum depth for nested scheduler calls (muos_wait() muos_yield())
MUOS_CONFIG += -DMUOS_SCHED_DEPTH=3



#:MUOS_SCHED_SLEEP
#: Set sleep mode for the main loop if supported
MUOS_CONFIG += -DMUOS_SCHED_SLEEP=SLEEP_MODE_IDLE



#:MUOS_QUEUE_INDEX
#: the bits used for indexing queues
#: comes in 3 variants
#:  4 bits allow only small queues for up to (8-16) functions, use only for really small targets
#:  8 bits allow queues for up to 256 functions
#:  16 bits allow huge queues, use only when really required
MUOS_CONFIG += -DMUOS_QUEUE_INDEX=4
#MUOS_CONFIG += -DMUOS_QUEUE_INDEX=8
#MUOS_CONFIG += -DMUOS_QUEUE_INDEX=16



#:MUOS_RTQ_LENGTH
#: How many entries the realtime queue can hold, set to 0 to disable the rtq
#MUOS_CONFIG += -DMUOS_RTQ_LENGTH=0
MUOS_CONFIG += -DMUOS_RTQ_LENGTH=8
#UOS_CONFIG += -DMUOS_RTQ_LENGTH=32
#MUOS_CONFIG += -DMUOS_RTQ_LENGTH=128



#:MUOS_BGQ_LENGTH
#: How many entries the background queue can hold, set to 0 to disable the bgq
#MUOS_CONFIG += -DMUOS_BGQ_LENGTH=0
MUOS_CONFIG += -DMUOS_BGQ_LENGTH=8
#MUOS_CONFIG += -DMUOS_BGQ_LENGTH=32
#MUOS_CONFIG += -DMUOS_BGQ_LENGTH=128



#PLANNED: MUOS_QUEUE_ARGTAG
# : How functions with arguments are tagged in queues, currently only the 'negate is implemened
# : Works with mpu's with up to 64k flash
#MUOS_CONFIG += -DMUOS_QUEUE_ARGTAG=NEGATE



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
MUOS_CONFIG += -DMUOS_SPRIQ_INDEX=uint8_t
#MUOS_CONFIG += -DMUOS_SPRIQ_INDEX=uint16_t



#:MUOS_LPRIQ_TYPE
#: type used for the 'priorities' of the priority queue
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
MUOS_CONFIG += -DMUOS_CLPQ_LENGTH=4
#MUOS_CONFIG += -DMUOS_CLPQ_LENGTH=16
#MUOS_CONFIG += -DMUOS_CLPQ_LENGTH=32
#MUOS_CONFIG += -DMUOS_CLPQ_LENGTH=128



#:MUOS_CLOCK_HW
#: Which Hardware timer to use.
#: This is a hardware dependent config. usually a number or a simple name
#: timer hardware setup is tightly bound to the hardware capabilities and only barely
#: abstracted, check for the respective hardware implementation about possible settings.
MUOS_CONFIG += -DMUOS_CLOCK_HW=0
#MUOS_CONFIG += -DMUOS_CLOCK_HW=1



#:MUOS_CLOCK_HW_PRESCALER
#: Prescaler from some hardware defined master clock
#MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=0
#MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=8
#MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=64
#MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=256
MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=1024



#:MUOS_CLOCK_HW_COMPAREMATCH
#: The Compare/Match unit of the Timer to use
#: This is a hardware dependent config. usually a number or a simple name
MUOS_CONFIG += -DMUOS_CLOCK_HW_COMPAREMATCH=A



#:MUOS_CLOCK_TYPE
#: MµOS clock configuration above the hardware
#: The type used for the overflow counter of the clock.
#: together with the harware register of the clock this gives the full time.
MUOS_CONFIG += -DMUOS_CLOCK_TYPE=uint32_t
#MUOS_CONFIG += -DMUOS_CLOCK_TYPE=uint64_t



#:MUOS_CLOCK_SHORT_TYPE
#: The type used for shorter timespans.
#MUOS_CONFIG += -DMUOS_CLOCK_SHORT_TYPE=uint8_t
MUOS_CONFIG += -DMUOS_CLOCK_SHORT_TYPE=uint16_t
#MUOS_CONFIG += -DMUOS_CLOCK_SHORT_TYPE=uint32_t
#MUOS_CONFIG += -DMUOS_CLOCK_SHORT_TYPE=uint64_t



#:MUOS_CLOCK_CALIBRATE
#: Frequency calibration api for syncing the
#: master clock with some external signal.
#:  0 = disabled
#MUOS_CONFIG += -DMUOS_CLOCK_CALIBRATE=0
MUOS_CONFIG += -DMUOS_CLOCK_CALIBRATE=1



#:MUOS_CLOCK_CALIBRATE_MAX_DERVIATION
MUOS_CONFIG += -D'MUOS_CLOCK_CALIBRATE_MAX_DERVIATION(x)=((x)/8)'



#:MUOS_CLOCK_CALIBRATE_DRIFT
#: If defined compensate drift.
#: The Value should be the time for a rolling average to compensate the drift over.
#: Sycnronization must then be called at regular time intervals.
MUOS_CONFIG += -DMUOS_CLOCK_CALIBRATE_DRIFT='MUOS_CLOCK_MILLISECONDS(10000)'


#TODO: static calibration interval


#:MUOS_BUFFER_INDEX
#: type used to the cyclic buffer length and indexing
MUOS_CONFIG += -DMUOS_BUFFER_INDEX=uint8_t
#MUOS_CONFIG += -DMUOS_BUFFER_INDEX=uint16_t



#:MUOS_SERIAL_HW
#: Which serial hardware to use
MUOS_CONFIG += -DMUOS_SERIAL_HW=0



#:MUOS_SERIAL_BAUD
#: Baudrate for the serial interface
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD=50UL
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD=300UL
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD=1200UL
MUOS_CONFIG += -DMUOS_SERIAL_BAUD=9600UL
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD=57600UL
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD=115200UL



#:MUOS_SERIAL_BAUD_TXBOOST
#: Inrcease the TX baudrate by this much permille
#: Setting the TX speed little higher makes the link a bit more
#: reliable when the clocks are out of sync, esp when one echos back each
#: character received. 0.3% as default should work fine
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD_TXBOOST=0ULL
MUOS_CONFIG += -DMUOS_SERIAL_BAUD_TXBOOST=3ULL
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD_TXBOOST=5ULL



#PLANNED: parity bits stoppbits flowcontrol
#MUOS_SERIAL_DATABITS=8
#MUOS_SERIAL_PARITY=N
#MUOS_SERIAL_STOPBITS=1
#MUOS_SERIAL_XONOFF=0



#:MUOS_SERIAL_TXBUFFER
#: length of the transmission buffer
#: 0 disables sending data
#: >1 creates a buffer of that size
#FIXME: handle the case when txbuffer is smaller than lineedit buffer
MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=0
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=2
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=4
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=8
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=16
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=32
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=64
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=128



#:MUOS_SERIAL_TXQUEUE
#: use a tagged queue for TX requests.
#: A tagged queue can store data as references and in binary
#: format. This may reduce the memory needed for queueing
#: massivly and improve latency. The TX buffer above can then be smaller.
#:
#: 0 disables the queue
#: >1 creates a queue of that size
MUOS_CONFIG += -DMUOS_SERIAL_TXQUEUE=0
#MUOS_CONFIG += -DMUOS_SERIAL_TXQUEUE=16
#MUOS_CONFIG += -DMUOS_SERIAL_TXQUEUE=32
#MUOS_CONFIG += -DMUOS_SERIAL_TXQUEUE=64
#MUOS_CONFIG += -DMUOS_SERIAL_TXQUEUE=80
#MUOS_CONFIG += -DMUOS_SERIAL_TXQUEUE=128


#:MUOS_SERIAL_TXQUEUE_OPT
#: Optimization level
#:
#: optimize for:
#: 	ram
#: 	flash
#: 	cpu
#:	latency
#:
#:	type support for 24 48 64 bit
#:
#:
#MUOS_CONFIG += -DMUOS_SERIAL_TXQUEUE_OPT =

#MUOS_CONFIG += -DMUOS_SERIAL_TXQFMT= format specifiers
#MUOS_CONFIG += -DMUOS_SERIAL_TXQCTRL= ansi control sequences



#:MUOS_SERIAL_RXBUFFER
#: length of the receive buffer
#: 0 disables receiving data
#: >1 creates a buffer of that size
MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=0
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=2
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=4
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=8
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=16
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=32
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=64
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=128



#:MUOS_SERIAL_RXSYNC
#: Starting
#: 0 disables receiving data
#: >1 creates a buffer of that size
MUOS_CONFIG += "-DMUOS_SERIAL_RXSYNC='\r'"




#:MUOS_SERIAL_RXCALLBACK
#: function to be called from rtq when a character is received
#: either a user defined function or muos_lineedit for a default line editor
#MUOS_CONFIG += -DMUOS_SERIAL_RXCALLBACK=serial_echo
MUOS_CONFIG += -DMUOS_SERIAL_RXCALLBACK=muos_lineedit



#:MUOS_LINEEDIT_BUFFER
#: length of the linedit buffer
MUOS_CONFIG += -DMUOS_LINEEDIT_BUFFER=0
#MUOS_CONFIG += -DMUOS_LINEEDIT_BUFFER=16
#MUOS_CONFIG += -DMUOS_LINEEDIT_BUFFER=32
#MUOS_CONFIG += -DMUOS_LINEEDIT_BUFFER=64
#MUOS_CONFIG += -DMUOS_LINEEDIT_BUFFER=128



#:MUOS_LINEEDIT_UTF8
#: enable UTF8 support for lineedit
#: when disabled only 7bit ascii is used
#MUOS_CONFIG += -DMUOS_LINEEDIT_UTF8=0
MUOS_CONFIG += -DMUOS_LINEEDIT_UTF8=1



# 0 = simple (only backspace)
# 1 = cursors
# 2 = history
# 3 = completion



#:MUOS_LINEEDIT_RECALL
#: most simple history, only one byte in ram and litte flash needed
#: One can recall the previous line with cursor up, immediately after pressing return
#MUOS_CONFIG += -DMUOS_LINEEDIT_RECALL=0
MUOS_CONFIG += -DMUOS_LINEEDIT_RECALL=1



#:MUOS_LINEEDIT_HISTORY
#: buffers for history

#:MUOS_LINEEDIT_CALLBACK
#: user defined function to call when a line from lineedit is done (return pressed)
MUOS_CONFIG += -DMUOS_LINEEDIT_CALLBACK=lineecho



# now load the main makefile
include muos/muos.mk
