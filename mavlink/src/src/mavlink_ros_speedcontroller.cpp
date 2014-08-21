#include "ros/ros.h"
#include "std_msgs/String.h"
#include "mavlink/Mavlink.h"
#include "mavlink.h"
#include <sstream>
#include "sensor_msgs/Joy.h"
#include "std_msgs/Float32MultiArray.h"
#include <dynamic_reconfigure/server.h>
#include <mavlink/p_adjustConfig.h>

#define ARM 1
#define DISARM 0
#define RC_OFFSET 1500
#define RC_INCREMENT 500

#define CH3_OFFSET 1550

#define JOY_BUTTON_A      0
#define JOY_BUTTON_B      1
#define JOY_BUTTON_Y	  3
#define JOY_BUTTON_X	  2

#define JOY_AXES_YAW      0
#define JOY_AXES_THRUST   1
#define JOY_AXES_ROLL     3
#define JOY_AXES_PITCH    4

#define JOY_BINARY_PAD    7
#define JOY_RIGHT_BACK_BUTTON 5
#define JOY_LEFT_BACK_BUTTON 4

#define MODE_ALTITUDE 1500
#define MODE_AUTO 2000

///////////CUSTOM MODES////////
#define HOLD_ALTITUDE 1
#define MANUAL 0
//////////////////////////////
#define RC6_ARM   2000

//#define ALTITUDE_KP_UP 0.06
//#define ALTITUDE_KP_DOWN 0.02


#define SETPOINT_SPEED -0.5 //    m/sec

//history
//0.05= good but oscillates
//0.001 still up
//0.2 too hight


int seq = 0;
bool arm_sent = false;
bool got_heartbeat = false;
bool show_battery = false;

mavlink::Mavlink create_heartbeat_msg(int seq);
mavlink::Mavlink create_arm_msg(bool);
mavlink::Mavlink create_rc_override_msg(int, int, int, int);
mavlink::Mavlink create_manual_control_msg(int x, int y, int z, int r, int buttons);


bool ch_reset = false;
int mode = MANUAL;

ros::Publisher chatter_pub;
ros::Publisher plot_pub;
ros::Subscriber joy_sub;

float p_up = 0;
float p_down = 0;


void callback(mavlink::p_adjustConfig &config, uint32_t level) {
	ROS_WARN("Update p_up: %f, p-down: %f", config.p_up, config.p_down);
	p_up = config.p_up;
	p_down = config.p_down;
}

void joyCallback(const sensor_msgs::Joy::ConstPtr& joy){
	mavlink::Mavlink msg_out;
	bool button_press=false;
	static bool last_button_press = false;
	int ch1 = 0;
	int ch2 = 0;
	static int ch3 = 0; //changed to 1700 from 1000
	int ch4 = 0;

	if(joy->buttons[JOY_BUTTON_B]){
		show_battery = true;
		button_press = true;
	}

	if(joy->buttons[JOY_RIGHT_BACK_BUTTON]){ // Button RB pushed
		ROS_INFO("JOY -> armed");
		msg_out = create_arm_msg(ARM);
		button_press = true;
	}

	if(joy->buttons[JOY_LEFT_BACK_BUTTON]){ //Button LB pushed
		ROS_INFO("JOY -> disarmed");
		msg_out = create_arm_msg(DISARM);
		button_press = true;
	}


	if(joy->axes[JOY_BINARY_PAD] == 1){ //up
		ROS_INFO("upp");
		ch3+=100;
	}

	if(joy->axes[JOY_BINARY_PAD] == -1){ //down
		ROS_INFO("down");
		ch3-=100;
	}


	if(joy->buttons[JOY_BUTTON_Y]){
		ch_reset = true;
		mode = MANUAL;
	}

	if(joy->buttons[JOY_BUTTON_A]){
		ch_reset = false;
	}

	if(joy->buttons[JOY_BUTTON_X]){
		mode =  HOLD_ALTITUDE;
	}


	chatter_pub.publish(msg_out);

	if(button_press == last_button_press){ //if a button has been pressd, don't send RC

		ch1 = RC_OFFSET + joy->axes[JOY_AXES_ROLL]*RC_INCREMENT;
		ch2 = RC_OFFSET + joy->axes[JOY_AXES_PITCH]*RC_INCREMENT;
//		ch3 = 1000 + joy->axes[JOY_AXES_THRUST]*1000;
		ch4 = RC_OFFSET + joy->axes[JOY_AXES_YAW]*RC_INCREMENT;

		if(ch3 <= RC_OFFSET) ch3 = RC_OFFSET;
		if(ch3 >= 2000) ch3 = 2000;

//Setting channels to zero to keep control to the transmitter
		ch1 = ch2 = ch4 = 0;

		mavlink::Mavlink msg_out = create_rc_override_msg(ch1, ch2, ch3, ch4);
		chatter_pub.publish(msg_out);
	}

	last_button_press = button_press;

}


