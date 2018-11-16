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
#: When not defined then the wait and yield functions become unavailable
#MUOS_CONFIG += -DMUOS_SCHED_DEPTH=3



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



#lib_spriq_config:MUOS_SPRIQ_TYPE
#: MUOS_SPRIQ_TYPE::
#: Type used for the 'priorities' in spriq.
MUOS_CONFIG += -DMUOS_SPRIQ_TYPE=MUOS_CLOCK_SHORT_TYPE
#MUOS_CONFIG += -DMUOS_SPRIQ_TYPE=uint8_t
#MUOS_CONFIG += -DMUOS_SPRIQ_TYPE=uint16_t
#MUOS_CONFIG += -DMUOS_SPRIQ_TYPE=uint32_t



#lib_spriq_config:MUOS_SPRIQ_INDEX
#: MUOS_SPRIQ_INDEX::
#: Type to keep track of the size of the spriq.
#: +uint8_t+ for up to 255 entries, +uint16_t+ for up to 65k entries.
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



#debug_config:MUOS_DEBUG
#: MUOS_DEBUG::
#: When defined, the debug driver is enabled
MUOS_CONFIG += -DMUOS_DEBUG



#debug_config:MUOS_DEBUG_BUSY
#: MUOS_DEBUG_BUSY::
#: Uses a GPIO to indicate when the CPU is busy.
#: While sleeping the GPIO is turned off.
MUOS_CONFIG += -DMUOS_DEBUG_BUSY='(B,5)'



#debug_config:MUOS_DEBUG_INTR
#: MUOS_DEBUG_INTR::
#: Uses a GPIO to indicate when the CPU is handling interrupts.
MUOS_CONFIG += -DMUOS_DEBUG_INTR='(B,4)'



#debug_config:MUOS_DEBUG_ERROR
#: MUOS_DEBUG_ERROR::
#: Uses a GPIO to indicate when an error got set,
#: Turned off when no errors are pending.
MUOS_CONFIG += -DMUOS_DEBUG_ERROR='(B,2)'



#debug_config:MUOS_DEBUG_Cx
#: MUOS_DEBUG_Cx::
#: *MUOS_DEBUG_C1, MUOS_DEBUG_C2, MUOS_DEBUG_C3, MUOS_DEBUG_C4*.
#: User defined Debugging channels.
MUOS_CONFIG += -DMUOS_DEBUG_C1='(B,3)'
#MUOS_CONFIG += -DMUOS_DEBUG_C2='()'
#MUOS_CONFIG += -DMUOS_DEBUG_C3='()'
#MUOS_CONFIG += -DMUOS_DEBUG_C4='()'




#PLANNED: make MUOS_CLOCK configureable, system w/o clock
MUOS_CONFIG += -DMUOS_CLOCK


#clock_config:MUOS_CLOCK_HW
#: MUOS_CLOCK_HW::
#: The Hardware timer to use. This is a hardware dependent config.
#: Timer hardware setup is tightly bound to the hardware capabilities check
#: for the respective hardware documentation about possible settings.
MUOS_CONFIG += -DMUOS_CLOCK_HW='(0,A)'
#MUOS_CONFIG += -DMUOS_CLOCK_HW='(1,A)'



#clock_config:MUOS_CLOCK_HW_PRESCALER
#: MUOS_CLOCK_HW_PRESCALER::
#: Prescaler from some hardware defined master clock. Possible
#: Values depend on the hardware.
#MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=0
#MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=1
#MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=8
#MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=64
#MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=256
MUOS_CONFIG += -DMUOS_CLOCK_PRESCALER=1024



#clock_config:MUOS_CLOCK_TYPE
#: MUOS_CLOCK_TYPE::
#: The type used for the primary clock functions. Should be some unsigned integer,
#: 32 bit width is recommended.
MUOS_CONFIG += -DMUOS_CLOCK_TYPE=uint32_t
#MUOS_CONFIG += -DMUOS_CLOCK_TYPE=uint64_t



#clock_config:MUOS_CLOCK_SHORT_TYPE
#: MUOS_CLOCK_SHORT_TYPE::
#: The type used for short time spans. Should be some unsigned integer.
#: What suits best depends on the application, often 16 bit is enough with higher Prescaler.
MUOS_CONFIG += -DMUOS_CLOCK_SHORT_TYPE=uint16_t
#MUOS_CONFIG += -DMUOS_CLOCK_SHORT_TYPE=uint32_t
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



#lib_cbuffer_config:MUOS_CBUFFER_INDEX
#: MUOS_CBUFFER_INDEX::
#: type used to the cyclic buffer length and indexing
MUOS_CONFIG += -DMUOS_CBUFFER_INDEX=uint8_t
#MUOS_CONFIG += -DMUOS_CBUFFER_INDEX=uint16_t



