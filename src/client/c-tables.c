/* File: tables.c */

/* Purpose: Angband Tables */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

/*
 * Only a little of the file is used for the client, the rest is commented out.
 */

#include "angband.h"



/*
 * Global array for looping through the "keypad directions"
 */
s16b ddd[9] =
{ 2, 8, 6, 4, 3, 1, 9, 7, 5 };

/*
 * Global arrays for converting "keypad direction" into offsets
 */
s16b ddx[10] =
{ 0, -1, 0, 1, -1, 0, 1, -1, 0, 1 };

s16b ddy[10] =
{ 0, 1, 1, 1, 0, 0, 0, -1, -1, -1 };

/*
 * Global arrays for optimizing "ddx[ddd[i]]" and "ddy[ddd[i]]"
 */
s16b ddx_ddd[9] =
{ 0, 0, 1, -1, 1, -1, 1, -1, 0 };

s16b ddy_ddd[9] =
{ 1, -1, 0, 0, 1, 1, -1, -1, 0 };


/*
 * Global array for converting numbers to uppercase hecidecimal digit
 * This array can also be used to convert a number to an octal digit
 */
char hexsym[16] =
{
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

#if 0

/*
 * Stat Table (INT/WIS) -- Number of half-spells per level
 */
byte adj_mag_study[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	2	/* 12 */,
	2	/* 13 */,
	2	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	2	/* 18/00-18/09 */,
	2	/* 18/10-18/19 */,
	2	/* 18/20-18/29 */,
	2	/* 18/30-18/39 */,
	2	/* 18/40-18/49 */,
	3	/* 18/50-18/59 */,
	3	/* 18/60-18/69 */,
	3	/* 18/70-18/79 */,
	3	/* 18/80-18/89 */,
	4	/* 18/90-18/99 */,
	4	/* 18/100-18/109 */,
	4	/* 18/110-18/119 */,
	5	/* 18/120-18/129 */,
	5	/* 18/130-18/139 */,
	5	/* 18/140-18/149 */,
	5	/* 18/150-18/159 */,
	5	/* 18/160-18/169 */,
	5	/* 18/170-18/179 */,
	5	/* 18/180-18/189 */,
	5	/* 18/190-18/199 */,
	5	/* 18/200-18/209 */,
	5	/* 18/210-18/219 */,
	5	/* 18/220+ */
};


/*
 * Stat Table (INT/WIS) -- extra half-mana-points per level
 */
byte adj_mag_mana[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	2	/* 9 */,
	2	/* 10 */,
	2	/* 11 */,
	2	/* 12 */,
	2	/* 13 */,
	2	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18/00-18/09 */,
	3	/* 18/10-18/19 */,
	3	/* 18/20-18/29 */,
	3	/* 18/30-18/39 */,
	3	/* 18/40-18/49 */,
	4	/* 18/50-18/59 */,
	4	/* 18/60-18/69 */,
	5	/* 18/70-18/79 */,
	6	/* 18/80-18/89 */,
	7	/* 18/90-18/99 */,
	8	/* 18/100-18/109 */,
	9	/* 18/110-18/119 */,
	10	/* 18/120-18/129 */,
	11	/* 18/130-18/139 */,
	12	/* 18/140-18/149 */,
	13	/* 18/150-18/159 */,
	14	/* 18/160-18/169 */,
	15	/* 18/170-18/179 */,
	16	/* 18/180-18/189 */,
	16	/* 18/190-18/199 */,
	16	/* 18/200-18/209 */,
	16	/* 18/210-18/219 */,
	16	/* 18/220+ */
};


/*
 * Stat Table (INT/WIS) -- Minimum failure rate (percentage)
 */
byte adj_mag_fail[] =
{
	99	/* 3 */,
	99	/* 4 */,
	99	/* 5 */,
	99	/* 6 */,
	99	/* 7 */,
	50	/* 8 */,
	30	/* 9 */,
	20	/* 10 */,
	15	/* 11 */,
	12	/* 12 */,
	11	/* 13 */,
	10	/* 14 */,
	9	/* 15 */,
	8	/* 16 */,
	7	/* 17 */,
	6	/* 18/00-18/09 */,
	6	/* 18/10-18/19 */,
	5	/* 18/20-18/29 */,
	5	/* 18/30-18/39 */,
	5	/* 18/40-18/49 */,
	4	/* 18/50-18/59 */,
	4	/* 18/60-18/69 */,
	4	/* 18/70-18/79 */,
	4	/* 18/80-18/89 */,
	3	/* 18/90-18/99 */,
	3	/* 18/100-18/109 */,
	2	/* 18/110-18/119 */,
	2	/* 18/120-18/129 */,
	2	/* 18/130-18/139 */,
	2	/* 18/140-18/149 */,
	1	/* 18/150-18/159 */,
	1	/* 18/160-18/169 */,
	1	/* 18/170-18/179 */,
	1	/* 18/180-18/189 */,
	1	/* 18/190-18/199 */,
	0	/* 18/200-18/209 */,
	0	/* 18/210-18/219 */,
	0	/* 18/220+ */
};


/*
 * Stat Table (INT/WIS) -- Various things
 */
byte adj_mag_stat[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18/00-18/09 */,
	3	/* 18/10-18/19 */,
	3	/* 18/20-18/29 */,
	3	/* 18/30-18/39 */,
	3	/* 18/40-18/49 */,
	4	/* 18/50-18/59 */,
	4	/* 18/60-18/69 */,
	5	/* 18/70-18/79 */,
	6	/* 18/80-18/89 */,
	7	/* 18/90-18/99 */,
	8	/* 18/100-18/109 */,
	9	/* 18/110-18/119 */,
	10	/* 18/120-18/129 */,
	11	/* 18/130-18/139 */,
	12	/* 18/140-18/149 */,
	13	/* 18/150-18/159 */,
	14	/* 18/160-18/169 */,
	15	/* 18/170-18/179 */,
	16	/* 18/180-18/189 */,
	17	/* 18/190-18/199 */,
	18	/* 18/200-18/209 */,
	19	/* 18/210-18/219 */,
	20	/* 18/220+ */
};


/*
 * Stat Table (CHR) -- payment percentages
 */
byte adj_chr_gold[] =
{
	130	/* 3 */,
	125	/* 4 */,
	122	/* 5 */,
	120	/* 6 */,
	118	/* 7 */,
	116	/* 8 */,
	114	/* 9 */,
	112	/* 10 */,
	110	/* 11 */,
	108	/* 12 */,
	106	/* 13 */,
	104	/* 14 */,
	103	/* 15 */,
	102	/* 16 */,
	101	/* 17 */,
	100	/* 18/00-18/09 */,
	99	/* 18/10-18/19 */,
	98	/* 18/20-18/29 */,
	97	/* 18/30-18/39 */,
	96	/* 18/40-18/49 */,
	95	/* 18/50-18/59 */,
	94	/* 18/60-18/69 */,
	93	/* 18/70-18/79 */,
	92	/* 18/80-18/89 */,
	91	/* 18/90-18/99 */,
	90	/* 18/100-18/109 */,
	89	/* 18/110-18/119 */,
	88	/* 18/120-18/129 */,
	87	/* 18/130-18/139 */,
	86	/* 18/140-18/149 */,
	85	/* 18/150-18/159 */,
	84	/* 18/160-18/169 */,
	83	/* 18/170-18/179 */,
	82	/* 18/180-18/189 */,
	81	/* 18/190-18/199 */,
	80	/* 18/200-18/209 */,
	80	/* 18/210-18/219 */,
	80	/* 18/220+ */
};


/*
 * Stat Table (INT) -- Magic devices
 */
byte adj_int_dev[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18/00-18/09 */,
	3	/* 18/10-18/19 */,
	4	/* 18/20-18/29 */,
	4	/* 18/30-18/39 */,
	5	/* 18/40-18/49 */,
	5	/* 18/50-18/59 */,
	6	/* 18/60-18/69 */,
	6	/* 18/70-18/79 */,
	7	/* 18/80-18/89 */,
	7	/* 18/90-18/99 */,
	8	/* 18/100-18/109 */,
	9	/* 18/110-18/119 */,
	10	/* 18/120-18/129 */,
	11	/* 18/130-18/139 */,
	12	/* 18/140-18/149 */,
	13	/* 18/150-18/159 */,
	14	/* 18/160-18/169 */,
	15	/* 18/170-18/179 */,
	16	/* 18/180-18/189 */,
	17	/* 18/190-18/199 */,
	18	/* 18/200-18/209 */,
	19	/* 18/210-18/219 */,
	20	/* 18/220+ */
};


/*
 * Stat Table (WIS) -- Saving throw
 */
byte adj_wis_sav[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18/00-18/09 */,
	3	/* 18/10-18/19 */,
	3	/* 18/20-18/29 */,
	3	/* 18/30-18/39 */,
	3	/* 18/40-18/49 */,
	4	/* 18/50-18/59 */,
	4	/* 18/60-18/69 */,
	5	/* 18/70-18/79 */,
	5	/* 18/80-18/89 */,
	6	/* 18/90-18/99 */,
	7	/* 18/100-18/109 */,
	8	/* 18/110-18/119 */,
	9	/* 18/120-18/129 */,
	10	/* 18/130-18/139 */,
	11	/* 18/140-18/149 */,
	12	/* 18/150-18/159 */,
	13	/* 18/160-18/169 */,
	14	/* 18/170-18/179 */,
	15	/* 18/180-18/189 */,
	16	/* 18/190-18/199 */,
	17	/* 18/200-18/209 */,
	18	/* 18/210-18/219 */,
	19	/* 18/220+ */
};


/*
 * Stat Table (DEX) -- disarming
 */
byte adj_dex_dis[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	0	/* 8 */,
	0	/* 9 */,
	0	/* 10 */,
	0	/* 11 */,
	0	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	1	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	4	/* 18/00-18/09 */,
	4	/* 18/10-18/19 */,
	4	/* 18/20-18/29 */,
	4	/* 18/30-18/39 */,
	5	/* 18/40-18/49 */,
	5	/* 18/50-18/59 */,
	5	/* 18/60-18/69 */,
	6	/* 18/70-18/79 */,
	6	/* 18/80-18/89 */,
	7	/* 18/90-18/99 */,
	8	/* 18/100-18/109 */,
	8	/* 18/110-18/119 */,
	8	/* 18/120-18/129 */,
	8	/* 18/130-18/139 */,
	8	/* 18/140-18/149 */,
	9	/* 18/150-18/159 */,
	9	/* 18/160-18/169 */,
	9	/* 18/170-18/179 */,
	9	/* 18/180-18/189 */,
	9	/* 18/190-18/199 */,
	10	/* 18/200-18/209 */,
	10	/* 18/210-18/219 */,
	10	/* 18/220+ */
};


/*
 * Stat Table (INT) -- disarming
 */
byte adj_int_dis[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18/00-18/09 */,
	3	/* 18/10-18/19 */,
	3	/* 18/20-18/29 */,
	4	/* 18/30-18/39 */,
	4	/* 18/40-18/49 */,
	5	/* 18/50-18/59 */,
	6	/* 18/60-18/69 */,
	7	/* 18/70-18/79 */,
	8	/* 18/80-18/89 */,
	9	/* 18/90-18/99 */,
	10	/* 18/100-18/109 */,
	10	/* 18/110-18/119 */,
	11	/* 18/120-18/129 */,
	12	/* 18/130-18/139 */,
	13	/* 18/140-18/149 */,
	14	/* 18/150-18/159 */,
	15	/* 18/160-18/169 */,
	16	/* 18/170-18/179 */,
	17	/* 18/180-18/189 */,
	18	/* 18/190-18/199 */,
	19	/* 18/200-18/209 */,
	19	/* 18/210-18/219 */,
	19	/* 18/220+ */
};


/*
 * Stat Table (DEX) -- bonus to ac (plus 128)
 */
byte adj_dex_ta[] =
{
	128 + -4	/* 3 */,
	128 + -3	/* 4 */,
	128 + -2	/* 5 */,
	128 + -1	/* 6 */,
	128 + 0	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 1	/* 15 */,
	128 + 1	/* 16 */,
	128 + 1	/* 17 */,
	128 + 2	/* 18/00-18/09 */,
	128 + 2	/* 18/10-18/19 */,
	128 + 2	/* 18/20-18/29 */,
	128 + 2	/* 18/30-18/39 */,
	128 + 2	/* 18/40-18/49 */,
	128 + 3	/* 18/50-18/59 */,
	128 + 3	/* 18/60-18/69 */,
	128 + 3	/* 18/70-18/79 */,
	128 + 4	/* 18/80-18/89 */,
	128 + 5	/* 18/90-18/99 */,
	128 + 6	/* 18/100-18/109 */,
	128 + 7	/* 18/110-18/119 */,
	128 + 8	/* 18/120-18/129 */,
	128 + 9	/* 18/130-18/139 */,
	128 + 9	/* 18/140-18/149 */,
	128 + 10	/* 18/150-18/159 */,
	128 + 11	/* 18/160-18/169 */,
	128 + 12	/* 18/170-18/179 */,
	128 + 13	/* 18/180-18/189 */,
	128 + 14	/* 18/190-18/199 */,
	128 + 15	/* 18/200-18/209 */,
	128 + 15	/* 18/210-18/219 */,
	128 + 15	/* 18/220+ */
};


/*
 * Stat Table (STR) -- bonus to dam (plus 128)
 */
byte adj_str_td[] =
{
	128 + -2	/* 3 */,
	128 + -2	/* 4 */,
	128 + -1	/* 5 */,
	128 + -1	/* 6 */,
	128 + 0	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 0	/* 15 */,
	128 + 1	/* 16 */,
	128 + 2	/* 17 */,
	128 + 2	/* 18/00-18/09 */,
	128 + 2	/* 18/10-18/19 */,
	128 + 3	/* 18/20-18/29 */,
	128 + 3	/* 18/30-18/39 */,
	128 + 3	/* 18/40-18/49 */,
	128 + 3	/* 18/50-18/59 */,
	128 + 3	/* 18/60-18/69 */,
	128 + 4	/* 18/70-18/79 */,
	128 + 5	/* 18/80-18/89 */,
	128 + 5	/* 18/90-18/99 */,
	128 + 6	/* 18/100-18/109 */,
	128 + 7	/* 18/110-18/119 */,
	128 + 8	/* 18/120-18/129 */,
	128 + 9	/* 18/130-18/139 */,
	128 + 10	/* 18/140-18/149 */,
	128 + 11	/* 18/150-18/159 */,
	128 + 12	/* 18/160-18/169 */,
	128 + 13	/* 18/170-18/179 */,
	128 + 14	/* 18/180-18/189 */,
	128 + 15	/* 18/190-18/199 */,
	128 + 16	/* 18/200-18/209 */,
	128 + 18	/* 18/210-18/219 */,
	128 + 20	/* 18/220+ */
};


/*
 * Stat Table (DEX) -- bonus to hit (plus 128)
 */
byte adj_dex_th[] =
{
	128 + -3	/* 3 */,
	128 + -2	/* 4 */,
	128 + -2	/* 5 */,
	128 + -1	/* 6 */,
	128 + -1	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 0	/* 15 */,
	128 + 1	/* 16 */,
	128 + 2	/* 17 */,
	128 + 3	/* 18/00-18/09 */,
	128 + 3	/* 18/10-18/19 */,
	128 + 3	/* 18/20-18/29 */,
	128 + 3	/* 18/30-18/39 */,
	128 + 3	/* 18/40-18/49 */,
	128 + 4	/* 18/50-18/59 */,
	128 + 4	/* 18/60-18/69 */,
	128 + 4	/* 18/70-18/79 */,
	128 + 4	/* 18/80-18/89 */,
	128 + 5	/* 18/90-18/99 */,
	128 + 6	/* 18/100-18/109 */,
	128 + 7	/* 18/110-18/119 */,
	128 + 8	/* 18/120-18/129 */,
	128 + 9	/* 18/130-18/139 */,
	128 + 9	/* 18/140-18/149 */,
	128 + 10	/* 18/150-18/159 */,
	128 + 11	/* 18/160-18/169 */,
	128 + 12	/* 18/170-18/179 */,
	128 + 13	/* 18/180-18/189 */,
	128 + 14	/* 18/190-18/199 */,
	128 + 15	/* 18/200-18/209 */,
	128 + 15	/* 18/210-18/219 */,
	128 + 15	/* 18/220+ */
};


/*
 * Stat Table (STR) -- bonus to hit (plus 128)
 */
byte adj_str_th[] =
{
	128 + -3	/* 3 */,
	128 + -2	/* 4 */,
	128 + -1	/* 5 */,
	128 + -1	/* 6 */,
	128 + 0	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 0	/* 15 */,
	128 + 0	/* 16 */,
	128 + 0	/* 17 */,
	128 + 1	/* 18/00-18/09 */,
	128 + 1	/* 18/10-18/19 */,
	128 + 1	/* 18/20-18/29 */,
	128 + 1	/* 18/30-18/39 */,
	128 + 1	/* 18/40-18/49 */,
	128 + 1	/* 18/50-18/59 */,
	128 + 1	/* 18/60-18/69 */,
	128 + 2	/* 18/70-18/79 */,
	128 + 3	/* 18/80-18/89 */,
	128 + 4	/* 18/90-18/99 */,
	128 + 5	/* 18/100-18/109 */,
	128 + 6	/* 18/110-18/119 */,
	128 + 7	/* 18/120-18/129 */,
	128 + 8	/* 18/130-18/139 */,
	128 + 9	/* 18/140-18/149 */,
	128 + 10	/* 18/150-18/159 */,
	128 + 11	/* 18/160-18/169 */,
	128 + 12	/* 18/170-18/179 */,
	128 + 13	/* 18/180-18/189 */,
	128 + 14	/* 18/190-18/199 */,
	128 + 15	/* 18/200-18/209 */,
	128 + 15	/* 18/210-18/219 */,
	128 + 15	/* 18/220+ */
};


/*
 * Stat Table (STR) -- weight limit in deca-pounds
 */
byte adj_str_wgt[] =
{
	5	/* 3 */,
	6	/* 4 */,
	7	/* 5 */,
	8	/* 6 */,
	9	/* 7 */,
	10	/* 8 */,
	11	/* 9 */,
	12	/* 10 */,
	13	/* 11 */,
	14	/* 12 */,
	15	/* 13 */,
	16	/* 14 */,
	17	/* 15 */,
	18	/* 16 */,
	19	/* 17 */,
	20	/* 18/00-18/09 */,
	22	/* 18/10-18/19 */,
	24	/* 18/20-18/29 */,
	26	/* 18/30-18/39 */,
	28	/* 18/40-18/49 */,
	30	/* 18/50-18/59 */,
	30	/* 18/60-18/69 */,
	30	/* 18/70-18/79 */,
	30	/* 18/80-18/89 */,
	30	/* 18/90-18/99 */,
	30	/* 18/100-18/109 */,
	30	/* 18/110-18/119 */,
	30	/* 18/120-18/129 */,
	30	/* 18/130-18/139 */,
	30	/* 18/140-18/149 */,
	30	/* 18/150-18/159 */,
	30	/* 18/160-18/169 */,
	30	/* 18/170-18/179 */,
	30	/* 18/180-18/189 */,
	30	/* 18/190-18/199 */,
	30	/* 18/200-18/209 */,
	30	/* 18/210-18/219 */,
	30	/* 18/220+ */
};


/*
 * Stat Table (STR) -- weapon weight limit in pounds
 */
byte adj_str_hold[] =
{
	4	/* 3 */,
	5	/* 4 */,
	6	/* 5 */,
	7	/* 6 */,
	8	/* 7 */,
	10	/* 8 */,
	12	/* 9 */,
	14	/* 10 */,
	16	/* 11 */,
	18	/* 12 */,
	20	/* 13 */,
	22	/* 14 */,
	24	/* 15 */,
	26	/* 16 */,
	28	/* 17 */,
	30	/* 18/00-18/09 */,
	30	/* 18/10-18/19 */,
	35	/* 18/20-18/29 */,
	40	/* 18/30-18/39 */,
	45	/* 18/40-18/49 */,
	50	/* 18/50-18/59 */,
	55	/* 18/60-18/69 */,
	60	/* 18/70-18/79 */,
	65	/* 18/80-18/89 */,
	70	/* 18/90-18/99 */,
	80	/* 18/100-18/109 */,
	80	/* 18/110-18/119 */,
	80	/* 18/120-18/129 */,
	80	/* 18/130-18/139 */,
	80	/* 18/140-18/149 */,
	90	/* 18/150-18/159 */,
	90	/* 18/160-18/169 */,
	90	/* 18/170-18/179 */,
	90	/* 18/180-18/189 */,
	90	/* 18/190-18/199 */,
	100	/* 18/200-18/209 */,
	100	/* 18/210-18/219 */,
	100	/* 18/220+ */
};


/*
 * Stat Table (STR) -- digging value
 */
byte adj_str_dig[] =
{
	0	/* 3 */,
	0	/* 4 */,
	1	/* 5 */,
	2	/* 6 */,
	3	/* 7 */,
	4	/* 8 */,
	4	/* 9 */,
	5	/* 10 */,
	5	/* 11 */,
	6	/* 12 */,
	6	/* 13 */,
	7	/* 14 */,
	7	/* 15 */,
	8	/* 16 */,
	8	/* 17 */,
	9	/* 18/00-18/09 */,
	10	/* 18/10-18/19 */,
	12	/* 18/20-18/29 */,
	15	/* 18/30-18/39 */,
	20	/* 18/40-18/49 */,
	25	/* 18/50-18/59 */,
	30	/* 18/60-18/69 */,
	35	/* 18/70-18/79 */,
	40	/* 18/80-18/89 */,
	45	/* 18/90-18/99 */,
	50	/* 18/100-18/109 */,
	55	/* 18/110-18/119 */,
	60	/* 18/120-18/129 */,
	65	/* 18/130-18/139 */,
	70	/* 18/140-18/149 */,
	75	/* 18/150-18/159 */,
	80	/* 18/160-18/169 */,
	85	/* 18/170-18/179 */,
	90	/* 18/180-18/189 */,
	95	/* 18/190-18/199 */,
	100	/* 18/200-18/209 */,
	100	/* 18/210-18/219 */,
	100	/* 18/220+ */
};


/*
 * Stat Table (STR) -- help index into the "blow" table
 */
byte adj_str_blow[] =
{
	3	/* 3 */,
	4	/* 4 */,
	5	/* 5 */,
	6	/* 6 */,
	7	/* 7 */,
	8	/* 8 */,
	9	/* 9 */,
	10	/* 10 */,
	11	/* 11 */,
	12	/* 12 */,
	13	/* 13 */,
	14	/* 14 */,
	15	/* 15 */,
	16	/* 16 */,
	17	/* 17 */,
	20 /* 18/00-18/09 */,
	30 /* 18/10-18/19 */,
	40 /* 18/20-18/29 */,
	50 /* 18/30-18/39 */,
	60 /* 18/40-18/49 */,
	70 /* 18/50-18/59 */,
	80 /* 18/60-18/69 */,
	90 /* 18/70-18/79 */,
	100 /* 18/80-18/89 */,
	110 /* 18/90-18/99 */,
	120 /* 18/100-18/109 */,
	130 /* 18/110-18/119 */,
	140 /* 18/120-18/129 */,
	150 /* 18/130-18/139 */,
	160 /* 18/140-18/149 */,
	170 /* 18/150-18/159 */,
	180 /* 18/160-18/169 */,
	190 /* 18/170-18/179 */,
	200 /* 18/180-18/189 */,
	210 /* 18/190-18/199 */,
	220 /* 18/200-18/209 */,
	230 /* 18/210-18/219 */,
	240 /* 18/220+ */
};


/*
 * Stat Table (DEX) -- index into the "blow" table
 */
byte adj_dex_blow[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	0	/* 8 */,
	0	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	1	/* 15 */,
	1	/* 16 */,
	1	/* 17 */,
	1	/* 18/00-18/09 */,
	2	/* 18/10-18/19 */,
	2	/* 18/20-18/29 */,
	2	/* 18/30-18/39 */,
	2	/* 18/40-18/49 */,
	3	/* 18/50-18/59 */,
	3	/* 18/60-18/69 */,
	4	/* 18/70-18/79 */,
	4	/* 18/80-18/89 */,
	5	/* 18/90-18/99 */,
	6	/* 18/100-18/109 */,
	7	/* 18/110-18/119 */,
	8	/* 18/120-18/129 */,
	9	/* 18/130-18/139 */,
	10	/* 18/140-18/149 */,
	11	/* 18/150-18/159 */,
	12	/* 18/160-18/169 */,
	14	/* 18/170-18/179 */,
	16	/* 18/180-18/189 */,
	18	/* 18/190-18/199 */,
	20	/* 18/200-18/209 */,
	20	/* 18/210-18/219 */,
	20	/* 18/220+ */
};


/*
 * Stat Table (DEX) -- chance of avoiding "theft" and "falling"
 */
byte adj_dex_safe[] =
{
	0	/* 3 */,
	1	/* 4 */,
	2	/* 5 */,
	3	/* 6 */,
	4	/* 7 */,
	5	/* 8 */,
	5	/* 9 */,
	6	/* 10 */,
	6	/* 11 */,
	7	/* 12 */,
	7	/* 13 */,
	8	/* 14 */,
	8	/* 15 */,
	9	/* 16 */,
	9	/* 17 */,
	10	/* 18/00-18/09 */,
	10	/* 18/10-18/19 */,
	15	/* 18/20-18/29 */,
	15	/* 18/30-18/39 */,
	20	/* 18/40-18/49 */,
	25	/* 18/50-18/59 */,
	30	/* 18/60-18/69 */,
	35	/* 18/70-18/79 */,
	40	/* 18/80-18/89 */,
	45	/* 18/90-18/99 */,
	50	/* 18/100-18/109 */,
	60	/* 18/110-18/119 */,
	70	/* 18/120-18/129 */,
	80	/* 18/130-18/139 */,
	90	/* 18/140-18/149 */,
	100	/* 18/150-18/159 */,
	100	/* 18/160-18/169 */,
	100	/* 18/170-18/179 */,
	100	/* 18/180-18/189 */,
	100	/* 18/190-18/199 */,
	100	/* 18/200-18/209 */,
	100	/* 18/210-18/219 */,
	100	/* 18/220+ */
};


/*
 * Stat Table (CON) -- base regeneration rate
 */
byte adj_con_fix[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	0	/* 8 */,
	0	/* 9 */,
	0	/* 10 */,
	0	/* 11 */,
	0	/* 12 */,
	0	/* 13 */,
	1	/* 14 */,
	1	/* 15 */,
	1	/* 16 */,
	1	/* 17 */,
	2	/* 18/00-18/09 */,
	2	/* 18/10-18/19 */,
	2	/* 18/20-18/29 */,
	2	/* 18/30-18/39 */,
	2	/* 18/40-18/49 */,
	3	/* 18/50-18/59 */,
	3	/* 18/60-18/69 */,
	3	/* 18/70-18/79 */,
	3	/* 18/80-18/89 */,
	3	/* 18/90-18/99 */,
	4	/* 18/100-18/109 */,
	4	/* 18/110-18/119 */,
	5	/* 18/120-18/129 */,
	6	/* 18/130-18/139 */,
	6	/* 18/140-18/149 */,
	7	/* 18/150-18/159 */,
	7	/* 18/160-18/169 */,
	8	/* 18/170-18/179 */,
	8	/* 18/180-18/189 */,
	8	/* 18/190-18/199 */,
	9	/* 18/200-18/209 */,
	9	/* 18/210-18/219 */,
	9	/* 18/220+ */
};


/*
 * Stat Table (CON) -- extra half-hitpoints per level (plus 128)
 */
byte adj_con_mhp[] =
{
	128 + -5	/* 3 */,
	128 + -3	/* 4 */,
	128 + -2	/* 5 */,
	128 + -1	/* 6 */,
	128 + 0	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 1	/* 15 */,
	128 + 1	/* 16 */,
	128 + 2	/* 17 */,
	128 + 3	/* 18/00-18/09 */,
	128 + 4	/* 18/10-18/19 */,
	128 + 4	/* 18/20-18/29 */,
	128 + 4	/* 18/30-18/39 */,
	128 + 4	/* 18/40-18/49 */,
	128 + 5	/* 18/50-18/59 */,
	128 + 6	/* 18/60-18/69 */,
	128 + 7	/* 18/70-18/79 */,
	128 + 8	/* 18/80-18/89 */,
	128 + 9	/* 18/90-18/99 */,
	128 + 10	/* 18/100-18/109 */,
	128 + 11	/* 18/110-18/119 */,
	128 + 12	/* 18/120-18/129 */,
	128 + 13	/* 18/130-18/139 */,
	128 + 14	/* 18/140-18/149 */,
	128 + 15	/* 18/150-18/159 */,
	128 + 16	/* 18/160-18/169 */,
	128 + 18	/* 18/170-18/179 */,
	128 + 20	/* 18/180-18/189 */,
	128 + 22	/* 18/190-18/199 */,
	128 + 25	/* 18/200-18/209 */,
	128 + 25	/* 18/210-18/219 */,
	128 + 25	/* 18/220+ */
};


/*
 * This table is used to help calculate the number of blows the player can
 * make in a single round of attacks (one player turn) with a normal weapon.
 *
 * This number ranges from a single blow/round for weak players to up to six
 * blows/round for powerful warriors.
 *
 * Note that certain artifacts and ego-items give "bonus" blows/round.
 *
 * First, from the player class, we extract some values:
 *
 *    Warrior --> num = 6; mul = 5; div = MAX(30, weapon_weight);
 *    Mage    --> num = 4; mul = 2; div = MAX(40, weapon_weight);
 *    Priest  --> num = 5; mul = 3; div = MAX(35, weapon_weight);
 *    Rogue   --> num = 5; mul = 3; div = MAX(30, weapon_weight);
 *    Ranger  --> num = 5; mul = 4; div = MAX(35, weapon_weight);
 *    Paladin --> num = 5; mul = 4; div = MAX(30, weapon_weight);
 *
 * To get "P", we look up the relevant "adj_str_blow[]" (see above),
 * multiply it by "mul", and then divide it by "div", rounding down.
 *
 * To get "D", we look up the relevant "adj_dex_blow[]" (see above),
 * note especially column 6 (DEX 18/101) and 11 (DEX 18/150).
 *
 * The player gets "blows_table[P][D]" blows/round, as shown below,
 * up to a maximum of "num" blows/round, plus any "bonus" blows/round.
 */
byte blows_table[12][12] =
{
	/* P/D */
	/* 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11+ */

	/* 0  */
	{  1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   3 },

	/* 1  */
	{  1,   1,   1,   1,   2,   2,   3,   3,   3,   4,   4,   4 },

	/* 2  */
	{  1,   1,   2,   2,   3,   3,   4,   4,   4,   5,   5,   5 },

	/* 3  */
	{  1,   2,   2,   3,   3,   4,   4,   4,   5,   5,   5,   5 },

	/* 4  */
	{  1,   2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5 },

	/* 5  */
	{  2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5,   6 },

	/* 6  */
	{  2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5,   6 },

	/* 7  */
	{  2,   3,   3,   4,   4,   4,   5,   5,   5,   5,   5,   6 },

	/* 8  */
	{  3,   3,   3,   4,   4,   4,   5,   5,   5,   5,   6,   6 },

	/* 9  */
	{  3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   6,   6 },

	/* 10 */
	{  3,   3,   4,   4,   4,   4,   5,   5,   5,   6,   6,   6 },

	/* 11+ */
	{  3,   3,   4,   4,   4,   4,   5,   5,   6,   6,   6,   6 },
};


#if 0

/*
 * This is the "old" table used to calculate multiple blows.
 *
 * Note that this table used a different indexing scheme to determine "P"
 */

byte old_blows_table[11][12] =
{
	/* P/D */
	/* 3,  10, /01, /50, /90,/100,/101,/110,/120,/130,/140,/150 */

	/* 0+ */
	{  1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   3},

	/* 2+ */
	{  1,   1,   1,   1,   2,   2,   3,   3,   3,   3,   3,   4},

	/* 3+ */
	{  1,   1,   1,   2,   2,   3,   4,   4,   4,   4,   4,   5},

	/* 4+ */
	{  1,   1,   2,   2,   3,   3,   4,   4,   4,   5,   5,   5},

	/* 6+ */
	{  1,   2,   2,   3,   3,   4,   4,   4,   5,   5,   5,   5},

	/* 8+ */
	{  1,   2,   2,   3,   4,   4,   4,   5,   5,   5,   5,   5},

	/* 10+ */
	{  2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5,   6},

	/* 13+ */
	{  2,   3,   3,   3,   4,   4,   5,   5,   5,   5,   5,   6},

	/* 15+ */
	{  3,   3,   3,   4,   4,   4,   5,   5,   5,   5,   6,   6},

	/* 18+ */
	{  3,   3,   3,   4,   4,   4,   5,   5,   5,   5,   6,   6},

	/* 20+ */
	{  3,   3,   4,   4,   4,   4,   5,   5,   5,   6,   6,   6}
};

#endif

#endif


/*
 * Store owners (exactly four "possible" owners per store, chosen randomly)
 * { name, purse, max greed, min greed, haggle_per, tolerance, race, unused }
 */
owner_type owners[MAX_STORES][MAX_OWNERS] =
{
	{
		/* General store */
		{ "Bilbo the Friendly",		200*15,	170, 108,  5, 15, RACE_HOBBIT},
		{ "Rincewind the Chicken",	200*15,	175, 108,  4, 12, RACE_HUMAN},
		{ "Sultan the Midget",		300*15,	170, 107,  5, 15, RACE_GNOME},
		{ "Lyar-el the Comely",		300*15,	165, 107,  6, 18, RACE_ELF},
	},
	{
		/* Armoury */
		{ "Kon-Dar the Ugly",		5000*15,	210, 115,  5,  7, RACE_HALF_ORC},
		{ "Darg-Low the Grim",		10000*15,	190, 111,  4,  9, RACE_HUMAN},
		{ "Decado the Handsome",	25000*15,  	200, 112,  4, 10, RACE_DUNADAN},
		{ "Mauglin the Grumpy",		30000*15,	200, 112,  4,  5, RACE_DWARF},
	},
	{
		/* Weapon Smith */
		{ "Ithyl-Mak the Beastly",	5000*15,	210, 115,  6,  6, RACE_HALF_TROLL},
		{ "Arndal Beast-Slayer",	10000*15,	185, 110,  5,  9, RACE_HALF_ELF},
		{ "Tarl Beast-Master",		25000*15,	190, 115,  5,  7, RACE_HOBBIT},
		{ "Oglign Dragon-Slayer",	30000*15,	195, 112,  4,  8, RACE_DWARF},
	},
	{
		/* Temple */
		{ "Ludwig the Humble",		5000*15,	175, 109,  6, 15, RACE_HUMAN},
		{ "Gunnar the Paladin",		10000*15,	185, 110,  5, 23, RACE_HUMAN},
		{ "Delilah the Pure",		25000*15,	180, 107,  6, 20, RACE_ELF},
		{ "Keldon the Wise",		30000*15,	185, 109,  5, 15, RACE_DWARF},
	},
	{
		{ "Mauser the Chemist",		10000*15,	190, 111,  5,  8, RACE_HALF_ELF},
		{ "Wizzle the Chaotic",		10000*15,	190, 110,  6,  8, RACE_HOBBIT},
		{ "Ga-nat the Greedy",		15000*15,	200, 116,  6,  9, RACE_GNOME},
		{ "Sasha the Slender",		15000*15,	220, 111,  4,  9, RACE_ELF},
	},
	{
		/* Magic Shop */
		{ "Ariel the Sorceress",	20000*15,	200, 110,  7,  8, RACE_HALF_ELF},
		{ "Buggerby the Great",		20000*15,	215, 113,  6, 10, RACE_GNOME},
		{ "Inglorian the Mage",		30000*15,	200, 110,  7, 10, RACE_HUMAN},
		{ "Luthien Starshine",		30000*15,	175, 110,  5, 11, RACE_HIGH_ELF},
	},
	{
		/* Black Market */
		{ "Lo-Hak the Awful",		20000*25,	250, 150, 10,  5, RACE_HALF_TROLL},
		{ "Histor the Goblin",		20000*25,	250, 150, 10,  5, RACE_HALF_ORC},
		{ "Durwin the Shifty",		30000*25,	250, 150, 10,  5, RACE_HUMAN},
		{ "Drago the Fair",		30000*25,	250, 150, 10,  5, RACE_ELF},
	},
	{
		/* Home */
		{ "Your home",				0,      100, 100,  0, 99, 99},
		{ "Your home",				0,      100, 100,  0, 99, 99},
		{ "Your home",				0,      100, 100,  0, 99, 99},
		{ "Your home",				0,      100, 100,  0, 99, 99}
	}
};



#if 0

/*
 * This table allows quick conversion from "speed" to "energy"
 * The basic function WAS ((S>=110) ? (S-110) : (100 / (120-S)))
 * Note that table access is *much* quicker than computation.
 *
 * Note that the table has been changed at high speeds.  From
 * "Slow (-40)" to "Fast (+30)" is pretty much unchanged, but
 * at speeds above "Fast (+30)", one approaches an asymptotic
 * effective limit of 50 energy per turn.  This means that it
 * is relatively easy to reach "Fast (+30)" and get about 40
 * energy per turn, but then speed becomes very "expensive",
 * and you must get all the way to "Fast (+50)" to reach the
 * point of getting 45 energy per turn.  After that point,
 * furthur increases in speed are more or less pointless,
 * except to balance out heavy inventory.
 *
 * Note that currently the fastest monster is "Fast (+30)".
 *
 * It should be possible to lower the energy threshhold from
 * 100 units to 50 units, though this may interact badly with
 * the (compiled out) small random energy boost code.  It may
 * also tend to cause more "clumping" at high speeds.
 */
byte extract_energy[200] =
{
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* S-50 */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* S-40 */     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
	/* S-30 */     2,  2,  2,  2,  2,  2,  2,  3,  3,  3,
	/* S-20 */     3,  3,  3,  3,  3,  4,  4,  4,  4,  4,
	/* S-10 */     5,  5,  5,  5,  6,  6,  7,  7,  8,  9,
	/* Norm */    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	/* F+10 */    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
	/* F+20 */    30, 31, 32, 33, 34, 35, 36, 36, 37, 37,
	/* F+30 */    38, 38, 39, 39, 40, 40, 40, 41, 41, 41,
	/* F+40 */    42, 42, 42, 43, 43, 43, 44, 44, 44, 44,
	/* F+50 */    45, 45, 45, 45, 45, 46, 46, 46, 46, 46,
	/* F+60 */    47, 47, 47, 47, 47, 48, 48, 48, 48, 48,
	/* F+70 */    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
	/* Fast */    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
};







/*
 * Base experience levels, may be adjusted up for race and/or class
 */
s32b player_exp[PY_MAX_LEVEL] =
{
	10,
	25,
	45,
	70,
	100,
	140,
	200,
	280,
	380,
	500,
	650,
	850,
	1100,
	1400,
	1800,
	2300,
	2900,
	3600,
	4400,
	5400,
	6800,
	8400,
	10200,
	12500,
	17500,
	25000,
	35000L,
	50000L,
	75000L,
	100000L,
	150000L,
	200000L,
	275000L,
	350000L,
	450000L,
	550000L,
	700000L,
	850000L,
	1000000L,
	1250000L,
	1500000L,
	1800000L,
	2100000L,
	2400000L,
	2700000L,
	3000000L,
	3500000L,
	4000000L,
	4500000L,
	5000000L
};

#endif

/*
 * Player Race Information:
 *    Title,
 *    {STR,INT,WIS,DEX,CON,CHR},
 *    r_dis, r_dev, r_sav, r_stl, r_srh, r_fos, r_thn, r_thb,
 *    hitdie, exp base,
 *    Age (Base, Mod),
 *    Male (Hgt, Wgt),
 *    Female (Hgt, Wgt)
 *    infra,
 *    class-choices
 */
player_race race_info[MAX_RACES] =
{
	{
		"Human",
		{  0,  0,  0,  0,  0,  0 },
		0,  0,  0,  0,  0,  10,  0,  0,
		10,  100,
		14,  6,
		72,  6, 180, 25,
		66,  4, 150, 20,
		0,
                0xFFFFFFFF,
                100,
	},

	{
		"Half-Elf",
		{ -1,  1,  0,  1, -1,  1 },
		2,  3,  3,  1, 6,  11, -1,  5,
		9,  110,
		24, 16,
		66,  6, 130, 15,
		62,  6, 100, 10,
		2,
                0xFFFFFFFF - BITS(CLASS_MIMIC) - BITS(CLASS_PALADIN),
                110,
	},

	{
		"Elf",
		{ -1,  2,  1,  1, -2,  1 },
		5,  6,  6,  1, 8,  12, -5, 15,
		8,  120,
		75, 75,
		60,  4, 100,  6,
		54,  4, 80,  6,
		3,
                0xFFFFFFFF - BITS(CLASS_MIMIC) - BITS(CLASS_PALADIN) - BITS(CLASS_UNBELIEVER),
                120,
	},

	{
		"Hobbit",
		{ -2,  2,  1,  3,  2,  1 },
		15, 18, 18, 4, 12,  15, -10, 20,
		7,  110,
		21, 12,
		36,  3, 60,  3,
		33,  3, 50,  3,
		4,
                0xFFFFFFFF - BITS(CLASS_PALADIN) - BITS(CLASS_MONK) - BITS(CLASS_TELEPATH),
                90,
	},

	{
		"Gnome",
		{ -1,  2,  0,  2,  1, -2 },
		10, 12, 12,  3, 6,  13, -8, 12,
		8,  125,
		50, 40,
		42,  3, 90,  6,
		39,  3, 75,  3,
		4,
                0xFFFFFFFF - BITS(CLASS_UNBELIEVER),
                120,
	},

	{
		"Dwarf",
		{  2, -3,  2, -2,  2, -3 },
		2,  9,  9,  -1,  7,  10, 15,  0,
		11,  120,
		35, 15,
		48,  3, 150, 10,
		46,  3, 120, 10,
		5,
                0xFFFFFFFF - BITS(CLASS_MAGE) - BITS(CLASS_SORCERER) - BITS(CLASS_TELEPATH),
                95,
	},

	{
		"Half-Orc",
		{  2, -1,  0,  0,  1, -4 },
		-3, -3, -3,  -1,  0, 7, 12, -5,
		10,  110,
		11,  4,
		66,  1, 150,  5,
		62,  1, 120,  5,
		3,
                0xFFFFFFFF - BITS(CLASS_MAGE) - BITS(CLASS_SORCERER) - BITS(CLASS_TELEPATH),
                50,
	},

	{
		"Half-Troll",
		{ 4, -4, -2, -4,  3, -6 },
		-5, -8, -8, -2,  -1, 5, 20, -10,
		12,  120,
		20, 10,
		96, 10, 250, 50,
		84,  8, 225, 40,
		3,
                0xFFFFFFFF - BITS(CLASS_MAGE) - BITS(CLASS_SORCERER) - BITS(CLASS_TELEPATH),
                20,
	},

	{
		"Dunadan",
		{  1,  2,  2,  2,  3,  2 },
		4,  5,  5,  2, 3, 13, 15, 10,
		10,  180,
		50, 20,
		82, 5, 190, 20,
		78,  6, 180, 15,
		0,
                0xFFFFFFFF,
                105,
        },

	{
		"High-Elf",
		{  1,  3, -1,  3,  1,  5 },
		4,  20, 20,  3,  3, 14, 10, 25,
		10,  200,
		100, 30,
		90, 10, 190, 20,
		82, 10, 180, 15,
		4,
                0xFFFFFFFF - BITS(CLASS_UNBELIEVER),
                125,
	},

	{
		"Yeek",
		{  -5, -5, -5, -5, -5, -5 },
		0,  0,  0,  0,  0,  10,  0,  0,
		8,  30,
		14,  6,
		72,  6, 180, 25,
		66,  4, 150, 20,
		0,
                0xFFFFFFFF,
                50,
	},

	{
		"Goblin",
		{  0, -1, -5,  4,  2, -5 },
		2, -3, -3,  0,  2, 8, 12, -5,
		11,  115,
		11,  4,
		66,  1, 150,  5,
		62,  1, 120,  5,
		3,
                0xFFFFFFFF - BITS(CLASS_SORCERER) - BITS(CLASS_TELEPATH),
                70,
	},

	{
                "Ent",
		{ 8, -4, +2, -4,  8, -5 },
                5, 0, 20, -6, 5, 4, 15, 5,
                14, 300,
                255, 70,
		72, 6, 100, 25,
		66, 4, 100, 20,
		5,
                0xFFFFFFFF - BITS(CLASS_MAGE) - BITS(CLASS_SORCERER),
                90,
	},
	{
                "DragonRider",
                {  6,  3,  -10,  0,  5,  5 },
                6,  0,  10,  -16,  30,  10,  15,  5,
                12,  450,
		14,  6,
                180,  6, 255, 25,
                150,  4, 230, 20,
                0,
                0xFFFFFFFF - BITS(CLASS_MIMIC),
                95,
        },
};
/*
 * Player Race Information:
 *    Title,
 *    {STR,INT,WIS,DEX,CON,CHR},
 *    r_dis, r_dev, r_sav, r_stl, r_srh, r_fos, r_thn, r_thb,
 *    hitdie, exp base,
 *    Age (Base, Mod),
 *    Male (Hgt, Wgt),
 *    Female (Hgt, Wgt)
 *    infra,
 *    class-choices
 */


/*
 * Player Classes.
 *
 *   Title,
 *   {STR,INT,WIS,DEX,CON,CHR},
 *   c_dis, c_dev, c_sav, c_stl, c_srh, c_fos, c_thn, c_thb,
 *   x_dis, x_dev, x_sav, x_stl, x_srh, x_fos, x_thn, x_thb,
 *   HD, Exp
 */
player_class class_info[MAX_CLASS] =
{
	{
		"Warrior",
		{ 5, -2, -2, 2, 2, -1},
		25, 18, 18, 1,  14, 2, 70, 55,
		10, 7,  10, 0,  0,  0,  45, 45,
		9,  0
	},

	{
		"Warlock",
		{-5, 3, 0, 1, -2, 1},
		30, 36, 30, 2,  16, 20, 34, 20,
		7,  13, 9,  0,  0,  0,  15, 15,
		0, 30
	},

	{
		"Priest",
		{-1, -3, 3, -1, 0, 2},
		25, 30, 32, 2,  16, 8, 48, 35,
		7,  10, 12, 0,  0,  0, 20, 20,
		2, 20
	},

	{
		"Rogue",
		{ 2, 1, -2, 3, 1, -1},
		45, 32, 28, 5, 32, 24, 60, 66,
		15, 10, 10, 0,  0,  0, 40, 30,
		6, 25
	},

	{
		"Ranger",
		{ 2, 2, 0, 1, 1, 1},
		30, 32, 28, 3,  24, 16, 56, 72,
		8,  10, 10, 0,  0,  0,  30, 45,
		4, 30
	},

	{
		"Paladin",
		{ 3, -3, 1, 0, 2, 2},
		20, 24, 25, 1,  12, 2, 68, 40,
		7,  10, 11, 0,  0,  0,  35, 30,
		6, 35
	},

	{
		"Sorceror",
		{-5, 5, 1, 0, -1, 0},
		35, 40, 30, 2, 16, 20, 30, 20,
		8,  15,  9, 0,  0,  0, 11, 15,
		0, 30
	},

	{
		"Mimic",
		{ 1, 0, 0, 1, 1, -4},
		25, 35, 28, 7, 18, 18, 60, 66,
	         9, 10, 10, 0,  0,  0, 40, 30,
		6, 25
	},
	
	{
		"Unbeliever",
		{ 5, -5, -5, 3, 3, 1},
		25, 3, 32, 2,  18, 4, 75, 55,
		15, 2, 14, 0,  0,  0, 50, 45,
		9,  0
	},

	{
		"Archer",
		{ 1, 0, 2, 3, 1, -1},
		30, 32, 28, 2,  24, 20, 56, 82,
		8,  10, 10, 0,  0,  0,  30, 55,
		4, 30
	},

	{
		"Monk",
		{ 2, -1, 1, 3, 2, 1},
		45, 32, 28, 5, 32, 24, 64, 60,
		15, 11, 10, 0,  0,  0, 40, 30,
		6, 40
	},

	{
		"Telepath",
		{ -1, 1, 1, 0, 0, 4},
		30, 36, 30, 2,  16, 20, 34, 20,
		7,  13, 9,  0,  0,  0,  15, 15,
		0, 20
	},
};





/*
 * Spells in each book (mage spells then priest spells)
 */
u32b spell_flags[3][9][2] =
{
	{
		/*** Mage spell books ***/
		{ 0x000001ff, 0x00000000 },
		{ 0x0003fe00, 0x00000000 },
		{ 0x03fc0000, 0x00000000 },
		{ 0xfc000000, 0x00000001 },
		{ 0x00000000, 0x003e0000 },
		{ 0x00000000, 0x0000003e },
		{ 0x00000000, 0x0001f000 },
		{ 0x00000000, 0x07c00000 },
		{ 0x00000000, 0x00000fc0 }
	},

	{
		/*** Priest spell books ***/
		{ 0x000000ff, 0x00000000 },
		{ 0x0000ff00, 0x00000000 },
		{ 0x01ff0000, 0x00000000 },
		{ 0x7e000000, 0x00000000 },
		{ 0x00000000, 0x03f00000 },
		{ 0x80000000, 0x0000000f },
		{ 0x00000000, 0x000001f0 },
		{ 0x00000000, 0x000fc000 },
		{ 0x00000000, 0x00003e00 }
	},
	
	{
		/*** Sorcery spell books ***/
		{ 0x000001ff, 0x00000000 },
		{ 0x0003fe00, 0x00000000 },
		{ 0x03fc0000, 0x00000000 },
		{ 0xfc000000, 0x00000001 },
		{ 0x00000000, 0x003e0000 },
		{ 0x00000000, 0x0000003e },
		{ 0x00000000, 0x0001f000 },
		{ 0x00000000, 0x07c00000 },
		{ 0x00000000, 0x00000fc0 }
	},
};


/*
 * Names of the spells (mage spells then priest spells)
 */
cptr spell_names[3][64] =
{
	/*** Mage Spells ***/

	{
		/* Magic for Beginners (sval 0) */
		"Magic Missile",
		"Detect Monsters",
		"Phase Door",
		"Light Area",
		"Treasure Detection",
		"Cure Light Wounds",
		"Object Detection",
		"Find Hidden Traps/Doors",
		"Stinking Cloud",

		/* Conjurings and Tricks (sval 1) */
		"Confuse Monster",
		"Lightning Bolt",
		"Trap/Door Destruction",
		"Sleep I",
		"Cure Poison",
		"Teleport Self",
		"Spear of Light",
		"Frost Bolt",
		"Turn Stone to Mud",

		/* Incantations and Illusions (sval 2) */
		"Satisfy Hunger",
		"Recharge Item I",
		"Sleep II",
		"Polymorph Other",
		"Identify",
		"Sleep III",
		"Fire Bolt",
		"Slow Monster",

		/* Sorcery and Evocations (sval 3) */
		"Frost Ball",
		"Recharge Item II",
		"Teleport Other",
		"Haste Self",
		"Fire Ball",
		"Word of Destruction",
		"Genocide",

		/* Mordenkainen's Escapes (sval 5) */
		"Door Creation",
		"Stair Creation",
		"Teleport Level",
		"Earthquake",
		"Word of Recall",

		/* Raal's Tome of Destruction (sval 8) */
		"Acid Bolt",
		"Cloud Kill",
		"Acid Ball",
		"Ice Storm",
		"Meteor Swarm",
		"Mana Storm",

		/* Kelek's Grimoire of Power (sval 6) */
		"Detect Evil",
		"Detect Enchantment",
		"Recharge Item III",
		"Genocide",
		"Mass Genocide",

		/* Resistance of Scarabtarices (sval 4) */
		"Resist Fire",
		"Resist Cold",
		"Resist Acid",
		"Resist Poison",
		"Resistance",

		/* Tenser's transformations... (sval 7) */
		"Heroism",
		"Shield",
		"Berserker",
		"Essence of Speed",
		"Globe of Invulnerability",

		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)"
	},


	/*** Priest Spells ***/

	{
		/* Beginners Handbook (sval 0) */
		"Detect Evil",
		"Cure Light Wounds",
		"Bless",
		"Remove Fear",
		"Call Light",
		"Find Traps",
		"Detect Doors/Stairs",
		"Slow Poison",

		/* Words of Wisdom (sval 1) */
		"Scare Monster",
		"Portal",
		"Cure Serious Wounds",
		"Chant",
		"Sanctuary",
		"Satisfy Hunger",
		"Remove Curse",
		"Resist Heat and Cold",

		/* Chants and Blessings (sval 2) */
		"Neutralize Poison",
		"Orb of Draining",
		"Cure Critical Wounds",
		"Sense Invisible",
		"Protection from Evil",
		"Earthquake",
		"Sense Surroundings",
		"Cure Mortal Wounds",
		"Turn Undead",

		/* Exorcism and Dispelling (sval 3) */
		"Prayer",
		"Dispel Undead",
		"Heal",
		"Dispel Evil",
		"Glyph of Warding",
		"Holy Word",

		/* Godly Insights... (sval 5) */
		"Detect Monsters",
		"Detection",
		"Perception",
		"Probing",
		"Clairvoyance",

		/* Purifications and Healing (sval 6) */
		"Cure Serious Wounds",
		"Cure Mortal Wounds",
		"Healing",
		"Restoration",
		"Remembrance",

		/* Wrath of God (sval 8) */
		"Dispel Undead",
		"Dispel Evil",
		"Banishment",
		"Word of Destruction",
		"Annihilation",

		/* Holy Infusions (sval 7) */
		"Unbarring Ways",
		"Recharging",
		"Dispel Curse",
		"Enchant Weapon",
		"Enchant Armour",
		"Elemental Brand",

		/* Ethereal openings (sval 4) */
		"Blink",
		"Teleport Self",
		"Teleport Other",
		"Teleport Level",
		"Word of Recall",
		"Alter Reality",

		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)"
	},
	
	/*** Sorcery Spells ***/
	{
		/* Magic for Beginners (sval 0) */
		"Magic Missile",
		"Detect Creatures",
		"Phase Door",
		"Light Area",
		"Treasure Detection",
		"Cure Light Wounds",
		"Object Detection",
		"Find Hidden Traps/Doors",
		"Stinking Cloud",

		/* Conjurings and Tricks (sval 1) */
		"Confuse Monster",
		"Lightning Bolt",
		"Trap/Door Destruction",
		"Sleep I",
		"Cure Poison",
		"Teleport Self",
		"Spear of Light",
		"Frost Bolt",
		"Turn Stone to Mud",

		/* Incantations and Illusions (sval 2) */
		"Satisfy Hunger",
		"Recharge Item I",
		"Tidal Wave",
		"Polymorph Other",
		"Identify",
		"Sleep II",
		"Fire Bolt",
		"Slow Monster",

		/* Sorcery and Evocations (sval 3) */
		"Frost Ball",
		"Recharge Item II",
		"Teleport Other",
		"Haste Self",
		"Fire Ball",
		"Word of Destruction",
		"Genocide",

		/* Mordenkainen's Escapes (sval 5) */
		"Door Creation",
		"Stair Creation",
		"Teleport Level",
		"Earthquake",
		"Word of Recall",

		/* Raal's Tome of Destruction (sval 8) */
		"Cloud Kill",
		"Acid Ball",
		"Ice Storm",
		"Meteor Swarm",
		"Mana Storm",
		"Mana Strike",

		/* Kelek's Grimoire of Power (sval 6) */
		"Detect Evil",
		"Detect Enchantment",
		"Recharge Item III",
		"Genocide",
		"Mass Genocide",

		/* Resistance of Scarabtarices (sval 4) */
		"Resist Fire",
		"Resist Cold",
		"Resist Acid",
		"Resist Poison",
		"Resistance",

		/* Tenser's transformations... (sval 7) */
		"Heroism",
		"Shield",
		"Berserker",
		"Essence of Speed",
		"Globe of Invulnerability",

		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)"
	},
};



