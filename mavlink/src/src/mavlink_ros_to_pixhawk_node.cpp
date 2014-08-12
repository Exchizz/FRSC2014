#include "ros/ros.h"
#include "std_msgs/String.h"
#include "mavlink/Mavlink.h"
#include "mavlink.h"
#include <sstream>


#define ARM 1
#define DISARM 0
/*
uint8 len
uint8 seq
uint8 sysid
uint8 compid
uint8 msgid
bool fromlcm
uint64[] payload64
*/

/*
 int16_t checksum;
int8_t magic;
int8_t len;
int8_t seq;
int8_t sysid;
int8_t compid;
int8_t msgid;
int64_t payload64[33];
*/
int seq = 0;
bool arm_sent = false;
bool got_heartbeat = false;

mavlink::Mavlink create_heartbeat_msg(int seq);
ros::Publisher chatter_pub;

void chatterCallback(const mavlink::Mavlink::ConstPtr& msg){


	mavlink_message_t mav_msg;


	mav_msg.len = msg->len;
	mav_msg.seq = msg->seq;
	mav_msg.sysid = msg->sysid;
	mav_msg.compid = msg->compid;
	mav_msg.msgid = msg->msgid;

	copy(msg->payload64.begin(), msg->payload64.end(), mav_msg.payload64);



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
/*
		case MAVLINK_MSG_ID_SYSTEM_TIME:
//			ROS_INFO("Recv: System time");
		break;

		case MAVLINK_MSG_ID_RAW_IMU :
//			ROS_INFO("Recv: Raw IMU");
		break;

		case MAVLINK_MSG_ID_SYS_STATUS:
//			ROS_INFO("Recv: SYS status");
		break;

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
mavlink::Mavlink create_rc_override_msg(){

	static int ch3 = 1000;
	static int increment = 200;

	ROS_INFO("RC_override sent");
	std::cout << "ch3 value: " << ch3 << std::endl;
	mavlink_message_t msg_first = {0};
	mavlink::Mavlink msg_out;

	msg_out.seq=seq++;
	msg_out.len=MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN;
	msg_out.sysid=255;
	msg_out.compid=0;
	msg_out.msgid=MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE;
	msg_out.fromlcm=false;

	mavlink_rc_channels_override_t packet;
	packet.chan1_raw = 1000;
	packet.chan2_raw = 1000;
	packet.chan3_raw = 1200;
	packet.chan4_raw = 1000;
	packet.chan5_raw = ch3+=increment;
	packet.chan6_raw = 1000;
	packet.chan7_raw = 1000;
	packet.chan8_raw = 1000;
	packet.target_system = 1;
	packet.target_component = 0;

	memcpy(_MAV_PAYLOAD_NON_CONST(&msg_first), &packet, 33);

	for(int i = 0; i < 33; ++i){
		msg_out.payload64.push_back(msg_first.payload64[i]);
	}

	if(ch3 >= 2000 || ch3 <= 1000){
		increment = -increment;
	}


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
	packet.param1 = 1;
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
  ros::Subscriber sub = n.subscribe("/mavlink/from", 1000, chatterCallback);
  ros::Rate loop_rate(1);

  int counter = 0;



  while(ros::ok()){

//	mavlink::Mavlink msg_out = create_heartbeat_msg();
//	chatter_pub.publish(msg_out);

	std::cout << "counter: "<< counter << std::endl;

	if(got_heartbeat && !arm_sent){
		mavlink::Mavlink msg_out = create_arm_msg(ARM);
		chatter_pub.publish(msg_out);
		arm_sent = true;
	}

	if(counter == 3){
		mavlink::Mavlink msg_out = create_rc_override_msg();
		chatter_pub.publish(msg_out);
	}

	//overrun
	counter=(++counter)%9;

//    chatter_pub.publish((mavlink::Mavlink)msg);
	ros::spinOnce();

	loop_rate.sleep();

	++seq;
   }
  return 0;
}