void chatterCallback(const mavlink::Mavlink::ConstPtr& msg){
	static bool altitude_locked = false;
	static int setpoint_altitude = 0;

	mavlink_message_t mav_msg;


	mav_msg.len = msg->len;
	mav_msg.seq = msg->seq;
	mav_msg.sysid = msg->sysid;
	mav_msg.compid = msg->compid;
	mav_msg.msgid = msg->msgid;

	copy(msg->payload64.begin(), msg->payload64.end(), mav_msg.payload64);

	switch(msg->msgid){
		case MAVLINK_MSG_ID_HEARTBEAT:
       		{
			mavlink::Mavlink msg_out = create_heartbeat_msg(msg->seq);
			chatter_pub.publish(msg_out);


			mavlink_heartbeat_t heartbeat;
			mavlink_msg_heartbeat_decode(&mav_msg, &heartbeat);


			if(heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED){
				ROS_INFO("System armed");
//				drone_armed = true;
			} else {
				ROS_INFO("System not armed");
			}
	  	}
	    	break;

//		case MAVLINK_MSG_ID_SYSTEM_TIME:
//			ROS_INFO("Recv: System time");
//		break;

//		case MAVLINK_MSG_ID_RAW_IMU :
//			ROS_INFO("Recv: Raw IMU");
//		break;

		case MAVLINK_MSG_ID_SYS_STATUS:
		{
			mavlink_sys_status_t sys_status;
			mavlink_msg_sys_status_decode(&mav_msg, &sys_status);

//			ROS_INFO("Recv: SYS status");
			if(show_battery){
				show_battery = false;
				ROS_INFO("Battery remaining: %d %%", sys_status.battery_remaining);
			}
		}
		break;

		case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
		{
			mavlink_global_position_int_t global_position;
			mavlink_msg_global_position_int_decode(&mav_msg, &global_position);


			int current_altitude = global_position.alt;
			float error = 0;
			float current_speed = 0;
			static float last_altitude = 0;
			static ros::Time time_prev = ros::Time::now();

			if(mode == HOLD_ALTITUDE){

				if(setpoint_altitude == 0){
					setpoint_altitude = current_altitude;
					ROS_WARN("altitude locked");
				}

//				error = setpoint_altitude - current_altitude;
//				int ch3 = 0;
//				if(error > 0 ){
//					ch3 = 1550 + (error*p_up); //change to 1700
//				} else {
//					ch3 = 1550 + (error*p_down); //change to 1700
//				}
                                ros::Time time_latest = ros::Time::now();
                                double dt = (time_latest - time_prev).toSec();

//				ROS_INFO("dt: %f",dt);
				ROS_INFO("distance: %f", current_altitude - last_altitude);
				current_speed = (current_altitude - last_altitude)/dt;
				error = SETPOINT_SPEED - current_speed;
//				ROS_INFO("speed error : %d", error);

				ROS_WARN("Setpoint: %f, Error: %f, current_speed: %f", SETPOINT_SPEED, error, current_speed);


				int ch3 = 0;
				if(error > 0 ){
					ch3 = CH3_OFFSET + (error*p_up); //change to 1700
				} else {
					ch3 = CH3_OFFSET + (error*p_down); //change to 1700
				}

		                if(ch3 <= 1000) ch3 = 1000; //changed from 1500 to 1000
		                if(ch3 >= 2000) ch3 = 2000;
//				ROS_WARN("ch3: %d", ch3);

		                mavlink::Mavlink msg_out = create_rc_override_msg(0, 0, ch3, 0);
		                chatter_pub.publish(msg_out);

				last_altitude = current_altitude;
				time_prev = time_latest;
			}

			//plot
			std_msgs::Float32MultiArray plot_out;
			plot_out.data.push_back(SETPOINT_SPEED); // setpoint
			plot_out.data.push_back(error); //error
			plot_out.data.push_back(current_speed); //current speed
			plot_pub.publish(plot_out);



			ROS_INFO("Altitude: %d", global_position.alt);
		}
		break;
	}
}

