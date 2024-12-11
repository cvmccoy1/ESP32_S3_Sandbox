#pragma once

class Heartbeat
{
public:
    // Deleted copy constructor and assignment operator to prevent copies
    Heartbeat(const Heartbeat&) = delete;
    Heartbeat& operator=(const Heartbeat&) = delete;

    static Heartbeat& Instance();

    void StartHeartbeatLED();
    void StopHeartbeatLED();

private:
    // Define the LED state flag 
    bool heartbeatFlag;

    esp_timer_handle_t timer;

    // Private constructor/destructor ensures that this Singleton class can only be created/destroyed inside the class
    Heartbeat();
    ~Heartbeat();
};