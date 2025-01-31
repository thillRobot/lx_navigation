#include "ros/ros.h"
#include "geometry_msgs/PoseStamped.h"
#include <sstream>
#include "std_msgs/String.h"
#include "actionlib_msgs/GoalStatusArray.h"
#include "actionlib_msgs/GoalID.h"
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

int status;

void statusCB(const actionlib_msgs::GoalStatusArray::ConstPtr& msg) 
{ 
    ROS_INFO("Subscriber Callback Executed"); 
    if (!msg->status_list.empty()) 
    { 
        actionlib_msgs::GoalStatus goalStatus = msg->status_list[0]; 
        ROS_INFO("Status Recieved: %i",goalStatus.status); 
        status=goalStatus.status;
    } 
} 

int main(int argc, char **argv)
{
    ros::init(argc, argv, "publish_goallist");
    ros::NodeHandle n;
    ros::Publisher goal_publisher =
    n.advertise<geometry_msgs::PoseStamped>("/move_base_simple/goal", 1000);
    ros::Publisher cancel_publisher =
    n.advertise<actionlib_msgs::GoalID>("/move_base/cancel", 1000);
    float loop_freq=10.0;
    ros::Rate loop_rate(loop_freq);
    //geometry_msgs::PoseStamped msg;
    //msg.header.stamp=ros::Time::now();
    //msg.header.frame_id="map";

    actionlib_msgs::GoalID cmsg;
    //cmsg.id="";

    ros::Subscriber sub = n.subscribe("/move_base/status", 1000, statusCB);

    // goal point list {{x1,y1,theta1},{x2,y2,theta2},...}
    float goallist[3][3]={{1.0,1.0,0.0},{3.0,1.0,0.7},{5.0,1.0,1.5}};

    tf2::Quaternion q;

    // int count = 0;   
    int goal_idx=0;
    bool goal_reached=false;   

    while (goal_idx<3)
    {
    
      geometry_msgs::PoseStamped msg;
      msg.header.stamp=ros::Time::now();
      msg.header.frame_id="map";

          
      float loop_time=0;
      // wait for 3 seconds, there is definitely a better way...
      while ((loop_time<3)){
        ROS_INFO("status: %i",status);
        ROS_INFO("waiting");
        loop_rate.sleep();
        loop_time=loop_time+1/loop_freq;      
      }
      
      //if ~goal_reached{
      // publish the goal_idxth goal, yes I said it... 
      ROS_INFO("status: %i",status);
      
      msg.header.stamp=ros::Time::now();
      msg.pose.position.x = goallist[goal_idx][0];
      msg.pose.position.y = goallist[goal_idx][1];
      msg.pose.position.z = 0.0;
      q.setRPY(0,0,goallist[goal_idx][2]);
      tf2::convert(q,msg.pose.orientation);
      
      goal_publisher.publish(msg);

      //}
      

      while ((status==0)){
        ROS_INFO("status: %i", status);
        ROS_INFO("waiting for goal to be processed");
        ros::spinOnce();  
      }         
  
      // wait for the goal to be reached
      //while (status!=2&&status!=3){
      while (status==1){
        ROS_INFO("status: %i", status);
        ROS_INFO("navigating to goal");
        ros::spinOnce();
      }

      // check if goal has been reached or canceled
      if (status==2){
        ROS_INFO("status: %i",status);
        ROS_INFO("goal canceled");    
      }else if (status==3){
        ROS_INFO("status: %i",status);
        ROS_INFO("goal reached!");   
        

        // wait for canceling to happen
        while (status!=2){
          ROS_INFO("canceling goal");        
          cancel_publisher.publish(cmsg);
          ROS_INFO("status: %i",status);
          ROS_INFO("waiting");
          loop_rate.sleep();
          loop_time=loop_time+1/loop_freq;      
          
          ros::spinOnce();
        }
        goal_idx++;
      }
      
     

      loop_rate.sleep();
      
    }
}
