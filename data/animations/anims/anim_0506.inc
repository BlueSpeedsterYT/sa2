

anim_data__0506_0:
	mGetPalette 0x3E 16 0xD0
	mGetTiles 0x1A04F 4
	mAnimCmd6 0x0 0x202FEFE
	.4byte	2, 0

	mGetTiles 0x1A053 4
	.4byte	2, 1

	mGetTiles 0x1A057 4
	.4byte	2, 2

	mJumpBack 0x15


anim_0506:
	.4byte anim_data__0506_0