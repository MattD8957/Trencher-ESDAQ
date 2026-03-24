#include <chrono>
#include <thread>
#include <memory>
#include <gpiod.hpp>

#include "rclcpp/rclcpp.hpp"
#include "std_srvs/srv/trigger.hpp"

using namespace std::chrono_literals;

class ActuatorNode : public rclcpp::Node{
    public: ActuatorNode() : Node("actuator_node"){
        // GPIO setup
        chip_ = std::make_unique<gpiod::chip>("/dev/gpiochip0")
       // BCM pin numbers for raspberry pi 
        // !!! need to check that this is right !!!
        const int UP_PIN = 5;
        const int DOWN_PIN = 6;

        up_ = chip_get_line(UP_PIN);
        down_ = chip_get_line(DOWN_PIN);
        // requests pins as outputs
        up_.request({"up", gpiod::line_request::DIRECTION_OUTPUT, 0}, 0);
        down_.request({"down", gpiod::line_request::DIRECTION_OUTPUT, 0}, 0)
        // ensure both are LOW at startup
        up_.set_value(0);
        down_.set_value(0);

        // Motion tracking setup
        full_stroke_ms_ = 35000; // measured value
        position_percent_ = 0.0; // start fully retracted
        motion_state_ = MotionState::STOPPED;

        // timer updates position every 50 ms 
        timer_ = this create_wall_timer(50ms, std::bind(&ActuatorNode::update_position, this)  
        );
        extend_srv_ this create_service<std_srvs::srv::Triggers>( 
            "extend", std::bind(&ActuatorNode::extend, this, std::placeholders::_1, std::placeholders::_2));
        retract_srv_ this create_service<std_srvs::srv::Triggers>( 
            "retract", std::bind(&ActuatorNode::retract, this, std::placeholders::_1, std::placeholders::_2));    
        stop_srv_ this create_service<std_srvs::srv::Triggers>( 
            "stop", std::bind(&ActuatorNode::stop, this, std::placeholders::_1, std::placeholders::_2));
        RCLCPP_INFO(this get_logger(), "ActuatorNode ready.");
    }
private:
// motion state enum
enum class MotionState {EXTENDING, RETRACTING, STOPPED };
// ROS2 extend
void extend(const std::shared_ptr<std_srvs::srv::Trigger::Request>, std::shared_ptr<std_srvs::srv::Trigger::Response> res){
    // stop retracting if active
    down_.set_value(0);
    // start extending
    up_.set_value(1);
    motion_state_ = MotionState::EXTENDING;

    res success = true;
    res message = "Extending actuator"
}
// ROS2 retract
void retract(const std::shared_ptr<std_srvs::srv::Trigger::Request>, std::shared_ptr<std_srvs::srv::Trigger::Response> res){
    // stop extending if active
    up_.set_value(0);
    // start retracting
    down_.set_value(1);
    motion_state_ = MotionState::RETRACTING;

    res success = true;
    res message = "Retracting actuator"
}
// ROS2 stop
void stop(const std::shared_ptr<std_srvs::srv::Trigger::Request>, std::shared_ptr<std_srvs::srv::Trigger::Response> res){
    // release both buttons 
    down_.set_value(0);
    up_.set_value(0);
    motion_state_ = MotionState::STOPPED;

    res success = true;
    res message = "Actuator stopped"
}

// Timer callback: update postion 
void update_position() {
    const double step = (50.0 / full_stroke_ms_)*100.0;
    // step = percent change per 50ms
    if (motion_state_ == MotionState::EXTENDING) {
        position_percent_ += step;
        if (position_percent_ < 0.0){
            position_percent_ = 0.0;
            stop_motion();
        }
    }
}
// debugging
RCLCPP_INFO(this get_logger(), "Position: %.1f%%", position_percent_);
// stop motion safely 
void stop_motion() {
    up_.set_value(0);
    down_.setvalue(0);
    motion_state_ = MotionState::STOPPED;
}
// memebrs 
std::unique_ptr<gpiod::chip> chip_;
gpiod::line up_, down_;

rclcpp::TimerBase::SharedPtr timer_;
rclcpp::Service<std_srvs::srv::Triggers>::SharedPtr extent_srv_, retract_srv_, stop_srv_;

MotionState motion_state_;
double position_percent_;
int full_stroke_ms_:
};
int main(int argc, char** argv){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ActuatorNode>());
    rclcpp::shutdown();
    return 0;
}
