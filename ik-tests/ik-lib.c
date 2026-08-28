#include<math.h>

typedef enum {
    IK_OK = 0,
    IK_COXA_OUT_OF_REACH,// Точка лежит за пределами радиуса действия ноги
    IK_FEMUR_OUT_OF_REACH,
    IK_TIBIA_OUT_OF_REACH,
    IK_ERR_SINGULARITY,  // Математический тупик (деление на ноль / вытянутая в струну нога)
    IK_ERR_INVALID_PARAM // Нулевой указатель или некорректные входные данные
} ik_status_t;


const PI_3 = 1.0471975512f;
const PI_6 = 0.52359877559f;

typedef struct{
    float X;
    float Y;
    float Z;
} vec3_float;

typedef struct{
    float coxa_servo_angle;
    float femur_servo_angle;
    float tibia_servo_angle;
} servo_angles;

typedef struct
{
    vec3_float desired_pos;//sm or mm?
    float leg_femur;//l1
    float leg_tarsus;//l2
    float servo_max_angle;//max angle +-
    float delta_coxa_femur_angle = 0.0f;
    float delta_tibia_leg_angle = 0.0f;
} servo_data;


int convert_pos_into_angles(servo_angles* angles,const servo_data* data){
    if(angles == nullptr || data == nullptr){
        return IK_ERR_INVALID_PARAM;
    }
    float alpha = atanf(data->desired_pos.Y/data->desired_pos.X);
    float l_sqr = data->desired_pos.X*data->desired_pos.X 
                        + data->desired_pos.Y*data->desired_pos.Y
    float diametr_sqr = l_sqr
                        +data->desired_pos.Z*data->desired_pos.Z;
    float gamma = acosf((data->leg_femur*data->leg_femur+data->leg_tarsus*data->leg_tarsus-diametr_sqr)
                        /2*data->leg_femur*data->leg_tarsus);
    
    float beta = atanf(z/l_sqr) + gamma;
    float alpha_real =alpha + data->delta_coxa_femur_angle;
    float gamma_real = gamma - data->delta_tibia_leg_angle;

    float max = data->servo_max_angle;
    if(alpha_real >= max || alpha_real <= -max){
        return IK_COXA_OUT_OF_REACH;
    }
    if(beta >= max || beta <= -max){
        return IK_FEMUR_OUT_OF_REACH;
    }
    if(gamma_real >= max || gamma_real <=-max){
        return IK_TIBIA_OUT_OF_REACH;
    }
    angles->coxa_servo_angle = alpha;
    angles->femur_servo_angle = beta;
    angles->tibia_servo_angle = gamma;

    return IK_OK;
} 

const char* ik_status_to_string(ik_status_t status){
    switch (status)
    {
    case IK_OK:
        return "Ok status";
    case IK_COXA_OUT_OF_REACH:
        return "Error: Coxa out of reach";
    case IK_FEMUR_OUT_OF_REACH:
        return "Error: Femur out of reach";
    case IK_TIBIA_OUT_OF_REACH:
        return "Error: Tibia out of reach";
    case IK_ERR_SINGULARITY:
        return "Errror: Arithmetic singularity";
    case IK_ERR_INVALID_PARAM:
        return "Error: Invalid parameters";
    default:
        break;
    }
}

int main(){
    
}