#pragma once

class dc_input
{
public:
    static void Initialize();
    static void ScanPads();
    static void Clear();

    static bool Exit();
    static bool Pause();
    static bool NewGame();

    static bool LaunchBallDown();
    static bool LaunchBallUp();

    static bool MoveLeftPaddleDown();
    static bool MoveLeftPaddleUp();
    static bool MoveRightPaddleDown();
    static bool MoveRightPaddleUp();

    static bool NudgeLeftDown();
    static bool NudgeLeftUp();
    static bool NudgeRightDown();
    static bool NudgeRightUp();
    static bool NudgeUpDown();
    static bool NudgeUpUp();

    static bool Button1();
    static bool Button2();

private:
    static unsigned int dcButtonsDown;
    static unsigned int dcButtonsUp;
    static unsigned int dcButtonsHeld;
    static bool dcLeftTriggerDown;
    static bool dcLeftTriggerUp;
    static bool dcRightTriggerDown;
    static bool dcRightTriggerUp;
};