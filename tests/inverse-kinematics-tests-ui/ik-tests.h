#ifndef IK_LIB_H
#define IK_LIB_H

#include <math.h>


// Enum for Inverse Kinematics status codes
typedef enum {
    IK_OK = 0,
    IK_COXA_OUT_OF_REACH, 
    IK_FEMUR_OUT_OF_REACH,
    IK_TIBIA_OUT_OF_REACH,
    IK_ERR_SINGULARITY,// Arithmetic singularity
    IK_ERR_INVALID_PARAM // Invalid parameters
} ik_status_t;

const float PI_3 = 1.0471975512f;
const float PI_6 = 0.52359877559f;

// Structs for 3D vector, servo angles, and servo data
typedef struct {
    float X;
    float Y;
    float Z;
} vec3_float;

typedef struct {
    float coxa_servo_angle;
    float femur_servo_angle;
    float tibia_servo_angle;
} servo_angles;


// servo angles limit - max angle()
typedef struct {
    vec3_float desired_pos;
    float leg_femur;
    float leg_tarsus;


    float servo_coxa_max_angle_rad;
    float servo_femur_max_angle_rad;
    float servo_tibia_max_angle_rad;    
    
    float delta_coxa_femur_angle = 0.0f;// Angle offset between coxa and femur
    float delta_tibia_leg_angle = 0.0f;// Angle offset between tibia and leg
    
    float coxa_angle_offset = 0.0f;
    float femur_angle_offset = 0.0f;
    float tibia_angle_offset = 0.0f;
} leg_data;



static inline int convert_pos_into_angles(servo_angles* angles, const leg_data* data) {
    if (angles == nullptr || data == nullptr) {
        return IK_ERR_INVALID_PARAM;
    }
    
    float alpha = atan2f(data->desired_pos.Y, data->desired_pos.X);
    
    float r = sqrtf(data->desired_pos.X * data->desired_pos.X + data->desired_pos.Y * data->desired_pos.Y);
    float z = data->desired_pos.Z;

    float d_sqr = r * r + z * z;
    float d = sqrtf(d_sqr);

    float l1 = data->leg_femur;
    float l2 = data->leg_tarsus;

    // Вычисление угла между звеньями (gamma)
    float cos_gamma_inner = (l1 * l1 + l2 * l2 - d_sqr) / (2.0f * l1 * l2);
    if (cos_gamma_inner < -1.0f || cos_gamma_inner > 1.0f) {
        return IK_ERR_SINGULARITY;
    }
    float gamma = acosf(cos_gamma_inner);

    float cos_beta_1 = (l2 * l2 - l1 * l1 - d_sqr) / (2.0f * l1 * d);
    if (cos_beta_1 < -1.0f || cos_beta_1 > 1.0f) {
        return IK_ERR_SINGULARITY;
    }
    float beta_1 = acosf(cos_beta_1);
    float beta_2 = atan2f(z, r);
    float beta = beta_2 + beta_1;

    angles->coxa_servo_angle = alpha;
    angles->femur_servo_angle = beta;
    angles->tibia_servo_angle = gamma;

    return IK_OK;
} 

//convert math model angles into servo angles with offsets and check for limits
static inline int apply_angle_offsets(servo_angles* angles, const leg_data* data) {
    if (angles == nullptr || data == nullptr) {
        return IK_ERR_INVALID_PARAM;
    }
    float alpha = angles->coxa_servo_angle;
    float beta = angles->femur_servo_angle;
    float gamma = angles->tibia_servo_angle;
    

    float alpha_servo = alpha + data->coxa_angle_offset + data->delta_coxa_femur_angle;;
    if(alpha_servo < -data->servo_coxa_max_angle_rad/2 || alpha_servo > data->servo_coxa_max_angle_rad/2) {
        return IK_COXA_OUT_OF_REACH;
    }

//вот тут хз + или -
    float beta_servo = beta - data->femur_angle_offset; 
    if(beta_servo < -data->servo_femur_max_angle_rad/2 || beta_servo > data->servo_femur_max_angle_rad/2) {
        return IK_FEMUR_OUT_OF_REACH;
    }

    float gamma_servo = gamma - data->delta_tibia_leg_angle - data->tibia_angle_offset;
    if(gamma_servo < -data->servo_tibia_max_angle_rad/2 || gamma_servo > data->servo_tibia_max_angle_rad/2) {
        return IK_TIBIA_OUT_OF_REACH;
    }

    angles->coxa_servo_angle = alpha_servo;
    angles->femur_servo_angle = beta_servo;
    angles->tibia_servo_angle = gamma_servo;

    return IK_OK;
}

