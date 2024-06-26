#include "core.h"
#include "task.h"
#include "malloc_vram.h"

#include "lib/m4a.h"

#include "sakit/globals.h"
#include "sakit/collision.h"

#include "game/stage/collision.h"
#include "game/stage/stage.h"
#include "game/stage/player.h"
#include "game/stage/camera.h"

#include "game/multiplayer/mp_player.h"
#include "game/stage/item_tasks.h"

#include "game/multiplayer/player_unk_1.h"
#include "game/unknown_effect.h"
#include "game/multiplayer/player_unk_2.h"
#include "game/multiplayer/multipak_connection.h"

#include "constants/animations.h"
#include "constants/game_modes.h"
#include "constants/songs.h"

u32 unused_3005838 = 0;
u8 gUnknown_0300583C = 0;

void Task_CreateMultiplayerPlayer(void);
void TaskDestructor_MultiplayerPlayer(struct Task *);

void sub_8017F34(void);
bool32 sub_8018300(void);

u32 sub_80181E0(void);
void sub_8018120(void);
void sub_8016D20(void);
void sub_801707C(void);
void sub_8017670(void);
void sub_8017C28(void);
void LaunchPlayer(s16);

void CreateMultiplayerPlayer(u8 id)
{
    u16 *p;
    Sprite *s;
    struct Task *t = TaskCreate(Task_CreateMultiplayerPlayer, sizeof(MultiplayerPlayer),
                                0x2000, 0, TaskDestructor_MultiplayerPlayer);
    MultiplayerPlayer *player = TASK_DATA(t);
    player->unk56 = id;
    player->unk54 = 0x40;
    player->unk44 = 0;
    player->unk48 = 0;
    player->unk66 = 0;
    player->unk68 = 0;
    player->unk6A = 0;

    if (id != SIO_MULTI_CNT->id) {
        player->unk60 = 30;
    } else {
        player->unk60 = 0;
    }

    player->unk57 = 128;
    player->unk61 = 0;
    player->unk5C = 0;
    player->unk64 = player->unk56;

    if (gGameMode != GAME_MODE_MULTI_PLAYER_COLLECT_RINGS) {
        player->unk5C |= 2;
    }

    player->unk50 = 0;
    player->unk52 = 0;

    if (gGameMode < GAME_MODE_MULTI_PLAYER_COLLECT_RINGS) {
        player->unk50 = gSpawnPositions[gCurrentLevel][0];
        player->unk52 = gSpawnPositions[gCurrentLevel][1];
    } else {
        switch (SIO_MULTI_CNT->id) {
            case 0: {
                player->unk50 = 232;
                player->unk52 = 829;
                break;
            }
            case 1: {
                player->unk50 = 1585;
                player->unk52 = 279;
                break;
            }
            case 2: {
                player->unk50 = 1585;
                player->unk52 = 926;
                break;
            }
            case 3: {
                player->unk50 = 232;
                player->unk52 = 348;
                break;
            }
        }
    }

    s = &player->s;
    s->unk1A = SPRITE_OAM_ORDER(16);
    s->graphics.size = 0;
    s->animCursor = 0;
    s->timeUntilNextFrame = 0;
    s->prevVariant = -1;
    s->animSpeed = SPRITE_ANIM_SPEED(1.0);
    s->palId = player->unk56;
    s->hitboxes[0].index = HITBOX_STATE_INACTIVE;
    s->unk10 = SPRITE_FLAG(PRIORITY, 2);

    s->graphics.anim = 0;
    s->variant = 0;
    s->x = 0;
    s->y = 0;

    player->transform.height = 256;
    s->graphics.anim = gPlayerCharacterIdleAnims[gMultiplayerCharacters[player->unk56]];

    if (player->unk56 != SIO_MULTI_CNT->id) {
        s->graphics.dest = VramMalloc(64);
        s->unk10 |= SPRITE_FLAG_MASK_MOSAIC;
    } else {
        s->graphics.dest = (void *)OBJ_VRAM0;
    }

    UpdateSpriteAnimation(s);
    gMultiplayerPlayerTasks[player->unk56] = t;
}

