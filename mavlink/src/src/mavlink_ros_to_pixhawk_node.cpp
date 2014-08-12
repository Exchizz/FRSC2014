#include "ros/ros.h"
#include "std_msgs/String.h"
#include "mavlink/Mavlink.h"
#include "mavlink.h"
#include <sstream>
#include "sensor_msgs/Joy.h"

#define ARM 1
#define DISARM 0
#define RC_OFFSET 1500
#define RC_INCREMENT 500

#define JOY_BUTTON_B      1

#define JOY_AXES_YAW      0
#define JOY_AXES_THRUST   1
#define JOY_AXES_ROLL     3
#define JOY_AXES_PITCH    4

#define JOY_RIGHT_BACK_BUTTON 5
#define JOY_LEFT_BACK_BUTTON 4

int seq = 0;
bool arm_sent = false;
bool got_heartbeat = false;

mavlink::Mavlink create_heartbeat_msg(int seq);
mavlink::Mavlink create_arm_msg(bool);
mavlink::Mavlink create_rc_override_msg(int, int, int, int);

ros::Publisher chatter_pub;
ros::Subscriber joy_sub;
bool show_battery = false;

void joyCallback(const sensor_msgs::Joy::ConstPtr& joy){
	mavlink::Mavlink msg_out;
	bool button_press=false;
	static bool last_button_press = false;
	int ch1 = 0;
	int ch2 = 0;
	int ch3 = 0;
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

	chatter_pub.publish(msg_out);

	if(button_press == last_button_press){ //if a button has been pressd, don't send RC

		ch1 = RC_OFFSET + joy->axes[JOY_AXES_ROLL]*RC_INCREMENT;
		ch2 = RC_OFFSET + joy->axes[JOY_AXES_PITCH]*RC_INCREMENT;
		ch3 = RC_OFFSET + joy->axes[JOY_AXES_THRUST]*RC_INCREMENT;
		ch4 = RC_OFFSET + joy->axes[JOY_AXES_YAW]*RC_INCREMENT;

		ROS_INFO("Joy-> CH1 %i, CH2 %i, CH3 %i, CH4 %i", ch1, ch2, ch3, ch4);

		mavlink::Mavlink msg_out = create_rc_override_msg(ch1, ch2, ch3, ch4);
		chatter_pub.publish(msg_out);
	}

	last_button_press = button_press;
        //joy->axes[0]; yaw
	//joy->axes[1]; thrust
//        ch3 = joy->axes[1];
//        if(x_axis > 0.755){
//                x_axis = 0.755;
//        }

}


/*
PS3frobyte::PS3frobyte(){
        deadman_button.data = false;
        z_axis = 0;
        x_axis = 0;

        sub = n.subscribe("/joy", 1000, &PS3frobyte::joyCallback, this);
        twist_pub_ = n.advertise<geometry_msgs::TwistStamped>("/fmSignals/cmd_vel", 1000);
        deadman_pub_ = n.advertise<std_msgs::Bool>("/fmSignals/deadman", 1000);
}

void PS3frobyte::updateDeadman(){
        //called every 0.1 sec to publish deadman
        deadman_pub_.publish(deadman_button);
}

void PS3frobyte::updateVel(){
        //called every 0.1 sec to publish cmd_vel
        twist.twist.angular.z = z_axis;
        twist.twist.linear.x = x_axis;

        std::cout << "x: " << x_axis << "\t";
        std::cout << "z: " << z_axis << "\n";
        twist_pub_.publish(twist);
}
*/



