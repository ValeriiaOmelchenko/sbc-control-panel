#pragma once

namespace States {


    enum class ButtonState {
        Idle,       //is not pressed
        Pressed,    //has just been pressed
        Holding,    //is pressed and held
        Triggered   //is pressed by 3 sec and more
    };
// i hope other states are clear
    enum class LedPattern {
        Off,
        Solid,
        BlinkSlow,
        BlinkFast
    };

    enum class NetworkStatus {
        NoNetwork,
        LocalOnly,
        Connected
    };

}
