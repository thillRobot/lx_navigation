# lx_navigation
This is a ROS package for 2D navigation with the Pioneer LX Robot. Adept might be gone, but the robots are still here...

## Supporting Packages
- [lx_control](https://github.com/thillRobot/lx_control) - package for control computer (base station)
- [lx_vision](https://github.com/thillRobot/lx_vision) - package for cameras and robot vision

You need a map to navigate. Unless you are using explore.
Here is how to Make a Map with LX, ROS and gmapping.

Definitions:

```
<laserscan> your laser topic name
<wspname> what you want you map to be called
<mapname> what you want you map to be called
<robotip> IP address of the robot
<remoteip> computer name of the control computer
<remotecomputer> computer name of the control computer
```

### Making a map with the LX Robot
#### 1) Record the Laser data with a robot to make a map.

  ##### i)  Turn on the navigation and mapping nodes

  `$ roslaunch lx_navigation lx_map.launch`

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

##### Turn on the robot. 
This can be done locally or through SSH. Local is nice if you are making a large map with keyboard.

`roslaunch lx_navigation lx_drive.launch robot_ip:=X.X.X.X`

##### IN A NEW TERMINAL start recording data   
Change to the directory that the map bag file will be saved. The map can be moved afterward

`cd ~/lx_ros/src/lx_navigation/maps`
`rosbag record -O bh1_map0 /RosAria/S3Series_1_laserscan /tf`

##### Starting at the origin record a map using keyboard drive. 
When you are done collecting date, close both processes then process the map.
Turn on roscore
`roscore`

In a separate terminal set the sim time param, then run gmapping in the same terminal

`rosparam set use_sim_time true`
`rosrun gmapping slam_gmapping scan:=/RosAria/S3Series_1_laserscan _odom_frame:=odom`

In a separate terminal play the bag file to compile the scan data
`rosbag play --clock bh1_map0.bag`

##### Wait for the processing to take place. 
You should see an esitmate of how long it will take. When complete, run the map saver in the same terminal as the rosbag command

`rosrun map_server map_saver -f bh1_map0`



##### NOW YOU CAN CLOSE ALL PROCESSES, you should have a map called 'labmap.pgm'

### NAVIGATION using a map with the LX Robot

#### Turn on NAVIGATION on the Robot

`$ roslaunch lx_navigation lx_navigation.launch map:=bh124_map1 robot_ip:=X.X.X.X`

#### now turn on rviz on the remote computer
`$ roslaunch lx_control lx_control.launch robot_ip:=X.X.X.X control_ip:X.X.X.X`