#uart_config:MUOS_SERIAL
#: MUOS_SERIAL::
#: Enable Serial
MUOS_CONFIG += -DMUOS_SERIAL



#uart_config:MUOS_SERIAL_HW
#: MUOS_SERIAL_HW::
#: Serial hardware to use
MUOS_CONFIG += -DMUOS_SERIAL_HW=0



#uart_config:MUOS_SERIAL_BAUD
#: MUOS_SERIAL_BAUD::
#: Baudrate for the serial interface
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD=50UL
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD=300UL
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD=1200UL
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD=9600UL
#MUOS_CONFIG += -DMUOS_SERIAL_BAUD=57600UL
MUOS_CONFIG += -DMUOS_SERIAL_BAUD=115200UL



#PLANNED: parity bits stoppbits flowcontrol
#MUOS_SERIAL_DATABITS=8
#MUOS_SERIAL_PARITY=N
#MUOS_SERIAL_STOPBITS=1
#MUOS_SERIAL_XONOFF=0



#uart_config:MUOS_SERIAL_TXBUFFER
#: MUOS_SERIAL_TXBUFFER::
#: length of the transmission buffer
#:  * 0 disables sending data
#:  * >1 creates a buffer of that size
#FIXME: handle the case when txbuffer is smaller than lineedit buffer (also TXQUEUE) push reference, disable lineedit until send
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=0
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=2
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=4
MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=8
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=16
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=32
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=64
#MUOS_CONFIG += -DMUOS_SERIAL_TXBUFFER=128



#uart_config:MUOS_SERIAL_TX_BLOCKING
#: MUOS_SERIAL_TX_BLOCKING::
#: Make the TX blocking by default.
#MUOS_CONFIG += -DMUOS_SERIAL_TX_BLOCKING



#uart_config:MUOS_SERIAL_RXBUFFER
#: MUOS_SERIAL_RXBUFFER::
#: length of the receive buffer
#:  * 0 disables receiving data
#:  * >1 creates a buffer of that size
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=0
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=2
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=4
MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=8
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=16
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=32
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=64
#MUOS_CONFIG += -DMUOS_SERIAL_RXBUFFER=128



#uart_config:MUOS_SERIAL_RX_BLOCKING
#: MUOS_SERIAL_RX_BLOCKING::
#: Make the RX blocking by default.
#MUOS_CONFIG += -DMUOS_SERIAL_RX_BLOCKING



#PLANNED: flag for disabling blocking io
#PLANNED: avr-libc stdio
#PLANNED: investigate if its better to return int16_t with negative error code than async errors on rx_byte



#uart_config:MUOS_SERIAL_RXSYNC
#: MUOS_SERIAL_RXSYNC::
#: The receiver stays in desynced state unti the given character got received
MUOS_CONFIG += "-DMUOS_SERIAL_RXSYNC='\r'"



#PLANNED: configure if hpq or bgq shall be used
#uart_config:MUOS_SERIAL_RXCALLBACK
#: MUOS_SERIAL_RXCALLBACK::
#: Function to be called from 'hpq' to handle the RX buffer data
#MUOS_CONFIG += -DMUOS_SERIAL_RXCALLBACK=serial_echo
MUOS_CONFIG += -DMUOS_SERIAL_RXCALLBACK=muos_lineedit


#TODO: also disable facilities by undefined not only 0


#txqueue_config:MUOS_SERIAL_TXQUEUE
#: MUOS_SERIAL_TXQUEUE::
#: length of the txqueue
#: * 0 disables the txqueue
#: * >1 creates a txqueue of the given size
#TODO: check that simply undef is enough for disable
#MUOS_CONFIG += -DMUOS_SERIAL_TXQUEUE=0
#MUOS_CONFIG += -DMUOS_SERIAL_TXQUEUE=16
#MUOS_CONFIG += -DMUOS_SERIAL_TXQUEUE=32
MUOS_CONFIG += -DMUOS_SERIAL_TXQUEUE=64
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
#PLANNED: MUOS_CONFIG += -DMUOS_SERIAL_TXQUEUE_OPT =
#PLANNED: MUOS_CONFIG += -DMUOS_SERIAL_TXQFMT= format specifiers
#PLANNED: MUOS_CONFIG += -DMUOS_SERIAL_TXQCTRL= ansi control sequences




#lineedit_config:MUOS_LINEEDIT_BUFFER
#: MUOS_LINEEDIT_BUFFER::
#: capacity of the linedit buffer
#MUOS_CONFIG += -DMUOS_LINEEDIT_BUFFER=0
#MUOS_CONFIG += -DMUOS_LINEEDIT_BUFFER=16
#MUOS_CONFIG += -DMUOS_LINEEDIT_BUFFER=32
MUOS_CONFIG += -DMUOS_LINEEDIT_BUFFER=64
#MUOS_CONFIG += -DMUOS_LINEEDIT_BUFFER=128



