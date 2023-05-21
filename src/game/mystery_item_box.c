#include "core.h"
#include "malloc_vram.h"
#include "game/game.h"
#include "game/mystery_item_box.h"
#include "lib/m4a.h"

#include "constants/animations.h"
#include "constants/songs.h"
#include "constants/zones.h"

typedef struct {
    SpriteBase base; /* 0x00 */
    Sprite box; /* 0x0C*/
    Sprite identifier; /* 0x3C */
    struct UNK_808D124_UNK180 transformer;
    s32 unk78; // x
    s32 unk7C; // y
    s16 unk80;
    u8 unk82;
    u8 unk83;
    u8 unk84;
} Sprite_MysteryItemBox;

static inline void sub_80865E4_inline(void);
static void sub_80865E4(void);
static void sub_808673C(void);
static void sub_80867E8(struct Task *);
static void sub_808623C(void);
static void sub_8086858(Sprite_MysteryItemBox *);
static bool32 sub_808693C(Sprite_MysteryItemBox *);
static bool32 sub_80868F4(Sprite_MysteryItemBox *);
static void sub_8086474(Sprite_MysteryItemBox *);
static void sub_80868A8(Sprite_MysteryItemBox *, u32);
static void sub_8086804(Sprite_MysteryItemBox *);
static void sub_8086890(Sprite_MysteryItemBox *);
static void sub_80866FC(void);
static void sub_808665C(void);
static void sub_808673C(void);

static const u8 sRingBonuses[] = { 1, 5, 10, 30, 50 };

static const u16 gUnknown_080E029A[] = { 0, 1, 1, 0, 1, 1, 0, 1 };

static const u16 gUnknown_080E02AA[][3]
    = { { SA2_ANIM_ITEMBOX_TYPE, 9, 4 }, { SA2_ANIM_ITEMBOX_TYPE, 12, 4 } };

static const u16 unused = 0;

void initSprite_Interactable_MysteryItemBox(MapEntity *me, u16 spriteRegionX,
                                            u16 spriteRegionY, u8 spriteY)
{
    Sprite *sprite;
    Sprite_MysteryItemBox *itemBox;
    struct Task *t;
    if ((gRandomItemBox & 7) == me->d.sData[0]
        && me->d.sData[1] <= (gRandomItemBox >> 4)) {
        t = TaskCreate(sub_80865E4, sizeof(Sprite_MysteryItemBox), 0x2000, 0,
                       sub_80867E8);
    } else {
        t = TaskCreate(sub_808673C, sizeof(Sprite_MysteryItemBox), 0x2000, 0,
                       sub_80867E8);
    }

    if (me->d.sData[1] <= (gRandomItemBox >> 4)) {
        me->d.sData[1] = (gRandomItemBox >> 4);
    }

    itemBox = TaskGetStructPtr(t);
    itemBox->unk82 = gUnknown_080E029A[gMultiplayerPseudoRandom & 7];
    itemBox->unk80 = 0;
    itemBox->unk78 = TO_WORLD_POS(me->x, spriteRegionX);
    itemBox->unk7C = TO_WORLD_POS(me->y, spriteRegionY);
    itemBox->base.regionX = spriteRegionX;
    itemBox->base.regionY = spriteRegionY;
    itemBox->base.me = me;
    itemBox->base.spriteX = me->x;
    itemBox->base.spriteY = spriteY;
    SET_MAP_ENTITY_INITIALIZED(me);

    sprite = &itemBox->box;
    sprite->unk22 = 0x10;
    sprite->graphics.size = 0;
    sprite->unk14 = 0;
    sprite->unk1C = 0;
    sprite->unk21 = -1;
    sprite->focused = 0;
    sprite->unk1A = 0x480;
    sprite->unk28[0].unk0 = -1;
    sprite->unk10 = SPRITE_FLAG_PRIORITY(2);
    sprite->graphics.dest = VramMalloc(16);
    sprite->graphics.anim = SA2_ANIM_ITEMBOX;
    sprite->variant = 0;
    sub_8004558(sprite);

    sprite = &itemBox->identifier;
    sprite->unk22 = 0x10;
    sprite->graphics.size = 0;
    sprite->unk14 = 0;
    sprite->unk1C = 0;
    sprite->unk21 = -1;
    sprite->focused = 0;
    sprite->unk1A = 0x4C0;
    sprite->unk28[0].unk0 = -1;
    sprite->unk10 = SPRITE_FLAG_PRIORITY(2);
    sprite->graphics.dest = VramMalloc(4);
    sprite->graphics.anim = gUnknown_080E02AA[gUnknown_080E029A[itemBox->unk82]][0];
    sprite->variant = gUnknown_080E02AA[gUnknown_080E029A[itemBox->unk82]][1];
    sub_8004558(sprite);
}