// around 70%: https://decomp.me/scratch/KNjEN
NONMATCH("asm/non_matching/game/multiplayer/mp_player__Task_CreateMultiplayerPlayer.inc",
         void Task_CreateMultiplayerPlayer())
{
    MultiplayerPlayer *mpp = TASK_DATA(gCurTask);
    Sprite *s = &mpp->s;
    SpriteTransform *transform = &mpp->transform;
    s32 id = mpp->unk56;
    union MultiSioData *recv = &gMultiSioRecv[id];

    u32 i;

    if ((gMultiSioStatusFlags & MULTI_SIO_RECV_ID(id))) {
        mpp->unk61 = 0;
        if (gMultiSioStatusFlags & MULTI_SIO_RECV_ID(id) && recv->pat0.unk0 == 0x5000) {
            s32 x;
            mpp->unk61 = 0;
            if (gGameMode != GAME_MODE_MULTI_PLAYER_COLLECT_RINGS) {
                mpp->unk44 = recv->pat4.x - mpp->unk50;
                mpp->unk48 = recv->pat4.y - mpp->unk52;
                mpp->unk50 = recv->pat4.x;
                mpp->unk52 = recv->pat4.y;

            } else {
                mpp->unk44 = recv->pat4.x - mpp->unk50;
                mpp->unk48 = recv->pat4.y - mpp->unk52;
                x = ABS(mpp->unk44);

                if (x > 15) {
                    x = ABS(mpp->unk48);
                    if (mpp->unk44 > 0 && mpp->unk48 > 0) {
                        mpp->unk44 = recv->pat4.x - mpp->unk50 - 1440;
                        mpp->unk48 = recv->pat4.y - mpp->unk52 - 864;
                    } else {
                        if (mpp->unk44 < 0 && mpp->unk48 < 0) {
                            mpp->unk44 = recv->pat4.x - mpp->unk50 - 1440;
                            mpp->unk48 = recv->pat4.y - mpp->unk52 - 864;
                        }
                    }
                }
                mpp->unk50 = recv->pat4.x;
                mpp->unk52 = recv->pat4.y;
            }
            if (ABS(mpp->unk44) < 0x41) {
                if (ABS(mpp->unk48) >= 0x40) {
                    mpp->unk66 = Q(mpp->unk44);
                    mpp->unk68 = Q(mpp->unk48);
                } else {
                    mpp->unk66 = 0;
                    mpp->unk68 = 0;
                }
            } else {
                mpp->unk66 = 0;
                mpp->unk68 = 0;
            }
            {
                u8 val = recv->pat4.unk10 & ~mpp->unk57;
                if (SIO_MULTI_CNT->id == mpp->unk56) {
                    if (val & 2 && !(mpp->unk57 & 2)) {
                        CreateItemTask_Invincibility(mpp->unk56);
                    }
                    if (val & 0x50 && !(mpp->unk57 & 2)) {
                        CreateItemTask_Confusion(mpp->unk56);
                    }
                }
            }
            mpp->unk57 = recv->pat4.unk10;
            // TODO: not pat4
            mpp->unk54 = recv->pat4.unk8;

            if (SIO_MULTI_CNT->id != mpp->unk56) {
                mpp->unk64 = ((recv->pat4.unk8) & 0x600) >> 9;
            } else {
                if (SIO_MULTI_CNT->id != mpp->unk64) {
                    MultiplayerPlayer *mpp2
                        = TASK_DATA(gMultiplayerPlayerTasks[mpp->unk64]);
                    if (SIO_MULTI_CNT->id == mpp2->unk64) {
                        mpp2->unk64 = SIO_MULTI_CNT->id;
                        gPlayer.moveState &= ~MOVESTATE_8;
                        gPlayer.moveState &= ~MOVESTATE_20;
                        gPlayer.moveState &= ~MOVESTATE_4;
                        gPlayer.moveState &= ~MOVESTATE_10;
                        gPlayer.moveState |= MOVESTATE_IN_AIR;
                        gPlayer.moveState &= ~MOVESTATE_400;
                        gPlayer.moveState &= ~MOVESTATE_100;
                        sub_8023B5C(&gPlayer, 14);
                        gPlayer.unk16 = 6;
                        gPlayer.unk17 = 14;
                        gPlayer.unk61 = 0;
                        gPlayer.unk62 = 0;

                        gPlayer.unk64 = SA2_CHAR_ANIM_WALK;
                        gPlayer.moveState |= MOVESTATE_800000;
                        gPlayer.callback = PlayerCB_8025318;
                        gPlayer.moveState &= ~MOVESTATE_400000;
                        mpp->unk5C &= ~4;
                        if (mpp2->unk5C & 4) {
                            gPlayer.moveState &= ~MOVESTATE_400000;
                            mpp2->unk5C &= ~4;
                        }
                    }
                }
            }

            {
                u32 anim;
                if (gGameMode == GAME_MODE_MULTI_PLAYER_COLLECT_RINGS) {
                    gMultiplayerCharRings[mpp->unk56] = I(recv->pat4.unk6);
                    mpp->unk6A = recv->pat4.unk6;
                    anim = gPlayerCharacterIdleAnims[mpp->unk6A];
                } else {
                    anim = recv->pat4.unk6;
                }

                if (s->graphics.anim != anim || s->variant != (recv->pat4.unkB % 16)) {
                    s->hitboxes[0].index = -1;
                    s->hitboxes[1].index = -1;
                }

                if (mpp->unk54 & 4) {
                    if (mpp->unk5C & 4) {
                        gPlayer.moveState &= ~MOVESTATE_400000;
                        mpp->unk5C &= ~4;
                    }

                    if (gPlayer.moveState & MOVESTATE_8 && gPlayer.unk3C == s) {
                        gPlayer.moveState &= ~MOVESTATE_8;
                        gPlayer.moveState |= MOVESTATE_IN_AIR;
                    }
                }

                s->graphics.anim = anim;
                s->variant = recv->pat4.unk6;
                mpp->unk58[0] = recv->pat4.unkB >> 4;
                s->animSpeed = recv->pat4.unkC;
                transform->rotation = recv->pat4.unkD << 2;
            }
        } else {
            goto thing;
            // mpp->unk50 += I(mpp->unk66);
            // mpp->unk52 += I(mpp->unk68);

            // if (mpp->unk61++ > 30) {
            //     // TODO: macro this
            //     TasksDestroyAll();
            //     gUnknown_03002AE4 = gUnknown_0300287C;
            //     gUnknown_03005390 = 0;
            //     gVramGraphicsCopyCursor = gVramGraphicsCopyQueueIndex;
            //     MultiPakCommunicationError();
            //     return;
            // }
        }
    } else {
    thing:
        mpp->unk50 += I(mpp->unk66);
        mpp->unk52 += I(mpp->unk68);

        if (mpp->unk61++ > 30) {
            // TODO: macro this
            TasksDestroyAll();
            gUnknown_03002AE4 = gUnknown_0300287C;
            gUnknown_03005390 = 0;
            gVramGraphicsCopyCursor = gVramGraphicsCopyQueueIndex;
            MultiPakCommunicationError();
            return;
        }
    }

    {
        s8 i;
        for (i = 3; i >= 0; i--) {
            if (s->graphics.anim >= gPlayerCharacterIdleAnims[i]) {
                break;
            }
        }

        s->x = mpp->unk50 - gCamera.x;
        s->y = mpp->unk52 - gCamera.y;
        transform->x = mpp->unk50 - gCamera.x;
        transform->y = mpp->unk52 - gCamera.y;

        if (mpp->unk54 & 0x800) {
            s->prevVariant = -1;
            s->hitboxes[0].index = -1;
            s->hitboxes[1].index = -1;
        }
        UpdateSpriteAnimation(s);
    }

    if (gGameMode == GAME_MODE_TEAM_PLAY && gUnknown_030054B4[SIO_MULTI_CNT->id] == -1) {
        u32 someBool = TRUE;
        for (i = 0; i < 4; i++) {
            if (gMultiplayerPlayerTasks[i] == NULL) {
                break;
            }

            {
                MultiplayerPlayer *mpp2 = TASK_DATA(gMultiplayerPlayerTasks[i]);
                if ((gMultiplayerConnections & (0x10 << (i))) >> ((i + 4))
                        != (gMultiplayerConnections & (0x10 << (SIO_MULTI_CNT->id)))
                            >> (SIO_MULTI_CNT->id + 4)
                    && !(mpp2->unk54 & 0x100)) {

                    someBool = FALSE;
                    break;
                }
            }
        }

        if (someBool) {
            sub_8019224()->unk0 = 7;
        }
    }

    if (!(gUnknown_03005424 & 1) && !(mpp->unk54 & 0x80000004)) {
        if (!(gPlayer.itemEffect & 0x80)) {
            if (!(mpp->unk5C & 1) && (gPlayer.timerInvulnerability == 0)
                && !(gPlayer.moveState & (MOVESTATE_400000 | MOVESTATE_DEAD))) {
                if (gGameMode != GAME_MODE_MULTI_PLAYER_COLLECT_RINGS) {
                    if (mpp->unk56 != SIO_MULTI_CNT->id) {
                        switch (gMultiplayerCharacters[mpp->unk56]) {

                            case CHARACTER_SONIC:
                            case CHARACTER_CREAM:
                                sub_8016D20();
                                break;
                            case CHARACTER_TAILS:
                                sub_801707C();
                                break;
                            case CHARACTER_KNUCKLES:
                                sub_8017670();
                                break;
                            case CHARACTER_AMY:
                                sub_8017C28();
                                break;
                        }
                    }
                } else {
                    if (mpp->unk56 != SIO_MULTI_CNT->id) {
                        sub_8018120();
                    }
                }
            }
        }
    } else {
        if ((gPlayer.itemEffect & PLAYER_ITEM_EFFECT__80) || (mpp->unk57 & 0x80)
            || gPlayer.timerInvincibility == 0 || !PLAYER_IS_ALIVE
            || gUnknown_030054B4[mpp->unk56] != -1) {
            if (gGameMode != GAME_MODE_MULTI_PLAYER_COLLECT_RINGS) {
                if (mpp->unk56 == SIO_MULTI_CNT->id
                    && gMultiplayerCharacters[mpp->unk56] == CHARACTER_TAILS
                    && gMultiplayerCharacters[mpp->unk56] == CHARACTER_KNUCKLES
                    && mpp->unk5C & 4) {
                    mpp->unk5C &= ~0x6;
                    if (!(gPlayer.itemEffect & PLAYER_ITEM_EFFECT__80)) {
                        gPlayer.moveState &= ~MOVESTATE_400000;
                    }
                    {
                        struct UNK_3005510 *thing = sub_8019224();
                        thing->unk0 = 8;
                        thing->unk1 = mpp->unk56;
                        thing->unk2 = 0;
                    }
                }
                if (gPlayer.moveState & MOVESTATE_8 && gPlayer.unk3C == s) {
                    gPlayer.moveState &= ~(MOVESTATE_8 | MOVESTATE_FACING_LEFT);
                    gPlayer.moveState |= MOVESTATE_IN_AIR;
                    mpp->unk60 = 30;
                }
            } else {
                if (gPlayer.moveState & MOVESTATE_8 && gPlayer.unk3C == s) {
                    gPlayer.moveState &= ~(MOVESTATE_10 | MOVESTATE_IN_AIR);
                    gPlayer.moveState |= MOVESTATE_IN_AIR;
                    mpp->unk60 = 30;
                }
            }
        }
    }

    if (mpp->unk54 & 1) {
        s->unk10 &= ~SPRITE_FLAG_MASK_ROT_SCALE;
        s->unk10 = gUnknown_030054B8++ | 0x20;
        if (mpp->unk54 & 2) {
            transform->width = -256;
        } else {
            transform->width = 256;
        }

        if (mpp->unk54 & 8) {
            transform->width = -transform->width;
        }
        sub_8004860(s, transform);
    } else {
        s->unk10 &= ~0x30;
        if (mpp->unk54 & 2) {
            s->unk10 |= SPRITE_FLAG_MASK_X_FLIP;
        } else {
            s->unk10 &= ~SPRITE_FLAG_MASK_X_FLIP;
        }
        if (mpp->unk54 & 8) {
            s->unk10 |= SPRITE_FLAG_MASK_Y_FLIP;
        } else {
            s->unk10 &= ~SPRITE_FLAG_MASK_Y_FLIP;
        }
    }

    s->unk10 &= ~SPRITE_FLAG_MASK_PRIORITY;
    s->unk10 |= (mpp->unk54 & 0x30) << 8;

    if (!(mpp->unk54 & 0x40)
        && ((gStageTime & 2 || mpp->unk57 & 0x20 || mpp->unk5C & 1
             || gUnknown_030054B4[mpp->unk56] != -1)
            || (mpp->unk60 == 0 && !(mpp->unk54 & 4) && !(mpp->unk5C & 2)))) {
        s->unk1A = 0x400;
        if (mpp->unk54 & 0x80) {
            s->unk1A |= 0x40;
        }

        s->unk10 &= ~(0x100 | 0x80);
        if (mpp->unk57 & 0x20
            && (gGameMode != GAME_MODE_TEAM_PLAY
                || ((gMultiplayerConnections & (0x10 << (mpp->unk56)))
                        >> ((mpp->unk56 + 4))
                    != (gMultiplayerConnections & (0x10 << (SIO_MULTI_CNT->id)))
                        >> (SIO_MULTI_CNT->id + 4)))
            && mpp->unk60 == 0 && mpp->unk56 != SIO_MULTI_CNT->id) {
            s->unk10 |= 0x100;
            gDispCnt |= 0x8000;
            gWinRegs[5] = 0x83F;
        }
        if ((u16)(s->x + 0x3F) < 0x16F && (s->y > -0x40 && s->y < 0xE0)) {
            DisplaySprite(s);
        } else if (gGameMode == 5) {
            if ((gCamera.x + s->x) < 0x3C1) {
                s->x += 0x5A0;
                transform->x += 0x5A0;
                s->y += 0x360;
                transform->y += 0x360;
            } else {
                s->x -= 0x5A0;
                transform->x -= 0x5A0;
                s->y -= 0x360;
                transform->y -= 0x360;
            }

            if ((u16)(s->x + 0x3F) < 0x16F && (s->y > -0x40 && s->y < 0xE0)) {
                if (mpp->unk54 & 1) {
                    sub_8004860(s, transform);
                }
                DisplaySprite(s);
            }
        }
    }

    if (gUnknown_03005B7C != 0) {
        sub_8087368();
        gUnknown_03005B7C = 0;
    }

    if (gUnknown_0300583C != 0) {
        sub_801A384();
        gUnknown_0300583C = 0;
    }
}
END_NONMATCH