mavlink::Mavlink create_manual_control_msg(int x, int y, int z, int r, int buttons){
	mavlink_message_t msg_first = {0};
	mavlink::Mavlink msg_out;


	ROS_INFO("Joy-> CH1 %i, CH2 %i, CH3 %i, CH4 %i", x, y, z, r);


	msg_out.seq=seq++;
	msg_out.len=MAVLINK_MSG_ID_MANUAL_CONTROL_LEN;
	msg_out.sysid=255;
	msg_out.compid=0;
	msg_out.msgid=MAVLINK_MSG_ID_MANUAL_CONTROL;
	msg_out.fromlcm=false;

	mavlink_manual_control_t packet;

	packet.target = 255;
	packet.x = x;
	packet.y = y;
	packet.z = z;
	packet.r = r;
	packet.buttons = buttons;

	memcpy(_MAV_PAYLOAD_NON_CONST(&msg_first), &packet, 33);

	for(int i = 0; i < 33; ++i){
		msg_out.payload64.push_back(msg_first.payload64[i]);
	}
	return msg_out;
}


mavlink::Mavlink create_rc_override_msg(int RC1_parm, int RC2_parm, int RC3_parm, int RC4_parm){
	 int RC1 = 0;
	 int RC2 = 0;
	 int RC3 = 0;
	 int RC4 = 0;
	 int RC5 = MODE_ALTITUDE;
	 int RC6 = RC6_ARM;
	 int RC7 = 0;
	 int RC8 = 0;


	if(RC1_parm != NULL) RC1 = RC1_parm;
	if(RC2_parm != NULL) RC2 = RC2_parm;
	if(RC3_parm != NULL) RC3 = RC3_parm;
	if(RC4_parm != NULL) RC4 = RC4_parm;




	ROS_INFO("RC_override sent");
	mavlink_message_t msg_first = {0};
	mavlink::Mavlink msg_out;

	msg_out.seq=seq++;
	msg_out.len=MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN;
	msg_out.sysid=255;
	msg_out.compid=0;
	msg_out.msgid=MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE;
	msg_out.fromlcm=false;


        if(ch_reset){
        	RC1 = RC2 = RC3 = RC4 = RC5 = RC6 = RC7 = RC8 = 0;
        }

	ROS_INFO("Joy-> CH1 %i, CH2 %i, CH3 %i, CH4 %i", RC1, RC2, RC3, RC4);


	mavlink_rc_channels_override_t packet;
	packet.chan1_raw = RC1;
	packet.chan2_raw = RC2;
	packet.chan3_raw = RC3;
	packet.chan4_raw = RC4;
	packet.chan5_raw = RC5;
	packet.chan6_raw = RC6;
	packet.chan7_raw = RC7;
	packet.chan8_raw = RC8;
	packet.target_system = 1;
	packet.target_component = 0;

	memcpy(_MAV_PAYLOAD_NON_CONST(&msg_first), &packet, 33);

	for(int i = 0; i < 33; ++i){
		msg_out.payload64.push_back(msg_first.payload64[i]);
	}

//	if(ch3 >= 2000 || ch3 <= 1000){
//		increment = -increment;
//	}


	return msg_out;

}

