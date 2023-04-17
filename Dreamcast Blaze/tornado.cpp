#include "pch.h"
#include "SADXModLoader.h"
#include "tornado.h"
#include "TornadoModels.h"

static bool isBlaze(NJS_MODEL_SADX* model) // if i could use a switch i would. this is awful
{
	// sky chase act 1
	if (model == &attach_00059D20) { return true; } // head
	if (model == &attach_00057000) { return true; } // torso
	if (model == &attach_00057108) { return true; } // tail
	if (model == &attach_00057B94) { return true; } // right upper arm
	if (model == &attach_00057E94) { return true; } // right lower arm
	if (model == &attach_0005736C) { return true; } // left upper arm
	if (model == &attach_0005766C) { return true; } // left lower arm

	// sky chase act 2 before transformation
	if (model == &attach_000E4310) { return true; } // head
	if (model == &attach_000E1600) { return true; } // torso
	if (model == &attach_000E1708) { return true; } // tail
	if (model == &attach_000E2194) { return true; } // right upper arm
	if (model == &attach_000E2494) { return true; } // right lower arm
	if (model == &attach_000E1960) { return true; } // left upper arm
	if (model == &attach_000E1C60) { return true; } // left lower arm

	// sky chase act 2 transformation event
	if (model == &attach_00143E94) { return true; } // head
	if (model == &attach_0013EBE0) { return true; } // torso
	if (model == &attach_0013ED64) { return true; } // tail
	if (model == &attach_0013F7E0) { return true; } // right upper arm
	if (model == &attach_0013FAE0) { return true; } // right lower arm
	if (model == &attach_0013EFBC) { return true; } // left upper arm
	if (model == &attach_0013F2C8) { return true; } // left lower arm
	if (model == &attach_001416BC) { return true; } // right shoe heel
	if (model == &attach_00141274) { return true; } // right shoe toe
	if (model == &attach_001409F4) { return true; } // left shoe heel
	if (model == &attach_001405AC) { return true; } // left shoe toe

	// sky chase act 2 after transformation
	if (model == &attach_000FA7A4) { return true; } // head
	if (model == &attach_000F7A88) { return true; } // torso
	if (model == &attach_000F7B90) { return true; } // tail
	if (model == &attach_000F861C) { return true; } // right upper arm
	if (model == &attach_000F891C) { return true; } // right lower arm
	if (model == &attach_000F7DE8) { return true; } // left upper arm
	if (model == &attach_000F80E8) { return true; } // left lower arm

	return false;
}

static void __cdecl TornadoCallBack(NJS_MODEL_SADX* model, LATE flgs)
{
	// if the model is part of blaze use the sonic texlist instead
	if (isBlaze(model))
	{
		// store tornado texlist in memory
		NJS_TEXLIST* tex_orig = CurrentTexList;
		// load sonic texlist
		njSetTexture(&SONIC_TEXLIST);
		// draw model
		DrawModelMesh(model, flgs);
		// reload tornado texlist so the rest of the model looks normal
		njSetTexture(tex_orig);
	}
	else
	{
		// draw model
		DrawModelMesh(model, flgs);
	}
}

static void __cdecl njAction_Queue_Tornado(NJS_ACTION* action, float frame, LATE flags)
{
	// call custom draw model function
	DrawAction(action, frame, flags, 0.0f, TornadoCallBack);
}

void Tornado_init()
{
	// sky chase act 1
	WriteData((NJS_OBJECT**)0x0028B7A0C, &Tornado1_Object);
	WriteData((NJS_OBJECT**)0x0028BA71C, &Tornado1_Object);
	WriteData((NJS_OBJECT**)0x0028BDDBC, &Tornado1_Object);
	WriteData((NJS_OBJECT**)0x0028C09FC, &Tornado1_Object);

	// sky chase act 2 before transformation
	WriteData((NJS_OBJECT**)0x0027EFDDC, &Tornado2Before_Object);
	WriteData((NJS_OBJECT**)0x0027F2AA4, &Tornado2Before_Object);
	WriteData((NJS_OBJECT**)0x0027F612C, &Tornado2Before_Object);
	WriteData((NJS_OBJECT**)0x0027F8974, &Tornado2Before_Object);

	// sky chase act 2 transformation event
	WriteData((NJS_OBJECT**)0x0028988FC, &Tornado2Transformation_Object);

	// sky chase act 2 after transformation
	WriteData((NJS_OBJECT**)0x00280F23C, &Tornado2Change_Object);
	WriteData((NJS_OBJECT**)0x002811CE4, &Tornado2Change_Object);
	WriteData((NJS_OBJECT**)0x002814D9C, &Tornado2Change_Object);
	WriteData((NJS_OBJECT**)0x002817514, &Tornado2Change_Object);

	// call custom action queue
	WriteCall((void*)0x62753A, njAction_Queue_Tornado);
}
