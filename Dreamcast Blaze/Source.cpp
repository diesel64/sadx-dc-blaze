#include "pch.h"
#include "chrmodels.h"
#include "hover.h"
// or #include "stdafx.h" for previous Visual Studio versions

uint16_t Sonic_UpperArmIndices[] = {
	0, 2,
	1, 3,
	4, 6,
	5, 7,
};

uint16_t Sonic_LowerArmIndices[] = {
	0, 2,
	1, 3,
	4, 6,
	5, 7,
};

uint16_t Sonic_LowerArmIndices_Ring[] = {
	0, 10,
	1, 11,
	4, 14,
	5, 15,
};

uint16_t Sonic_UpperLegIndices[] = {
	0, 2,
	1, 3,
	4, 6,
	5, 7,
};

uint16_t Sonic_LowerLegIndices[] = {
	0, 2,
	1, 3,
	4, 6,
	5, 7,
};

uint16_t Sonic_ShoeIndices[] = {
	2, 3,
	12, 8,
	0, 1,
	1, 0,
	17, 13,
	3, 2,
};

uint16_t Sonic_HandIndices[] = {
	4, 15,
	0, 14,
	1, 13,
	5, 12
};

// this does nothing???
static void __cdecl SetLSDColor()
{
	// #FF32A852 would translate to SetMaterialAndSpriteColor(1.0f, 0.19f, 0.65f, 0.32f);
	// To get a float color (range 0-1): 32 in hexadecimal is 50, divided by 255 it gives us 0.19.
	SetMaterial(0.8f, 1.0f, 0.42f, 0.42f);
}

// vectors for disabling the spindash deformations
NJS_VECTOR nullVector[] = { 0.0, 0.0, 0.0 };
NJS_VECTOR scaleVector[] = { 1.0, 1.0, 1.0 };
// probably a better way to do this but i couldn't seem to find anyone that's done it before so whatever
static void __cdecl SetSDPos()
{
	njTranslateV(0, nullVector);
}
static void __cdecl SetSDRot()
{
	njRotateZ(0, 0);
}
static void __cdecl SetSDScale()
{
	njScaleV(0, scaleVector);
}

FunctionHook<void> InitSonicWeldInfo_h(0x7D0B50);

FunctionHook<void, task*> Sonic_Display_h(0x4948C0);

// copied from the disassembly because i couldn't find it in any of the provided header files
void __cdecl ProjectVectorZXY(EntityData1* a1, NJS_VECTOR* a2)
{
	Angle v2; // eax
	float y; // ecx
	float z; // edx
	Angle v5; // eax
	NJS_VECTOR v; // [esp+4h] [ebp-Ch] BYREF

	if (a1)
	{
		y = a2->y;
		z = a2->z;
		v.x = a2->x;
		v.y = y;
		v.z = z;
		njPushMatrixEx();
		njUnitMatrix(0);
		v2 = a1->Rotation.z;
		if (v2)
		{
			njRotateZ(0, (unsigned __int16)v2);
		}
		v5 = a1->Rotation.x;
		if (v5)
		{
			njRotateX(0, (unsigned __int16)v5);
		}
		if (a1->Rotation.y)
		{
			njRotateY(0, (unsigned __int16)-LOWORD(a1->Rotation.y));
		}
		njCalcVector(0, &v, a2);
		njPopMatrix(1u);
	}
}

