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

// extremely crude function for blocking 'pause()'
void pause(float seconds){
  float loop_time=0;
  float loop_freq=100.0;
  ros::Rate loop_rate(loop_freq);
  float wait_time=seconds;
  // wait for 3 seconds, there is definitely a better way...
  ROS_INFO("waiting for %f seconds", seconds);
  while (loop_time<wait_time){
    loop_rate.sleep();
    loop_time=loop_time+1/loop_freq;      
  }    
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "publish_goal_array");
    ros::NodeHandle n;
    ros::Publisher goal_publisher =
    n.advertise<geometry_msgs::PoseStamped>("/move_base_simple/goal", 1000);
    float loop_freq=10.0;
    ros::Rate loop_rate(loop_freq);

    ros::Subscriber sub = n.subscribe("/move_base/status", 1000, statusCB);

    // goal points       {{x1,y1,theta1},{x2,y2,theta2},...}
    float goallist[6][3]={{-2.0,10.0,0.0},
                          {-2.0,15.0,0.7},
                          {-2.0,20.0,1.5},
                          {-2.0,25.0,1.5},
                          {-2.0,30.0,1.5},
                          {-2.0,35.0,1.5}};  

    int num_goals=6;
  
tf2::Quaternion q;

    // int count = 0;   
    int goal_idx=0;
    bool goal_reached=false;   

    while (goal_idx<num_goals)
    {
    
      move_base_msgs::MoveBaseGoal goal_msg;
      
      geometry_msgs::PoseStamped pose;
      pose.header.stamp=ros::Time::now();
      pose.header.frame_id="map";
          
      pause(2.0);      

      // publish the goal_idxth goal, yes I said it...  
      pose.header.stamp=ros::Time::now();
      pose.pose.position.x = goallist[goal_idx][0];
      pose.pose.position.y = goallist[goal_idx][1];
      pose.pose.position.z = 0.0;
      q.setRPY(0,0,goallist[goal_idx][2]);
      tf2::convert(q,pose.pose.orientation);
      
      goal_msg.target_pose=pose;

      ROS_INFO("publishing goal %i, status: %i", goal_idx, status); 
      goal_publisher.publish(goal_msg);
      goal_reached=false; 
      pause(2.0);
      ros::spinOnce();  
      
      ROS_INFO("waiting for goal %i to be processed, status: %i", goal_idx, status);
      while ((status==0)){
        ros::spinOnce();  
      }          
      ROS_INFO("navigating to goal %i, status: %i", goal_idx, status);
      
      // wait for the goal to be reached
      ROS_INFO("waiting for goal %i to be reached, status: %i", goal_idx, status);
      while(status!=3){
        ros::spinOnce();
      }
      ROS_INFO("goal %i reached, status: %i\n", goal_idx, status);
      
      goal_reached=true;
      goal_idx++;
      pause(5);      
  
      loop_rate.sleep();
      
    }
}