void sub_8016D20(void)
{
    Sprite *playerS = &gPlayer.unk90->s;
    MultiplayerPlayer *mpp = TASK_DATA(gCurTask);
    Sprite *s = &mpp->s;
    SpriteTransform *transform = &mpp->transform;
    u32 val;

    if (gPlayer.moveState & MOVESTATE_8 && gPlayer.unk3C == s) {
        sub_8017F34();
    }

    if (gGameMode != GAME_MODE_TEAM_PLAY
        || ((gMultiplayerConnections & (0x10 << (mpp->unk56))) >> ((mpp->unk56 + 4))
            != (gMultiplayerConnections & (0x10 << (SIO_MULTI_CNT->id)))
                >> (SIO_MULTI_CNT->id + 4))) {
        if (!sub_8018300()) {
            return;
        }

        if (!(mpp->unk4C & MOVESTATE_20)) {
            return;
        }

        gPlayer.moveState &= ~MOVESTATE_20;
        mpp->unk4C = 0;
        return;
    }

    if (mpp->unk60 == 0) {
        if (sub_80181E0() != 0) {
            if (mpp->unk4C & MOVESTATE_20) {
                gPlayer.moveState &= ~MOVESTATE_20;
                mpp->unk4C = 0;
            }

            if (s->graphics.anim
                != SA2_ANIM_CHAR(SA2_CHAR_ANIM_SPIN_DASH, CHARACTER_SONIC)) {
                return;
            }

            gPlayer.unk64 = SA2_CHAR_ANIM_WALK;
            gPlayer.callback = PlayerCB_8025318;
            gPlayer.unk61 = 0;
            gPlayer.unk62 = 0;

            if (gPlayer.moveState & MOVESTATE_8 && gPlayer.unk3C == s) {
                gPlayer.moveState &= ~MOVESTATE_8;
                gPlayer.moveState |= MOVESTATE_IN_AIR;
            }

            if (!GRAVITY_IS_INVERTED && I(gPlayer.y) > mpp->unk52) {
                mpp->unk60 = 30;
                return;
            } else if (GRAVITY_IS_INVERTED && I(gPlayer.y) < mpp->unk52) {
                mpp->unk60 = 30;
                return;
            } else if (s->unk10 & SPRITE_FLAG_MASK_X_FLIP) {
                gPlayer.moveState &= ~MOVESTATE_FACING_LEFT;
                gPlayer.moveState &= ~MOVESTATE_20;
                gPlayer.speedGroundX = Q_8_8(12);
                gPlayer.speedAirX = Q_8_8(12);
            } else {
                gPlayer.moveState |= MOVESTATE_FACING_LEFT;
                gPlayer.moveState &= ~MOVESTATE_20;
                gPlayer.speedGroundX = -Q_8_8(12);
                gPlayer.speedAirX = -Q_8_8(12);
            }
            mpp->unk60 = 30;
        } else {
            if (HITBOX_IS_ACTIVE(playerS->hitboxes[1])
                && HITBOX_IS_ACTIVE(s->hitboxes[1])) {
                return;
            }

            if (gPlayer.unk61 != 0
                && (gPlayer.character == CHARACTER_TAILS
                    || gPlayer.character == CHARACTER_KNUCKLES)) {
                return;
            }

            val = sub_800D0A0(s, mpp->unk50, mpp->unk52, mpp->unk66, mpp->unk68,
                              mpp->unk54 >> 7 & 1, 1);

            if (mpp->unk4C & MOVESTATE_20 && !(val & MOVESTATE_20)) {
                gPlayer.moveState &= ~MOVESTATE_20;
            }

            mpp->unk4C = val;

            if (val & MOVESTATE_IGNORE_INPUT) {
                mpp->unk60 = 30;
            }

            if (!(val & (MOVESTATE_10000 | MOVESTATE_20 | MOVESTATE_8))) {
                return;
            }

            if (s->graphics.anim
                != SA2_ANIM_CHAR(SA2_CHAR_ANIM_SPIN_DASH, CHARACTER_SONIC)) {
                return;
            }
            gPlayer.unk64 = SA2_CHAR_ANIM_WALK;
            gPlayer.callback = PlayerCB_8025318;
            gPlayer.unk61 = 0;
            gPlayer.unk62 = 0;

            if (gPlayer.moveState & MOVESTATE_8 && gPlayer.unk3C == s) {
                gPlayer.moveState &= ~MOVESTATE_8;
                gPlayer.moveState |= MOVESTATE_IN_AIR;
            }

            if (!GRAVITY_IS_INVERTED && I(gPlayer.y) > mpp->unk52) {
                mpp->unk60 = 30;
                return;
            } else if (GRAVITY_IS_INVERTED && I(gPlayer.y) < mpp->unk52) {
                mpp->unk60 = 30;
                return;
            } else if (s->unk10 & SPRITE_FLAG_MASK_X_FLIP) {
                gPlayer.moveState &= ~MOVESTATE_FACING_LEFT;
                gPlayer.moveState &= ~MOVESTATE_20;
                gPlayer.speedGroundX = Q(12);
                gPlayer.speedAirX = Q(12);
            } else {
                gPlayer.moveState |= MOVESTATE_FACING_LEFT;
                gPlayer.moveState &= ~MOVESTATE_20;
                gPlayer.speedGroundX = -Q(12);
                gPlayer.speedAirX = -Q(12);
            }
            mpp->unk60 = 30;
        }
    } else {
        mpp->unk60--;

        if (!(gPlayer.moveState & MOVESTATE_8)) {
            return;
        }

        if (gPlayer.unk3C != s) {
            return;
        }

        val = sub_800D0A0(s, mpp->unk50, mpp->unk52, mpp->unk66, mpp->unk68,
                          mpp->unk54 >> 7 & 1, 0);

        if ((mpp->unk4C & MOVESTATE_20) && !(val & MOVESTATE_20)) {
            gPlayer.moveState &= ~MOVESTATE_20;
        }

        mpp->unk4C = val;
        return;
    }
}