static void Sonic_Display_r(task* tp)
{
	// hook the original display function
	Sonic_Display_h.Original(tp);

	// various variable declarations, to make referencing the original memory locations simpler
	auto twp = tp->twp;
	motionwk2* mwp = (motionwk2*)tp->mwp;
	playerwk* co2 = (playerwk*)tp->mwp->work.l;
	auto pwp = (playerwk*)tp->mwp->work.ptr;

	//get player position
	NJS_VECTOR pos = twp->pos;
	//get player forward vector
	NJS_VECTOR unit = { 1.0f, 0.0f, 0.0f };
	ProjectVectorZXY(EntityData1Ptrs[0], &unit);
	//get player up vector
	NJS_VECTOR up;
		up.x = sin(twp->ang.x) * sin(twp->ang.y);
		up.y = cos(twp->ang.x);
		up.z = sin(twp->ang.x) * cos(twp->ang.y);
	//fire position vector
	NJS_VECTOR a = { pos.x + up.x, pos.y + up.y, pos.z + up.z };
	//fire velocity vector
	NJS_VECTOR a2a = { 0.0, 0.10, 0.0 };
	
	//if on the ground, do fire check
	if (EntityData1Ptrs[0]->Status & (Status_Ground))
	{
		//if running or at max speed, continue
		if (pwp->mj.action == 12 || pwp->mj.action == 13)
		{
			//if animation frame has either foot on the ground, create fire particles
			if ((pwp->mj.nframe >= 30 || pwp->mj.nframe <= 1) || (pwp->mj.nframe >= 14 && pwp->mj.nframe <= 16))
			{
				CreateFire(&a, &a2a, 0.89999998);
			}
		}
	}

	// if not in the chao garden, continue (fire sound slot loads chao babbling while in gardens)
	if (!IsChaoGarden)
	{
		switch (twp->mode)
		{
			case 8:
			case 9:
			case 12: // check for hover input if jumping, launched, or falling
				if (hover_CheckInput(twp, pwp))
				{
					twp->flag &= ~Status_Attack;
					twp->flag &= ~Status_Ball;
					//PlaySound(SE_UNI_FIRE, 0, 0, 0);
					dsPlay_oneshot_v(SE_UNI_FIRE, 0, 0, 0, pos.x, pos.y, pos.z);
				}
				break;
			case 110: //don't do anything if currently hovering
				break;
			default: // reset hover usage if doing anything else
				hoverUsedGlobal = false;
				break;
		}
		// if in the hover action, continue
		if (twp->mode == 110)
		{
			// set fire position relative to current player forward vector
			a = { pos.x + unit.x * -4, pos.y+5, pos.z + unit.z * -4 };
			// set velocity
			a2a.y = -1.5;
			// perform hover physics
			hover_Physics(twp, mwp, co2);
			// create fire particles
			CreateFire(&a, &a2a, 0.89999998);
			// if timer is nonzero and the hover button is held, decrement the timer
			if ((hoverTimerGlobal > 0) && ((ButtonBits_Y & Controllers[0].HeldButtons) != 0) && !(EntityData1Ptrs[0]->Status & (Status_Ground)))
			{
				hoverTimerGlobal--;
			}
			else // otherwise, stop hovering
			{
				twp->mode = 12; // set action to falling
				co2->mj.reqaction = 18; // set animation to falling
				dsStop_num(SE_UNI_FIRE); // stop fire sound
			}
		}
	}
}

HelperFunctions HelperFunctionsGlobal;

#define ReplacePNG(a) _snprintf_s(pathbuf, LengthOfArray(pathbuf), "%s\\textures\\pvr_common\\index.txt", path); \
        helperFunctions.ReplaceFile("system\\" a ".PVR", pathbuf);

#define ReplacePNG_StageE(a) _snprintf_s(pathbuf, LengthOfArray(pathbuf), "%s\\textures\\pvr_stage_en\\index.txt", path); \
        helperFunctions.ReplaceFile("system\\" a ".PVR", pathbuf);

