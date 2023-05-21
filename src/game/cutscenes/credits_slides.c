#include "global.h"
#include "game/cutscenes/credits.h"
#include "game/cutscenes/credits_end.h"
#include "core.h"
#include "game/game.h"
#include "sprite.h"
#include "game/screen_transition.h"
#include "task.h"
#include "lib/m4a.h"
#include "game/save.h"

#include "constants/animations.h"

struct CreditsSlidesCutScene {
    Background unk0;
    struct TransitionState unk40;

    u8 creditsVariant;
    u8 unk4D;

    u8 unk4E;

    u8 unk4F;
    u8 unk50;
    u8 unk51;
    u8 unk52;
    u32 unk54;
    u32 unk58;
};

void sub_808F004(void);
void sub_808F148(struct Task *);

static const u16 gUnknown_080E1278[] = {
    234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246,
    247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258,
};

static const u8 gUnknown_080E12AA[] = { 6, 6, 8, 5, 0, 0 };

void CreateCreditsSlidesCutScene(u8 creditsVariant, u8 b, u8 c)
{
    struct Task *t;
    struct CreditsSlidesCutScene *scene = NULL;
    Background *background;
    struct TransitionState *transition = NULL;
    u8 i;

    gDispCnt = 0x1241;
    gBgCntRegs[1] = 0x1C04;
    gBgScrollRegs[1][0] = 0;
    gBgScrollRegs[1][1] = 0;
    gUnknown_03004D80[1] = 0;
    gUnknown_03002280[1][0] = 0;
    gUnknown_03002280[1][1] = 0;
    gUnknown_03002280[1][2] = 0xff;
    gUnknown_03002280[1][3] = 0x20;

    t = TaskCreate(sub_808F004, 0x5C, 0x3100, 0, sub_808F148);
    scene = TaskGetStructPtr(t);
    scene->creditsVariant = creditsVariant;
    scene->unk4E = b;
    scene->unk4F = c;
    scene->unk52 = 0;
    scene->unk54 = 0x96;
    scene->unk50 = 0;

    if (scene->creditsVariant == CREDITS_VARIANT_FINAL_ENDING
        && gLoadedSaveGame->completedCharacters[CHARACTER_AMY]) {
        scene->unk4D = 1;
    } else if (scene->creditsVariant == CREDITS_VARIANT_EXTRA_ENDING
               && gLoadedSaveGame->extraEndingCreditsPlayed) {
        scene->unk4D = 2;
    } else {
        scene->unk4D = 0;
    }

    for (i = 0; i < scene->unk4F; i++) {
        scene->unk50 += gUnknown_080E12AA[i];
    }

    scene->unk51 = scene->unk50 + gUnknown_080E12AA[scene->unk4F];

    transition = &scene->unk40;
    transition->unk0 = 1;
    transition->unk4 = Q_8_8(0);
    transition->unkA = 0;
    transition->speed = 0x200;
    transition->unk8 = 0x3FFF;

    if (gUnknown_080E1278[scene->unk50] != 0) {
        background = &scene->unk0;
        background->graphics.dest = (void *)BG_SCREEN_ADDR(8);
        background->graphics.anim = 0;
        background->tilesVram = (void *)BG_SCREEN_ADDR(28);
        background->unk18 = 0;
        background->unk1A = 0;
        background->unk1C = gUnknown_080E1278[scene->unk50];
        background->unk1E = 0;
        background->unk20 = 0;
        background->unk22 = 0;
        background->unk24 = 0;
        background->unk26 = 0x1E;
        background->unk28 = 0x14;
        background->unk2A = 0;
        background->unk2E = 1;
        sub_8002A3C(background);
    }
}

void sub_808F10C(void);

void sub_808EF38(void)
{
    struct CreditsSlidesCutScene *scene = TaskGetStructPtr(gCurTask);
    struct TransitionState *transition = &scene->unk40;

    transition->unk2 = 1;
    if (NextTransitionFrame(transition) == SCREEN_TRANSITION_COMPLETE) {
        transition->unk4 = Q_8_8(0);
        scene->unk50++;

        if (scene->unk50 < scene->unk51) {
            if (gUnknown_080E1278[scene->unk50] != 0) {
                Background *background = &scene->unk0;
                background->graphics.dest = (void *)BG_SCREEN_ADDR(8);
                background->graphics.anim = 0;
                background->tilesVram = (void *)BG_SCREEN_ADDR(28);
                background->unk18 = 0;
                background->unk1A = 0;
                background->unk1C = gUnknown_080E1278[scene->unk50];
                background->unk1E = 0;
                background->unk20 = 0;
                background->unk22 = 0;
                background->unk24 = 0;
                background->unk26 = 0x1E;
                background->unk28 = 0x14;
                background->unk2A = 0;
                background->unk2E = 1;
                sub_8002A3C(background);
            }
            gCurTask->main = sub_808F004;
        } else {
            gCurTask->main = sub_808F10C;
        }
    }
}

void sub_808F0BC(void);
void sub_808F068(void);

void sub_808F004(void)
{
    struct CreditsSlidesCutScene *scene = TaskGetStructPtr(gCurTask);
    struct TransitionState *transition = &scene->unk40;
    transition->unk2 = 2;

    if (scene->unk4D != 0 && (gPressedKeys & START_BUTTON)) {
        gCurTask->main = sub_808F0BC;
    }

    if (NextTransitionFrame(transition) == SCREEN_TRANSITION_COMPLETE) {
        transition->unk4 = Q_8_8(0);
        gCurTask->main = sub_808F068;
    }
}

void sub_808F068(void)
{
    struct CreditsSlidesCutScene *scene = TaskGetStructPtr(gCurTask);

    if (scene->unk54 != 0) {
        scene->unk54--;
    } else {
        scene->unk54 = 0x96;
        gCurTask->main = sub_808EF38;
    }

    if (scene->unk4D != 0 && (gPressedKeys & START_BUTTON)) {
        gCurTask->main = sub_808F0BC;
    }
}

void sub_808F0BC(void)
{
    struct CreditsSlidesCutScene *scene = TaskGetStructPtr(gCurTask);
    struct TransitionState *transition = &scene->unk40;
    transition->unk2 = 1;
    m4aMPlayFadeOutTemporarily(&gMPlayInfo_BGM, 24);

    if (NextTransitionFrame(transition) == SCREEN_TRANSITION_COMPLETE) {
        transition->unk4 = Q_8_8(0);
        CreateCreditsEndCutScene(scene->creditsVariant);
        TaskDestroy(gCurTask);
    }
}

void sub_808F10C(void)
{
    struct CreditsSlidesCutScene *scene = TaskGetStructPtr(gCurTask);
    scene->unk4F++;
    CreateCreditsCutScene(scene->creditsVariant, scene->unk4E, scene->unk4F);
    TaskDestroy(gCurTask);
}

void sub_808F148(UNUSED struct Task *t)
{
    // unused logic
}