# remapping

The aim of this package is to standardize in ROS the messages of [Ellipse](http://wiki.ros.org/sbg_driver) IMU from SBG Systems.
The driver for this unit is fully available at 

(https://github.com/ENSTABretagneRobotics/sbg_ros_driver)

## topics

The written topics are 3: 
```bash
/fix (sensor_msgs/NavSatFix)
/odom_gps_ekf (nav_msgs/Odometry)
/imu_data (sensor_msgs/Imu).
```

Latitude, Longitude and Altitude are stored in ```/fix``` from ```/gps_pos``` of ```sbg_driver```.
The odometry present in ```/odom_gps_ekf``` is the result of the merging of the position (```/ekf_nav```) and orientation (quaternion in ```/ekf_quat```).
The last topic ```/imu``` contains the orientation as before plus angular velocity and linear acceleration, both coming from ```/imu_data``` topic.