#define SOME_INVERTED_GRAVITY_MACRO                                                     \
    (!GRAVITY_IS_INVERTED != !(mpp->unk54 & 8)                                          \
     || !(mpp->unk5C & 0x100) != !GRAVITY_IS_INVERTED)

void sub_801707C(void)
{

    Sprite *playerSprite, *s;
    MultiplayerPlayer *mpp;

    bool8 someBool;
    u32 moveStateVal, val;
    s32 result;
    u8 unusedByte;

    playerSprite = &gPlayer.unk90->s;
    someBool = FALSE;
    mpp = TASK_DATA(gCurTask);
    s = &mpp->s;
    moveStateVal = (gPlayer.moveState >> 5) & 1;

    if (gPlayer.moveState & MOVESTATE_8 && gPlayer.unk3C == s) {
        sub_8017F34();
        someBool = TRUE;
    }

    if (gGameMode != GAME_MODE_TEAM_PLAY
        || ((gMultiplayerConnections & (0x10 << (mpp->unk56))) >> ((mpp->unk56 + 4))
            != (gMultiplayerConnections & (0x10 << (SIO_MULTI_CNT->id)))
                >> (SIO_MULTI_CNT->id + 4))) {
        if (sub_8018300() == 0) {
            return;
        }

        if (!(mpp->unk4C & MOVESTATE_20)) {
            return;
        }

        gPlayer.moveState &= ~MOVESTATE_20;
        mpp->unk4C = 0;
        return;
    }

    if (gPlayer.unk61 != 0 && gPlayer.character == CHARACTER_KNUCKLES) {
        return;
    }

    if (mpp->unk60 == 0 || (mpp->unk5C & 4)) {
        u32 someOtherBool;

        mpp->unk60 = 0;

        if (s->graphics.anim != SA2_ANIM_CHAR(SA2_CHAR_ANIM_22, CHARACTER_TAILS)
            && s->graphics.anim != SA2_ANIM_CHAR(SA2_CHAR_ANIM_21, CHARACTER_TAILS)
            && s->graphics.anim != SA2_ANIM_CHAR(SA2_CHAR_ANIM_20, CHARACTER_TAILS)) {
            if (sub_80181E0()) {
                if (!(mpp->unk4C & 0x20)) {
                    return;
                }

                gPlayer.moveState &= ~MOVESTATE_20;
                mpp->unk4C = 0;
                return;
            }
        }

        if (s->graphics.anim != SA2_ANIM_CHAR(SA2_CHAR_ANIM_22, CHARACTER_TAILS)
            && s->graphics.anim != SA2_ANIM_CHAR(SA2_CHAR_ANIM_21, CHARACTER_TAILS)
            && s->graphics.anim != SA2_ANIM_CHAR(SA2_CHAR_ANIM_20, CHARACTER_TAILS)
            && HITBOX_IS_ACTIVE(playerSprite->hitboxes[1])
            && HITBOX_IS_ACTIVE(s->hitboxes[1])) {
            return;
        }

        someOtherBool = (gPlayer.moveState >> 1) & 1;
        val = sub_800D0A0(s, mpp->unk50, mpp->unk52, mpp->unk66, mpp->unk68,
                          mpp->unk54 >> 7 & 1, 1);

        if ((mpp->unk4C & 0x20) && !(val & 0x20)) {
            gPlayer.moveState &= ~MOVESTATE_20;
        }

        mpp->unk4C = val;

        if (val & 0x200000) {
            mpp->unk60 = 30;
        }

        if (!(mpp->unk5C & 4)) {
            if ((someOtherBool || someBool)
                && (val & 0x10028
                    && (s->graphics.anim
                            == SA2_ANIM_CHAR(SA2_CHAR_ANIM_22, CHARACTER_TAILS)
                        || s->graphics.anim
                            == SA2_ANIM_CHAR(SA2_CHAR_ANIM_21, CHARACTER_TAILS)
                        || s->graphics.anim
                            == SA2_ANIM_CHAR(SA2_CHAR_ANIM_20, CHARACTER_TAILS))
                    && ((!!GRAVITY_IS_INVERTED != !(mpp->unk54 & 8))))) {

                s32 y;
                if (!GRAVITY_IS_INVERTED) {
                    result = sub_801E4E4(MAX(I(gPlayer.y), mpp->unk52) + gPlayer.unk17,
                                         I(gPlayer.x), gPlayer.unk38, 8, &unusedByte,
                                         sub_801EE64);
                } else {
                    result = sub_801E4E4(MIN(I(gPlayer.y), mpp->unk52) - gPlayer.unk17,
                                         I(gPlayer.x), gPlayer.unk38, -8, &unusedByte,
                                         sub_801EE64);
                }

                if (result - gPlayer.unk17 > 0) {
                    gPlayer.moveState |= MOVESTATE_400000;
                    gPlayer.moveState &= ~MOVESTATE_8;

                    mpp->unk5C |= 4;
                    sub_8023B5C(&gPlayer, 14);
                    gPlayer.unk16 = 6;
                    gPlayer.unk17 = 14;

                    {
                        struct UNK_3005510 *thing = sub_8019224();
                        thing->unk0 = 8;
                        thing->unk1 = mpp->unk56;
                        thing->unk2 = 1;
                    }

                    if (!GRAVITY_IS_INVERTED) {
                        mpp->unk5C &= ~0x100;
                    } else {
                        mpp->unk5C |= 0x100;
                    }
                }
            }

            if (!(mpp->unk5C & 4)) {
                return;
            }
        }

        gPlayer.unk64 = SA2_CHAR_ANIM_IDLE;
        gPlayer.callback = PlayerCB_8025318;
        gPlayer.moveState |= MOVESTATE_IN_AIR;
        gPlayer.unk61 = 0;
        gPlayer.unk62 = 0;

        if (sub_8029E6C(&gPlayer)) {
            mpp->unk60 = 30;
            gPlayer.moveState &= ~MOVESTATE_400000;
            mpp->unk5C &= ~4;

        } else {
            if ((s->graphics.anim != SA2_ANIM_CHAR(SA2_CHAR_ANIM_22, CHARACTER_TAILS)
                 && s->graphics.anim != SA2_ANIM_CHAR(SA2_CHAR_ANIM_21, CHARACTER_TAILS)
                 && s->graphics.anim != SA2_ANIM_CHAR(SA2_CHAR_ANIM_20, CHARACTER_TAILS))
                || I(gPlayer.x) <= gCamera.minX || I(gPlayer.x) >= gCamera.maxX
                || SOME_INVERTED_GRAVITY_MACRO || moveStateVal != 0) {
                gPlayer.moveState &= ~MOVESTATE_400000;
                mpp->unk5C &= ~4;
                gPlayer.unk64 = SA2_CHAR_ANIM_IDLE;
                gPlayer.callback = PlayerCB_8025318;
                if (SOME_INVERTED_GRAVITY_MACRO) {
                    gPlayer.timerInvulnerability = 60;
                }
            } else {
                if (gPlayer.moveState & MOVESTATE_8) {
                    gPlayer.moveState &= ~MOVESTATE_400000;
                    mpp->unk5C &= ~4;
                } else {
                    s32 x, y;
                    bool32 invertedGravity = GRAVITY_IS_INVERTED;
                    mpp->unk5C |= 4;
                    x = Q(mpp->unk50);

                    // TODO: potential macro
                    if (!invertedGravity) {
                        y = Q(mpp->unk52 + (s->hitboxes[0].bottom) + 17);
                        result = sub_801E4E4(I(gPlayer.y) + gPlayer.unk17, I(gPlayer.x),
                                             gPlayer.unk38, 8, &unusedByte, sub_801EE64);

                        if (result < 0) {
                            y += Q(result);
                            gPlayer.moveState &= ~MOVESTATE_400000;
                            gPlayer.moveState |= MOVESTATE_IN_AIR;
                            mpp->unk5C &= ~4;
                        }

                    } else {
                        y = Q(mpp->unk52 + (s->hitboxes[0].top) - 17);
                        result
                            = sub_801E4E4(I(gPlayer.y) - gPlayer.unk17, I(gPlayer.x),
                                          gPlayer.unk38, -8, &unusedByte, sub_801EE64);

                        if (result < 0) {
                            y -= Q(result);
                            gPlayer.moveState &= ~MOVESTATE_400000;
                            gPlayer.moveState |= MOVESTATE_IN_AIR;
                            mpp->unk5C &= ~4;
                        }
                    }
                    gPlayer.x = x;
                    gPlayer.y = y;
                    if (mpp->unk5C & 4) {
#ifndef NON_MATCHING
                        // thanks pidgey
                        u32 speed = 0;
                        gPlayer.speedAirX = speed;
                        invertedGravity = GRAVITY_IS_INVERTED;
                        if (!invertedGravity) {
                            invertedGravity = FALSE;
                        } else {
                            invertedGravity = FALSE;
                        }
                        gPlayer.speedAirY = speed;
                        asm("" ::: "r2");
#else
                        gPlayer.speedAirX = 0;
                        gPlayer.speedAirY = 0;
#endif

                        gPlayer.moveState &= ~MOVESTATE_20;
                    }
                }
            }
        }

        if (!(mpp->unk5C & 4)) {
            struct UNK_3005510 *thing = sub_8019224();
            thing->unk0 = 8;
            thing->unk1 = mpp->unk56;
            thing->unk2 = 0;
        }

        if (!GRAVITY_IS_INVERTED) {
            mpp->unk5C &= ~0x100;
        } else {
            mpp->unk5C |= 0x100;
        }
    } else {
        mpp->unk60--;

        if (!(gPlayer.moveState & MOVESTATE_8)) {
            return;
        }

        if (gPlayer.unk3C != s) {
            return;
        }

        val = sub_800D0A0(s, mpp->unk50, mpp->unk52, mpp->unk66, mpp->unk68,
                          mpp->unk54 >> 7 & 1, 0);

        if ((mpp->unk4C & MOVESTATE_20) && !(val & MOVESTATE_20)) {
            gPlayer.moveState &= ~MOVESTATE_20;
        }

        mpp->unk4C = val;
    }
}

