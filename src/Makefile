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

# muos_config:MCU
# : 
MCU = atmega328p
#MCU = attiny85
#MCU = attiny84

# muos_config:MAIN
# : 
MAIN = example

# main_config:PROGRAMMER
# : 
PROGRAMMER = avrdude
#PROGRAMMER = micronucleus

# avrdude_config:
# : 
AVRDUDE_BAUDRATE = 57600
AVRDUDE_PORT = $(wildcard /dev/ttyUSB?)
#AVRDUDE_PROGRAMMER = usbasp
AVRDUDE_PROGRAMMER = arduino

# build_config:
# additional C Source file
SOURCES += $(wildcard *.c)

# build_config:CCFLAGS
#: Common CC Flags
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

# build_config:DEPFLAGS
#: Dependency generation Flags
#DEPFLAGS +=

# build_config:CPPFLAGS
#: Preprocessor Flags
#CPPFLAGS +=

# build_config:CFLAGS
#: Compile Flags
CFLAGS += -Wall -Wextra -Werror


# build_config:LDFLAGS
#: # Linker flags
LDFLAGS += -Wl,--relax,--gc-sections
LDFLAGS += -Xlinker --no-fatal-warnings



# muos_config:MUOS_DEBUG_BUSY
# : Uses a GPIO to indicate when the CPU is busy,
# : otherwise, while sleeping the GPIO is turned off.
#TODO: currently hardcoded GPIO
MUOS_CONFIG += -DMUOS_DEBUG_BUSY=1


# muos_config:MUOS_DEBUG_INTR
# : Uses a GPIO to indicate when the CPU is handling interrupts,
# : otherwise, the GPIO is turned off.
#TODO: currently hardcoded GPIO
MUOS_CONFIG += -DMUOS_DEBUG_INTR=1


# muos_config:MUOS_DEBUG_ERROR
# : Uses a GPIO to indicate when an error got set,
# : otherwise, the GPIO is turned off.
#TODO: currently hardcoded GPIO
MUOS_CONFIG += -DMUOS_DEBUG_ERROR=1

#muos_config:MUOS_INITFN
#: MUOS_INITFN::
#: Name of the user-defined initialization function.
#: This function is pushed on the hpq (or, if not available the bgq) when starting up. It is
#: responsible for initializing the system. Interrupts are still disabled and the clock is
#: stopped and will be started after this init function returns.
MUOS_CONFIG += -DMUOS_INITFN=init



#error_config:MUOS_ERRORFN
#: MUOS_ERRORFN::
#: Name of the user-defined function which is called when am error is pending.
MUOS_CONFIG += -DMUOS_ERRORFN=error



#PLANNED: MUOS_EXPLICIT_INIT
# muos_config:MUOS_EXPLICIT_INIT
# : when set to 1, then all static initialization of muos internal data structures is disabled
# : initialization will then be done explicitly in init sections or in main before any of these
# : facilities are used
MUOS_CONFIG += -DMUOS_EXPLICIT_INIT=0
#MUOS_CONFIG += -DMUOS_EXPLICIT_INIT=1 #PLANNED: not implemented yet



#muos_config:MUOS_SCHED_DEPTH
#: MUOS_SCHED_DEPTH::
#: Set the maximum depth for nested scheduler calls (muos_wait() muos_yield()).
#: When this depth is exceeded, these calls return immediately, flagging 'muos_warn_sched_depth'.
MUOS_CONFIG += -DMUOS_SCHED_DEPTH=3



#muos_config:MUOS_SCHED_SLEEP
#: MUOS_SCHED_SLEEP::
#: Set sleep mode for the main loop if supported
MUOS_CONFIG += -DMUOS_SCHED_SLEEP=SLEEP_MODE_IDLE



#lib_queue_config:MUOS_QUEUE_INDEX
#: MUOS_QUEUE_INDEX::
#:  the bits used for indexing queues
#:  comes in 3 variants
#:  - 4 bits allow only small queues for up to 16 elements, use only for really small targets
#:  - 8 bits allow queues for up to 256 elements, default
#:  - 16 bits allow huge queues, use only when really required
#MUOS_CONFIG += -DMUOS_QUEUE_INDEX=4
MUOS_CONFIG += -DMUOS_QUEUE_INDEX=8
#MUOS_CONFIG += -DMUOS_QUEUE_INDEX=16