static void sub_808616C(void)
{
    Sprite_MysteryItemBox *itemBox = TaskGetStructPtr(gCurTask);
    struct UNK_808D124_UNK180 *transformer;
    Sprite *identifier;

    itemBox->unk82
        = gUnknown_080E029A[gMultiplayerPseudoRandom % ARRAY_COUNT(gUnknown_080E029A)];

    identifier = &itemBox->identifier;
    identifier->graphics.anim = gUnknown_080E02AA[gUnknown_080E029A[itemBox->unk82]][0];
    identifier->variant = gUnknown_080E02AA[gUnknown_080E029A[itemBox->unk82]][1];
    sub_8004558(identifier);

    itemBox->box.unk10 |= SPRITE_FLAG_MASK_ROT_SCALE_ENABLE;
    itemBox->identifier.unk10 |= SPRITE_FLAG_MASK_ROT_SCALE_ENABLE;

    transformer = &itemBox->transformer;
    transformer->unk0 = 0;
    transformer->unk2 = 0x100;
    transformer->unk4 = 0;
    transformer->unk6[0] = 0;
    transformer->unk6[1] = 0;
    gCurTask->main = sub_808623C;
    sub_808623C();
}

// TODO: figure out how to move this down
static inline void sub_80865E4_inline(void)
{
    Sprite_MysteryItemBox *itemBox = TaskGetStructPtr(gCurTask);
    MapEntity *me = itemBox->base.me;

    if (me->d.sData[0] != (gRandomItemBox & 7)) {
        sub_8086804(itemBox);
    } else {
        if (me->d.sData[1] > (gRandomItemBox >> 4)) {
            sub_8086858(itemBox);
            return;
        }

        if (sub_808693C(itemBox)) {
            sub_8086474(itemBox);
        }

        if (sub_80868F4(itemBox)) {
            SET_MAP_ENTITY_NOT_INITIALIZED(me, itemBox->base.spriteX);
            TaskDestroy(gCurTask);
            return;
        }

        sub_80868A8(itemBox, 0);
    }
}

static void sub_808623C(void)
{
    Sprite_MysteryItemBox *itemBox = TaskGetStructPtr(gCurTask);

    struct UNK_808D124_UNK180 *transformer = &itemBox->transformer;
    transformer->unk6[0] = itemBox->unk78 - gCamera.x;
    transformer->unk6[1] = itemBox->unk7C - gCamera.y;

    transformer->unk4 += 8;

    if (transformer->unk4 >= 0x100) {
        MapEntity *me;
        Sprite_MysteryItemBox *itemBox2;
        itemBox->box.unk10 &= ~SPRITE_FLAG_MASK_ROT_SCALE_ENABLE;
        itemBox->identifier.unk10 &= ~SPRITE_FLAG_MASK_ROT_SCALE_ENABLE;
        transformer->unk4 = 0x100;
        itemBox->unk80 = 0;
        gCurTask->main = sub_80865E4;

        sub_80865E4_inline();
        return;
    }

    itemBox->box.unk10 &= ~SPRITE_FLAG_MASK_ROT_SCALE;

    itemBox->box.unk10 |= gUnknown_030054B8;
    itemBox->identifier.unk10 &= ~SPRITE_FLAG_MASK_ROT_SCALE;
    itemBox->identifier.unk10 |= gUnknown_030054B8++;

    sub_8004860(&itemBox->box, transformer);
    sub_8004860(&itemBox->identifier, transformer);
    sub_80051E8(&itemBox->box);
    sub_80051E8(&itemBox->identifier);
}

static inline void sub_808673C_inline(void)
{
    Sprite_MysteryItemBox *itemBox = TaskGetStructPtr(gCurTask);
    MapEntity *me = itemBox->base.me;

    if (me->d.sData[0] == (gRandomItemBox & 7)
        && me->d.sData[1] <= (gRandomItemBox >> 4)) {
        me->d.sData[1] = gRandomItemBox >> 4;
        gCurTask->main = sub_808616C;
        return;
    }

    if (sub_80868F4(itemBox) != 0) {
        SET_MAP_ENTITY_NOT_INITIALIZED(me, itemBox->base.spriteX);
        TaskDestroy(gCurTask);
        return;
    }
}