void sub_8017670(void)
{
    Sprite *playerSprite, *s;
    MultiplayerPlayer *mpp;

    u32 moveStateVal, val;
    s32 result;
    u8 unusedByte;

    playerSprite = &gPlayer.unk90->s;
    mpp = TASK_DATA(gCurTask);
    s = &mpp->s;
    moveStateVal = (gPlayer.moveState >> 5) & 1;

    if (gPlayer.moveState & MOVESTATE_8 && gPlayer.unk3C == s) {
        sub_8017F34();
    }

    if (gGameMode != GAME_MODE_TEAM_PLAY
        || ((gMultiplayerConnections & (0x10 << (mpp->unk56))) >> ((mpp->unk56 + 4))
            != (gMultiplayerConnections & (0x10 << (SIO_MULTI_CNT->id)))
                >> (SIO_MULTI_CNT->id + 4))) {
        if (!sub_8018300()) {
            return;
        }

        if (!(mpp->unk4C & 0x20)) {
            return;
        }

        gPlayer.moveState &= ~MOVESTATE_20;
        mpp->unk4C = 0;
        return;
    }

    if (gPlayer.unk61 != 0 && gPlayer.character == CHARACTER_TAILS) {
        return;
    }

    if (mpp->unk60 == 0 || (mpp->unk5C & 4)) {
        mpp->unk60 = 0;
        if (s->graphics.anim != SA2_ANIM_CHAR(SA2_CHAR_ANIM_19, CHARACTER_KNUCKLES)
            && s->graphics.anim != SA2_ANIM_CHAR(SA2_CHAR_ANIM_20, CHARACTER_KNUCKLES)
            && s->graphics.anim != SA2_ANIM_CHAR(SA2_CHAR_ANIM_21, CHARACTER_KNUCKLES)) {
            if (sub_80181E0()) {
                if (!(mpp->unk4C & 0x20)) {
                    return;
                }
                gPlayer.moveState &= ~MOVESTATE_20;
                mpp->unk4C = 0;
                return;
            }

            if ((s->graphics.anim != SA2_ANIM_CHAR(SA2_CHAR_ANIM_19, CHARACTER_KNUCKLES)
                 && s->graphics.anim
                     != SA2_ANIM_CHAR(SA2_CHAR_ANIM_20, CHARACTER_KNUCKLES)
                 && s->graphics.anim
                     != SA2_ANIM_CHAR(SA2_CHAR_ANIM_21, CHARACTER_KNUCKLES))
                && HITBOX_IS_ACTIVE(playerSprite->hitboxes[1])
                && HITBOX_IS_ACTIVE(s->hitboxes[1])) {
                return;
            }
        }

        {
            s8 rect[4]
                = { -gPlayer.unk16, -gPlayer.unk17, gPlayer.unk16, gPlayer.unk17 };
            val = sub_800D0A0(s, mpp->unk50, mpp->unk52, mpp->unk66, mpp->unk68,
                              mpp->unk54 >> 7 & 1, 1);

            if (mpp->unk4C & 0x20 && !(val & 0x20)) {
                gPlayer.moveState &= ~MOVESTATE_20;
            }

            mpp->unk4C = val;

            if (val & 0x200000) {
                mpp->unk60 = 30;
            }

            if (CheckRectCollision_SpritePlayer(s, mpp->unk50, mpp->unk52, &gPlayer,
                                                (struct Rect8 *)rect)) {
                u8 temp = ((mpp->unk54 >> 7) & 1);
                if ((temp == gPlayer.unk38)
                    && (s->graphics.anim
                            == SA2_ANIM_CHAR(SA2_CHAR_ANIM_19, CHARACTER_KNUCKLES)
                        || s->graphics.anim
                            == SA2_ANIM_CHAR(SA2_CHAR_ANIM_20, CHARACTER_KNUCKLES)
                        || s->graphics.anim
                            == SA2_ANIM_CHAR(SA2_CHAR_ANIM_21, CHARACTER_KNUCKLES))
                    && !GRAVITY_IS_INVERTED == !(mpp->unk54 & 8)) {
                    if ((!GRAVITY_IS_INVERTED && I(gPlayer.y) > mpp->unk52)
                        || (GRAVITY_IS_INVERTED && I(gPlayer.y) < mpp->unk52)) {
                        gPlayer.moveState |= MOVESTATE_400000;
                        sub_8023B5C(&gPlayer, 14);
                        gPlayer.unk16 = 6;
                        gPlayer.unk17 = 14;
                        gPlayer.speedGroundX = 0;
                        gPlayer.speedAirX = 0;
                        gPlayer.unk64 = SA2_CHAR_ANIM_IDLE;
                        gPlayer.unk61 = 0;
                        gPlayer.unk62 = 0;
                        if (s->unk10 & SPRITE_FLAG_MASK_X_FLIP) {
                            gPlayer.moveState &= ~MOVESTATE_FACING_LEFT;
                        } else {
                            gPlayer.moveState |= MOVESTATE_FACING_LEFT;
                        }
                        if (!(mpp->unk5C & 4)) {
                            {
                                struct UNK_3005510 *thing = sub_8019224();
                                thing->unk0 = 8;
                                thing->unk1 = mpp->unk56;
                                thing->unk2 = 1;
                            }
                            if (!GRAVITY_IS_INVERTED) {
                                mpp->unk5C &= ~0x100;
                            } else {
                                mpp->unk5C |= 0x100;
                            }
                        }
                        mpp->unk5C |= 4;
                    }
                }
            }

            if (!(mpp->unk5C & 4)) {
                return;
            }

            if (sub_8029E6C(&gPlayer) != 0) {
                mpp->unk60 = 30;
                gPlayer.moveState &= ~MOVESTATE_400000;
                mpp->unk5C &= ~0x4;
                return;
            }

            if ((s->graphics.anim != SA2_ANIM_CHAR(SA2_CHAR_ANIM_19, CHARACTER_KNUCKLES)
                 && s->graphics.anim
                     != SA2_ANIM_CHAR(SA2_CHAR_ANIM_20, CHARACTER_KNUCKLES)
                 && s->graphics.anim
                     != SA2_ANIM_CHAR(SA2_CHAR_ANIM_21, CHARACTER_KNUCKLES))
                || I(gPlayer.x) <= gCamera.minX || I(gPlayer.x) >= gCamera.maxX
                || SOME_INVERTED_GRAVITY_MACRO || moveStateVal != 0) {
                gPlayer.moveState &= ~MOVESTATE_400000;
                mpp->unk5C &= ~4;
                gPlayer.unk64 = SA2_CHAR_ANIM_IDLE;
                if (SOME_INVERTED_GRAVITY_MACRO) {
                    mpp->unk60 = 30;
                }
                // gPlayer.unk64 = 0;
                return;
            }
            {
                s32 x, y;
                s32 playerUnk17 = gPlayer.unk17;
                bool32 gravityInverted = GRAVITY_IS_INVERTED;
                // mpp->unk5C |= 4;
                x = Q_24_8_NEW(mpp->unk50);

                // TODO: potential macro
                if (!(gravityInverted)) {
                    y = Q_24_8_NEW((mpp->unk52 + (s->hitboxes[0].top)) - rect[3]);
                    result = sub_801F100((mpp->unk52 + (s->hitboxes[0].top) - rect[3])
                                             - playerUnk17,
                                         I(x), gPlayer.unk38, -8, sub_801EC3C);

                    if (result < 0) {
                        y -= Q_24_8_NEW(result);
                        gPlayer.moveState &= ~MOVESTATE_400000;
                        gPlayer.moveState |= MOVESTATE_IN_AIR;
                        mpp->unk5C &= ~4;
                    }
                } else {
                    y = Q_24_8_NEW(mpp->unk52 + (s->hitboxes[0].bottom) + rect[3]);
                    result = sub_801F100(
                        ((mpp->unk52 + (s->hitboxes[0].bottom) + rect[3]) + playerUnk17),
                        I(x), gPlayer.unk38, 8, sub_801EC3C);

                    if (result < 0) {
                        y += Q_24_8_NEW(result);
                        gPlayer.moveState &= ~MOVESTATE_400000;
                        gPlayer.moveState |= MOVESTATE_IN_AIR;
                        mpp->unk5C &= ~4;
                    }
                }
                gPlayer.x = x;
                gPlayer.y = y;
            }

            if ((mpp->unk5C & 4)) {
                gPlayer.moveState |= MOVESTATE_8;
                gPlayer.moveState &= ~MOVESTATE_4;
                gPlayer.moveState &= ~MOVESTATE_IN_AIR;

                gPlayer.unk3C = s;
                gPlayer.speedAirY = 0;

                if ((s->unk10 & SPRITE_FLAG_MASK_X_FLIP)) {
                    gPlayer.moveState &= ~MOVESTATE_FACING_LEFT;
                } else {
                    gPlayer.moveState |= MOVESTATE_FACING_LEFT;
                }
            }

            if (!(mpp->unk5C & 4)) {
                struct UNK_3005510 *thing = sub_8019224();
                thing->unk0 = 8;
                thing->unk1 = mpp->unk56;
                thing->unk2 = 0;
            }

            if (!GRAVITY_IS_INVERTED) {
                mpp->unk5C &= ~0x100;
            } else {
                mpp->unk5C |= 0x100;
            }
        }
    } else {
        mpp->unk60--;

        if (!(gPlayer.moveState & MOVESTATE_8)) {
            return;
        }

        if (gPlayer.unk3C != s) {
            return;
        }

        val = sub_800D0A0(s, mpp->unk50, mpp->unk52, mpp->unk66, mpp->unk68,
                          mpp->unk54 >> 7 & 1, 0);

        if ((mpp->unk4C & MOVESTATE_20) && !(val & MOVESTATE_20)) {
            gPlayer.moveState &= ~MOVESTATE_20;
        }

        mpp->unk4C = val;
    }
}

