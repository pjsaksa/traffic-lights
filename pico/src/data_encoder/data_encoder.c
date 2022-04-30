#include "data_encoder.h"

#define ENC_RESPONSE_ID_UNUSED              (0)
#define ENC_RESPONSE_ID_LANE_STATES         (1)
#define ENC_RESPONSE_ID_PARAMETER_VALUE     (2)

#define ENC_LANE_STATE_NONE                 (0)
#define ENC_LANE_STATE_STOP                 (1)
#define ENC_LANE_STATE_TO_GO                (2)
#define ENC_LANE_STATE_GO                   (3)
#define ENC_LANE_STATE_TO_STOP              (4)
#define ENC_LANE_STATE_OUT_OF_ORDER         (5)

#define ENC_RIGHT_LANE_INDEX                (0)
#define ENC_CENTER_LANE_INDEX               (1)
#define ENC_LEFT_LANE_INDEX                 (2)

#define ENC_NO_CARS_ON_LANE                 (0)
#define ENC_CARS_ON_LANE                    (1)

static uint8_t encode_lane_state(response_lane_state_t state, bool cars_on_lane)
{
    uint8_t encoded_cars = cars_on_lane ? ENC_CARS_ON_LANE : ENC_NO_CARS_ON_LANE;
    uint8_t encoded_state = ENC_LANE_STATE_NONE;

    switch (state)
    {
    case RESPONSE_LANE_STATE_UNKNOWN:
    case RESPONSE_LANE_STATE_NONE:
        encoded_state = ENC_LANE_STATE_NONE;
        break;
    case RESPONSE_LANE_STATE_STOP:
        encoded_state = ENC_LANE_STATE_STOP;
        break;
    case RESPONSE_LANE_STATE_TO_GO:
        encoded_state = ENC_LANE_STATE_TO_GO;
        break;
    case RESPONSE_LANE_STATE_GO:
        encoded_state = ENC_LANE_STATE_GO;
        break;
    case RESPONSE_LANE_STATE_TO_STOP:
        encoded_state = ENC_LANE_STATE_TO_STOP;
        break;
    case RESPONSE_LANE_STATE_OUT_OF_ORDER:
        encoded_state = ENC_LANE_STATE_OUT_OF_ORDER;
        break;
    default:
        break;
    }

    return encoded_cars << 4 | encoded_state;
}

static raw_response_t encode_lane_states_response(response_t response)
{
    raw_response_t raw_response = { .response_id = ENC_RESPONSE_ID_LANE_STATES };

    uint8_t* data = (uint8_t*)&raw_response.data;

    data[ENC_RIGHT_LANE_INDEX] =
        encode_lane_state(response.lane_states.right_lane_state,
                          response.lane_states.right_lane_cars_on_lane);

    data[ENC_CENTER_LANE_INDEX] =
        encode_lane_state(response.lane_states.center_lane_state,
                          response.lane_states.center_lane_cars_on_lane);

    data[ENC_LEFT_LANE_INDEX] =
        encode_lane_state(response.lane_states.left_lane_state,
                          response.lane_states.left_lane_cars_on_lane);

    return raw_response;
}

raw_response_t data_encoder_encode_response(uint8_t frame_id,
                                            response_t response)
{
    raw_response_t raw_response = {.response_id = ENC_RESPONSE_ID_UNUSED};

    if (response.id == RESPONSE_ID_LANE_STATES)
    {
        raw_response = encode_lane_states_response(response);
    }

    raw_response.frame_id = frame_id;

    return raw_response;
}