static void sub_808636C(void)
{
    Sprite_MysteryItemBox *itemBox = TaskGetStructPtr(gCurTask);

    struct UNK_808D124_UNK180 *transformer = &itemBox->transformer;
    transformer->unk6[0] = itemBox->unk78 - gCamera.x;
    transformer->unk6[1] = itemBox->unk7C - gCamera.y;

    transformer->unk4 -= 8;

    if (transformer->unk4 < 1) {
        MapEntity *me;
        Sprite_MysteryItemBox *itemBox2;
        itemBox->box.unk10 &= ~SPRITE_FLAG_MASK_ROT_SCALE_ENABLE;
        itemBox->identifier.unk10 &= ~SPRITE_FLAG_MASK_ROT_SCALE_ENABLE;
        transformer->unk4 = 0x100;
        gCurTask->main = sub_808673C;

        sub_808673C_inline();
        return;
    }

    itemBox->box.unk10 &= ~SPRITE_FLAG_MASK_ROT_SCALE;

    itemBox->box.unk10 |= gUnknown_030054B8;
    itemBox->identifier.unk10 &= ~SPRITE_FLAG_MASK_ROT_SCALE;
    itemBox->identifier.unk10 |= gUnknown_030054B8++;

    sub_8004860(&itemBox->box, transformer);
    sub_8004860(&itemBox->identifier, transformer);
    sub_80051E8(&itemBox->box);
    sub_80051E8(&itemBox->identifier);
}

static void sub_8086474(Sprite_MysteryItemBox *itemBox)
{
    struct UNK_3005510 *unk5510;
    MapEntity *me;
    if (itemBox->unk84 != 1 || gPlayer.moveState & 2) {
        gPlayer.speedAirY = -0x300;
        gPlayer.unk64 = 0x26;
        gPlayer.unk66 = -1;
        gPlayer.unk6D = 5;
    }

    itemBox->base.me->d.sData[1] += 1;

    m4aSongNumStart(SE_ITEM_BOX_2);
    sub_800B9B8(itemBox->unk78, itemBox->unk7C);
    itemBox->unk83 = 0;
    unk5510 = sub_8019224();

    unk5510->unk0 = 5;
    unk5510->unk1 = itemBox->base.regionX;
    unk5510->unk2 = itemBox->base.regionY;
    unk5510->unk3 = itemBox->base.spriteY;
    unk5510->unk4 = itemBox->base.me->d.sData[1];

    gCurTask->main = sub_808665C;
}

static void sub_8086504(Sprite_MysteryItemBox *itemBox)
{
    switch (itemBox->unk82) {
        case 0: {
            u8 boxVal = sRingBonuses[(u32)PseudoRandom32() % ARRAY_COUNT(sRingBonuses)];
            u16 prevRingCount = gRingCount;
            gRingCount = prevRingCount + boxVal;

            if (gCurrentLevel != LEVEL_INDEX(ZONE_FINAL, ACT_TRUE_AREA_53)) {
                if (Div((u16)gRingCount, 100) != Div(prevRingCount, 100)
                    && gGameMode == 0) {
                    gNumLives = MIN(gNumLives + 1, 255u);
                    gUnknown_030054A8[3] = 0x10;
                }
            }

            if (gGameMode == GAME_MODE_MULTI_PLAYER_COLLECT_RINGS
                && (u16)gRingCount > 255) {
                gRingCount = 255;
            }

            m4aSongNumStart(SE_RING_COPY);
            break;
        }
        case 1: {
            struct UNK_3005510 *unk5510 = sub_8019224();

            unk5510->unk0 = 6;
            unk5510->unk1 = 4;
            break;
        }
    }

    itemBox->unk83 = 0;
    gCurTask->main = sub_80866FC;
}

static void sub_80865E4(void)
{
    Sprite_MysteryItemBox *itemBox = TaskGetStructPtr(gCurTask);
    MapEntity *me = itemBox->base.me;

    if (me->d.sData[0] != (gRandomItemBox & 7)) {
        sub_8086804(itemBox);
    } else {
        if (me->d.sData[1] > (gRandomItemBox >> 4)) {
            sub_8086858(itemBox);
        } else {
            if (sub_808693C(itemBox) != 0) {
                sub_8086474(itemBox);
            }

            if (sub_80868F4(itemBox) != 0) {
                SET_MAP_ENTITY_NOT_INITIALIZED(me, itemBox->base.spriteX);
                TaskDestroy(gCurTask);
                return;
            } else {
                sub_80868A8(itemBox, 0);
            }
        }
    }
}

static void sub_808665C(void)
{
    Sprite_MysteryItemBox *itemBox = TaskGetStructPtr(gCurTask);

    if (itemBox->unk83++ >= 0x3C) {
        sub_8086504(itemBox);
    } else {
        itemBox->unk80 -= 0x100;
    }

    sub_80868A8(itemBox, 1);
}

static void sub_80866AC(void)
{
    Sprite_MysteryItemBox *itemBox = TaskGetStructPtr(gCurTask);

    if (itemBox->unk83++ >= 0x3C) {
        sub_8086890(itemBox);
    } else {
        itemBox->unk80 -= 0x100;
    }

    sub_80868A8(itemBox, 1);
}

static void sub_80866FC(void)
{
    Sprite_MysteryItemBox *itemBox = TaskGetStructPtr(gCurTask);

    if (itemBox->unk83++ >= 0x1E) {
        gCurTask->main = sub_808673C;
        return;
    }

    sub_80868A8(itemBox, 1);
}