static inline int convert_angles_into_pos(vec3_float* pos, const servo_angles* angles, const leg_data* data) {
    if (pos == nullptr || angles == nullptr || data == nullptr) {
        return IK_ERR_INVALID_PARAM;
    }
    float alpha = angles->coxa_servo_angle - data->delta_coxa_femur_angle;
    float beta = angles->femur_servo_angle;
    float gamma = angles->tibia_servo_angle + data->delta_tibia_leg_angle;

    float l1 = data->leg_femur;
    float l2 = data->leg_tarsus;

    float r = l1 * cosf(beta) + l2 * cosf(beta - gamma);
    float z = l1 * sinf(beta) + l2 * sinf(beta - gamma);

    pos->X = r * cosf(alpha);
    pos->Y = r * sinf(alpha);
    pos->Z = z;

    return IK_OK;
}

static inline int convert_to_global_coords(vec3_float* global_pos, const vec3_float* local_pos, const vec3_float* leg_base_delta, const float leg_angle) {
    if (global_pos == nullptr || local_pos == nullptr || leg_base_delta == nullptr) {
        return IK_ERR_INVALID_PARAM;
    }
    float cos = cosf(leg_angle);
    float sin = sinf(leg_angle);

    global_pos->X = (local_pos->X + leg_base_delta->X) * cos - (local_pos->Y + leg_base_delta->Y) * sin;
    global_pos->Y = (local_pos->X + leg_base_delta->X) * sin + (local_pos->Y + leg_base_delta->Y) * cos;
    global_pos->Z = local_pos->Z + leg_base_delta->Z;

    return IK_OK;
}

static inline int convert_to_local_pos(const vec3_float* global_pos, vec3_float* local_pos, const vec3_float* leg_base_delta, const float leg_angle) {
    if (global_pos == nullptr || local_pos == nullptr || leg_base_delta == nullptr) {
        return IK_ERR_INVALID_PARAM;
    }
    float cos = cosf(leg_angle);
    float sin = sinf(leg_angle);

    local_pos->X = (global_pos->X - leg_base_delta->X) * cos + (global_pos->Y - leg_base_delta->Y) * sin;
    local_pos->Y = -(global_pos->X - leg_base_delta->X) * sin + (global_pos->Y - leg_base_delta->Y) * cos;
    local_pos->Z = global_pos->Z - leg_base_delta->Z;

    return IK_OK;
}

static inline int conversion_pipeline(servo_angles* angles, const leg_data* data) {
    if (angles == nullptr || data == nullptr) {
        return IK_ERR_INVALID_PARAM;
    }

    // int res = convert_to_local_pos(global_pos, local_pos, leg_base_delta, leg_angle);
    // if (res != IK_OK) {
    //     return res;
    // }

    int res = convert_pos_into_angles(angles, data);
    if (res != IK_OK) {
        return res;
    }

    res = apply_angle_offsets(angles, data);
    if (res != IK_OK) {
        return res;
    }

    return IK_OK;
}

static inline const char* ik_status_to_string(ik_status_t status) {
    switch (status) {
    case IK_OK:
        return "Ok status";
    case IK_COXA_OUT_OF_REACH:
        return "Error: Coxa out of reach";
    case IK_FEMUR_OUT_OF_REACH:
        return "Error: Femur out of reach";
    case IK_TIBIA_OUT_OF_REACH:
        return "Error: Tibia out of reach";
    case IK_ERR_SINGULARITY:
        return "Error: Arithmetic singularity";
    case IK_ERR_INVALID_PARAM:
        return "Error: Invalid parameters";
    default:
        return "Unknown error";
    }
}

#endif