/*
 * Each chest has a certain set of traps, determined by pval
 * Each chest has a "pval" from 1 to the chest level (max 55)
 * If the "pval" is negative then the trap has been disarmed
 * The "pval" of a chest determines the quality of its treasure
 * Note that disarming a trap on a chest also removes the lock.
 */
byte chest_traps[64] =
{
	0,					/* 0 == empty */
	(CHEST_POISON),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),			/* 5 == best small wooden */
	0,
	(CHEST_POISON),
	(CHEST_POISON),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),
	(CHEST_POISON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_SUMMON),			/* 15 == best large wooden */
	0,
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),
	(CHEST_PARALYZE),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_SUMMON),
	(CHEST_PARALYZE),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),
	(CHEST_EXPLODE),			/* 25 == best small iron */
	0,
	(CHEST_POISON | CHEST_LOSE_STR),
	(CHEST_POISON | CHEST_LOSE_CON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_PARALYZE),
	(CHEST_POISON | CHEST_SUMMON),
	(CHEST_SUMMON),
	(CHEST_EXPLODE),
	(CHEST_EXPLODE | CHEST_SUMMON),	/* 35 == best large iron */
	0,
	(CHEST_SUMMON),
	(CHEST_EXPLODE),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_POISON | CHEST_PARALYZE),
	(CHEST_EXPLODE),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_POISON | CHEST_PARALYZE),	/* 45 == best small steel */
	0,
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_POISON | CHEST_PARALYZE | CHEST_LOSE_STR),
	(CHEST_POISON | CHEST_PARALYZE | CHEST_LOSE_CON),
	(CHEST_POISON | CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_POISON | CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_POISON | CHEST_PARALYZE | CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_POISON | CHEST_PARALYZE),
	(CHEST_POISON | CHEST_PARALYZE),	/* 55 == best large steel */
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
};








