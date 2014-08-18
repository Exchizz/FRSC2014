#include "ros/ros.h"
#include "std_msgs/String.h"
#include "mavlink/Mavlink.h"
#include "mavlink.h"
#include <sstream>
#include "sensor_msgs/Joy.h"
#include <iostream>
#include <fstream>

#define ARM 1
#define DISARM 0
#define RC_OFFSET 1500
#define RC_INCREMENT 500

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

#define ALTITUDE_KP_UP 0.06
#define ALTITUDE_KP_DOWN 0.02


//history
//0.05= good but oscillates
//0.001 still up
//0.2 too hight


int seq = 0;
bool arm_sent = false;
bool got_heartbeat = false;


int i = 0;
//mavlink::Mavlink create_heartbeat_msg(int seq);
//mavlink::Mavlink create_arm_msg(bool);
//mavlink::Mavlink create_rc_override_msg(int, int, int, int);
//mavlink::Mavlink create_manual_control_msg(int x, int y, int z, int r, int buttons);


bool ch_reset = false;
int mode = MANUAL;

ros::Publisher chatter_pub;
ros::Subscriber joy_sub;
bool show_battery = false;


std::ofstream logfile_altitude;
std::ofstream logfile_error;
std::ofstream logfile_output;

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
//			mavlink::Mavlink msg_out = create_heartbeat_msg(msg->seq);
//			chatter_pub.publish(msg_out);


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

			logfile_altitude << global_position.alt << ",";

//			if(mode == HOLD_ALTITUDE){
				int current_altitude = global_position.alt;

				if(setpoint_altitude == 0){
					setpoint_altitude = current_altitude;
					ROS_WARN("altitude locked");
				}

				int error = setpoint_altitude - current_altitude;
				int ch3 = 0;
				if(error > 0 ){
					ch3 = 1550 + (error*ALTITUDE_KP_UP); //change to 1700

				} else {
					ch3 = 1550 + (error*ALTITUDE_KP_DOWN); //change to 1700
				}



				std::cout << "error counter: " << i++ << "\n";
				logfile_error << error << ",";
				ROS_INFO("altitude error : %d", error);

		                if(ch3 <= 1000) ch3 = 1000; //changed from 1500 to 1000
		                if(ch3 >= 2000) ch3 = 2000;
				ROS_WARN("ch3: %d", ch3);

				logfile_output<<ch3 << ",";

				logfile_error.flush();
				logfile_altitude.flush();
				logfile_output.flush();
//		                mavlink::Mavlink msg_out = create_rc_override_msg(0, 0, ch3, 0);
//		                chatter_pub.publish(msg_out);

//			}



			ROS_INFO("Altitude: %d", global_position.alt);
		}
		break;
	}
}

int main(int argc, char **argv){

  logfile_altitude.open ("latest_latitudes.cvr");
  logfile_error.open ("latest_errors.cvr");
  logfile_output.open ("latest_output.cvr");

  ros::init(argc, argv, "send_command");
  ros::NodeHandle n;

  chatter_pub = n.advertise<mavlink::Mavlink>("/mavlink/to", 1000);
//  joy_sub = n.subscribe("/joy", 1000, joyCallback);
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