static void sub_808673C(void)
{
    Sprite_MysteryItemBox *itemBox = TaskGetStructPtr(gCurTask);
    MapEntity *me = itemBox->base.me;

    if (me->d.sData[0] == (gRandomItemBox & 7)
        && me->d.sData[1] <= (gRandomItemBox >> 4)) {
        me->d.sData[1] = gRandomItemBox >> 4;
        gCurTask->main = sub_808616C;
        return;
    }

    if (sub_80868F4(itemBox) != 0) {
        SET_MAP_ENTITY_NOT_INITIALIZED(me, itemBox->base.spriteX);
        TaskDestroy(gCurTask);
        return;
    }
}

static inline void sub_808679C_inline(void)
{
    struct UNK_808D124_UNK180 *transformer;
    Sprite_MysteryItemBox *itemBox = TaskGetStructPtr(gCurTask);
    itemBox->box.unk10 |= SPRITE_FLAG_MASK_ROT_SCALE_ENABLE;
    itemBox->identifier.unk10 |= SPRITE_FLAG_MASK_ROT_SCALE_ENABLE;

    transformer = &itemBox->transformer;
    transformer->unk0 = 0;
    transformer->unk2 = 0x100;
    transformer->unk4 = 0x100;
    transformer->unk6[0] = 0;
    transformer->unk6[1] = 0;
    gCurTask->main = sub_808636C;
    sub_808636C();
}

static void sub_808679C(void)
{
    struct UNK_808D124_UNK180 *transformer;
    Sprite_MysteryItemBox *itemBox = TaskGetStructPtr(gCurTask);
    itemBox->box.unk10 |= SPRITE_FLAG_MASK_ROT_SCALE_ENABLE;
    itemBox->identifier.unk10 |= SPRITE_FLAG_MASK_ROT_SCALE_ENABLE;

    transformer = &itemBox->transformer;
    transformer->unk0 = 0;
    transformer->unk2 = 0x100;
    transformer->unk4 = 0x100;
    transformer->unk6[0] = 0;
    transformer->unk6[1] = 0;
    gCurTask->main = sub_808636C;
    sub_808636C();
}

static void sub_80867E8(struct Task *t)
{
    Sprite_MysteryItemBox *itemBox = TaskGetStructPtr(t);
    VramFree(itemBox->box.graphics.dest);
    VramFree(itemBox->identifier.graphics.dest);
}

static void sub_8086804(Sprite_MysteryItemBox *unused)
{
    gCurTask->main = sub_808679C;
    // Has to be inline :/
    sub_808679C_inline();
}

static void sub_8086858(Sprite_MysteryItemBox *itemBox)
{
    m4aSongNumStart(SE_ITEM_BOX_2);
    sub_800B9B8(itemBox->unk78, itemBox->unk7C);
    itemBox->unk83 = 0;
    gCurTask->main = sub_80866AC;
}

static void sub_8086890(Sprite_MysteryItemBox *itemBox)
{
    itemBox->unk83 = 0;
    gCurTask->main = sub_80866FC;
}

static void sub_80868A8(Sprite_MysteryItemBox *itemBox, u32 p2)
{
    itemBox->box.x = itemBox->unk78 - gCamera.x;
    itemBox->box.y = itemBox->unk7C - gCamera.y;
    itemBox->identifier.x = itemBox->box.x;
    itemBox->identifier.y = itemBox->box.y + Q_24_8_TO_INT(itemBox->unk80);

    if (p2 == 0) {
        sub_80051E8(&itemBox->box);
    }

    sub_80051E8(&itemBox->identifier);
}

static bool32 sub_80868F4(Sprite_MysteryItemBox *itemBox)
{
    s16 x = itemBox->unk78 - gCamera.x;
    s16 y = itemBox->unk7C - gCamera.y;

    if (IS_OUT_OF_GRAV_TRIGGER_RANGE(x, y)) {
        return TRUE;
    }

    return FALSE;
}

u32 sub_800C944(Sprite *, s32, s32);

static bool32 sub_808693C(Sprite_MysteryItemBox *itemBox)
{
    if (PLAYER_IS_ALIVE) {
        if (sub_800C944(&itemBox->box, itemBox->unk78, itemBox->unk7C) != 0) {
            itemBox->unk84 = 1;
            return TRUE;
        } else if (sub_800C204(&itemBox->box, itemBox->unk78, itemBox->unk7C, 0,
                               &gPlayer, 0)
                   == 0) {
#ifndef NON_MATCHING
        ret0:
#endif
            return FALSE;
        } else {
            itemBox->unk84 = 0;
            return TRUE;
        }
    } else {
#ifndef NON_MATCHING
        goto ret0;
#else
        return FALSE;
#endif
    }
}