/*
 * Class titles for the player.
 *
 * The player gets a new title every five levels, so each class
 * needs only ten titles total.
 */
cptr player_title[MAX_CLASS][PY_MAX_LEVEL/5] =
{
	/* Warrior */
	{
		"Rookie",
		"Soldier",
		"Mercenary",
		"Veteran",
		"Swordsman",
		"Champion",
		"Hero",
		"Baron",
		"Duke",
		"Lord",
	},

	/* Mage */
	{
		"Novice",
		"Apprentice",
		"Trickster",
		"Illusionist",
		"Spellbinder",
		"Evoker",
		"Conjurer",
		"Warlock",
		"Sorcerer",
		"Mage Lord",
	},

	/* Priest */
	{
		"Believer",
		"Acolyte",
		"Adept",
		"Curate",
		"Canon",
		"Lama",
		"Patriarch",
		"Priest",
		"High Priest",
		"Priest Lord",
	},

	/* Rogues */
	{
		"Vagabond",
		"Cutpurse",
		"Robber",
		"Burglar",
		"Filcher",
		"Sharper",
		"Low Thief",
		"High Thief",
		"Master Thief",
		"Assassin",
	},

	/* Rangers */
	{
		"Runner",
		"Strider",
		"Scout",
		"Courser",
		"Tracker",
		"Guide",
		"Pathfinder",
		"Low Ranger",
		"High Ranger",
		"Ranger Lord",
	},

	/* Paladins */
	{
		"Gallant",
		"Keeper",
		"Protector",
		"Defender",
		"Warder",
		"Knight",
		"Guardian",
		"Low Paladin",
		"High Paladin",
		"Paladin Lord",
	},

	/* Sorcerer */
	{
		"Novice",
		"Apprentice",
		"Trickster",
		"Illusionist",
		"Spellbinder",
		"Evoker",
		"Conjurer",
		"Warlock",
		"Sorcerer",
		"Mage Lord",
	},
};