void sub_8017C28(void)
{
    MultiplayerPlayer *mpp = TASK_DATA(gCurTask);
    Sprite *s = &mpp->s;
    u32 val;

    if (gPlayer.moveState & MOVESTATE_8 && gPlayer.unk3C == s) {
        sub_8017F34();
    }

    if (gGameMode != GAME_MODE_TEAM_PLAY
        || ((gMultiplayerConnections & (0x10 << (mpp->unk56))) >> ((mpp->unk56 + 4))
            != (gMultiplayerConnections & (0x10 << (SIO_MULTI_CNT->id)))
                >> (SIO_MULTI_CNT->id + 4))) {
        if (!sub_8018300()) {
            return;
        }

        if (!(mpp->unk4C & 0x20)) {
            return;
        }

        gPlayer.moveState &= ~MOVESTATE_20;
        mpp->unk4C = 0;
        return;
    }

    if (mpp->unk60 == 0) {
        if (!HITBOX_IS_ACTIVE(s->hitboxes[1])) {
            if (gPlayer.unk61 != 0
                && (gPlayer.character == CHARACTER_TAILS
                    || gPlayer.character == CHARACTER_KNUCKLES)) {
                return;
            }

            val = sub_800D0A0(s, mpp->unk50, mpp->unk52, mpp->unk66, mpp->unk68,
                              mpp->unk54 >> 7 & 1, 1);

            if (mpp->unk4C & 0x20 && !(val & 0x20)) {
                gPlayer.moveState &= ~MOVESTATE_20;
            }
            mpp->unk4C = val;
            return;
        } else {
            if (gPlayer.unk61 != 0
                && (gPlayer.character == CHARACTER_TAILS
                    || gPlayer.character == CHARACTER_KNUCKLES)) {
                return;
            }
            val = sub_800DA4C(s, mpp->unk50, mpp->unk52, mpp->unk66, mpp->unk68,
                              (mpp->unk54 >> 7) & 1);
            if ((val & 2) && !(gPlayer.moveState & MOVESTATE_IN_AIR)
                && gPlayer.rotation == 0) {
                if (s->graphics.anim == SA2_ANIM_CHAR(SA2_CHAR_ANIM_14, CHARACTER_AMY)) {
                    LaunchPlayer(-Q_8_8(7.5));
#ifndef NON_MATCHING
                    goto lab;
#else
                    mpp->unk60 = 30;
                    return;
#endif
                }

                if (s->graphics.anim == SA2_ANIM_CHAR(SA2_CHAR_ANIM_19, CHARACTER_AMY)) {
                    LaunchPlayer(-Q_8_8(10.5));
#ifndef NON_MATCHING
                    goto lab;
#else
                    mpp->unk60 = 30;
                    return;
#endif
                }
            }

            if ((val & 1)) {
                if ((val & 0x20000)) {
                    if (gPlayer.speedAirX > 0) {
                        gPlayer.speedAirX = -gPlayer.speedAirX;
                        gPlayer.speedGroundX = -gPlayer.speedGroundX;
                    }
                } else if ((val & 0x40000)) {
                    if (gPlayer.speedAirX < 0) {
                        gPlayer.speedAirX = -gPlayer.speedAirX;
                        gPlayer.speedGroundX = -gPlayer.speedGroundX;
                    }
                }

                if (val & 0x100000 && gPlayer.speedAirY > 0) {
                    gPlayer.speedAirY = -gPlayer.speedAirY;
                }
#ifndef NON_MATCHING
            lab:
#endif
                mpp->unk60 = 30;
                return;
            } else {
                val = sub_800D0A0(s, mpp->unk50, mpp->unk52, mpp->unk66, mpp->unk68,
                                  mpp->unk54 >> 7 & 1, 1);

                if (mpp->unk4C & 0x20 && !(val & 0x20)) {
                    gPlayer.moveState &= ~MOVESTATE_20;
                }

                mpp->unk4C = val;
                return;
            }
        }

    } else {
        mpp->unk60--;

        if (!(gPlayer.moveState & MOVESTATE_8)) {
            return;
        }

        if (gPlayer.unk3C != s) {
            return;
        }

        val = sub_800D0A0(s, mpp->unk50, mpp->unk52, mpp->unk66, mpp->unk68,
                          mpp->unk54 >> 7 & 1, 0);

        if (mpp->unk4C & 0x20 && !(val & 0x20)) {
            gPlayer.moveState &= ~MOVESTATE_20;
        }
        mpp->unk4C = val;
    }
}

