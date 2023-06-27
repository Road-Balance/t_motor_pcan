/* Helper */

typedef enum {
    CAN_PACKET_SET_DUTY = 0, //Duty cycle mode
    CAN_PACKET_SET_CURRENT, //Current loop mode
    CAN_PACKET_SET_CURRENT_BRAKE, // Current brake mode
    CAN_PACKET_SET_RPM, //Velocity mode
    CAN_PACKET_SET_POS, // Position mode
    CAN_PACKET_SET_ORIGIN_HERE, //Set origin mode
    CAN_PACKET_SET_POS_SPD, //Position velocity loop mode
} CAN_PACKET_ID;

void comm_can_transmit_eid(uint32_t id, const uint8_t *data, uint8_t len) {
    uint8_t i=0;
    
    if (len > 8)
        len = 8;

    CanTxMsg TxMessage;
    TxMessage.StdId = 0;
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.ExtId = id;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = len;
    
    //memcpy(txmsg.data8, data, len);
    for(i=0;i<len;i++){
        TxMessage.Data[i]=data[i];
        CAN_Transmit(CHASSIS_CAN, &TxMessage);
    }
}

void buffer_append_int32(uint8_t* buffer, int32_t number, int32_t *index) {
    buffer[(*index)++] = number >> 24;
    buffer[(*index)++] = number >> 16;
    buffer[(*index)++] = number >> 8;
    buffer[(*index)++] = number;
}

void buffer_append_int16(uint8_t* buffer, int16_t number, int16_t *index) {
    buffer[(*index)++] = number >> 8;
    buffer[(*index)++] = number;
}

/* Duty cycle mode  */
void comm_can_set_duty(uint8_t controller_id, float duty) {
    int32_t send_index = 0;
    uint8_t buffer[4];
    buffer_append_int32(buffer, (int32_t)(duty * 100000.0), &send_index);
    comm_can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_DUTY << 8), buffer, send_index);
}

/* Current loop mode */
void comm_can_set_current(uint8_t controller_id, float current) {
    int32_t send_index = 0;
    uint8_t buffer[4];
    buffer_append_int32(buffer, (int32_t)(current * 1000.0), &send_index);
    comm_can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_CURRENT << 8), buffer, send_index);
}

/* Current Brake Mode */
void comm_can_set_cb(uint8_t controller_id, float current) {
    int32_t send_index = 0;
    uint8_t buffer[4];
    buffer_append_int32(buffer, (int32_t)(current * 1000.0), &send_index);
    comm_can_transmit_eid(controller_id |
    ((uint32_t)CAN_PACKET_SET_CURRENT_BRAKE << 8), buffer, send_index);
}

/* Velocity mode */
void comm_can_set_rpm(uint8_t controller_id, float rpm) {
    int32_t send_index = 0;
    uint8_t buffer[4];
    buffer_append_int32(buffer, (int32_t)rpm, &send_index);
    comm_can_transmit_eid(controller_id |
    ((uint32_t)CAN_PACKET_SET_RPM << 8), buffer, send_index);
}

/* Position loop mode */
void comm_can_set_pos(uint8_t controller_id, float pos) {
    int32_t send_index = 0;
    uint8_t buffer[4];
    buffer_append_int32(buffer, (int32_t)(pos * 1000000.0), &send_index);
    comm_can_transmit_eid(controller_id |
    ((uint32_t)CAN_PACKET_SET_POS << 8), buffer, send_index);
}

/* Set origin mode */
void comm_can_set_origin(uint8_t controller_id, uint8_t set_origin_mode) {
    comm_can_transmit_eid(controller_id |
    ((uint32_t) CAN_PACKET_SET_ORIGIN_HERE << 8), buffer, send_index);
}

/* Position and Velocity Loop Mode */
void comm_can_set_pos_spd(uint8_t controller_id, float pos,int16_t spd, int16_t RPA ) {
    int32_t send_index = 0;
    Int16_t send_index1 = 0;
    uint8_t buffer[4];
    buffer_append_int32(buffer, (int32_t)(pos * 10000.0), &send_index);
    buffer_append_int16(buffer, spd, & send_index1);
    buffer_append_int16(buffer, RPA, & send_index1);
    comm_can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_POS << 8), buffer, send_index);
}

