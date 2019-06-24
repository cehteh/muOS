#define MUOS_ERRORS                                                                                     \
  MUOS_ERROR(error_error) /*: {ERRORDEF} unspecified error */                                           \
    MUOS_ERROR(error_assert) /*: {ERRORDEF} assertion failed */                                         \
    MUOS_ERROR(warn_sched_depth) /*: {ERRORDEF} recursive scheduler calls exceeded */                   \
    MUOS_ERROR(warn_wait_timeout) /*: {ERRORDEF} muos_wait() timed out */                               \
    MUOS_ERROR(error_clpq_overflow) /*: {ERRORDEF} clock priority queue full */                         \
    MUOS_ERROR(error_clpq_repeat) /*: {ERRORDEF} can not repeat */                                      \
    MUOS_ERROR(error_rtq_overflow) /*: {ERRORDEF} realtime priority queue full */                       \
    MUOS_ERROR(error_hpq_overflow) /*: {ERRORDEF} high priority queue full */                           \
    MUOS_ERROR(error_bgq_overflow) /*: {ERRORDEF} background priority queue full */                     \
    MUOS_ERROR(error_nodev) /*: {ERRORDEF} device index out of range */                                 \
    MUOS_ERROR(error_serial_status) /*: {ERRORDEF} serial port flagged an async error */                \
    MUOS_ERROR(error_serial_config) /*: {ERRORDEF} error in serial configuration parameters */          \
    MUOS_ERROR(error_tx_overflow) /*: {ERRORDEF} To much data to send */                                \
    MUOS_ERROR(error_rx_underflow) /*: {ERRORDEF} read while no data available */                       \
    MUOS_ERROR(warn_io_locked) /*: {ERRORDEF} io is locked by another job */                            \
    MUOS_ERROR(error_txqueue_overflow) /*: {ERRORDEF} To much data to send (TXQUEUE) */                 \
    MUOS_ERROR(error_sm_state) /*: {ERRORDEF} state transition not possible */                          \
    MUOS_ERROR(error_eeprom_busy) /*: {ERRORDEF} eeprom busy */                                         \
    MUOS_ERROR(error_eeprom_verify) /*: {ERRORDEF} eeprom verification failed */                        \
    MUOS_ERROR(error_configstore) /*: {ERRORDEF} configstore has a problem (check status) */            \
    MUOS_ERROR(error_stepper_noinit) /*: {ERRORDEF} not initialized */                                  \
    MUOS_ERROR(error_stepper_state) /*: {ERRORDEF} action not possible from current state */            \
    MUOS_ERROR(error_stepper_range) /*: {ERRORDEF} some parameter out of range */                       \
    MUOS_ERROR(error_stepper_noslot) /*: {ERRORDEF} no position match slot */                           \
    MUOS_ERROR(error_stepper_slope) /*: {ERRORDEF} slope not ready */                                   \
    MUOS_ERROR(error_stepper_sync) /*: {ERRORDEF} sync move not possible */                             \
    MUOS_ERROR(error_cppm_frame) /*: {ERRORDEF} received broken cppm frame */                           \
    MUOS_ERROR(error_cppm_hpq_callback) /*: {ERRORDEF} hpq overflow when pushing cppm handler */        \
    MUOS_APP_ERRORS