void chatterCallback(const mavlink::Mavlink::ConstPtr& msg){


	mavlink_message_t mav_msg;


	mav_msg.len = msg->len;
	mav_msg.seq = msg->seq;
	mav_msg.sysid = msg->sysid;
	mav_msg.compid = msg->compid;
	mav_msg.msgid = msg->msgid;

	copy(msg->payload64.begin(), msg->payload64.end(), mav_msg.payload64);

//	for(int i = 0; i < 33; ++i){
//		std::cout << "our: " << i << ": " << mav_msg.payload64[i] << std::endl;
//	}


//	mavlink::Mavlink rosmavlink_msg;

//	rosmavlink_msg.len = message.len;
//	rosmavlink_msg.seq = message.seq;
//	rosmavlink_msg.sysid = message.sysid;
//	rosmavlink_msg.compid = message.compid;
//	rosmavlink_msg.msgid = message.msgid;

//	for (int i = 0; i < message.len / 8; i++){
//        	(rosmavlink_msg.payload64).push_back(message.payload64[i]);
//	}

	/**
	  * Send the received MAVLink message to ROS (topic: mavlink, see main())
	*/
//	mavlink_pub.publish(rosmavlink_msg);


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
/*
		case MAVLINK_MSG_ID_MISSION_CURRENT:
//			ROS_INFO("Recv: Mission current");
		break;

		case MAVLINK_MSG_ID_GPS_RAW_INT:
//			ROS_INFO("Recv: GPS Raw int");
		break;
		case  MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT:
//			ROS_INFO("Recv: NAV controller output");
		break;
		case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
//			ROS_INFO("Recv: global postion int");
		break;
		case MAVLINK_MSG_ID_RC_CHANNELS_RAW:
//			ROS_INFO("Recv: RC channels raw");
		break;
		case MAVLINK_MSG_ID_SERVO_OUTPUT_RAW:
//			ROS_INFO("Recv: Server output raw");
		break;
		case MAVLINK_MSG_ID_ATTITUDE:
//			ROS_INFO("Recv: attitude");
		break;
		case MAVLINK_MSG_ID_VFR_HUD:
//			ROS_INFO("Recv: vfr_hud");
		break;
		case MAVLINK_MSG_ID_SCALED_PRESSURE:
//			ROS_INFO("Recv: scaled pressure");
		break;



		default:
//			ROS_INFO("%s %i", "Recv: Unknown command. msgid: ",msg->msgid);
		break;
*/

//		for(int i = 0; i < msg->payload64.size(); ++i){
//			std::cout << "parm " << i << ": " << msg->payload64[i] << std::endl;
//		}

	}

}
mavlink::Mavlink create_rc_override_msg(int RC1_parm, int RC2_parm, int RC3_parm, int RC4_parm){
	static int RC1 = 1000;
	static int RC2 = 1000;
	static int RC3 = 1000;
	static int RC4 = 1000;

	if(RC1_parm != NULL) RC1 = RC1_parm;
	if(RC2_parm != NULL) RC2 = RC2_parm;
	if(RC3_parm != NULL) RC3 = RC3_parm;
	if(RC4_parm != NULL) RC4 = RC4_parm;



//	static int ch3 = 1000;
//	static int increment = 200;

	ROS_INFO("RC_override sent");
//	std::cout << "ch3 value: " << ch3 << std::endl;
	mavlink_message_t msg_first = {0};
	mavlink::Mavlink msg_out;

	msg_out.seq=seq++;
	msg_out.len=MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN;
	msg_out.sysid=255;
	msg_out.compid=0;
	msg_out.msgid=MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE;
	msg_out.fromlcm=false;

	mavlink_rc_channels_override_t packet;
	packet.chan1_raw = RC1;
	packet.chan2_raw = RC2;
	packet.chan3_raw = RC3;
	packet.chan4_raw = RC4;
	packet.chan5_raw = 1000;
	packet.chan6_raw = 1000;
	packet.chan7_raw = 1000;
	packet.chan8_raw = 1000;
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
  joy_sub = n.subscribe("/joy", 1000, joyCallback);
  ros::Subscriber sub = n.subscribe("/mavlink/from", 1000, chatterCallback);
  ros::Rate loop_rate(1);

  int counter = 0;

//  std::cout << "Waiting...5 sec" << std::endl;
//  ros::Duration(5).sleep();
//  std::cout << "Begnning" << std::endl;
//        if(got_heartbeat && !arm_sent){
//  mavlink::Mavlink msg_out = create_arm_msg(ARM);
//  chatter_pub.publish(msg_out);
//  arm_sent = true;
//        }

//  while(ros::ok()){

//	mavlink::Mavlink msg_out = create_heartbeat_msg();
//	chatter_pub.publish(msg_out);

//	std::cout << "counter: "<< counter << std::endl;

//	if(got_heartbeat && !arm_sent){
//		mavlink::Mavlink msg_out = create_arm_msg(ARM);
//		chatter_pub.publish(msg_out);
//		arm_sent = true;
//	}

//	if(counter == 3){
//		mavlink::Mavlink msg_out = create_rc_override_msg();
//		chatter_pub.publish(msg_out);
//	}

	//overrun
//	counter=(++counter)%9;

//    chatter_pub.publish((mavlink::Mavlink)msg);
//	ros::spinOnce();

//	loop_rate.sleep();

//	++seq;
//   }

  ros::spin();
  return 0;
}




