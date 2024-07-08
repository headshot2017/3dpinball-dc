#include "dc_input.h"

#include <kos.h>
#include <dc/maple.h>
#include <dc/maple/controller.h>

unsigned int dc_input::dcButtonsDown = 0;
unsigned int dc_input::dcButtonsUp = 0;
unsigned int dc_input::dcButtonsHeld = 0;
bool dc_input::dcLeftTriggerDown = false;
bool dc_input::dcLeftTriggerUp = false;
bool dc_input::dcRightTriggerDown = false;
bool dc_input::dcRightTriggerUp = false;

static unsigned int nudgeKey, launchKey, leftKey, rightKey, upKey;

void dc_input::Initialize()
{
	cont_btn_callback(0, CONT_START | CONT_A | CONT_B | CONT_X | CONT_Y, (cont_btn_callback_t)arch_exit);

	leftKey = CONT_DPAD_LEFT | CONT_X;
	rightKey = CONT_DPAD_RIGHT | CONT_B;
	launchKey = CONT_DPAD_DOWN | CONT_A;
	upKey = CONT_DPAD_UP;
	nudgeKey = CONT_Y;
}

void dc_input::ScanPads()
{
	static unsigned int lastBtns = 0;
	static bool lastLT = false;
	static bool lastRT = false;

	maple_device_t *cont;
    cont_state_t *state;

    cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
    if (!cont) return;
    state = (cont_state_t*)maple_dev_status(cont);

	dcButtonsDown = state->buttons &~ lastBtns;
	dcButtonsUp = lastBtns &~ state->buttons;
	dcButtonsHeld = state->buttons;

	dcLeftTriggerDown = (state->ltrig >= 64 && !lastLT);
	dcLeftTriggerUp = (state->ltrig < 64 && lastLT);
	dcRightTriggerDown = (state->rtrig >= 64 && !lastRT);
	dcRightTriggerUp = (state->rtrig < 64 && lastRT);

	lastBtns = state->buttons;
	lastLT = state->ltrig >= 64;
	lastRT = state->rtrig >= 64;
}

bool dc_input::Exit()
{
	return false;
}

bool dc_input::Pause()
{
	return dcButtonsDown & CONT_START;
}

bool dc_input::NewGame()
{
	return (dcButtonsHeld & CONT_START) && (dcButtonsDown & CONT_A);
}

bool dc_input::LaunchBallDown()
{
	return !(dcButtonsHeld & nudgeKey) && (dcButtonsDown & launchKey);
}

bool dc_input::LaunchBallUp()
{
	return !(dcButtonsHeld & nudgeKey) && (dcButtonsUp & launchKey);
}

bool dc_input::MoveLeftPaddleDown()
{
	return !(dcButtonsHeld & nudgeKey) && ((dcButtonsDown & leftKey) || dcLeftTriggerDown);
}

bool dc_input::MoveLeftPaddleUp()
{
	return !(dcButtonsHeld & nudgeKey) && ((dcButtonsUp & leftKey) || dcLeftTriggerUp);
}

bool dc_input::MoveRightPaddleDown()
{
	return !(dcButtonsHeld & nudgeKey) && ((dcButtonsDown & rightKey) || dcRightTriggerDown);
}

bool dc_input::MoveRightPaddleUp()
{
	return !(dcButtonsHeld & nudgeKey) && ((dcButtonsUp & rightKey) || dcRightTriggerUp);
}

bool dc_input::NudgeLeftDown()
{
	return (dcButtonsHeld & nudgeKey) && (dcButtonsDown & leftKey);
}

bool dc_input::NudgeLeftUp()
{
	return (dcButtonsHeld & nudgeKey) && (dcButtonsUp & leftKey);
}

bool dc_input::NudgeRightDown()
{
	return (dcButtonsHeld & nudgeKey) && (dcButtonsDown & rightKey);
}

bool dc_input::NudgeRightUp()
{
	return (dcButtonsHeld & nudgeKey) && (dcButtonsUp & rightKey);
}

bool dc_input::NudgeUpDown()
{
	return (dcButtonsHeld & nudgeKey) && (dcButtonsDown & upKey);
}

bool dc_input::NudgeUpUp()
{
	return (dcButtonsHeld & nudgeKey) && (dcButtonsUp & upKey);
}

bool dc_input::SkipError()
{
	return dcButtonsDown & CONT_A;
}