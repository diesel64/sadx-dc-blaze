#include "pch.h"
#include "chrmodels.h"
#include "hover.h"
#include "tornado.h"
#include "IniFile.hpp"
#pragma warning(disable : 4996)
// or #include "stdafx.h" for previous Visual Studio versions

// this file is very unstructured and messy and could probably be split up but it's cool i guess at least it works

// global reference for helperfunctions
HelperFunctions HelperFunctionsGlobal;

// how often to play the "woah!" voice clip when falling
int voiceChance;

// wowow i wonder what these could be for???
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

// overwriting PlayEggmanVoice does nothing, so this is fine i guess - duplicate of vanilla with an extra check
void __cdecl PlayVoice_New(int a1)
{
	// say "she" instead of "he", but only as blaze since tails uses the same eggman audio
	if (GetCurrentCharacterID() == Characters_Sonic)
	{
		if (a1 == 176 || a1 == 1420)
			a1 = 65109;
	}
	if (VoicesEnabled)
	{
		CurrentVoiceNumber = a1;
	}
}

// duplicate of vanilla - dreamcast conversion's fixed version cause a crash when the spindash ball aura thing is disabled
void __cdecl sub_4A0E70(ObjectMaster* a1)
{
	EntityData1* v1; // esi
	Angle v2; // eax
	Angle v3; // eax
	Float x; // [esp+0h] [ebp-20h]
	Float y; // [esp+4h] [ebp-1Ch]
	Float z; // [esp+8h] [ebp-18h]
	NJS_ARGB a1a; // [esp+10h] [ebp-10h] BYREF

	v1 = a1->Data1;
	BackupConstantAttr();
	AddConstantAttr((NJD_FLAG)0, NJD_FLAG_IGNORE_LIGHT | NJD_FLAG_USE_ALPHA);
	a1a.b = 0.0;
	a1a.g = 0.0;
	a1a.r = 0.0;
	a1a.a = (double)(unsigned __int16)v1->InvulnerableTime * -1.0 * 0.2;
	SetMaterialAndSpriteColor(&a1a);
	njSetTexture((NJS_TEXLIST*)v1->LoopData);
	njPushMatrix(0);
	z = v1->Scale.z + v1->Position.z;
	y = v1->Scale.y + v1->Position.y;
	x = v1->Scale.x + v1->Position.x;
	njTranslate(0, x, y, z);
	v2 = v1->Rotation.z;
	if (v2)
	{
		njRotateZ(0, (unsigned __int16)v2);
	}
	v3 = v1->Rotation.x;
	if (v3)
	{
		njRotateX(0, (unsigned __int16)v3);
	}
	if (v1->Rotation.y)
	{
		njRotateY(0, (unsigned __int16)-LOWORD(v1->Rotation.y));
	}
	DrawQueueDepthBias = 1000.0;
	ProcessModelNode_A_WrapperB(SONIC_OBJECTS[56], QueuedModelFlagsB_SomeTextureThing);
	DrawQueueDepthBias = 0.0;
	njPopMatrix(1u);
	ClampGlobalColorThing_Thing();
	RestoreConstantAttr();
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

// self explanatory
FunctionHook<void> InitSonicWeldInfo_h(0x7D0B50);
FunctionHook<void, task*> Sonic_Display_h(0x4948C0);

// copied from the disassembly because i couldn't find it in any of the provided header files :D
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

// did we already create flaming footsteps?
bool madeFire = false;

// did we already play the falling sound?
bool playedSound = false;

// display hook for actions and stuff
static void Sonic_Display_r(task* tp)
{
	// hook the original display function
	Sonic_Display_h.Original(tp);

	// various variable declarations, to make referencing the original memory locations simpler
	auto twp = tp->twp;
	motionwk2* mwp = (motionwk2*)tp->mwp;
	playerwk* co2 = (playerwk*)tp->mwp->work.l;
	auto pwp = (playerwk*)tp->mwp->work.ptr;

	// get player position
	NJS_VECTOR pos = twp->pos;
	// get player forward vector
	NJS_VECTOR unit = { 1.0f, 0.0f, 0.0f };
	ProjectVectorZXY(EntityData1Ptrs[0], &unit);
	// get player up vector
	NJS_VECTOR up;
		up.x = sin(twp->ang.x) * sin(twp->ang.y);
		up.y = cos(twp->ang.x);
		up.z = sin(twp->ang.x) * cos(twp->ang.y);
	// fire position vector
	NJS_VECTOR a = { pos.x + up.x, pos.y + up.y, pos.z + up.z };
	// fire velocity vector
	NJS_VECTOR a2a;
	
	// if on the ground, do fire check
	if (EntityData1Ptrs[0]->Status & (Status_Ground))
	{
		a2a = { (0.1f * up.x), (0.1f * up.y), (0.1f * up.z) };
		// if running, create for each footstep
		if (pwp->mj.action == 12)
		{
			// if animation frame has either foot on the ground, create fire particles
			if ((pwp->mj.nframe >= 30 || pwp->mj.nframe <= 1) || (pwp->mj.nframe >= 14 && pwp->mj.nframe <= 16))
			{
				if (!madeFire)
				{
					CreateFire(&a, &a2a, 0.89999998);
					madeFire = true;
				}
			}
			else {
				madeFire = false;
			}
		}
		// if at max speed, constantly create fire
		else if (pwp->mj.action == 13)
		{
			CreateFire(&a, &a2a, 0.89999998);
		}
	}

	int rngVoiceClip = rand() % 100 + 1;

	// play falling audio
	if (co2->mj.action == 18)
	{
		if (!playedSound && pwp->mj.nframe >= 55)
		{
			if (rngVoiceClip < voiceChance && co2->spd.y < 0)
			{
				if (VoiceLanguage && !IsInDeathZone_(EntityData1Ptrs[0]))
				{
					PlayVoice(54321);
				}
			}
			playedSound = true;
		}
		if (playedSound && pwp->mj.nframe < 40)
		{
			pwp->mj.nframe = 40;
		}
	}
	else
	{
		playedSound = false;
	}

	// if not in the chao garden, continue (flamethrower sound slot loads chao babbling while in gardens)
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
					dsPlay_oneshot_v(SE_UNI_FIRE, 0, 0, 32, pos.x, pos.y, pos.z);
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
			a2a = { 0, -1.5, 0 };
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

// replace snowboard texture
#define ReplacePNG(a) _snprintf_s(pathbuf, LengthOfArray(pathbuf), "%s\\textures\\pvr_common\\index.txt", path); \
        helperFunctions.ReplaceFile("system\\" a ".PVR", pathbuf);

// replace title card textures
#define ReplacePNG_StageE(a) _snprintf_s(pathbuf, LengthOfArray(pathbuf), "%s\\textures\\pvr_stage_en\\index.txt", path); \
        helperFunctions.ReplaceFile("system\\" a ".PVR", pathbuf);

// replace the pvr textures
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

// replace the default jump sound with a fiery one
void Blaze_JumpSound()
{
	PlaySound(SE_BOMB2, 0, 32, 0);
}

// burning blaze's scrolling textures
void burningBlazeScroll()
{
	for (int i = 0; i < 27; i++)
	{
		uv_8DB372077395BE39B46[i].u += 24; // right wrist
		switch (i) { // right wrist switch case
		case 1: case 3: case 5: case 6: case 7: case 10: case 11: case 16: case 17: case 20: case 21: case 22: case 24: case 26:
			if (uv_8DB372077395BE39B46[i].u > 512) { uv_8DB372077395BE39B46[i].u = 256; }
			break;
		default:
			if (uv_8DB372077395BE39B46[i].u > 256) { uv_8DB372077395BE39B46[i].u = 0; }
			break;
		}
	}
	for (int i = 0; i < 29; i++)
	{
		uv_8DB372074B70C15AB6D[i].u += 24; // left wrist
		switch (i) { // left wrist switch case
		case 0: case 2: case 4: case 6: case 8: case 10: case 13: case 15: case 17: case 19: case 21: case 23: case 24: case 26: case 28:
			if (uv_8DB372074B70C15AB6D[i].u > 512) { uv_8DB372074B70C15AB6D[i].u = 256; }
			break;
		default:
			if (uv_8DB372074B70C15AB6D[i].u > 256) { uv_8DB372074B70C15AB6D[i].u = 0; }
			break;
		}
	}
	for (int i = 0; i < 29; i++)
	{
		uv_8DB37207C2178CC5D96[i].u += 24; // right ankle
		switch (i) { // right ankle switch case
		case 1: case 3: case 5: case 7: case 9: case 11: case 13: case 15: case 17: case 19: case 21: case 23: case 25: case 27: case 29:
			if (uv_8DB37207C2178CC5D96[i].u > 512) { uv_8DB37207C2178CC5D96[i].u = 256; }
			break;
		default:
			if (uv_8DB37207C2178CC5D96[i].u > 256) { uv_8DB37207C2178CC5D96[i].u = 0; }
			break;
		}
	}
	for (int i = 0; i < 29; i++)
	{
		uv_8DB372079D61F441885[i].u += 24; // left ankle
		switch (i) { // left ankle switch case
		case 1: case 3: case 5: case 6: case 8: case 10: case 13: case 15: case 17: case 19: case 21: case 23: case 25: case 27: case 29:
			if (uv_8DB372079D61F441885[i].u > 512) { uv_8DB372079D61F441885[i].u = 256; }
			break;
		default:
			if (uv_8DB372079D61F441885[i].u > 256) { uv_8DB372079D61F441885[i].u = 0; }
			break;
		}
	}
}

// used to change the animations used on the character select
DataArray(NJS_ACTION *, dword_3C5FF94, 0x3C5FF94, 4);
DataPointer(NJS_ACTION, dword_3C5E884, 0x3C5E884);
void __cdecl InitBlazeCharSelAnims()
{
	dword_3C5FF94[0] = SONIC_ACTIONS[1];
	dword_3C5FF94[1] = SONIC_ACTIONS[100];
	dword_3C5FF94[2] = &dword_3C5E884;
	dword_3C5FF94[3] = 0;
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

		// read the config file (didn't want one but realized some people might not like hearing "woah!" whenever they fall for more than like 2 seconds)
		const IniFile* config = new IniFile(std::string(path) + "\\config.ini");

		// set the falling voice clip chance
		voiceChance = config->getInt("", "voiceChance", 100);

		// replace sonic model
		WriteBlazeModel();

		// replace weld data
		InitSonicWeldInfo_h.Hook(InitSonicWeldInfo_r);
		WriteJump(InitNPCSonicWeldInfo, InitNPCSonicWeldInfo_r);

		// disable shoe morphs
		WriteData<1>((void*)0x493500, 0xC3);

		// disable sonic face motion (temporary - working on the cutscene head)
		WriteData<1>((int*)0x493730, 0xC3);

		// disable spin dash deformation
		WriteCall((void*)0x494A88, SetSDPos);
		WriteCall((void*)0x494A94, SetSDRot);
		WriteCall((void*)0x494AB7, SetSDScale);

		// disable spin dash aura ball
		SONIC_OBJECTS[56]->basicdxmodel = NULL;
		WriteJump((void*)0x004A0E70, sub_4A0E70);

		// blaze's actions! (not sure why i hooked the display to do this but it works)
		Sonic_Display_h.Hook(Sonic_Display_r);

		//  replace pvr textures
		PVR_Init(path, helperFunctions);

		// play a different jump sound for blaze
		WriteCall((void*)0x495EAA, Blaze_JumpSound);

		// replace sonic in the tornado stages
		Tornado_init();

		// update character select animations
		WriteJump((void*)0x7D24C0, InitBlazeCharSelAnims);

		// set light speed aura color (does nothing with the dreamcast conversion enabled haha)
		WriteCall((void*)0x4A1705, SetLSDColor);

		// make eggman refer to the player as female only when playing as blaze
		WriteJump((void*)0x425710, PlayVoice_New);
		
		// resize blaze's texlist because for some reason the chao from the cream mod use blaze's fireball textures (just kidding this does nothing???)
		SONIC_TEXLIST.nbTexture = 31;

		// replace textures
		helperFunctions.ReplaceFile("system\\SONIC.pvm", "system\\BLAZE_DC.pvm");
		helperFunctions.ReplaceFile("system\\SUPERSONIC.pvm", "system\\BURNINGBLAZE_DC.pvm");
		helperFunctions.ReplaceFile("system\\SON_EFF.pvm", "system\\BLZ_EFF_DC.pvm");

		// close the config file
		delete config;
	}

	__declspec(dllexport) void __cdecl OnInitEnd()
	{
		// Executed after every mod has been initialized, mainly used to check if a specific mod is also enabled.

		// cream support
		HMODULE CreamDC = GetModuleHandle(L"CreamtheRabbit(SA1-Style)");
		if (CreamDC)
		{
			HelperFunctionsGlobal.ReplaceFile("system\\sounddata\\voice_us\\wma\\64871.wma", "system\\sounddata\\voice_us\\wma\\64871b.wma");
			HelperFunctionsGlobal.ReplaceFile("system\\sounddata\\voice_us\\wma\\0493.wma", "system\\sounddata\\voice_us\\wma\\0493b.wma");
			HelperFunctionsGlobal.ReplaceFile("system\\sounddata\\voice_us\\wma\\0494.wma", "system\\sounddata\\voice_us\\wma\\0494b.wma");
			HelperFunctionsGlobal.ReplaceFile("system\\sounddata\\voice_us\\wma\\0496.wma", "system\\sounddata\\voice_us\\wma\\0496b.wma");
			// replace "he's not gonna get away with this" entirely since cream is also a girl
			HelperFunctionsGlobal.ReplaceFile("system\\sounddata\\voice_us\\wma\\0176.wma", "system\\sounddata\\voice_us\\wma\\65109.wma");
		}

		// female tails support if you're into that sort of thing i guess otherwise there wouldn't be like two separate mods for it
		HMODULE FemTails = GetModuleHandle(L"FemaleTails");
		if (FemTails)
		{
			HelperFunctionsGlobal.ReplaceFile("system\\sounddata\\voice_us\\wma\\0496.wma", "system\\sounddata\\voice_us\\wma\\0496b.wma");
			// replace "he's not gonna get away with this" entirely since tails is now a girl (good for her)
			HelperFunctionsGlobal.ReplaceFile("system\\sounddata\\voice_us\\wma\\0176.wma", "system\\sounddata\\voice_us\\wma\\65109.wma");
		}
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		// Executed every running frame of SADX

		// scrolling textures for burning blaze
		burningBlazeScroll();
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
