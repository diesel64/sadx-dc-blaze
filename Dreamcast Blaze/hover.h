#pragma once

extern Buttons hoverButton;
extern char hoverTimerGlobal;
extern bool hoverUsedGlobal;
void hover_Physics(taskwk* data, motionwk2* data2, playerwk* co2);
signed int hover_CheckInput(taskwk* data, playerwk* co2);

#define TWP_PNUM(twp) twp->counter.b[0]
