#define MUOS_ERRORS                                                                               \
  MUOS_FATAL(error) /*: {ERRORDEF} unspecified error */                                           \
    MUOS_FATAL(assert) /*: {ERRORDEF} assertion failed */                                         \
    MUOS_FATAL(stack_overflow) /*: {ERRORDEF} stack overflow */                                   \
    MUOS_WARN(sched_depth) /*: {ERRORDEF} recursive scheduler calls exceeded */                   \
    MUOS_WARN(wait_timeout) /*: {ERRORDEF} muos_wait() timed out */                               \
    MUOS_ERROR(hpt_active) /*: {ERRORDEF} one hpt is already active */                            \
    MUOS_ERROR(clpq_overflow) /*: {ERRORDEF} clock priority queue full */                         \
    MUOS_ERROR(clpq_nounique) /*: {ERRORDEF} uniqueness check failed */                           \
    MUOS_ERROR(clpq_past) /*: {ERRORDEF} time lies in the past */                                 \
    MUOS_ERROR(rtq_overflow) /*: {ERRORDEF} realtime priority queue full */                       \
    MUOS_ERROR(hpq_overflow) /*: {ERRORDEF} high priority queue full */                           \
    MUOS_ERROR(bgq_overflow) /*: {ERRORDEF} background priority queue full */                     \
    MUOS_FATAL(nodev) /*: {ERRORDEF} device index out of range */                                 \
    MUOS_ERROR(serial_status) /*: {ERRORDEF} serial port flagged an async error */                \
    MUOS_FATAL(serial_config) /*: {ERRORDEF} error in serial configuration parameters */          \
    MUOS_ERROR(tx_overflow) /*: {ERRORDEF} To much data to send */                                \
    MUOS_ERROR(rx_underflow) /*: {ERRORDEF} read while no data available */                       \
    MUOS_WARN(io_locked) /*: {ERRORDEF} io is locked by another job */                            \
    MUOS_ERROR(txqueue_overflow) /*: {ERRORDEF} To much data to send (TXQUEUE) */                 \
    MUOS_ERROR(sm_state) /*: {ERRORDEF} state transition not possible */                          \
    MUOS_ERROR(eeprom_busy) /*: {ERRORDEF} eeprom busy */                                         \
    MUOS_ERROR(eeprom_verify) /*: {ERRORDEF} eeprom verification failed */                        \
    MUOS_ERROR(configstore) /*: {ERRORDEF} configstore has a problem (check status) */            \
    MUOS_ERROR(stepper_noinit) /*: {ERRORDEF} not initialized */                                  \
    MUOS_ERROR(stepper_state) /*: {ERRORDEF} action not possible from current state */            \
    MUOS_ERROR(stepper_range) /*: {ERRORDEF} some parameter out of range */                       \
    MUOS_ERROR(stepper_noslot) /*: {ERRORDEF} no position match slot */                           \
    MUOS_ERROR(stepper_slope) /*: {ERRORDEF} slope not ready */                                   \
    MUOS_ERROR(stepper_sync) /*: {ERRORDEF} sync move not possible */                             \
    MUOS_ERROR(cppm_frame) /*: {ERRORDEF} received broken cppm frame */                           \
    MUOS_ERROR(cppm_hpq_callback) /*: {ERRORDEF} hpq overflow when pushing cppm handler */        \
    MUOS_APP_ERRORS