/*
 * Hack -- the "basic" color names (see "TERM_xxx")
 */
cptr color_names[16] =
{
	"Dark",
	"White",
	"Slate",
	"Orange",
	"Red",
	"Green",
	"Blue",
	"Umber",
	"Light Dark",
	"Light Slate",
	"Violet",
	"Yellow",
	"Light Red",
	"Light Green",
	"Light Blue",
	"Light Umber",
};


/*
 * Hack -- the "basic" sound names (see "SOUND_xxx")
 */
cptr sound_names[SOUND_MAX] =
{
	"",
	"hit",
	"miss",
	"flee",
	"drop",
	"kill",
	"level",
	"death",
};



/*
 * Abbreviations of healthy stats
 */
cptr stat_names[6] =
{
	"STR: ", "INT: ", "WIS: ", "DEX: ", "CON: ", "CHR: "
};

/*
 * Abbreviations of damaged stats
 */
cptr stat_names_reduced[6] =
{
	"Str: ", "Int: ", "Wis: ", "Dex: ", "Con: ", "Chr: "
};


/*
 * Standard window names
 */
cptr ang_term_name[8] =
{
	"Angband",
	"Mirror",
	"Recall",
	"Choice",
	"Term-4",
	"Term-5",
	"Term-6",
	"Term-7"
};


