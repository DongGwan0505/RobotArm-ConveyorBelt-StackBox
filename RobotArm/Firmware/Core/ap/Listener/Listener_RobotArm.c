/*
 * Listener_RobotArm.c
 *
 *  Created on: Jan 9, 2026
 *      Author: 82109
 */

#include "Listener_RobotArm.h"

hBtn hbtnBaseG;
hBtn hbtnBaseR;

hBtn hbtnShoulderG;
hBtn hbtnShoulderR;

hBtn hbtnElbowG;
hBtn hbtnElbowR;

hBtn hbtnGripperG;
hBtn hbtnGripperR;

void Listener_RobotArm_Init()
{
	Button_Init(&hbtnBaseG,     BTN_ROBOTARM_BASE_G_GPIO, BTN_ROBOTARM_BASE_G_PIN);
	Button_Init(&hbtnBaseR,     BTN_ROBOTARM_BASE_R_GPIO, BTN_ROBOTARM_BASE_R_PIN);

	Button_Init(&hbtnShoulderG, BTN_ROBOTARM_SHOULDER_G_GPIO, BTN_ROBOTARM_SHOULDER_G_PIN);
	Button_Init(&hbtnShoulderR, BTN_ROBOTARM_SHOULDER_R_GPIO, BTN_ROBOTARM_SHOULDER_R_PIN);

	Button_Init(&hbtnElbowG,    BTN_ROBOTARM_ELBOW_G_GPIO, BTN_ROBOTARM_ELBOW_G_PIN);
	Button_Init(&hbtnElbowR,    BTN_ROBOTARM_ELBOW_R_GPIO, BTN_ROBOTARM_ELBOW_R_PIN);

	Button_Init(&hbtnGripperG,  BTN_ROBOTARM_GRIPPER_G_GPIO, BTN_ROBOTARM_GRIPPER_G_PIN);
	Button_Init(&hbtnGripperR,  BTN_ROBOTARM_GRIPPER_R_GPIO, BTN_ROBOTARM_GRIPPER_R_PIN);
}

void Listener_RobotArm_Excute()
{
	Listener_RobotArm_CheckButton();
}

void Listener_RobotArm_CheckButton()
{
    button_state_t btnState;

    // =============================================================
    // (Base) - X축
    // =============================================================

    // [Base]-CW
    btnState = Button_GetState(&hbtnBaseG);
    if (btnState == ACT_PUSHED) {
        osMessagePut(robotEventMsgBox, EVENT_BASE_CW, 0);
    }
    else if (btnState == ACT_RELEASED) {
        osMessagePut(robotEventMsgBox, EVENT_STOP, 0);
    }

    // [Base]-CCW
    btnState = Button_GetState(&hbtnBaseR);
    if (btnState == ACT_PUSHED) {
        osMessagePut(robotEventMsgBox, EVENT_BASE_CCW, 0);
    }
    else if (btnState == ACT_RELEASED) {
        osMessagePut(robotEventMsgBox, EVENT_STOP, 0);
    }


    // =============================================================
    // (Shoulder) - Y축
    // =============================================================

    // [Shoulder]-CW
    btnState = Button_GetState(&hbtnShoulderG);
    if (btnState == ACT_PUSHED) {
        osMessagePut(robotEventMsgBox, EVENT_SHOULDER_CW, 0);
    }
    else if (btnState == ACT_RELEASED) {
        osMessagePut(robotEventMsgBox, EVENT_STOP, 0);
    }

    // [Shoulder]-CCW
    btnState = Button_GetState(&hbtnShoulderR);
    if (btnState == ACT_PUSHED) {
        osMessagePut(robotEventMsgBox, EVENT_SHOULDER_CCW, 0);
    }
    else if (btnState == ACT_RELEASED) {
        osMessagePut(robotEventMsgBox, EVENT_STOP, 0);
    }


    // =============================================================
    //(Elbow) - Z축
    // =============================================================

    // [Elbow]-CW
    btnState = Button_GetState(&hbtnElbowG);
    if (btnState == ACT_PUSHED) {
        osMessagePut(robotEventMsgBox, EVENT_ELBOW_CW, 0);
    }
    else if (btnState == ACT_RELEASED) {
        osMessagePut(robotEventMsgBox, EVENT_STOP, 0);
    }

    // [Elbow]-CCW
    btnState = Button_GetState(&hbtnElbowR);
    if (btnState == ACT_PUSHED) {
        osMessagePut(robotEventMsgBox, EVENT_ELBOW_CCW, 0);
    }
    else if (btnState == ACT_RELEASED) {
        osMessagePut(robotEventMsgBox, EVENT_STOP, 0);
    }


    // =============================================================
    // Gripper
    // =============================================================

    // [Gripper]-CW
    btnState = Button_GetState(&hbtnGripperG);
    if (btnState == ACT_PUSHED) {
        osMessagePut(robotEventMsgBox, EVENT_GRIPPER_CW, 0);
    }
    else if (btnState == ACT_RELEASED) {
        osMessagePut(robotEventMsgBox, EVENT_STOP, 0);
    }

    // [Gripper]-CCW
    btnState = Button_GetState(&hbtnGripperR);
    if (btnState == ACT_PUSHED) {
        osMessagePut(robotEventMsgBox, EVENT_GRIPPER_CCW, 0);
    }
    else if (btnState == ACT_RELEASED) {
        osMessagePut(robotEventMsgBox, EVENT_STOP, 0);
    }
}
