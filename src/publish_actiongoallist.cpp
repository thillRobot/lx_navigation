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
    ros::init(argc, argv, "publish_actiongoallist");
    ros::NodeHandle n;
    //ros::Publisher goal_publisher =
    //n.advertise<geometry_msgs::PoseStamped>("/move_base_simple/goal", 1000);
    ros::Publisher cancel_publisher =
    n.advertise<actionlib_msgs::GoalID>("/move_base/cancel", 1000);
    ros::Publisher actiongoal_publisher =
    n.advertise<move_base_msgs::MoveBaseActionGoal>("/move_base/goal", 1000);
    //geometry_msgs::PoseStamped msg;
    float loop_freq=10.0;
    ros::Rate loop_rate(loop_freq);
    //msg.header.stamp=ros::Time::now();
    //msg.header.frame_id="map";

    ros::Subscriber sub = n.subscribe("/move_base/status", 1000, statusCB);

    // goal point liactionlib_msgs/GoalIDst {{x1,y1,theta1},{x2,y2,theta2},...}
    float goallist[4][3]={{1.0,1.0,0.0},{3.0,1.0,0.7},{3.0,3.0,1.5},{1.0,3.0,1.5}};

    tf2::Quaternion q;

    // int count = 0;   
    int goal_idx=0;
    bool goal_reached=false;   

    while (goal_idx<4)
    {
    
      move_base_msgs::MoveBaseActionGoal agmsg;
      move_base_msgs::MoveBaseGoal goal;
      
      geometry_msgs::PoseStamped pose;
      pose.header.stamp=ros::Time::now();
      pose.header.frame_id="map";

          
      float loop_time=0;
      // wait for 3 seconds, there is definitely a better way...
      ROS_INFO("waiting to send goal %i, status: %i",goal_idx, status);
      while ((loop_time<3)){
        loop_rate.sleep();
        loop_time=loop_time+1/loop_freq;      
      }    
      ROS_INFO("status: %i",status);
      
      // publish the goal_idxth goal, yes I said it... 
      
      pose.header.stamp=ros::Time::now();
      pose.pose.position.x = goallist[goal_idx][0];
      pose.pose.position.y = goallist[goal_idx][1];
      pose.pose.position.z = 0.0;
      q.setRPY(0,0,goallist[goal_idx][2]);
      tf2::convert(q,pose.pose.orientation);
      
      goal.target_pose=pose;

      actionlib_msgs::GoalID idmsg;
      char id[10]; 
      sprintf(id,"goal%i",goal_idx);
      idmsg.id=id;
  
      //goal_publisher.publish(msg);
      agmsg.goal=goal;
      agmsg.goal_id=idmsg;

      actiongoal_publisher.publish(agmsg);
      //}
      
      ROS_INFO("Goal id: %s sent, status: %i",id,status);
      ROS_INFO("waiting for goal to be processed");
      while ((status==0)){
        ros::spinOnce();  
      }         
  
      // wait for the goal to be reached
      //while (status!=2&&status!=3){
      ROS_INFO("navigating to goal %i, status: %i",goal_idx, status);
      ROS_INFO("waiting for goal %i to be reached", goal_idx);
      while (status==1){
        ros::spinOnce();
      }
      ROS_INFO("status: %i\n", status);

      // check if goal has been reached or canceled
      if (status==2){
        ROS_INFO("goal %i canceled, status: %i", goal_idx, status);    
      }else if (status==3){
        ROS_INFO("goal %i reached! status: %i", goal_idx, status);   
        ROS_INFO("canceling goal %i", goal_idx);        
        cancel_publisher.publish(idmsg);
       
         // wait for canceling to happen
        //loop_time=0;
        //ROS_INFO("waiting for cancel to process, status: %i", status);
        //while (status!=2&&loop_time<2){
        int k=0;
	while(k<10){
	//  loop_rate.sleep();
        //  loop_time=loop_time+1/loop_freq;      
          
          ros::spinOnce();
	  k++;
        }
        printf("status: %i",status);
        goal_idx++;
      }
      
      loop_rate.sleep();
      
    }
}
