#ifndef msp_protocol_h
#define msp_protocol_h

#define MSP_FC_VARIANT_SIZE  4
#define MSP_RC_CHANNEL_COUNT 16

#define MSP_FC_VARIANT 2   // out message
#define MSP_VTX_CONFIG 88  // out message         Get vtx settings - betaflight
#define MSP_STATUS     101 // out message         cycletime & errors_count & sensor present & box activation & current setting number
#define MSP_RC         105 // out message         rc channels and more

#define MSP_DISPLAYPORT 182

#define MSP_DP_SUBCMD_HEARTBEAT 0
#define MSP_DP_SUBCMD_RELEASE   1
#define MSP_DP_SUBCMD_CLEAR     2
#define MSP_DP_SUBCMD_WRITE     3
#define MSP_DP_SUBCMD_DRAW      4
#define MSP_DP_SUBCMD_CONFIG    5

struct MSPVTXConfig {
    uint8_t vtx_type;
    uint8_t band;
    uint8_t channel;
    uint8_t power;
    uint8_t pitmode;
    uint8_t freq_lsb;
    uint8_t freq_msb;
    uint8_t device_is_ready;
    uint8_t low_power_disarm;
    uint8_t pit_mode_freq_lsb;
    uint8_t pit_mode_freq_msb;
    uint8_t vtx_table_available;
    uint8_t bands;
    uint8_t channels;
    uint8_t power_levels;
};

#endif