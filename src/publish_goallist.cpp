// lx_navigation/src/publish_goallist.cpp
// publishes an array (not list) of goal points to turtlebot3 with MoveBaseActionGoal msgs
// Tristan Hill - October 30, 2025

#include "ros/ros.h"
#include "geometry_msgs/PoseStamped.h"
#include <sstream>
#include "std_msgs/String.h"
#include "actionlib_msgs/GoalStatusArray.h"
#include "actionlib_msgs/GoalID.h"
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

int status;

void statusCB(const actionlib_msgs::GoalStatusArray::ConstPtr& msg) 
{ 
    //ROS_INFO("Subscriber Callback Executed"); 
    if (!msg->status_list.empty()) 
    { 
        actionlib_msgs::GoalStatus goalStatus = msg->status_list[0]; 
        //ROS_INFO("Status Recieved: %i",goalStatus.status); 
        status=goalStatus.status;
    } 
} 

int main(int argc, char **argv)
{
    ros::init(argc, argv, "publish_goallist");
    ros::NodeHandle n;
    //ros::Publisher cancel_publisher =
    //n.advertise<actionlib_msgs::GoalID>("/move_base/cancel", 1000);
    ros::Publisher actiongoal_publisher =
    n.advertise<move_base_msgs::MoveBaseActionGoal>("/move_base/goal", 1000);
    float loop_freq=10.0;
    ros::Rate loop_rate(loop_freq);
    //msg.header.stamp=ros::Time::now();
    //msg.header.frame_id="map";

    ros::Subscriber sub = n.subscribe("/move_base/status", 1000, statusCB);

    // goal point liactionlib_msgs/GoalIDst {{x1,y1,theta1},{x2,y2,theta2},...}
    float goallist[5][3]={{1.0,1.0,0.0},
	    	          {3.0,3.0,0.7},
			  {-1.0,0.0,1.5},
			  {-1.5,-7.0,1.5},
			  {6.5,-8.0,1.5}};
    int num_goals=5;
  
    tf2::Quaternion q;

    // int count = 0;   
    int goal_idx=0;
    bool goal_reached=false;   

    float loop_time=0;
    // wait for 3 seconds, there is definitely a better way...
    ROS_INFO("waiting to begin, status: %i", status);
    while ((loop_time<3)){
      loop_rate.sleep();
      loop_time=loop_time+1/loop_freq;      
      ros::spinOnce();  
    }    
    ROS_INFO("status: %i",status);
    
    while (goal_idx<num_goals)
    {
    
      //float loop_time=0;
      // wait for 3 seconds, there is definitely a better way...
     // ROS_INFO("waiting to send goal, status: %i", status);
     // while ((loop_time<3)){
     //   loop_rate.sleep();
     //   loop_time=loop_time+1/loop_freq;      
     //   ros::spinOnce();  
     // }    
     // ROS_INFO("status: %i",status);
      
      move_base_msgs::MoveBaseActionGoal agmsg;
      move_base_msgs::MoveBaseGoal goal;
      
      geometry_msgs::PoseStamped pose;
      pose.header.stamp=ros::Time::now();
      pose.header.frame_id="map";

      // publish the goal_idxth goal, yes I said it... 
      
      pose.header.stamp=ros::Time::now();
      pose.pose.position.x = goallist[goal_idx][0];
      pose.pose.position.y = goallist[goal_idx][1];
      pose.pose.position.z = 0.0;
      q.setRPY(0,0,goallist[goal_idx][2]);
      tf2::convert(q,pose.pose.orientation);
      
      goal.target_pose=pose;

      actionlib_msgs::GoalID idmsg;
      //actionlib_msgs::GoalID cidmsg;
      char id[10]; 
      sprintf(id,"goal%i",goal_idx);
      idmsg.id=id;
      //sprintf(id,"",goal_idx);
      //cidmsg.id=id;  
 
      agmsg.goal=goal;
      agmsg.goal_id=idmsg;

      actiongoal_publisher.publish(agmsg);
      
      ROS_INFO("Goal id: %s sent, status: %i",id,status);
      ROS_INFO("waiting for goal to be processed");
      //while ((status==0)){
      while ((status!=1)){
        ros::spinOnce();  
      }         
      ROS_INFO("status: %i\n", status);
      
      // wait for the goal to be reached
      //while (status!=2&&status!=3){
      ROS_INFO("navigating to goal, status: %i", status);
      ROS_INFO("waiting for goal to be reached");
      while (status==1){
        ros::spinOnce();
      }
      ROS_INFO("status: %i\n", status);

      if (status==3){
        ROS_INFO("navigation to goal succeeded, status: %i", status);
        status=0; 
      }     

      ROS_INFO("Goal id: %s complete, status: %i",id,status);
      goal_idx++;
      
      ros::spinOnce();
      loop_rate.sleep(); 
    }
}
