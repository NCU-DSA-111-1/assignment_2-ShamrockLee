/* SPDX-License-Identifier: MIT */
/**
 * Shogi Piece Name Abbrevations International to Kanji arrays
 *
 * Copyright (c) 2022
 * Yueh-Shun Li (Shamrock Lee) <44064051+ShamrockLee@users.noreply.github.com>
 *
 * shogi_piecename2kanji_table.h
 */

#ifndef __SHOGI_PIECENAME2KANJI_TABLE_H__
#define __SHOGI_PIECENAME2KANJI_TABLE_H__

#include <stddef.h>

/*
 * Number of unpromoted shogi pieces
 */
#define N_SHOGI_PIECENAMES_UNPRO 9

static const char *const a_shogi_piecenames[2][N_SHOGI_PIECENAMES_UNPRO] = {
	{
		"K王",
		"G金",
		"S銀",
		"N桂",
		"L香",
		"R飛",
		"B角",
		"P兵",
	},
	{
		"K王",
		"G金",
		"S全",
		"N圭",
		"L杏",
		"R竜",
		"B馬",
		"Pと",
	},
};

#define N_SHOGI_PIECENAMES_UNPRO_FAST 20

static const char
	*const a_shogi_piecenames_fast[2][N_SHOGI_PIECENAMES_UNPRO_FAST] = {
		{
			NULL, "角", NULL, NULL, NULL, // ABCDE
			NULL, "金", NULL, NULL, NULL, // FGHIJ
			"王", "香", NULL, "桂", NULL, // KLMNO
			"步", NULL, "飛", "銀", NULL, // PQRST
		},
		{
			NULL, "馬", NULL, NULL, NULL, // ABCDE
			NULL, "金", NULL, NULL, NULL, // FGHIJ
			"王", "杏", NULL, "圭", NULL, // KLMNO
			"と", NULL, "竜", "全", NULL, // PQRST
		},
	};

#endif /* __SHOGI_PIECENAME2KANJI_TABLE_H__ */