#PLANNED: MUOS_QUEUE_ARGTAG
# : How functions with arguments are tagged in queues, currently only the MSB tag is implemented
# : Works with MPUs with up to 64k flash
#MUOS_CONFIG += -DMUOS_QUEUE_ARGTAG=NEGATE



#hpq_config:MUOS_HPQ_LENGTH
#: MUOS_HPQ_LENGTH::
#: How many entries the high priority queue can hold, set to 0 to disable the hpq
#MUOS_CONFIG += -DMUOS_HPQ_LENGTH=0
#MUOS_CONFIG += -DMUOS_HPQ_LENGTH=8
MUOS_CONFIG += -DMUOS_HPQ_LENGTH=32
#MUOS_CONFIG += -DMUOS_HPQ_LENGTH=128



#bgq_config:MUOS_BGQ_LENGTH
#: MUOS_BGQ_LENGTH::
#: How many entries the background queue can hold, set to 0 to disable the bgq
#MUOS_CONFIG += -DMUOS_BGQ_LENGTH=0
#MUOS_CONFIG += -DMUOS_BGQ_LENGTH=8
MUOS_CONFIG += -DMUOS_BGQ_LENGTH=32
#MUOS_CONFIG += -DMUOS_BGQ_LENGTH=128



# spriq_config:MUOS_SPRIQ_TYPE
# : Type used for the 'priorities' of the small priority queue.
# : This priority queue uses a 'sliding window'. The rtpq uses this.
# : Scheduler needs to be called at least half of the range provided by this type.
MUOS_CONFIG += -DMUOS_SPRIQ_TYPE=MUOS_CLOCK_SHORT_TYPE
#MUOS_CONFIG += -DMUOS_SPRIQ_TYPE=uint8_t
#MUOS_CONFIG += -DMUOS_SPRIQ_TYPE=uint16_t
#MUOS_CONFIG += -DMUOS_SPRIQ_TYPE=uint32_t



# spriq_config:MUOS_SPRIQ_INDEX
# : Type to keep track of the size of the spriq.
# : uint8_t for up to 255 entries, uint16_t for up to 65k entries.
MUOS_CONFIG += -DMUOS_SPRIQ_INDEX=uint8_t
#MUOS_CONFIG += -DMUOS_SPRIQ_INDEX=uint16_t



# :MUOS_LPRIQ_TYPE
# : type used for the 'priorities' of the priority queue
MUOS_CONFIG += -DMUOS_LPRIQ_TYPE=MUOS_CLOCK_TYPE
#MUOS_CONFIG += -DMUOS_LPRIQ_TYPE=uint8_t
#MUOS_CONFIG += -DMUOS_LPRIQ_TYPE=uint16_t
#MUOS_CONFIG += -DMUOS_LPRIQ_TYPE=uint32_t



# :MUOS_LPRIQ_INDEX
# : Type to keep track of the size of the lpriq.
#MUOS_CONFIG += -DMUOS_SPRIQ_INDEX=uint8_t
MUOS_CONFIG += -DMUOS_LPRIQ_INDEX=uint16_t


#clpq_config:MUOS_CLPQ_LENGTH
#: MUOS_CLPQ_LENGTH::
#: Number of entries the scheduling Queue can hold, set to 0 to disable the clpq
#MUOS_CONFIG += -DMUOS_CLPQ_LENGTH=4
MUOS_CONFIG += -DMUOS_CLPQ_LENGTH=16
#MUOS_CONFIG += -DMUOS_CLPQ_LENGTH=32
#MUOS_CONFIG += -DMUOS_CLPQ_LENGTH=128



#clock_config:MUOS_CLOCK_HW
#: MUOS_CLOCK_HW::
#: The Hardware timer to use. This is a hardware dependent config.
#: Timer hardware setup is tightly bound to the hardware capabilities check
#: for the respective hardware documentation about possible settings.
#MUOS_CONFIG += -DMUOS_CLOCK_HW='(0,A)'
MUOS_CONFIG += -DMUOS_CLOCK_HW='(1,A)'



#clock_config:MUOS_CLOCK_HW_PRESCALER
#: MUOS_CLOCK_HW_PRESCALER::
#: Prescaler from some hardware defined master clock. Possible
#: Values depend on the hardware.
#MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=0
MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=1
#MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=8
#MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=64
#MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=256
#MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=1024



#clock_config:MUOS_CLOCK_TYPE
#: MUOS_CLOCK_TYPE::
#: The type used for the primary clock functions. Should be some unsigned integer,
#: 32 bit width is recommended.
#MUOS_CONFIG += -DMUOS_CLOCK_TYPE=uint32_t
MUOS_CONFIG += -DMUOS_CLOCK_TYPE=uint64_t



