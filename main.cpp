#include "mbed.h"

DigitalOut trig(D6);
InterruptIn echo(D7);

DigitalOut in1(D4);
DigitalOut in2(D5);
PwmOut motorSpeed(D3);
PwmOut motorSpeed1(D9);

Timer timer;


#define MAX_DISTANCE 55
#define MIN_DISTANCE 30

volatile float distance0 = 0.0f;


void echo_rise()
{
    timer.reset();
    timer.start();
}

void echo_fall()
{
    timer.stop();
    distance0 = (timer.elapsed_time().count() * 0.0343f) / 2.0f;
}


void ultrasonic_trigger()
{
    trig = 0;
    wait_us(2);

    trig = 1;
    wait_us(10);

    trig = 0;
}


void motor_stop()
{
    in1 = 1;
    in2 = 1;
    motorSpeed.write(0);
    motorSpeed1.write(0);
}

void motor_forward()
{
    in1 = 1;
    in2 = 1;
    motorSpeed.write(0.325f);
    motorSpeed1.write(0.325f);
}

bool should_move(float d)
{
    if (d > MAX_DISTANCE)
        return true;

    if (d <= MIN_DISTANCE)
        return false;

    return true;
}


int main()
{
    echo.rise(&echo_rise);
    echo.fall(&echo_fall);

    motorSpeed.period(0.001f);
    motorSpeed1.period(0.001f);

    while (true)
    {
        ultrasonic_trigger();



        motor_forward();

        if (should_move(distance0))
            motor_forward();
        else
            motor_stop();

        ThisThread::sleep_for(100ms);
    }
}