void sub_8017F34(void)
{
    MultiplayerPlayer *mpp = TASK_DATA(gCurTask);
    MultiplayerPlayer *otherMpp;
    s32 result;

    if ((ABS(mpp->unk44) > 0x80) || (ABS(mpp->unk48) > 0x80)) {
        gPlayer.moveState &= ~0x8;
        gPlayer.moveState |= 2;
        mpp->unk60 = 30;
        return;
    }

    otherMpp = TASK_DATA(gMultiplayerPlayerTasks[SIO_MULTI_CNT->id]);
    if ((otherMpp->unk54 & 0x80) != (mpp->unk54 & 0x80)) {
        gPlayer.moveState &= ~MOVESTATE_8;
        gPlayer.unk3C = (void *)-1;
        mpp->unk64 = mpp->unk56;
        return;
    }

    gPlayer.x += Q(mpp->unk44);
    if (!GRAVITY_IS_INVERTED) {
        gPlayer.y += Q(mpp->unk48) + Q(1);
    } else {
        gPlayer.y += Q(mpp->unk48) - Q(2);
    }

    if (mpp->unk48 < 0) {
        result = sub_801F100(I(gPlayer.y) - gPlayer.unk17, I(gPlayer.x), gPlayer.unk38,
                             -8, sub_801EC3C);
        if (result < 0) {
            gPlayer.y -= Q(result);
            gPlayer.moveState &= ~MOVESTATE_8;
            gPlayer.moveState |= MOVESTATE_IN_AIR;
            mpp->unk60 = 30;
        }
    } else if (mpp->unk48 > 0) {
        result = sub_801F100(I(gPlayer.y) + gPlayer.unk17, I(gPlayer.x), gPlayer.unk38,
                             8, sub_801EC3C);
        if (result < 0) {
            gPlayer.y += Q(result);
            gPlayer.moveState &= ~MOVESTATE_8;
            gPlayer.moveState |= MOVESTATE_IN_AIR;
            mpp->unk60 = 30;
        }
    }

    if (mpp->unk44 < 0) {
        result = sub_801F100(I(gPlayer.x) - gPlayer.unk16, I(gPlayer.y), gPlayer.unk38,
                             -8, sub_801EB44);
        if (result < 0) {
            gPlayer.x -= Q(result);
        }
        return;
    } else if (mpp->unk44 > 0) {
        result = sub_801F100(I(gPlayer.x) + gPlayer.unk16, I(gPlayer.y), gPlayer.unk38,
                             8, sub_801EB44);
        if (result < 0) {
            gPlayer.x += Q(result);
        }
        return;
    }
}

void sub_8018120(void)
{
    MultiplayerPlayer *mpp = TASK_DATA(gCurTask);
    Sprite *s = &mpp->s;
    if (gPlayer.moveState & MOVESTATE_8 && gPlayer.unk3C == s) {
        MultiplayerPlayer *otherMpp;
        gPlayer.x += Q(mpp->unk44);
        gPlayer.y += Q(mpp->unk48) + Q(1);

        otherMpp = TASK_DATA(gMultiplayerPlayerTasks[SIO_MULTI_CNT->id]);

        if ((otherMpp->unk54 & 0x80) != (mpp->unk54 & 0x80)) {
            gPlayer.moveState &= ~MOVESTATE_8;
            gPlayer.unk3C = (void *)-1;
            mpp->unk64 = mpp->unk56;
        }
    }
    if (sub_8018300() && (mpp->unk4C & 0x20)) {
        gPlayer.moveState &= ~MOVESTATE_20;
        mpp->unk4C = 0;
    }
}

bool32 sub_80181E0(void)
{
    Sprite *playerS = &gPlayer.unk90->s;
    MultiplayerPlayer *mpp = TASK_DATA(gCurTask);
    Sprite *s = &mpp->s;

    u32 val;

    if (HITBOX_IS_ACTIVE(playerS->hitboxes[1]) && HITBOX_IS_ACTIVE(s->hitboxes[1])) {
        val = sub_800DA4C(s, mpp->unk50, mpp->unk52, mpp->unk66, mpp->unk68,
                          (mpp->unk54 >> 7) & 1);

        if ((val & 1)) {
            if (gPlayer.unk61 == 0 && (val & 0x20000)) {
                if (gPlayer.speedAirX > 0) {
                    gPlayer.speedAirX = -gPlayer.speedAirX;
                    gPlayer.speedGroundX = -gPlayer.speedGroundX;
                }
            } else if (gPlayer.unk61 == 0 && (val & 0x40000)) {
                if (gPlayer.speedAirX < 0) {
                    gPlayer.speedAirX = -gPlayer.speedAirX;
                    gPlayer.speedGroundX = -gPlayer.speedGroundX;
                }
            }

            if (val & 0x100000 && gPlayer.speedAirY > 0) {
                gPlayer.speedAirY = -gPlayer.speedAirY;
            }
            mpp->unk60 = 30;
            return TRUE;
        }
    }

    if (gPlayer.moveState & MOVESTATE_8 && gPlayer.unk3C == s) {
        gPlayer.moveState &= ~MOVESTATE_8;
        gPlayer.moveState |= MOVESTATE_IN_AIR;
    }
    return FALSE;
}