mavlink::Mavlink create_arm_msg(bool ARM_state){


	mavlink_message_t msg_first = {0};

	mavlink::Mavlink msg_out;

	ROS_INFO("ARM sent");
	msg_out.seq=seq++;
	msg_out.len=MAVLINK_MSG_ID_COMMAND_LONG_LEN;
	msg_out.sysid=255;
	msg_out.compid=0;
	msg_out.msgid=MAVLINK_MSG_ID_COMMAND_LONG;
	msg_out.fromlcm=false;


	for(int i =0; i < 33; ++i){
		msg_first.payload64[i] =0;
	}

	mavlink_command_long_t packet;
	packet.param1 = ARM_state == ARM ? 1 : 0;
	packet.param2 = 0;
	packet.param3 = 0;
	packet.param4 = 0;
	packet.param5 = 0;
	packet.param6 = 0;
	packet.param7 = 0;
	packet.command = 400;
	packet.target_system = 1;
	packet.target_component = 250;
	packet.confirmation = 0;

	memcpy(_MAV_PAYLOAD_NON_CONST(&msg_first), &packet, 33);

	for(int i = 0; i < 33; ++i){
		msg_out.payload64.push_back(msg_first.payload64[i]);
//		std::cout << i << ": " <<  msg_first.payload64[i] << "\n";
	}
	return msg_out;

}

mavlink::Mavlink create_heartbeat_msg(int seq_heartbeat){
	static int heartbeat_seq = 0;
	mavlink_message_t msg_first = {0};

	mavlink::Mavlink msg_out;

	ROS_INFO("Heartbeat sent");
//	std::cout << "seq:"  << seq << std::endl;
	msg_out.seq=heartbeat_seq++;
	msg_out.len=MAVLINK_MSG_ID_HEARTBEAT_LEN;
	msg_out.sysid=255;
	msg_out.compid=0;
	msg_out.msgid=MAVLINK_MSG_ID_HEARTBEAT;
	msg_out.fromlcm=false;


	for(int i =0; i < 33; ++i){
		msg_first.payload64[i] =0;
	}

	mavlink_heartbeat_t packet;
	packet.custom_mode = 0;
	packet.type = MAV_TYPE_GCS;
	packet.autopilot = MAV_AUTOPILOT_INVALID;
	packet.base_mode = 0;
	packet.system_status = 0;
	packet.mavlink_version = 3;

	memcpy(_MAV_PAYLOAD_NON_CONST(&msg_first), &packet, 33);

	for(int i = 0; i < 33; ++i){
		msg_out.payload64.push_back(msg_first.payload64[i]);
	}
	got_heartbeat = true;
	return msg_out;
}




int main(int argc, char **argv){
  ros::init(argc, argv, "send_command");
  ros::NodeHandle n;

  chatter_pub = n.advertise<mavlink::Mavlink>("/mavlink/to", 1000);

  plot_pub = n.advertise<std_msgs::Float32MultiArray>("/fmInformation/error", 1000);
  joy_sub = n.subscribe("/joy", 1000, joyCallback);
  ros::Subscriber sub = n.subscribe("/mavlink/from", 1000, chatterCallback);
  dynamic_reconfigure::Server<mavlink::p_adjustConfig> server;
  dynamic_reconfigure::Server<mavlink::p_adjustConfig>::CallbackType f;

  f = boost::bind(&callback, _1, _2);
  server.setCallback(f);

  ros::Rate loop_rate(1);

  int counter = 0;

  ros::spin();
  return 0;
}