#clock_config:MUOS_CLOCK_SHORT_TYPE
#: MUOS_CLOCK_SHORT_TYPE::
#: The type used for short time spans. Should be some unsigned integer.
#: What suits best depends on the application, often 16 bit is enough with higher Prescaler.
#MUOS_CONFIG += -DMUOS_CLOCK_SHORT_TYPE=uint16_t
MUOS_CONFIG += -DMUOS_CLOCK_SHORT_TYPE=uint32_t
#MUOS_CONFIG += -DMUOS_CLOCK_SHORT_TYPE=uint64_t



#clock_config:MUOS_CLOCK_CALIBRATE
#: MUOS_CLOCK_CALIBRATE::
#: When defined the frequency calibration API for syncing the
#: µC frequency clock with some external signal is enabled.
#MUOS_CONFIG += -DMUOS_CLOCK_CALIBRATE



#clock_config:MUOS_CLOCK_CALIBRATE_MAX_DERIVATION
#: MUOS_CLOCK_CALIBRATE_MAX_DEVIATION::
#: When the time elapsed since the last sync differs more than
#: +/- this value, the calibration measurement is reset and starts over.
#: This filters out glitches from API calls.
MUOS_CONFIG += -D'MUOS_CLOCK_CALIBRATE_MAX_DERIVATION(x)=((x)/8)'



#clock_config:MUOS_CLOCK_CALIBRATE_DEADBAND
#: MUOS_CLOCK_CALIBRATE_DEADBAND::
#: Adds a deadband/range around the target clock. On µC where the clock
#: calibration is rather coarse an exact match can not be reached and constant
#: readjustments will result in high jitter.
MUOS_CONFIG += -DMUOS_CLOCK_CALIBRATE_DEADBAND=2500



#buffer_config:MUOS_BUFFER_INDEX
#: type used to the cyclic buffer length and indexing
MUOS_CONFIG += -DMUOS_BUFFER_INDEX=uint8_t
#MUOS_CONFIG += -DMUOS_BUFFER_INDEX=uint16_t



#serial_config:MUOS_SERIAL
#: Enable Serial
MUOS_CONFIG += -DMUOS_SERIAL



#serial_config:MUOS_SERIAL_HW
#: Which serial hardware to use
MUOS_CONFIG += -DMUOS_SERIAL_HW=0



#serial_config:MUOS_SERIAL_BAUD
#: Baudrate for the serial interface
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD=50UL
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD=300UL
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD=1200UL
MUOS_CONFIG += -DMUOS_SERIAL_BAUD=9600UL
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD=57600UL
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD=115200UL



#serial_config:MUOS_SERIAL_BAUD_TXBOOST
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



#serial_config:MUOS_SERIAL_TXBUFFER
#: length of the transmission buffer
#: 0 disables sending data
#: >1 creates a buffer of that size
#FIXME: handle the case when txbuffer is smaller than lineedit buffer
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=0
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=2
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=4
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=8
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=16
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=32
MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=64
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=128


#TODO: also disable facilities by undefined not only 0


#serial_config:MUOS_SERIAL_TXQUEUE
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



#serial_config:MUOS_SERIAL_RXBUFFER
#: length of the receive buffer
#: 0 disables receiving data
#: >1 creates a buffer of that size
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=0
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=2
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=4
MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=8
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=16
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=32
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=64
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=128



#serial_config:MUOS_SERIAL_RXSYNC
MUOS_CONFIG += "-DMUOS_SERIAL_RXSYNC='\r'"




#serial_config:MUOS_SERIAL_RXCALLBACK
#: function to be called from hpq when a character is received
#: either a user defined function or muos_lineedit for a default line editor
#MUOS_CONFIG += -DMUOS_SERIAL_RXCALLBACK=serial_echo
MUOS_CONFIG += -DMUOS_SERIAL_RXCALLBACK=muos_lineedit



#lineedit_config:MUOS_LINEEDIT_BUFFER
#: length of the linedit buffer
#MUOS_CONFIG += -DMUOS_LINEEDIT_BUFFER=0
#MUOS_CONFIG += -DMUOS_LINEEDIT_BUFFER=16
#MUOS_CONFIG += -DMUOS_LINEEDIT_BUFFER=32
MUOS_CONFIG += -DMUOS_LINEEDIT_BUFFER=64
#MUOS_CONFIG += -DMUOS_LINEEDIT_BUFFER=128



