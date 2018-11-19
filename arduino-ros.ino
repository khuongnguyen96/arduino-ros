//----------------------for ros ----------------------------------//
#include <ros.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Int64.h>
//----------------------------------------------------------------//

//----------------------define pins-------------------------------//
const int enA_left = 2, enB_left = 4;
const int enA_right = 3, enB_right = 5;
const int fmotor_left = 8, rmotor_left = 9;
const int fmotor_right= 10, rmotor_right = 11;
//---------------------------------------------------------------//

//---------------------define parameters-------------------------//
long int pulse_left = 0, pulse_right = 0;
long time = 0;
const int frequency = 40;
int pwm_left, pwm_right;

//---------------------for ros ----------------------------------//
ros::NodeHandle nh;
std_msgs::Int64 l_enc_value, r_enc_value;

void left_callback(const std_msgs::Int32& msg)
{
  pwm_left = msg.data;
  run_cmd();
}
void right_callback(const std_msgs::Int32&msg)
{
  pwm_right = msg.data;
  run_cmd();
}

ros::Publisher l_enc_pub("lwheel_enc", &l_enc_value);
ros::Publisher r_enc_pub("rwheel_enc", &r_enc_value);
ros::Subscriber<std_msgs::Int32> l_motor_sub("lwheel_motor_cmd_pub", left_callback);
ros::Subscriber<std_msgs::Int32> r_motor_sub("rwheel_motor_cmd_pub", right_callback);
//---------------------------------------------------------------//

void setup_pins()
{
  pinMode(enA_left, INPUT);
  pinMode(enB_left, INPUT);
  pinMode(enA_right, INPUT);
  pinMode(enB_right, INPUT);
  pinMode(fmotor_left, OUTPUT);
  pinMode(rmotor_left, OUTPUT);
  pinMode(fmotor_right, OUTPUT);
  pinMode(rmotor_right, OUTPUT);
  attachInterrupt(0, _interrupt_left, FALLING);
  attachInterrupt(1, _interrupt_right, FALLING);
  analogWrite(fmotor_left, 0);
  analogWrite(rmotor_left, 0);
  analogWrite(fmotor_right, 0);
  analogWrite(rmotor_right, 0);
}

//------------------control motors (Pwm)------------------------------//
void run_cmd()
{
  analogWrite(fmotor_left, (pwm_left < 0) ? -pwm_left : 0);
  analogWrite(rmotor_left, (pwm_left < 0) ? 0 : pwm_left);

  analogWrite(fmotor_right, (pwm_right < 0) ? -pwm_right : 0);
  analogWrite(rmotor_right, (pwm_right < 0) ? 0 : pwm_right);
}
//--------------------------------------------------------------------//

//------------------interrupt functions (Read encoder)----------------//
void _interrupt_left()
{
  if(digitalRead(enB_left))
    pulse_left--;
  else
    pulse_left++;
}

void _interrupt_right()
{
  if(digitalRead(enB_right))
    pulse_right--;
  else
    pulse_right++;
}
//-------------------------------------------------------------------//

void setup() 
{
  setup_pins();
  nh.initNode();
  nh.advertise(l_enc_pub);
  nh.advertise(r_enc_pub);
  nh.subscribe(l_motor_sub);
  nh.subscribe(r_motor_sub);
}

void loop() 
{
  if(millis() - time >= (1.0 /frequency) * 1000)
  {
    l_enc_value.data = pulse_left;
    r_enc_value.data = pulse_right;
    l_enc_pub.publish(&l_enc_value);
    r_enc_pub.publish(&r_enc_value);
  }
  nh.spinOnce();
}
