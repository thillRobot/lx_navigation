# lx_navigation

This is a ROS package for NAVIGATION for the Pioneer LX Robot.

You need a map to navigate. Unless you are using explore.
Here is how to Make a Map with LX, ROS and gmapping.

Replace these things:

```
<laserscan> your laser topic name
<wspname> what you want you map to be called
<mapname> what you want you map to be called
<robotip> IP address of the robot
<remoteip> computer name of the control computer
<remotecomputer> computer name of the control computer
```

### Making a map
#### 1) Record the Laser data with a robot to make a map.

  ##### i)  Turn on the navigation and mapping nodes

  `$ roslaunch lx_2dnav lx_map.launch`

  ##### ii) Change to the directory where you want to save the data

  `$ cd ~/<wspname>`

  ##### iii) Start recording data

  `$ rosbag record -O <mapname> <laserscan> /tf`  

  ##### iii) Now, starting at the origin record a map by driving the robot using keyboard drive

  `$ rosrun teleop_twist_keyboard teleop_twist_keyboard.py`

  ##### iV) close all processes from step 1, you should now have a bag file


#### 2) Process the map data. Navigate to the directory where the data was saved in Step 1 for all the following processes. This needs a launch file.

  ##### i) Turn on the ROS core

  `$ roscore`

  ##### ii) Set the sim time param

  `$ rosparam set use_sim_time true`

  ##### iii) Turn on gmapping

  `$ rosrun gmapping slam_gmapping scan:=<laserscan> _odom_frame:=odom`

  ##### iv) Now turn on rosbag and wait, it should tell you how long

  `$ rosbag play --clock <mapname>.bag`

  ##### v) Once that is finished now you can save the map

  `$ rosrun map_server map_saver -f <mapname>``

#### 3) Now you can close all processes, you should have a map call '<mapname>.pgm'. Try to use the map to navigate with AMCL.

  ##### i) Turn on the navigation

  `$ roslaunch lx_navigation lx_navigation.launch robot_ip:=<robotip> map:<mapname>`

  ##### ii) Now on a remote computer turn on rviz so you can drive the robot

  `$ roslaunch lx_control lx_control.launch robot_ip:=<robotip> control_ip:<remoteip>`



#### 4) Here is an example of the full workflow with the LX - tested with on 02/14/2020

##### urn on the robot

`ssh thill@robot$ roslaunch lx_2dnav lx_drive.launch`

##### IN A NEW TERMINAL start recording data   

`ssh thill@robot$ cd ~/ttu_ros`
`$ rosbag record -O labmap /RosAria/S3Series_1_laserscan /tf`

##### Now, starting at the origin record a map using keyboard drive. When you are done collecting date, close both processes then process the map.

`$ roscore`

`$ rosparam set use_sim_time true
`
`$ rosrun gmapping slam_gmapping scan:=/RosAria/S3Series_1_laserscan _odom_frame:=odom`

`$ rosbag play --clock labmap.bag`

##### Wait for the processing to take place. It will give you an esitmate of how long it will take

`$ rosrun map_server map_saver -f labmap
`
##### NOW YOU CAN CLOSE ALL PROCESSES, you should have a map called 'labmap.pgm'

##### Use the map to navigate with AMCL

`$ roslaunch lx_2dnav lx_config.launch` These are not up to date

`$ 3) roslaunch lx_2dnav lx_move_base.launch`

##### now turn on rviz remotely

`$ roslaunch lx_control lx_control.launch`
