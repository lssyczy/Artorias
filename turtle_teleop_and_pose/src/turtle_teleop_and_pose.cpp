#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim_msgs/msg/pose.hpp"  // 修改为 turtlesim 的 Pose 消息类型
#include <memory>
#include <chrono>
#include <termios.h>
#include <unistd.h>


using namespace std::chrono_literals;

class TurtleController : public rclcpp::Node
{
public:
    TurtleController() : Node("turtle_controller"), linear_speed_(0.0), angular_speed_(0.0)
    {
        // 创建一个发布者来控制小乌龟的速度
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);

        // 创建一个订阅者来获取小乌龟的位姿
        subscription_ = this->create_subscription<turtlesim_msgs::msg::Pose>(
            "turtle1/pose", 10,
            std::bind(&TurtleController::poseCallback, this, std::placeholders::_1));

        // 定时器，用于发布控制命令
        timer_ = this->create_wall_timer(
            500ms, std::bind(&TurtleController::move_turtle, this));
        
        // 初始化控制消息
        twist_msg_.linear.x = linear_speed_;  // 设置线速度
        twist_msg_.angular.z = angular_speed_; // 设置角速度

        // 创建一个线程来处理键盘输入
        input_thread_ = std::thread(&TurtleController::processInput, this);
    }

    ~TurtleController()
    {
        // 确保线程在销毁时退出
        input_thread_.join();
    }

private:
    void poseCallback(const turtlesim_msgs::msg::Pose::SharedPtr msg)
    {
        RCLCPP_INFO(this->get_logger(),
                    "Turtle Position -> X: '%f', Y: '%f', Theta: '%f'",
                    msg->x, msg->y, msg->theta);
    }

    void move_turtle()
    {
        twist_msg_.linear.x = linear_speed_;
        twist_msg_.angular.z = angular_speed_;
        publisher_->publish(twist_msg_);
    }

    void processInput()
    {
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        char c;
        while (rclcpp::ok())
        {
            c = getchar();
            switch (c)
            {
            case 'w':
                linear_speed_ += 0.1;
                break;
            case 's':
                linear_speed_ -= 0.1;
                break;
            case 'a':
                angular_speed_ += 0.1;
                break;
            case 'd':
                angular_speed_ -= 0.1;
                break;
            case 'q':
                linear_speed_ = 0.0;
                angular_speed_ = 0.0;
                break;
            default:
                break;
            }
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<turtlesim_msgs::msg::Pose>::SharedPtr subscription_;
    rclcpp::TimerBase::SharedPtr timer_;
    geometry_msgs::msg::Twist twist_msg_;
    std::thread input_thread_;

    double linear_speed_;
    double angular_speed_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TurtleController>());
    rclcpp::shutdown();
    return 0;
}
