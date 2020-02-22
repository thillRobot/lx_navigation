# lx_navigation
This is a ROS package for NAVIGATION for the Pioneer LX Robot.

You need a map to navigate. Unless you are using explore.
Here is how to Make a Map with LX, ROS and gmapping.

Replace these things:

<laserscan> your laser topic name
<mapname> what you want you map to be called
<robotip> IP address of the robot
<remoteip> computer name of the control computer
<remotecomputer> computer name of the control computer

However, some of these commands are specific to the robot (LX)

1) Record the Laser data with a robot to make a map.

  i)  Turn on the navigation and mapping nodes

  $ roslaunch lx_2dnav lx_map.launch

  ii) Change to the directory where you want to save the data

  $ cd ~/ttu_ros

  iii) Start recording data

  $ rosbag record -O <mapname> <laserscan> /tf    *** This needs to be adjusted for your laser scan

  iii) Now, starting at the origin record a map by driving the robot using keyboard drive

  $ rosrun teleop_twist_keyboard teleop_twist_keyboard.py

  iV) close all processes from step 1, you should now have a file called <mapname>.bag


2) Process the map data. Navigate to the directory where the data was saved in Step 1 for all the following processes.

  i) Turn on the ROS core

  $ roscore

  ii) Set the sim time param

  $ rosparam set use_sim_time true

  iii) Turn on gmapping

  $ rosrun gmapping slam_gmapping scan:=<laserscan> _odom_frame:=odom

  iv) Now turn on rosbag and wait, it should tell you how long

  $ rosbag play --clock <mapname>.bag

  v) Once that is finished now you can save the map

  $ rosrun map_server map_saver -f <mapname>

3) Now you can close all processes, you should have a map call '<mapname>.pgm'. Try to use the map to navigate with AMCL.

  i) Turn on the navigation

  $ roslaunch lx_2dnav lx_navigate.launch map:=<mapname>

  ii) Now on a remote computer turn on rviz so you can drive the robot


  $ roslaunch lx_navigation lx_navigation.launch robot_ip:=<robotip> map:<mapname>

  $ roslaunch lx_control lx_control.launch robot_ip:=<robotip> control_ip:<remoteip>


  4) Boom! Robots are fun!

below tested with lx on 02/14/2020
1) make a map with the LX again, a clean one hopefully.

i) ssh thill@robot$ roslaunch lx_2dnav lx_drive.launch

IN A NEW TERMINAL			

ii) ssh thill@robot$ cd ~/ttu_ros
$ rosbag record -O labmap /RosAria/S3Series_1_laserscan /tf

Now, starting at the origin record a map using keyboard drive


-> CLOSE BOTH PROCESSES		

-> $ roscore

-> $ rosparam set use_sim_time true

-> $ rosrun gmapping slam_gmapping scan:=/RosAria/S3Series_1_laserscan _odom_frame:=odom

-> $ rosbag play --clock labmap.bag

-> WAIT FOR IT TO MAKE THE MAP

-> $ rosrun map_server map_saver -f labmap

-> NOW YOU CAN CLOSE ALL PROCESSES, you should have a map call 'labmap.pgm'


2) try to use the map to navigate with AMCL

-> $ roslaunch lx_2dnav lx_config.launch

-> $ roslaunch lx_2dnav lx_move_base.launch

3) now turn on rviz remotely

-> $ roslaunch lx_control lx_control.launch