void PVR_Init(const char* path, const HelperFunctions& helperFunctions)
{
	char pathbuf[MAX_PATH];

	// snowboard
	ReplacePNG("MIW_B001");

	// stage title cards
	ReplacePNG_StageE("S_STAGE01_E");
	ReplacePNG_StageE("S_STAGE02_E");
	ReplacePNG_StageE("S_STAGE03_E");
	ReplacePNG_StageE("S_STAGE04_E");
	ReplacePNG_StageE("S_STAGE05_E");
	ReplacePNG_StageE("S_STAGE06_E");
	ReplacePNG_StageE("S_STAGE07_E");
	ReplacePNG_StageE("S_STAGE08_E");
	ReplacePNG_StageE("S_STAGE09_E");
	ReplacePNG_StageE("S_STAGE10_E");
}

void Blaze_JumpSound()
{
	PlaySound(SE_BOMB2, 0, 0, 0);
}

extern "C"
{
	void SetSonicWeldInfo(int id, int base, int modelA, int modelB, uint16_t* indices, size_t count)
	{
		SonicWeldInfo[id].BaseModel = SONIC_OBJECTS[base];
		SonicWeldInfo[id].ModelA = SONIC_OBJECTS[modelA];
		SonicWeldInfo[id].ModelB = SONIC_OBJECTS[modelB];
		SonicWeldInfo[id].WeldType = 2; // model linking weld
		SonicWeldInfo[id].VertIndexes = indices;
		SonicWeldInfo[id].VertexPairCount = static_cast<uint8_t>(count / 2);
		SonicWeldInfo[id].anonymous_5 = 0;
		SonicWeldInfo[id].VertexBuffer = 0;
	}
	
	void __cdecl InitNPCSonicWeldInfo_r()
	{
		memcpy(NPCSonicWeldInfo, SonicWeldInfo, sizeof(WeldInfo) * 15);
		NPCSonicWeldInfo[15] = {};
	}

	void __cdecl InitSonicWeldInfo_r()
	{
		// Call original to allow the game to initialize Metal Sonic welds.
		InitSonicWeldInfo_h.Original();
		
		//Blaze
		SetSonicWeldInfo(0, 0, 1, 2, arrayptrandlength(Sonic_UpperArmIndices));
		SetSonicWeldInfo(1, 0, 2, 3, arrayptrandlength(Sonic_LowerArmIndices));
		SetSonicWeldInfo(2, 0, 7, 8, arrayptrandlength(Sonic_UpperArmIndices));
		SetSonicWeldInfo(3, 0, 8, 9, arrayptrandlength(Sonic_LowerArmIndices));
		SetSonicWeldInfo(4, 0, 12, 13, arrayptrandlength(Sonic_UpperLegIndices));
		SetSonicWeldInfo(5, 0, 13, 14, arrayptrandlength(Sonic_LowerLegIndices));
		SetSonicWeldInfo(6, 0, 17, 18, arrayptrandlength(Sonic_UpperLegIndices));
		SetSonicWeldInfo(7, 0, 18, 19, arrayptrandlength(Sonic_LowerLegIndices));
		SetSonicWeldInfo(8, 0, 15, 16, arrayptrandlength(Sonic_ShoeIndices));
		SetSonicWeldInfo(9, 0, 20, 21, arrayptrandlength(Sonic_ShoeIndices));
		SetSonicWeldInfo(10, 0, 10, 11, arrayptrandlength(Sonic_HandIndices));
		SetSonicWeldInfo(11, 0, 4, 5, arrayptrandlength(Sonic_HandIndices));
		SetSonicWeldInfo(12, 0, 58, 59, arrayptrandlength(Sonic_ShoeIndices)); // light speed shoes
		SetSonicWeldInfo(13, 0, 60, 61, arrayptrandlength(Sonic_ShoeIndices)); // light speed shoes
		SetSonicWeldInfo(14, 0, 2, 63, arrayptrandlength(Sonic_LowerArmIndices_Ring)); // crystal ring wrist

		//Burning Blaze
		SetSonicWeldInfo(22, 22, 23, 24, arrayptrandlength(Sonic_UpperArmIndices));
		SetSonicWeldInfo(23, 22, 24, 25, arrayptrandlength(Sonic_LowerArmIndices));
		SetSonicWeldInfo(24, 22, 28, 29, arrayptrandlength(Sonic_UpperArmIndices));
		SetSonicWeldInfo(25, 22, 29, 30, arrayptrandlength(Sonic_LowerArmIndices));
		SetSonicWeldInfo(26, 22, 33, 34, arrayptrandlength(Sonic_UpperLegIndices));
		SetSonicWeldInfo(27, 22, 34, 35, arrayptrandlength(Sonic_LowerLegIndices));
		SetSonicWeldInfo(28, 22, 38, 39, arrayptrandlength(Sonic_UpperLegIndices));
		SetSonicWeldInfo(29, 22, 39, 40, arrayptrandlength(Sonic_LowerLegIndices));
		SetSonicWeldInfo(30, 22, 36, 37, arrayptrandlength(Sonic_ShoeIndices));
		SetSonicWeldInfo(31, 22, 41, 42, arrayptrandlength(Sonic_ShoeIndices));
		SetSonicWeldInfo(32, 22, 31, 32, arrayptrandlength(Sonic_HandIndices));
		SetSonicWeldInfo(33, 22, 26, 27, arrayptrandlength(Sonic_HandIndices));
		SetSonicWeldInfo(34, 22, 31, 32, arrayptrandlength(Sonic_HandIndices));
		SetSonicWeldInfo(35, 22, 26, 27, arrayptrandlength(Sonic_HandIndices));
	}

	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		// Executed at startup, contains helperFunctions and the path to your mod (useful for getting the config file.)
		// This is where we override functions, replace static data, etc.

		HelperFunctionsGlobal = helperFunctions;

		// replace sonic model
		WriteBlazeModel();

		// replace weld data
		InitSonicWeldInfo_h.Hook(InitSonicWeldInfo_r);
		WriteJump(InitNPCSonicWeldInfo, InitNPCSonicWeldInfo_r);

		// disable shoe morphs
		WriteData<1>((void*)0x493500, 0xC3);

		// set aura color
		WriteCall(reinterpret_cast<void*>(0x4A1705), SetLSDColor);

		// disable sonic face motion
		WriteData<1>((int*)0x493730, 0xC3);

		// disable spin dash deformation
		WriteCall((void*)0x494A88, SetSDPos);
		WriteCall((void*)0x494A94, SetSDRot);
		WriteCall((void*)0x494AB7, SetSDScale);

		// blaze's actions! (not sure why i hooked the display to do this but it works)
		Sonic_Display_h.Hook(Sonic_Display_r);

		//  replace pvr textures
		PVR_Init(path, helperFunctions);

		// play a different jump sound
		WriteCall((void*)0x495EAA, Blaze_JumpSound);

		// find a way to disable this:
		//___SONIC_OBJECTS[56]
	}

	__declspec(dllexport) void __cdecl OnInitEnd()
	{
		// Executed after every mod has been initialized, mainly used to check if a specific mod is also enabled.
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		// Executed every running frame of SADX
	}

	__declspec(dllexport) void __cdecl OnInput()
	{
		// Executed before the game processes input
	}

	__declspec(dllexport) void __cdecl OnControl()
	{
		// Executed when the game processes input
	}

	__declspec(dllexport) void __cdecl OnRenderDeviceReset()
	{
		// Executed when the window size changes
	}

	__declspec(dllexport) void __cdecl OnRenderDeviceLost()
	{
		// Executed when the game fails to render the scene
	}

	__declspec(dllexport) void __cdecl OnRenderSceneStart()
	{
		// Executed before the game starts rendering the scene
	}

	__declspec(dllexport) void __cdecl OnRenderSceneEnd()
	{
		// Executed when the game finishes rendering the scene
	}

	__declspec(dllexport) void __cdecl OnExit()
	{
		// Executed when the game is about to terminate
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer }; // This is needed for the Mod Loader to recognize the DLL.
}
