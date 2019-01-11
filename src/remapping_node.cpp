#include <ros/ros.h>

#include <geometry_msgs/Vector3.h>
#include <geometry_msgs/PoseWithCovariance.h>
#include <geometry_msgs/Pose.h>
#include <sensor_msgs/NavSatFix.h>
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/Imu.h>

#include "sbg_driver/SbgEkfNav.h"
#include "sbg_driver/SbgEkfQuat.h"
#include "sbg_driver/SbgGpsPos.h"
#include "sbg_driver/SbgImuData.h"


class ReMapping
{

	private:
	// /fix
	sensor_msgs::NavSatFix fix_data;
	geometry_msgs::Vector3 fix_data2;
	geometry_msgs::Vector3 stored_fix_msg;
	sensor_msgs::NavSatFix first_message_fix;
	sensor_msgs::NavSatFix second_message_fix;

	// / odom_gps_ekf
	nav_msgs::Odometry odom_data;
	geometry_msgs::PoseWithCovariance pose_data;
	nav_msgs::Odometry first_message_odom;
	nav_msgs::Odometry second_message_odom;
	bool flag_odom1;
	bool flag_odom2;

	// /imu
	sensor_msgs::Imu imu_data;
	sensor_msgs::Imu imu_msg;
	geometry_msgs::Quaternion temp_quat;
	bool flag_imu1;
	bool flag_imu2;

	//NodeHandles
	ros::NodeHandle n; // Subscribers
	ros::NodeHandle n_; // Publishers

  // /fix
	ros::Subscriber sub_fix;
	//ros::Subscriber sub_fix2;
//	ros::Subscriber sub1;
	ros::Publisher pub_fix;

	// /odom_gps_ekf
	ros::Subscriber sub_odom1;
	ros::Subscriber sub_odom2;
//	ros::Subscriber sub2;
	ros::Publisher pub_odom;

  // /imu
	ros::Subscriber sub_imu1;
	ros::Subscriber sub_imu2;
//	ros::Subscriber sub3;
	ros::Publisher pub_imu;


	public:
	ReMapping()
	{
	// init locks
	flag_odom1 = false;
	flag_odom2 = false;
	flag_imu1 = false;
	flag_imu2 = false;

	// /fix topic
	sub_fix = n.subscribe("gps_pos", 1000, &ReMapping::callback_fix2, this); //gps_pos (SbgGpsPos)
	//sub1 = n.subscribe("fix", 1000, &ReMapping::callback, this); // debug output
	pub_fix = n_.advertise<sensor_msgs::NavSatFix>("fix", 1000); // /fix topic

	// /odom_gps_ekf topic
	sub_odom1 = n.subscribe("ekf_nav",1000, &ReMapping::callback_odom1,this); //ekf_nav (SbgEkfNav)
	sub_odom2 = n.subscribe("ekf_quat",1000, &ReMapping::callback_odom2,this); //ekf_quat (SbgEkfQuat)
	//sub2 = n.subscribe("odom_gps_ekf", 1000, &ReMapping::callback2, this); //debug output
	pub_odom = n_.advertise<nav_msgs::Odometry>("odom_gps_ekf", 1000); // /odom_gps_ekf (Odometry)

	// /imu topic
	sub_imu1 = n.subscribe("imu_data",1000, &ReMapping::callback_imu1,this); // /imu_data (SbgImuData)
	sub_imu2 = n.subscribe("ekf_quat",1000, &ReMapping::callback_imu2,this); // /imu_data (SbgImuData)
	//sub3 = n.subscribe("imu", 1000, &ReMapping::callback3, this); //debug output
	pub_imu = n_.advertise<sensor_msgs::Imu>("imu", 1000); // /imu topic

	}

	void callback_fix2(const sbg_driver::SbgGpsPos msg)
	{
		fix_data2 = msg.position;

		publishFixData(fix_data2);

	}

	// void callback(const sensor_msgs::NavSatFix msg)
	// {
	//
	// ROS_INFO("Message from /fix received!");
	//
	// }


	void publishFixData(const geometry_msgs::Vector3 msg)
	{
		fix_data.latitude = msg.x;
		fix_data.longitude = msg.y;
		fix_data.altitude = msg.z;

		pub_fix.publish(fix_data);

		ROS_INFO("Published messages!");
	}

 // /odom_gps_ekf
	void callback_odom1(const sbg_driver::SbgEkfNav msg1)
  {
		// have to address them singularly since the type is different
		pose_data.pose.position.x = msg1.position.x;
		pose_data.pose.position.y = msg1.position.y;
		pose_data.pose.position.z = msg1.position.z;

		flag_odom1 = true;

  	if (flag_odom1 && flag_odom2)
  	{
	 		publishOdomData(pose_data);
  	}

  }

  void callback_odom2(const sbg_driver::SbgEkfQuat msg2)
  {
		//store msg for imu
	 temp_quat = msg2.quaternion;
	 pose_data.pose.orientation = msg2.quaternion;

	 flag_odom2 = true;

	 if (flag_odom1 && flag_odom2)
	 {
		 publishOdomData(pose_data);
	 }

 }

 //  void callback2(const nav_msgs::Odometry msg)
 //  {
 //
	// 	ROS_INFO("Message from /odom_gps_ekf received!");
 //
 // }


  void publishOdomData(const geometry_msgs::PoseWithCovariance msg)
  {
		odom_data.pose = msg;
	 	pub_odom.publish(odom_data);
	 	ROS_INFO("Published messages!");
	 	flag_odom1 = false;
	 	flag_odom2 = false;

  }

// /imu
 void callback_imu1(const sbg_driver::SbgImuData msg)
 {
	 imu_data.angular_velocity = msg.gyro;
	 imu_data.linear_acceleration = msg.accel;

	 flag_imu1 = true;
	 if (flag_imu1 && flag_imu2)
	 {
		 publishImuData(imu_data);
	 }

 }

 void callback_imu2(const sbg_driver::SbgEkfQuat msg)
 {
	  // Quaternion
	 imu_data.orientation = msg.quaternion;

	 flag_imu2 = true;
	 if (flag_imu1 && flag_imu2)
	 {
		 publishImuData(imu_data);
	 }


 }

 // void callback3(const sensor_msgs::Imu msg)
 // {
 //
 // ROS_INFO("Message from /imu received!");
 //
 // }


 void publishImuData(const sensor_msgs::Imu msg)
 {

	 pub_imu.publish(msg);
	 ROS_INFO("Published messages!");
	 flag_imu1 = false;
	 flag_imu2 = false;

 }


}; //end class ReMapping



int main(int argc, char **argv)
{

	ros::init(argc, argv, "remapping_node");

	//ros::NodeHandle n;

	ReMapping ReMappingObj;


	ros::spin();

	return 0;

}