#lineedit_config:MUOS_LINEEDIT_UTF8
#: MUOS_LINEEDIT_UTF8::
#: Enable UTF8 support for lineedit
#MUOS_CONFIG += -DMUOS_LINEEDIT_UTF8



# 0 = simple (only backspace)
# 1 = cursors
# 2 = history
# 3 = completion



#lineedit_config:MUOS_LINEEDIT_RECALL
#: MUOS_LINEEDIT_RECALL::
#: Enable lineedit recall
MUOS_CONFIG += -DMUOS_LINEEDIT_RECALL



#PLANNED: MUOS_LINEEDIT_HISTORY
# : buffers for history



#lineedit_config:MUOS_LINEEDIT_CALLBACK
#: MUOS_LINEEDIT_CALLBACK::
#: User defined function to call when a line from lineedit is done (return pressed)
MUOS_CONFIG += -DMUOS_LINEEDIT_CALLBACK=lineecho



#cppm_config:MUOS_CPPM
#: MUOS_CPPM::
#: Enable the CPPM Parser for Radio Control Signals
#MUOS_CONFIG += -DMUOS_CPPM


#cppm_config:MUOS_CPPM_CHANNELS
#: MUOS_CPPM_CHANNELS::
#: Number of CPPM channels
MUOS_CONFIG += -DMUOS_CPPM_CHANNELS=8
#MUOS_CONFIG += -DMUOS_CPPM_CHANNELS=16



#cppm_config:MUOS_CPPM_MIN
#: MUOS_CPPM_MIN::
#: Minimum CPPM Pulse length
MUOS_CONFIG += -DMUOS_CPPM_MIN='MUOS_CLOCK_MICROSECONDS(750)'



#cppm_config:MUOS_CPPM_MAX
#: MUOS_CPPM_MAX::
#: Maximum CPPM Pulse length, any longer pulse defines the frame start
MUOS_CONFIG += -DMUOS_CPPM_MAX='MUOS_CLOCK_MICROSECONDS(2250)'



#cppm_config:MUOS_CPPM_RAW
#: MUOS_CPPM_RAW::
#: Store the raw cppm timing data.
MUOS_CONFIG += -DMUOS_CPPM_RAW



#cppm_config:MUOS_CPPM_RAW_FILTER
#: MUOS_CPPM_RAW_FILTER::
#: Filter expression for raw channel data
MUOS_CONFIG += -D'MUOS_CPPM_RAW_FILTER(old,new)=(old + new)/2'



#cppm_config:MUOS_CPPM_COOKED
#: MUOS_CPPM_COOKED::
#: Store processed cppm data.
MUOS_CONFIG += -DMUOS_CPPM_COOKED



#cppm_config:MUOS_CPPM_COOKED_MIN
#: MUOS_CPPM_COOKED_MIN::
#: Defines the minimum signal length for cooked values.
MUOS_CONFIG += -DMUOS_CPPM_COOKED_MIN='MUOS_CLOCK_MICROSECONDS(1000)'



#cppm_config:MUOS_CPPM_COOKED_MAX
#: MUOS_CPPM_COOKED_MAX::
#: Defines the maximum signal length for cooked values
MUOS_CONFIG += -DMUOS_CPPM_COOKED_MAX='MUOS_CLOCK_MICROSECONDS(2000)'



#cppm_config:MUOS_CPPM_CAPTURE
#: MUOS_CPPM_CAPTURE::
#: Select Input-Capture driver
#TODO: derrive from clock definition
MUOS_CONFIG += -DMUOS_CPPM_CAPTURE='(1)'



# cppm_config:MUOS_CPPM_PCINT
# cppm_config:MUOS_CPPM_PCINT
#PLANNED: cppm pcint
# : Select PinChange driver
# : Pin change interrupt bit
#MUOS_CONFIG += -DMUOS_CPPM_PCINT=8



#cppm_config:MUOS_CPPM_FRAME
#: MUOS_CPPM_FRAME::
#: Length of a full CPPM frame, required for clocksync
MUOS_CONFIG += -DMUOS_CPPM_FRAME='MUOS_CLOCK_MILLISECONDS(27)'



#cppm_config:MUOS_CPPM_FRAME_CLOCKSYNC
#: MUOS_CPPM_FRAME_CLOCKSYNC::
#: Use the MUOS_CLOCK_CALIBRATE to synchronize the µC clock with
#: the CPPM frames
#MUOS_CONFIG += -DMUOS_CPPM_FRAME_CALIBRATE=0
MUOS_CONFIG += -DMUOS_CPPM_FRAME_CALIBRATE=1



#cppm_config:MUOS_CPPM_CALLBACK
#: MUOS_CPPM_CALLBACK::
#: function to be pushed to the hpq when a frame got received
MUOS_CONFIG += -DMUOS_CPPM_CALLBACK=cppm_output






# now load the main makefile
include muos/muos.mk