/*
 * Certain "screens" always use the main screen, including News, Birth,
 * Dungeon, Tomb-stone, High-scores, Macros, Colors, Visuals, Options.
 *
 * Later, special flags may allow sub-windows to "steal" stuff from the
 * main window, including File dump (help), File dump (artifacts, uniques),
 * Character screen, Small scale map, Previous Messages, Store screen, etc.
 *
 * The "ctrl-i" (tab) command flips the "Display inven/equip" and "Display
 * equip/inven" flags for all windows.
 *
 * The "ctrl-g" command (or pseudo-command) should perhaps grab a snapshot
 * of the main screen into any interested windows.
 */
cptr window_flag_desc[32] =
{
	"Display inven/equip",
	"Display equip/inven",
	"Display spell list",
	"Display character",
	NULL,
	NULL,
	"Display messages",
	"Display overhead view",
	"Display monster recall",
	"Display object recall",
	NULL,
	"Display snap-shot",
	NULL,
	NULL,
	"Display borg messages",
	"Display borg status",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};


/*
 * Available Options
 *
 * Option Screen Sets:
 *
 *	Set 1: User Interface
 *	Set 2: Disturbance
 *	Set 3: Inventory
 *	Set 4: Game Play
 *
 * Note that bits 28-31 of set 0 are currently unused.
 */