/* Servo mode of motor message format */
void motor_receive(float* motor_pos,float* motor_spd,float* cur,int_8* temp,int_8* error,rx_message)
{
    int16_t pos_int = (rx_message)->Data[0] << 8 | (rx_message)->Data[1]);
    int16_t spd_int = (rx_message)->Data[2] << 8 | (rx_message)->Data[3]);
    int16_t cur_int = (rx_message)->Data[4] << 8 | (rx_message)->Data[5]);
    &motor_pos= (float)( pos_int * 0.1f); //电机位置
    &motor_spd= (float)( spd_int * 10.0f);//电机速度
    &motor_cur= (float) ( cur_int * 0.01f);//电机电流
    &motor_temp= (rx_message)->Data[6] ;//电机温度
    &motor_error= (rx_message)->Data[7] ;//电机故障码
}

// MIT power mode communication protocol

/* Sends routine code */
void pack_cmd(CANMessage * msg, float p_des, float v_des, float kp, float kd, float t_ff){
    /// limit data to be within bounds ///
    float P_MIN =-95.5;
    float P_MAX =95.5;
    float V_MIN =-30;
    float V_MAX =30;
    float T_MIN =-18;
    float T_MAX =18;
    float Kp_MIN =0;
    float Kp_MAX =500;
    float Kd_MIN =0;
    float Kd_MAX =5;
    float Test_Pos=0.0;

    p_des = fminf(fmaxf(P_MIN, p_des), P_MAX);
    v_des = fminf(fmaxf(V_MIN, v_des), V_MAX);

    kp = fminf(fmaxf(Kp_MIN, kp), Kp_MAX);
    kd = fminf(fmaxf(Kd_MIN, kd), Kd_MAX);
    t_ff = fminf(fmaxf(T_MIN, t_ff), T_MAX);

    /// convert floats to unsigned ints ///
    int p_int = float_to_uint(p_des, P_MIN, P_MAX, 16);
    int v_int = float_to_uint(v_des, V_MIN, V_MAX, 12);
    int kp_int = float_to_uint(kp, KP_MIN, KP_MAX, 12);
    int kd_int = float_to_uint(kd, KD_MIN, KD_MAX, 12);
    int t_int = float_to_uint(t_ff, T_MIN, T_MAX, 12);
    
    /// pack ints into the can buffer ///
    msg->data[0] = p_int>>8; // Position 8 higher
    msg->data[1] = p_int&0xFF; // Position 8 lower
    msg->data[2] = v_int>>4; // Speed 8 higher
    msg->data[3] = ((v_int&0xF)<<4)|(kp_int>>8); // Speed 4 bit lower & KP 4bit higher
    msg->data[4] = kp_int&0xFF; // KP 8 bit lower
    msg->data[5] = kd_int>>4; // Kd 8 bit higher
    msg->data[6] = ((kd_int&0xF)<<4)|(kp_int>>8); // KP 4 bit lower torque 4 bit higher
    msg->data[7] = t_int&0xff; // torque 4 bit lower
}


int float_to_uint(float x, float x_min, float x_max, unsigned int bits)
{
    /// Converts a float to an unsigned int, given range and number of bits ///
    float span = x_max - x_min;
    
    if(x < x_min) 
        x = x_min;
    else if(x > x_max) 
        x = x_max;
    
    return (int) ((x- x_min)*((float)((1<<bits)/span)));
}

/* Receive routine code */
void unpack_reply(CANMessage msg){
    /// unpack ints from can buffer ///
    int id = msg.data[0]; //驱动 ID 号
    int p_int = (msg.data[1]<<8)|msg.data[2]; //Motor position data
    int v_int = (msg.data[3]<<4)|(msg.data[4]>>4); // Motor speed data
    int i_int = ((msg.data[4]&0xF)<<8)|msg.data[5]; // Motor torque data
    
    /// convert ints to floats ///
    float p = uint_to_float(p_int, P_MIN, P_MAX, 16);
    float v = uint_to_float(v_int, V_MIN, V_MAX, 12);
    float i = uint_to_float(i_int, -I_MAX, I_MAX, 12);

    if(id == 1){
        postion = p; //
        Read the corresponding data according to the ID code
        speed = v;
        torque = i;
    }
}

float uint_to_float(int x_int, float x_min, float x_max, int bits){
    /// converts unsigned int to float, given range and number of bits ///
    float span = x_max - x_min;
    float offset = x_min;

    return ((float)x_int)*span/((float)((1<<bits)-1)) + offset;
}