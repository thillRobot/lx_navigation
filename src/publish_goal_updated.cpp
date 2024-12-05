#include "ros/ros.h"
#include "geometry_msgs/PoseStamped.h"
#include <sstream>
int main(int argc, char **argv)
{
    ros::init(argc, argv, "publish_goal");
    ros::NodeHandle n;
    ros::Publisher goal_publisher =
    n.advertise<geometry_msgs::PoseStamped>("/move_base_simple/goal", 1000);
    ros::Rate loop_rate(10);
    geometry_msgs::PoseStamped goal_msg;
    goal_msg.header.stamp=ros::Time::now();
    goal_msg.header.frame_id="map";
     
    float xg,yg;
    if (argc>1)
    {
        xg=atof(argv[1]);
        yg=atof(argv[2]);
    }
 
    int count = 0;   
    while ((ros::ok())&&(count<5))
    {
        goal_msg.pose.position.x = xg;
        goal_msg.pose.position.y = yg;
        goal_msg.pose.position.z = 0;
        goal_msg.pose.orientation.w = 1.0;
        goal_publisher.publish(goal_msg);
        ros::spinOnce();
        loop_rate.sleep();
        count++;
    }
}