bool32 sub_8018300(void)
{
    MultiplayerPlayer *mpp;
    Sprite *s, *playerS;
    u32 val;

    playerS = &gPlayer.unk90->s;
    mpp = TASK_DATA(gCurTask);
    s = &mpp->s;

    if (mpp->unk60 == 0) {
        u32 val2 = sub_800DA4C(s, mpp->unk50, mpp->unk52, mpp->unk66, mpp->unk68,
                               (mpp->unk54 >> 7) & 1);
        if (gGameMode == GAME_MODE_MULTI_PLAYER_COLLECT_RINGS && !(val2 & 3)) {
            if (mpp->unk50 > 960) {
                val2 = sub_800DA4C(s, mpp->unk50 - 1440, mpp->unk52 - 864, mpp->unk66,
                                   mpp->unk68, (mpp->unk54 >> 7) & 1);
            } else {
                val2 = sub_800DA4C(s, mpp->unk50 + 1440, mpp->unk52 + 864, mpp->unk66,
                                   mpp->unk68, (mpp->unk54 >> 7) & 1);
            }
        }

        if (val2 & 1) {
            if (gPlayer.unk61 == 0 && (val2 & 0x20000)) {
                if (gPlayer.speedAirX > 0) {
                    gPlayer.speedAirX = -gPlayer.speedAirX;
                    gPlayer.speedGroundX = -gPlayer.speedGroundX;
                }
            } else if (gPlayer.unk61 == 0 && (val2 & 0x40000)) {
                if (gPlayer.speedAirX < 0) {
                    gPlayer.speedAirX = -gPlayer.speedAirX;
                    gPlayer.speedGroundX = -gPlayer.speedGroundX;
                }
            }

            if (val2 & 0x100000 && gPlayer.speedAirY > 0) {
                gPlayer.speedAirY = -gPlayer.speedAirY;
            }
            mpp->unk60 = 30;

            if (!(val2 & 2)) {
                return TRUE;
            }
        }
        if (val2 & 2) {
            if (val2 & 1) {
                if (mpp->unk50 < I(gPlayer.x)) {
                    gPlayer.moveState &= ~MOVESTATE_FACING_LEFT;
                } else {
                    gPlayer.moveState |= MOVESTATE_FACING_LEFT;
                }
                sub_800DE44(&gPlayer);
            } else {
                if (mpp->unk50 < I(gPlayer.x)) {
                    gPlayer.moveState |= MOVESTATE_FACING_LEFT;
                } else {
                    gPlayer.moveState &= ~MOVESTATE_FACING_LEFT;
                }
                sub_800DD54(&gPlayer);
            }

            mpp->unk60 = 30;
            return TRUE;
        }

        if (!HITBOX_IS_ACTIVE(playerS->hitboxes[1])
            && !HITBOX_IS_ACTIVE(s->hitboxes[1])) {
            u32 existingMoveState = gPlayer.moveState;
            Sprite *existingS = gPlayer.unk3C;
            s16 x, y;

            val = sub_800D0A0(s, mpp->unk50, mpp->unk52, mpp->unk66, mpp->unk68,
                              (mpp->unk54 >> 7) & 1, val2 & 2);

            if (gGameMode == GAME_MODE_MULTI_PLAYER_COLLECT_RINGS && val == 0) {
                gPlayer.moveState = existingMoveState;
                gPlayer.unk3C = existingS;

                if (mpp->unk50 > 960) {
                    val = sub_800D0A0(s, mpp->unk50 - 1440, mpp->unk52 - 864, mpp->unk66,
                                      mpp->unk68, (mpp->unk54 >> 7) & 1, val);
                } else {
                    val = sub_800D0A0(s, mpp->unk50 + 1440, mpp->unk52 + 864, mpp->unk66,
                                      mpp->unk68, (mpp->unk54 >> 7) & 1, val);
                }
            }

            if (mpp->unk4C & 0x20 && !(val & 0x20)) {
                gPlayer.moveState &= ~MOVESTATE_20;
            }
            mpp->unk4C = val;
            if ((val & 0x200000)) {
                mpp->unk60 = 30;
            }
        } else {
            if (gPlayer.moveState & MOVESTATE_8 && gPlayer.unk3C == s) {
                gPlayer.moveState &= ~MOVESTATE_8;
                gPlayer.moveState |= MOVESTATE_IN_AIR;
            }

            if (mpp->unk4C & 0x20) {
                gPlayer.moveState &= ~MOVESTATE_20;
            }
        }
        return FALSE;
    }

    mpp->unk60--;

    if (!(gPlayer.moveState & MOVESTATE_8)) {
        return FALSE;
    }

    if (gPlayer.unk3C != s) {
        return FALSE;
    }

    val = sub_800D0A0(s, mpp->unk50, mpp->unk52, mpp->unk66, mpp->unk68,
                      mpp->unk54 >> 7 & 1, 0);

    if (gGameMode == GAME_MODE_MULTI_PLAYER_COLLECT_RINGS && val == 0) {
        if (mpp->unk50 > 960) {
            val = sub_800D0A0(s, mpp->unk50 - 1440, mpp->unk52 - 864, mpp->unk66,
                              mpp->unk68, (mpp->unk54 >> 7) & 1, val);
        } else {
            val = sub_800D0A0(s, mpp->unk50 + 1440, mpp->unk52 + 864, mpp->unk66,
                              mpp->unk68, (mpp->unk54 >> 7) & 1, val);
        }
    }

    if (mpp->unk4C & 0x20 && !(val & 0x20)) {
        gPlayer.moveState &= ~MOVESTATE_20;
    }
    mpp->unk4C = val;

    if ((val & 0x200000)) {
        mpp->unk60 = 30;
    }

    return FALSE;
}

void Task_HandleLaunchPlayer(void)
{
    PlayerSpriteInfo *psi = gPlayer.unk90;
    Sprite *playerS = &psi->s;

    if (playerS->unk10 & SPRITE_FLAG_MASK_ANIM_OVER) {
        s16 *airSpeed = TASK_DATA(gCurTask);
        gPlayer.moveState &= ~MOVESTATE_IGNORE_INPUT;
        gPlayer.moveState &= ~MOVESTATE_800000;
        gPlayer.moveState &= ~MOVESTATE_8;
        gPlayer.moveState |= MOVESTATE_IN_AIR;
        gPlayer.moveState &= ~MOVESTATE_100;
        gPlayer.unk64 = SA2_CHAR_ANIM_38;
        playerS->prevVariant = -1;
        sub_8023B5C(&gPlayer, 14);
        gPlayer.unk16 = 6;
        gPlayer.unk17 = 14;
        m4aSongNumStart(SE_SPRING);
        gPlayer.speedAirY = *airSpeed;
        TaskDestroy(gCurTask);
        return;
    }

    if (gPlayer.unk64 != 109) {
        gPlayer.moveState &= ~MOVESTATE_IGNORE_INPUT;
        gPlayer.moveState &= ~MOVESTATE_800000;
        TaskDestroy(gCurTask);
    }
}

void sub_8018818(void)
{
    u32 i;
    if (IS_MULTI_PLAYER) {
        MultiplayerPlayer *mpp;
        for (i = 0; i < MULTI_SIO_PLAYERS_MAX; i++) {
            if (gMultiplayerPlayerTasks[i] == NULL) {
                break;
            }
            mpp = TASK_DATA(gMultiplayerPlayerTasks[i]);
            mpp->unk5C &= ~2;
        }

        gPlayer.timerInvulnerability = 120;
        gPlayer.itemEffect &= ~PLAYER_ITEM_EFFECT__80;
    }
}

void TaskDestructor_MultiplayerPlayer(struct Task *t)
{
    MultiplayerPlayer *mpp = TASK_DATA(t);
    gMultiplayerPlayerTasks[mpp->unk56] = NULL;
    VramFree(mpp->s.graphics.dest);
}

void LaunchPlayer(s16 airSpeedY)
{
    struct Task *t = TaskCreate(Task_HandleLaunchPlayer, sizeof(s16), 0x2000, 0, NULL);
    s16 *airSpeed = TASK_DATA(t);
    *airSpeed = airSpeedY;
    gPlayer.moveState |= MOVESTATE_IGNORE_INPUT;
    gPlayer.unk5C = 0;
    gPlayer.unk64 = 109; // TODO: wtf this is being set to larger than 91
    gPlayer.moveState |= MOVESTATE_800000;
}
