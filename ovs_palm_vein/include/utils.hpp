#pragma once

enum ProcessingState
{
    OVS_SUCCESS = 0,
    OVS_WAITING_DATA = 1,
    OVS_INIT = 2,
    OVS_NOT_READY = 3,
    OVS_ERR_TIME_OUT = -1,
    OVS_ERR_INVALID_PARAM = -10,
    OVS_ERR_NO_FOUND_PALM = -100,
    OVS_ERR_GET_ROI_FAILED = -101,
    OVS_ERR_ROI_TOOWEAK = -102,
    OVS_ERR_ROI_TOOSTRONG = -103,
    OVS_ERR_ROI_INVALID_POSITION = -104,
    OVS_ERR_GET_FEATURE_FAILED = -105,
    OVS_ERR_MATCHING_FAILED = -106,
    OVS_ERR_MATCHING_TIMEOUT = -107
};

struct PalmImageInfo
{
public:
    float center_x = 0;
    float center_y = 0;
    float score = 0;
    float mean_illumination = 0;
};