option_type option_info[] =
{
	/*** User-Interface ***/

	{ &rogue_like_commands,	FALSE,	1,	0, 0,
	"rogue_like_commands",	"Rogue-like commands" },

	{ &quick_messages,	 	FALSE,	1,	0, 1,
	"quick_messages",		"Activate quick messages" },

	{ &other_query_flag,	FALSE,	1,	0, 2,
	"other_query_flag",		"Prompt for various information" },

	{ &carry_query_flag,	FALSE,	1,	0, 3,
	"carry_query_flag",		"Prompt before picking things up" },

	{ &use_old_target,		FALSE,	1,	0, 4,
	"use_old_target",		"Use old target by default" },

	{ &always_pickup,		TRUE,	1,	0, 5,
	"always_pickup",		"Pick things up by default" },

	{ &always_repeat,		TRUE,	1,	0, 6,
	"always_repeat",		"Repeat obvious commands" },

	{ &depth_in_feet,		FALSE,	1,	0, 7,
	"depth_in_feet",		"Show dungeon level in feet" },

	{ &stack_force_notes,	FALSE,	1,	0, 8,
	"stack_force_notes",	"Merge inscriptions when stacking" },

	{ &stack_force_costs,	FALSE,	1,	0, 9,
	"stack_force_costs",	"Merge discounts when stacking" },

	{ &show_labels,			TRUE,	1,	0, 10,
	"show_labels",			"Show labels in object listings" },

	{ &show_weights,		FALSE,	1,	0, 11,
	"show_weights",			"Show weights in object listings" },

	{ &show_choices,		FALSE,	1,	0, 12,
	"show_choices",			"Show choices in certain sub-windows" },

	{ &show_details,		FALSE,	1,	0, 13,
	"show_details",			"Show details in certain sub-windows" },

	{ &ring_bell,			TRUE,	1,	0, 14,
	"ring_bell",			"Audible bell (on errors, etc)" },

	{ &use_color,			TRUE,	1,	0, 15,
	"use_color",			"Use color if possible (slow)" },


	/*** Disturbance ***/

	{ &find_ignore_stairs,	TRUE,	2,	0, 16,
	"find_ignore_stairs",	"Run past stairs" },

	{ &find_ignore_doors,	TRUE,	2,	0, 17,
	"find_ignore_doors",	"Run through open doors" },

	{ &find_cut,			TRUE,	2,	0, 18,
	"find_cut",				"Run past known corners" },

	{ &find_examine,		TRUE,	2,	0, 19,
	"find_examine",			"Run into potential corners" },

	{ &disturb_move,		TRUE,	2,	0, 20,
	"disturb_move",			"Disturb whenever any monster moves" },

	{ &disturb_near,		TRUE,	2,	0, 21,
	"disturb_near",			"Disturb whenever viewable monster moves" },

	{ &disturb_panel,		TRUE,	2,	0, 22,
	"disturb_panel",		"Disturb whenever map panel changes" },

	{ &disturb_state,		TRUE,	2,	0, 23,
	"disturb_state",		"Disturb whenever player state changes" },

	{ &disturb_minor,		TRUE,	2,	0, 24,
	"disturb_minor",		"Disturb whenever boring things happen" },

	{ &disturb_other,		TRUE,	2,	0, 25,
	"disturb_other",		"Disturb whenever various things happen" },

	{ &alert_hitpoint,		FALSE,	2,	0, 26,
	"alert_hitpoint",		"Alert user to critical hitpoints" },

	{ &alert_failure,		FALSE,	2,	0, 27,
	"alert_failure",		"Alert user to various failures" },


	/*** Game-Play ***/

	{ &auto_haggle,			FALSE,	3,	1, 0,
	"auto_haggle",			"Auto-haggle in stores" },

	{ &auto_scum,			FALSE,	3,	1, 1,
	"auto_scum",			"Auto-scum for good levels" },

	{ &stack_allow_items,	TRUE,	3,	1, 2,
	"stack_allow_items",	"Allow weapons and armor to stack" },

	{ &stack_allow_wands,	TRUE,	3,	1, 3,
	"stack_allow_wands",	"Allow wands/staffs/rods to stack" },

	{ &expand_look,			FALSE,	3,	1, 4,
	"expand_look",			"Expand the power of the look command" },

	{ &expand_list,			FALSE,	3,	1, 5,
	"expand_list",			"Expand the power of the list commands" },

	{ &view_perma_grids,	TRUE,	3,	1, 6,
	"view_perma_grids",		"Map remembers all perma-lit grids" },

	{ &view_torch_grids,	FALSE,	3,	1, 7,
	"view_torch_grids",		"Map remembers all torch-lit grids" },

	{ &dungeon_align,		TRUE,	3,	1, 8,
	"dungeon_align",		"Generate dungeons with aligned rooms" },

	{ &dungeon_stair,		TRUE,	3,	1, 9,
	"dungeon_stair",		"Generate dungeons with connected stairs" },

	{ &flow_by_sound,		FALSE,	3,	1, 10,
	"flow_by_sound",		"Monsters chase current location (v.slow)" },

	{ &flow_by_smell,		FALSE,	3,	1, 11,
	"flow_by_smell",		"Monsters chase recent locations (v.slow)" },

	{ &track_follow,		FALSE,	3,	1, 12,
	"track_follow",			"Monsters follow the player (broken)" },

	{ &track_target,		FALSE,	3,	1, 13,
	"track_target",			"Monsters target the player (broken)" },

	{ &smart_learn,			FALSE,	3,	1, 14,
	"smart_learn",			"Monsters learn from their mistakes" },

	{ &smart_cheat,			FALSE,	3,	1, 15,
	"smart_cheat",			"Monsters exploit players weaknesses" },


	/*** Efficiency ***/

	{ &view_reduce_lite,	FALSE,	4,	1, 16,
	"view_reduce_lite",		"Reduce lite-radius when running" },

	{ &view_reduce_view,	FALSE,	4,	1, 17,
	"view_reduce_view",		"Reduce view-radius in town" },

	{ &avoid_abort,			FALSE,	4,	1, 18,
	"avoid_abort",			"Avoid checking for user abort" },

	{ &avoid_other,			FALSE,	4,	1, 19,
	"avoid_other",			"Avoid processing special colors" },

	{ &flush_failure,		TRUE,	4,	1, 20,
	"flush_failure",		"Flush input on various failures" },

	{ &flush_disturb,		FALSE,	4,	1, 21,
	"flush_disturb",		"Flush input whenever disturbed" },

	{ &flush_command,		FALSE,	4,	1, 22,
	"flush_command",		"Flush input before every command" },

	{ &fresh_before,		TRUE,	4,	1, 23,
	"fresh_before",			"Flush output before every command" },

	{ &fresh_after,			FALSE,	4,	1, 24,
	"fresh_after",			"Flush output after every command" },

	{ &fresh_message,		FALSE,	4,	1, 25,
	"fresh_message",		"Flush output after every message" },

	{ &compress_savefile,	TRUE,	4,	1, 26,
	"compress_savefile",	"Compress messages in savefiles" },

	{ &hilite_player,		FALSE,	4,	1, 27,
	"hilite_player",		"Hilite the player with the cursor" },

	{ &view_yellow_lite,	FALSE,	4,	1, 28,
	"view_yellow_lite",		"Use special colors for torch-lit grids" },

	{ &view_bright_lite,	FALSE,	4,	1, 29,
	"view_bright_lite",		"Use special colors for 'viewable' grids" },

	{ &view_granite_lite,	FALSE,	4,	1, 30,
	"view_granite_lite",	"Use special colors for wall grids (slow)" },

	{ &view_special_lite,	FALSE,	4,	1, 31,
	"view_special_lite",	"Use special colors for floor grids (slow)" },


	/*** End of Table ***/

	{ NULL,			0, 0, 0, 0,
	NULL,			NULL }
};