#lineedit_config:MUOS_LINEEDIT_UTF8
#: enable UTF8 support for lineedit
#: when disabled only 7bit ascii is used
#MUOS_CONFIG += -DMUOS_LINEEDIT_UTF8



# 0 = simple (only backspace)
# 1 = cursors
# 2 = history
# 3 = completion



#lineedit_config:MUOS_LINEEDIT_RECALL
#: most simple history, only one byte in ram and litte flash needed
#: One can recall the previous line with cursor up, immediately after pressing return
#MUOS_CONFIG += -DMUOS_LINEEDIT_RECALL=0
MUOS_CONFIG += -DMUOS_LINEEDIT_RECALL=1



#:MUOS_LINEEDIT_HISTORY
#: buffers for history

#lineedit_config:MUOS_LINEEDIT_CALLBACK
#: user defined function to call when a line from lineedit is done (return pressed)
MUOS_CONFIG += -DMUOS_LINEEDIT_CALLBACK=lineecho



#cppm_config:MUOS_CPPM
#: enable the CPPM Parser for Radio Control Signals
MUOS_CONFIG += -DMUOS_CPPM


#cppm_config:MUOS_CPPM_CHANNELS
#: number of CPPM channels
MUOS_CONFIG += -DMUOS_CPPM_CHANNELS=8
#MUOS_CONFIG += -DMUOS_CPPM_CHANNELS=16



#cppm_config:MUOS_CPPM_RAW
#: store the raw cppm timing data in
#: uint16_t muos_cppm_channel_raw[]
MUOS_CONFIG += -DMUOS_CPPM_RAW



#cppm_config:MUOS_CPPM_RAW_FILTER
#: filter expression for raw channel data
MUOS_CONFIG += -D'MUOS_CPPM_RAW_FILTER(old,new)=(old + new)/2'



#cppm_config:MUOS_CPPM_COOKED
#: store the cppm data (glitch filtered) in
#:  int8_t muos_cppm_channel_cooked[]
#: with a range from -125 to 125 this simplifies access
#: saves a few bytes but looses resolution
MUOS_CONFIG += -DMUOS_CPPM_COOKED



#cppm_config:MUOS_CPPM_COOKED_MIN
#: defines the minimum signal length for cooked values
MUOS_CONFIG += -DMUOS_CPPM_COOKED_MIN='MUOS_CLOCK_MICROSECONDS(1000)'



#cppm_config:MUOS_CPPM_COOKED_MAX
#: defines the maximum signal length for cooked values
MUOS_CONFIG += -DMUOS_CPPM_COOKED_MAX='MUOS_CLOCK_MICROSECONDS(2000)'



#cppm_config:MUOS_CPPM_CAPTURE
#: Select Input Capture driver
#: Input capture is more precise than pinchange
#TODO: derrive from clock definition
MUOS_CONFIG += -DMUOS_CPPM_CAPTURE='(1)'



# cppm_config:MUOS_CPPM_PCINT
#PLANNED: cppm pcint
# : Select PinChange driver
# : Pin change interrupt bit
#MUOS_CONFIG += -DMUOS_CPPM_PCINT=8



#cppm_config:MUOS_CPPM_FRAME
#: Length of a full CPPM frame
MUOS_CONFIG += -DMUOS_CPPM_FRAME='MUOS_CLOCK_MILLISECONDS(27)'



#cppm_config:MUOS_CPPM_FRAME_CLOCKSYNC
#: Use the MUOS_CLOCK_CALIBRATE to synchronize the µC clock with the CPPM frames
#MUOS_CONFIG += -DMUOS_CPPM_FRAME_CALIBRATE=0
MUOS_CONFIG += -DMUOS_CPPM_FRAME_CALIBRATE=1



#cppm_config:MUOS_CPPM_MIN
#: Minimum CPPM Pulse length
MUOS_CONFIG += -DMUOS_CPPM_MIN='MUOS_CLOCK_MICROSECONDS(750)'



#cppm_config:MUOS_CPPM_MAX
#: Maximum CPPM Pulse length, also any longer pulse defines the frame start
MUOS_CONFIG += -DMUOS_CPPM_MAX='MUOS_CLOCK_MICROSECONDS(2250)'



#cppm_config:MUOS_CPPM_CALLBACK
#: function to be called from hpq when a frame got received
MUOS_CONFIG += -DMUOS_CPPM_CALLBACK=cppm_output






# now load the main makefile
include muos/muos.mk
