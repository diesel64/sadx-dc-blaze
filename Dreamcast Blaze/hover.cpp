#include "pch.h"
#include "SADXModLoader.h"
#include "hover.h"

// very special thanks to https://github.com/Sora-yx for the code used as a reference here

static const char hoverTimer = 70;
char hoverTimerGlobal;
bool hoverUsedGlobal;

FunctionPointer(void, PGetAccelerationGlik, (taskwk* a1, motionwk2* a2, playerwk* a3), 0x448000);
FunctionPointer(void, PGetSpeedGlik, (taskwk* a1, motionwk2* a2, playerwk* a3), 0x0444580);
FunctionPointer(void, PResetPosition, (taskwk* a1, motionwk2* a2, playerwk* a3), 0x43EE70);

signed int hover_CheckInput(taskwk* data, playerwk* co2)
{
	if ((ButtonBits_Y & Controllers[0].PressedButtons) == 0 || hoverUsedGlobal)
	{
		return 0;
	}

	data->mode = 110;
	co2->mj.reqaction = 64;
	co2->spd.y = 0.0f;
	hoverTimerGlobal = hoverTimer;
	hoverUsedGlobal = true;
	//PlayCustomSound(se_hover);
	return 1;
}
//SE_CH_FIRE
void hover_Physics(taskwk* data, motionwk2* data2, playerwk* co2)
{
	PResetAngle(data, data2, co2);
	if (co2->spd.y <= 0.0f)
	{
		PGetAccelerationGlik(data, data2, co2);
		PGetSpeedGlik(data, data2, co2);
	}
	else
	{
		PGetAccelerationAir(data, data2, co2);
		PGetSpeed(data, data2, co2);
	}
	PSetPosition(data, data2, co2);
	PResetPosition(data, data2, co2);
}