cptr monster_spells4[32] =
{
  "Shriek",
  "XXX2",
  "XXX3",
  "XXX4",
  "Arrow",
  "Arrow",
  "Arrow",
  "Arrow",
  "Breath Acid",
  "Breath Lightning",
  "Breath Fire",
  "Breath Cold",
  "Breath Poison",
  "Breath Nether",
  "Breath Lite",
  "Breath Darkness",
  "Breath Confusion",
  "Breath Sound",
  "Breath Chaos",
  "Breath Disenchantment",
  "Breath Nexus",
  "Breath Time",
  "Breath Inertia",
  "Breath Gravity",
  "Breath Shards",
  "Breath Plasma",
  "Breath Force",
  "Breath Mana",
  "XXX",
  "XXX",
  "XXX",
  "XXX",
};

/*
 * New monster race bit flags
 */
cptr monster_spells5[32] =
{
  "Ball of Acid",
  "Ball of Lightning",
  "Ball of Fire",
  "Ball of Cold",
  "Ball of Poison",
  "Ball of Nether",
  "Ball of Water",
  "Ball of Mana",
  "Ball of Darkness",
  "Drain Mana",
  "Mind Blast",
  "Brain Smash",
  "Cause Light Wounds",
  "Cause Serious Wounds",
  "Cause Critical Wounds",
  "Cause Mortal Wounds",
  "Bolt of Acid",
  "Bolt of Lightning",
  "Bolt of Fire",
  "Bolt of Cold",
  "Bolt of Poison",
  "Bolt of Nether",
  "Bolt of Water",
  "Bolt of Mana",  "Bolt of Plasma",
  "Bolt of Ice",
  "Magic Missile",
  "Scare",
  "Blind",
  "Confusion",
  "Slow",
  "Paralyze",
};

/*
 * New monster race bit flags
 */
cptr monster_spells6[32] =
{
  "Speed",
  "XXX",
  "Heal",
  "XXX",
  "Blink",
  "Teleport",
  "XXX",
  "XXX",
  "Teleport To",
  "Teleport Away",
  "Teleport Level",
  "XXX",
  "Darkness",
  "Traps",
  "Forget",
  "XXX",
  "XXX",
  "XXX",
  /* Summons follow, but players can't summon */
};
