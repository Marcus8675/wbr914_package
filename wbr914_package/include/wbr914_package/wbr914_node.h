#include "rclcpp/rclcpp.hpp"
#include <chrono>
#include <functional>
#include <memory>
#include "geometry_msgs/msg/twist.hpp"
#include "wbr914_m3_io.h"
#include "eye_interface.h"
#include <wbr914_package/srv/velocity_get.hpp>
#include <wbr914_package/srv/position_get.hpp>
#include <wbr914_package/srv/irdata_get.hpp>
#include <wbr914_package/srv/dio_set.hpp>
#include <wbr914_package/srv/dio_get.hpp>



using std::placeholders::_1;
using std::placeholders::_2;
using namespace std::chrono_literals;

/*
  A ROS listener(subscriber) reading Twist messages from the 'velocity' topic.
  Only the x value of the linear and angular velocity is used.
*/
class CmdVelListener : public rclcpp::Node
{
public:
  CmdVelListener() : Node("wbr914_node")
  {
    // Open connection with wbr914 usb,ready all needed settings and enable motors
    int mainSetupFlag = wbr914.MainSetup();
    if(mainSetupFlag==0)
    {
      printf("wbr914 MainSetup succeed\n");
    }
    wbr914.UpdateM3();
    bool enableMotorsFlag = wbr914.EnableMotors(true);
    if(enableMotorsFlag==true)
    {
      printf("enableMotors is true\n");
    }
    
    wbr914.UpdateM3();
    odineye.set_eye(0,0,0,0);  //Eye off
    
    // Create the subscriber to a 'velocity' topic.
    // Using placeholder _1 for keeping the option of using a ptr to Twist.
    sub = this->create_subscription<geometry_msgs::msg::Twist>(
      "velocity_cmd",10,std::bind(&CmdVelListener::velocity_callback,this,_1));

    // Create the service that will return velocity(Twist message) on request
    velocityGetService = this-> create_service<wbr914_package::srv::VelocityGet>("velocity_get_robot",
    std::bind(&CmdVelListener::get_velocity_service,this,_1,_2));

    // Create the service that will return position(Pose message) on request
    positionGetService = this-> create_service<wbr914_package::srv::PositionGet>("position_get_robot",
    std::bind(&CmdVelListener::get_position_service,this,_1,_2));
    
    // Create the service that will return IR data on request
    irdataGetService = this-> create_service<wbr914_package::srv::IrdataGet>("irdata_get_robot",
    std::bind(&CmdVelListener::get_irdata_service,this,_1,_2));

    // Create the service that will Set Digital ouput data on request
    dioSetService = this-> create_service<wbr914_package::srv::DioSet>("dio_set_robot",
    std::bind(&CmdVelListener::set_dio_service,this,_1,_2));

    // Create the service that will Get Digital input data on request
    dioGetService = this-> create_service<wbr914_package::srv::DioGet>("dio_get_robot",
    std::bind(&CmdVelListener::get_dio_service,this,_1,_2));

    //Create the service that will trigger on a timer for periodic events
    timer_ = this-> create_wall_timer(500ms, std::bind(&CmdVelListener::timer_callback, this));
  };

  ~CmdVelListener()
  {
    // Shutdown communication with wbr914
    wbr914.MainQuit();
  };
  
private:

  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;
  uint8_t white = 0;
  uint8_t LED = 0;

/*
 geomerty_msgs/Twist message is expected in the cmd_vel topic and returned by get_velocity service
 linear:
   float64 x: X
   float64 y: 0.0
   float64 z: 0.0
 angular:
   float64 x: X
   float64 y: 0.0
   float64 z: 0.0
*/
  void velocity_callback(const geometry_msgs::msg::Twist& msg);

  void get_velocity_service(const std::shared_ptr<wbr914_package::srv::VelocityGet::Request> request,
        std::shared_ptr<wbr914_package::srv::VelocityGet::Response> response);

/*
 geometry_msgs/Pose message is returned by the get_position service
 point:
  float64 x: X
  float64 y: Y
  float64 z: 0.0

 orientation:
  float64 x: X
  float64 y: 0.0
  float64 z: 0.0
  float64 w: 1.0

  */

  void get_position_service(const std::shared_ptr<wbr914_package::srv::PositionGet::Request> request,
        std::shared_ptr<wbr914_package::srv::PositionGet::Response> response);
    
  void get_irdata_service(const std::shared_ptr<wbr914_package::srv::IrdataGet::Request> request,
        std::shared_ptr<wbr914_package::srv::IrdataGet::Response> response);

  void set_dio_service(const std::shared_ptr<wbr914_package::srv::DioSet::Request> request,
        std::shared_ptr<wbr914_package::srv::DioSet::Response> response);

  void get_dio_service(const std::shared_ptr<wbr914_package::srv::DioGet::Request> request,
        std::shared_ptr<wbr914_package::srv::DioGet::Response> response);

  void timer_callback();

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub;
  rclcpp::Service<wbr914_package::srv::VelocityGet>::SharedPtr velocityGetService;
  rclcpp::Service<wbr914_package::srv::PositionGet>::SharedPtr positionGetService;
  rclcpp::Service<wbr914_package::srv::IrdataGet>::SharedPtr irdataGetService;
  rclcpp::Service<wbr914_package::srv::DioSet>::SharedPtr dioSetService;
  rclcpp::Service<wbr914_package::srv::DioGet>::SharedPtr dioGetService;
  wbr914_m3_io wbr914;
  eye_interface odineye;
};

