/* $Id$ */
/* File: cmd6.c */

/* Purpose: Object commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#define SERVER

#include "angband.h"



/*
 * This file includes code for eating food, drinking potions,
 * reading scrolls, aiming wands, using staffs, zapping rods,
 * and activating artifacts.
 *
 * In all cases, if the player becomes "aware" of the item's use
 * by testing it, mark it as "aware" and reward some experience
 * based on the object's level, always rounding up.  If the player
 * remains "unaware", mark that object "kind" as "tried".
 *
 * This code now correctly handles the unstacking of wands, staffs,
 * and rods.  Note the overly paranoid warning about potential pack
 * overflow, which allows the player to use and drop a stacked item.
 *
 * In all "unstacking" scenarios, the "used" object is "carried" as if
 * the player had just picked it up.  In particular, this means that if
 * the use of an item induces pack overflow, that item will be dropped.
 *
 * For simplicity, these routines induce a full "pack reorganization"
 * which not only combines similar items, but also reorganizes various
 * items to obey the current "sorting" method.  This may require about
 * 400 item comparisons, but only occasionally.
 *
 * There may be a BIG problem with any "effect" that can cause "changes"
 * to the inventory.  For example, a "scroll of recharging" can cause
 * a wand/staff to "disappear", moving the inventory up.  Luckily, the
 * scrolls all appear BEFORE the staffs/wands, so this is not a problem.
 * But, for example, a "staff of recharging" could cause MAJOR problems.
 * In such a case, it will be best to either (1) "postpone" the effect
 * until the end of the function, or (2) "change" the effect, say, into
 * giving a staff "negative" charges, or "turning a staff into a stick".
 * It seems as though a "rod of recharging" might in fact cause problems.
 * The basic problem is that the act of recharging (and destroying) an
 * item causes the inducer of that action to "move", causing "o_ptr" to
 * no longer point at the correct item, with horrifying results.
 *
 * Note that food/potions/scrolls no longer use bit-flags for effects,
 * but instead use the "sval" (which is also used to sort the objects).
 */






/*
 * Eat some food (from the pack or floor)
 */
void do_cmd_eat_food(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];

	int			ident, lev;

	object_type		*o_ptr;


	/* Restrict choices to food */
	item_tester_tval = TV_FOOD;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

        if( check_guard_inscription( o_ptr->note, 'E' )) {
                msg_print(Ind, "The item's inscription prevents it");
                return;
        };

        if (!can_use(Ind, o_ptr))
        {
                msg_print(Ind, "You are not high level enough.");
		return;
        }


	if (o_ptr->tval != TV_FOOD)
	{
		msg_print(Ind, "SERVER ERROR: Tried to eat non-food!");
		return;
	}


	/* Take a turn */
	p_ptr->energy -= level_speed(&p_ptr->wpos);

	/* Identity not known yet */
	ident = FALSE;

	/* Object level */
	lev = k_info[o_ptr->k_idx].level;

	/* Analyze the food */
	switch (o_ptr->sval)
	{
		case SV_FOOD_POISON:
		{
			if (!(p_ptr->resist_pois || p_ptr->oppose_pois))
			{
				if (set_poisoned(Ind, p_ptr->poisoned + rand_int(10) + 10))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_BLINDNESS:
		{
			if (!p_ptr->resist_blind)
			{
				if (set_blind(Ind, p_ptr->blind + rand_int(200) + 200))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_PARANOIA:
		{
			if (!p_ptr->resist_fear)
			{
				if (set_afraid(Ind, p_ptr->afraid + rand_int(10) + 10))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_CONFUSION:
		{
			if (!p_ptr->resist_conf)
			{
				if (set_confused(Ind, p_ptr->confused + rand_int(10) + 10))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_HALLUCINATION:
		{
			if (!p_ptr->resist_chaos)
			{
				if (set_image(Ind, p_ptr->image + rand_int(250) + 250))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_PARALYSIS:
		{
			if (!p_ptr->free_act)
			{
				if (set_paralyzed(Ind, p_ptr->paralyzed + rand_int(10) + 10))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_WEAKNESS:
		{
			take_hit(Ind, damroll(6, 6), "poisonous food.");
			(void)do_dec_stat(Ind, A_STR, STAT_DEC_NORMAL);
			ident = TRUE;
			break;
		}

		case SV_FOOD_SICKNESS:
		{
			take_hit(Ind, damroll(6, 6), "poisonous food.");
			(void)do_dec_stat(Ind, A_CON, STAT_DEC_NORMAL);
			ident = TRUE;
			break;
		}

		case SV_FOOD_STUPIDITY:
		{
			take_hit(Ind, damroll(8, 8), "poisonous food.");
			(void)do_dec_stat(Ind, A_INT, STAT_DEC_NORMAL);
			ident = TRUE;
			break;
		}

		case SV_FOOD_NAIVETY:
		{
			take_hit(Ind, damroll(8, 8), "poisonous food.");
			(void)do_dec_stat(Ind, A_WIS, STAT_DEC_NORMAL);
			ident = TRUE;
			break;
		}

		case SV_FOOD_UNHEALTH:
		{
			take_hit(Ind, damroll(10, 10), "poisonous food.");
			(void)do_dec_stat(Ind, A_CON, STAT_DEC_NORMAL);
			ident = TRUE;
			break;
		}

		case SV_FOOD_DISEASE:
		{
			take_hit(Ind, damroll(10, 10), "poisonous food.");
			(void)do_dec_stat(Ind, A_STR, STAT_DEC_NORMAL);
			ident = TRUE;
			break;
		}

		case SV_FOOD_CURE_POISON:
		{
			if (set_poisoned(Ind, 0)) ident = TRUE;
			break;
		}

		case SV_FOOD_CURE_BLINDNESS:
		{
			if (set_blind(Ind, 0)) ident = TRUE;
			break;
		}

		case SV_FOOD_CURE_PARANOIA:
		{
			if (set_afraid(Ind, 0)) ident = TRUE;
			break;
		}

		case SV_FOOD_CURE_CONFUSION:
		{
			if (set_confused(Ind, 0)) ident = TRUE;
			break;
		}

		case SV_FOOD_CURE_SERIOUS:
		{
			if (hp_player(Ind, damroll(4, 8))) ident = TRUE;
			break;
		}

		case SV_FOOD_RESTORE_STR:
		{
			if (do_res_stat(Ind, A_STR)) ident = TRUE;
			break;
		}

		case SV_FOOD_RESTORE_CON:
		{
			if (do_res_stat(Ind, A_CON)) ident = TRUE;
			break;
		}

		case SV_FOOD_RESTORING:
		{
			if (do_res_stat(Ind, A_STR)) ident = TRUE;
			if (do_res_stat(Ind, A_INT)) ident = TRUE;
			if (do_res_stat(Ind, A_WIS)) ident = TRUE;
			if (do_res_stat(Ind, A_DEX)) ident = TRUE;
			if (do_res_stat(Ind, A_CON)) ident = TRUE;
			if (do_res_stat(Ind, A_CHR)) ident = TRUE;
			break;
		}

		case SV_FOOD_FORTUNE_COOKIE:
		{
			msg_print(Ind, "That tastes good.");
			if (p_ptr->blind || no_lite(Ind))
			{
				msg_print(Ind, "You feel a paper in it - what a pity you cannot see!");
			}
			else
			{
				msg_print(Ind, "There is message in the cookie. It says:");
				fortune(Ind, FALSE);
			}
			ident = TRUE;
			break;
		}

		case SV_FOOD_ATHELAS:
		{
			msg_print(Ind, "A fresh, clean essence rises, driving away wounds and poison.");
			ident = set_poisoned(Ind, 0) |
					set_stun(Ind, 0) |
					set_cut(Ind, 0);
			if (p_ptr->black_breath)
			{
				msg_print(Ind, "The hold of the Black Breath on you is broken!");
				p_ptr->black_breath = FALSE;
			}
			ident = TRUE;
			break;
		}

		case SV_FOOD_RATION:
		case SV_FOOD_BISCUIT:
		case SV_FOOD_JERKY:
		case SV_FOOD_SLIME_MOLD:
		{
			msg_print(Ind, "That tastes good.");
			ident = TRUE;
			break;
		}

		case SV_FOOD_WAYBREAD:
		{
			msg_print(Ind, "That tastes very good.");
			(void)set_poisoned(Ind, 0);
			(void)hp_player(Ind, damroll(4, 8));
			set_food(Ind, PY_FOOD_MAX - 1);
			ident = TRUE;
			break;
		}

		case SV_FOOD_PINT_OF_ALE:
		case SV_FOOD_PINT_OF_WINE:
		{
			if (magik(o_ptr->name2? 50 : 20))
			{
				msg_format(Ind, "\377%c*HIC*", random_colour());
				msg_format_near(Ind, "\377%c%s hiccups!", random_colour(), p_ptr->name);

				if (magik(o_ptr->name2? 60 : 30))
					set_confused(Ind, p_ptr->confused + 20 + randint(20));
				if (magik(o_ptr->name2? 50 : 20))
					set_stun(Ind, p_ptr->stun + 10 + randint(10));

				if (magik(o_ptr->name2? 50 : 10))
					set_image(Ind, p_ptr->image + 10 + randint(10));
				if (magik(o_ptr->name2? 10 : 20))
					set_paralyzed(Ind, p_ptr->paralyzed + 10 + randint(10));
				if (magik(o_ptr->name2? 50 : 10))
					set_hero(Ind, p_ptr->hero + 10 + randint(10));
				if (magik(o_ptr->name2? 20 : 5))
					set_shero(Ind, p_ptr->shero + 5 + randint(10));
				if (magik(o_ptr->name2? 5 : 10))
					set_afraid(Ind, p_ptr->afraid + 15 + randint(10));
				if (magik(o_ptr->name2? 5 : 10))
					set_slow(Ind, p_ptr->slow + 10 + randint(10));
				else if (magik(o_ptr->name2? 20 : 5))
					set_fast(Ind, p_ptr->fast + 10 + randint(10));
				/* Methyl! */
				if (magik(o_ptr->name2? 0 : 3))
					set_blind(Ind, p_ptr->blind + 10 + randint(10));
				if (rand_int(100) < p_ptr->food * magik(o_ptr->name2? 40 : 60) / PY_FOOD_MAX)
				{
					msg_print(Ind, "You become nauseous and vomit!");
					msg_format_near(Ind, "%s vomits!", p_ptr->name);
					/* made salt water less deadly -APD */
					(void)set_food(Ind, (p_ptr->food/2));
					(void)set_poisoned(Ind, 0);
					(void)set_paralyzed(Ind, p_ptr->paralyzed + 4);
				}
				if (magik(o_ptr->name2? 2 : 3))
					(void)dec_stat(Ind, A_DEX, 1, STAT_DEC_TEMPORARY);
				if (magik(o_ptr->name2? 2 : 3))
					(void)dec_stat(Ind, A_WIS, 1, STAT_DEC_TEMPORARY);
				if (magik(o_ptr->name2? 0 : 1))
					(void)dec_stat(Ind, A_CON, 1, STAT_DEC_TEMPORARY);
				//			(void)dec_stat(Ind, A_STR, 1, STAT_DEC_TEMPORARY);
				if (magik(o_ptr->name2? 3 : 5))
					(void)dec_stat(Ind, A_CHR, 1, STAT_DEC_TEMPORARY);
				if (magik(o_ptr->name2? 2 : 3))
					(void)dec_stat(Ind, A_INT, 1, STAT_DEC_TEMPORARY);
			}
			else msg_print(Ind, "That tastes good.");

			ident = TRUE;
			break;
		}

		case SV_FOOD_UNMAGIC:
		{
			ident = unmagic(Ind);
			break;
		}
	}


	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* We have tried it */
	object_tried(Ind, o_ptr);

	/* The player is now aware of the object */
	if (ident && !object_aware_p(Ind, o_ptr))
	{
		object_aware(Ind, o_ptr);
		gain_exp(Ind, (lev + (p_ptr->lev >> 1)) / p_ptr->lev);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Food can feed the player */
	(void)set_food(Ind, p_ptr->food + o_ptr->pval);


	/* Destroy a food in the pack */
	if (item >= 0)
	{
		inven_item_increase(Ind, item, -1);
		inven_item_describe(Ind, item);
		inven_item_optimize(Ind, item);
	}

	/* Destroy a food on the floor */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}
}




/*
 * Quaff a potion (from the pack or the floor)
 */
void do_cmd_quaff_potion(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];

	int		ident, lev;

	object_type	*o_ptr;


	/* Restrict choices to potions (apparently meanless) */
	item_tester_tval = TV_POTION;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Hack -- allow to quaff ale/wine */
	if (o_ptr->tval == TV_FOOD)
	{
		do_cmd_eat_food(Ind, item);
		return;
	}


        if( check_guard_inscription( o_ptr->note, 'q' )) {
                msg_print(Ind, "The item's inscription prevents it");
                return;
        };

        if (!can_use(Ind, o_ptr))
        {
                msg_print(Ind, "You are not high level enough.");
		return;
        }


	if ((o_ptr->tval != TV_POTION) &&
		(o_ptr->tval != TV_POTION2))
	{
		msg_print(Ind, "SERVER ERROR: Tried to quaff non-potion!");
		return;
	}

	/* Take a turn */
	p_ptr->energy -= level_speed(&p_ptr->wpos);

	/* Not identified yet */
	ident = FALSE;

	/* Object level */
	lev = k_info[o_ptr->k_idx].level;

	process_hooks(HOOK_QUAFF, "d", Ind);

	/* Analyze the potion */
	if (o_ptr->tval == TV_POTION)
	{

	switch (o_ptr->sval)
	{
		case SV_POTION_WATER:
		case SV_POTION_APPLE_JUICE:
		case SV_POTION_SLIME_MOLD:
		{
			msg_print(Ind, "\377GYou feel less thirsty.");
			ident = TRUE;
			break;
		}

		case SV_POTION_SLOWNESS:
		{
			if (set_slow(Ind, p_ptr->slow + randint(25) + 15)) ident = TRUE;
			break;
		}

		case SV_POTION_SALT_WATER:
		{
			msg_print(Ind, "The potion makes you vomit!");
			msg_format_near(Ind, "%s vomits!", p_ptr->name);
			/* made salt water less deadly -APD */
			(void)set_food(Ind, (p_ptr->food/2)-400);
			(void)set_poisoned(Ind, 0);
			(void)set_paralyzed(Ind, p_ptr->paralyzed + 4);
			ident = TRUE;
			break;
		}

		case SV_POTION_POISON:
		{
			if (!(p_ptr->resist_pois || p_ptr->oppose_pois))
			{
				if (set_poisoned(Ind, p_ptr->poisoned + rand_int(15) + 10))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_POTION_BLINDNESS:
		{
			if (!p_ptr->resist_blind)
			{
				if (set_blind(Ind, p_ptr->blind + rand_int(100) + 100))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_POTION_CONFUSION:
		{
			if (!p_ptr->resist_conf)
			{
				if (set_confused(Ind, p_ptr->confused + rand_int(20) + 15))
				{
					ident = TRUE;
				}
			}
			break;
		}
		case SV_POTION_MUTATION:
		{
			ident = TRUE;
			break;
		}

		case SV_POTION_SLEEP:
		{
			if (!p_ptr->free_act)
			{
				if (set_paralyzed(Ind, p_ptr->paralyzed + rand_int(4) + 4))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_POTION_LOSE_MEMORIES:
		{
			if (!p_ptr->hold_life && (p_ptr->exp > 0))
			{
				msg_print(Ind, "\377GYou feel your memories fade.");
				lose_exp(Ind, p_ptr->exp / 4);
				ident = TRUE;
			}
			break;
		}

		case SV_POTION_RUINATION:
		{
			msg_print(Ind, "Your nerves and muscles feel weak and lifeless!");
			take_hit(Ind, damroll(10, 10), "a potion of Ruination");
			(void)dec_stat(Ind, A_DEX, 25, STAT_DEC_NORMAL);
			(void)dec_stat(Ind, A_WIS, 25, STAT_DEC_NORMAL);
			(void)dec_stat(Ind, A_CON, 25, STAT_DEC_NORMAL);
			(void)dec_stat(Ind, A_STR, 25, STAT_DEC_NORMAL);
			(void)dec_stat(Ind, A_CHR, 25, STAT_DEC_NORMAL);
			(void)dec_stat(Ind, A_INT, 25, STAT_DEC_NORMAL);
			ident = TRUE;
			break;
		}

		case SV_POTION_DEC_STR:
		{
			if (do_dec_stat(Ind, A_STR, STAT_DEC_NORMAL)) ident = TRUE;
			break;
		}

		case SV_POTION_DEC_INT:
		{
			if (do_dec_stat(Ind, A_INT, STAT_DEC_NORMAL)) ident = TRUE;
			break;
		}

		case SV_POTION_DEC_WIS:
		{
			if (do_dec_stat(Ind, A_WIS, STAT_DEC_NORMAL)) ident = TRUE;
			break;
		}

		case SV_POTION_DEC_DEX:
		{
			if (do_dec_stat(Ind, A_DEX, STAT_DEC_NORMAL)) ident = TRUE;
			break;
		}

		case SV_POTION_DEC_CON:
		{
			if (do_dec_stat(Ind, A_CON, STAT_DEC_NORMAL)) ident = TRUE;
			break;
		}

		case SV_POTION_DEC_CHR:
		{
			if (do_dec_stat(Ind, A_CHR, STAT_DEC_NORMAL)) ident = TRUE;
			break;
		}

		case SV_POTION_DETONATIONS:
		{
			msg_print(Ind, "Massive explosions rupture your body!");
			msg_format_near(Ind, "%s blows up!", p_ptr->name);
			take_hit(Ind, damroll(50, 20), "a potion of Detonation");
			(void)set_stun(Ind, p_ptr->stun + 75);
			(void)set_cut(Ind, p_ptr->cut + 5000);
			ident = TRUE;
			break;
		}

		case SV_POTION_DEATH:
		{
			msg_print(Ind, "A feeling of Death flows through your body.");
			take_hit(Ind, 5000, "a potion of Death");
			ident = TRUE;
			break;
		}

		case SV_POTION_INFRAVISION:
		{
			if (set_tim_infra(Ind, p_ptr->tim_infra + 100 + randint(100)))
			{
				ident = TRUE;
			}
			break;
		}

		case SV_POTION_DETECT_INVIS:
		{
			if (set_tim_invis(Ind, p_ptr->tim_invis + 12 + randint(12)))
			{
				ident = TRUE;
			}
			break;
		}
		case SV_POTION_INVIS:
		{
			p_ptr->tim_invisibility = 30+randint(40);
			p_ptr->tim_invis_power = p_ptr->lev * 4 / 5;
		}

		case SV_POTION_SLOW_POISON:
		{
			if (set_poisoned(Ind, p_ptr->poisoned / 2)) ident = TRUE;
			break;
		}

		case SV_POTION_CURE_POISON:
		{
			if (set_poisoned(Ind, 0)) ident = TRUE;
			break;
		}

		case SV_POTION_BOLDNESS:
		{
			if (set_afraid(Ind, 0)) ident = TRUE;
			break;
		}

		case SV_POTION_SPEED:
		{
			if (!p_ptr->fast)
			{
				if (set_fast(Ind, randint(25) + 15)) ident = TRUE;
			}
			else
			{
				(void)set_fast(Ind, p_ptr->fast + 5);
			}
			break;
		}

		case SV_POTION_RESIST_HEAT:
		{
			if (set_oppose_fire(Ind, p_ptr->oppose_fire + randint(10) + 10))
			{
				ident = TRUE;
			}
			break;
		}

		case SV_POTION_RESIST_COLD:
		{
			if (set_oppose_cold(Ind, p_ptr->oppose_cold + randint(10) + 10))
			{
				ident = TRUE;
			}
			break;
		}

		case SV_POTION_HEROISM:
		{
			if (hp_player(Ind, 10)) ident = TRUE;
			if (set_afraid(Ind, 0)) ident = TRUE;
			if (set_hero(Ind, p_ptr->hero + randint(25) + 25)) ident = TRUE;
			break;
		}

		case SV_POTION_BESERK_STRENGTH:
		{
			if (hp_player(Ind, 30)) ident = TRUE;
			if (set_afraid(Ind, 0)) ident = TRUE;
			if (set_shero(Ind, p_ptr->shero + randint(25) + 25)) ident = TRUE;
			break;
		}

		case SV_POTION_CURE_LIGHT:
		{
			if (hp_player(Ind, damroll(2, 8))) ident = TRUE;
			if (set_blind(Ind, 0)) ident = TRUE;
			if (set_cut(Ind, p_ptr->cut - 10)) ident = TRUE;
			break;
		}

		case SV_POTION_CURE_SERIOUS:
		{
			if (hp_player(Ind, damroll(4, 8))) ident = TRUE;
			if (set_blind(Ind, 0)) ident = TRUE;
			if (set_confused(Ind, 0)) ident = TRUE;
			if (set_cut(Ind, (p_ptr->cut / 2) - 50)) ident = TRUE;
			break;
		}

		case SV_POTION_CURE_CRITICAL:
		{
			if (hp_player(Ind, damroll(6, 8))) ident = TRUE;
			if (set_blind(Ind, 0)) ident = TRUE;
			if (set_confused(Ind, 0)) ident = TRUE;
			if (set_poisoned(Ind, 0)) ident = TRUE;
			if (set_stun(Ind, 0)) ident = TRUE;
			if (set_cut(Ind, 0)) ident = TRUE;
			break;
		}

		case SV_POTION_HEALING:
		{
			if (hp_player(Ind, 300)) ident = TRUE;
			if (set_blind(Ind, 0)) ident = TRUE;
			if (set_confused(Ind, 0)) ident = TRUE;
			if (set_poisoned(Ind, 0)) ident = TRUE;
			if (set_stun(Ind, 0)) ident = TRUE;
			if (set_cut(Ind, 0)) ident = TRUE;
			break;
		}

		case SV_POTION_STAR_HEALING:
		{
			if (hp_player(Ind, 1200)) ident = TRUE;
			if (set_blind(Ind, 0)) ident = TRUE;
			if (set_confused(Ind, 0)) ident = TRUE;
			if (set_poisoned(Ind, 0)) ident = TRUE;
			if (set_stun(Ind, 0)) ident = TRUE;
			if (set_cut(Ind, 0)) ident = TRUE;
			break;
		}

		case SV_POTION_LIFE:
		{
			msg_print(Ind, "\377GYou feel life flow through your body!");
			restore_level(Ind);
			hp_player(Ind, 5000);
			(void)set_poisoned(Ind, 0);
			(void)set_blind(Ind, 0);
			(void)set_confused(Ind, 0);
			(void)set_image(Ind, 0);
			(void)set_stun(Ind, 0);
			(void)set_cut(Ind, 0);
			(void)do_res_stat(Ind, A_STR);
			(void)do_res_stat(Ind, A_CON);
			(void)do_res_stat(Ind, A_DEX);
			(void)do_res_stat(Ind, A_WIS);
			(void)do_res_stat(Ind, A_INT);
			(void)do_res_stat(Ind, A_CHR);
                        if (p_ptr->black_breath)
                        {
                                msg_print(Ind, "The hold of the Black Breath on you is broken!");
                        }
                        p_ptr->black_breath = FALSE;
			ident = TRUE;
			break;
		}

		case SV_POTION_RESTORE_MANA:
		{
			if (p_ptr->csp < p_ptr->msp)
			{
//				p_ptr->csp += 300;
//				if (p_ptr->csp > p_ptr->msp) p_ptr->csp = p_ptr->msp;
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
				msg_print(Ind, "Your feel your head clear.");
				p_ptr->redraw |= (PR_MANA);
				p_ptr->window |= (PW_PLAYER);
				ident = TRUE;
			}
			break;
		}

		case SV_POTION_RESTORE_EXP:
		{
			if (restore_level(Ind)) ident = TRUE;
			break;
		}

		case SV_POTION_RES_STR:
		{
			if (do_res_stat(Ind, A_STR)) ident = TRUE;
			break;
		}

		case SV_POTION_RES_INT:
		{
			if (do_res_stat(Ind, A_INT)) ident = TRUE;
			break;
		}

		case SV_POTION_RES_WIS:
		{
			if (do_res_stat(Ind, A_WIS)) ident = TRUE;
			break;
		}

		case SV_POTION_RES_DEX:
		{
			if (do_res_stat(Ind, A_DEX)) ident = TRUE;
			break;
		}

		case SV_POTION_RES_CON:
		{
			if (do_res_stat(Ind, A_CON)) ident = TRUE;
			break;
		}

		case SV_POTION_RES_CHR:
		{
			if (do_res_stat(Ind, A_CHR)) ident = TRUE;
			break;
		}

		case SV_POTION_INC_STR:
		{
			if (do_inc_stat(Ind, A_STR)) ident = TRUE;
			break;
		}

		case SV_POTION_INC_INT:
		{
			if (do_inc_stat(Ind, A_INT)) ident = TRUE;
			break;
		}

		case SV_POTION_INC_WIS:
		{
			if (do_inc_stat(Ind, A_WIS)) ident = TRUE;
			break;
		}

		case SV_POTION_INC_DEX:
		{
			if (do_inc_stat(Ind, A_DEX)) ident = TRUE;
			break;
		}

		case SV_POTION_INC_CON:
		{
			if (do_inc_stat(Ind, A_CON)) ident = TRUE;
			break;
		}

		case SV_POTION_INC_CHR:
		{
			if (do_inc_stat(Ind, A_CHR)) ident = TRUE;
			break;
		}

		case SV_POTION_AUGMENTATION:
		{
			if (do_inc_stat(Ind, A_STR)) ident = TRUE;
			if (do_inc_stat(Ind, A_INT)) ident = TRUE;
			if (do_inc_stat(Ind, A_WIS)) ident = TRUE;
			if (do_inc_stat(Ind, A_DEX)) ident = TRUE;
			if (do_inc_stat(Ind, A_CON)) ident = TRUE;
			if (do_inc_stat(Ind, A_CHR)) ident = TRUE;
			break;
		}

		case SV_POTION_ENLIGHTENMENT:
		{
			msg_print(Ind, "An image of your surroundings forms in your mind...");
			wiz_lite(Ind);
			ident = TRUE;
			break;
		}

		case SV_POTION_STAR_ENLIGHTENMENT:
		{
			msg_print(Ind, "You begin to feel more enlightened...");
			msg_print(Ind, NULL);
			wiz_lite(Ind);
			(void)do_inc_stat(Ind, A_INT);
			(void)do_inc_stat(Ind, A_WIS);
			(void)detect_treasure(Ind, DEFAULT_RADIUS * 2);
			(void)detect_object(Ind, DEFAULT_RADIUS * 2);
			(void)detect_sdoor(Ind, DEFAULT_RADIUS * 2);
			(void)detect_trap(Ind, DEFAULT_RADIUS * 2);
			identify_pack(Ind);
			self_knowledge(Ind);
			ident = TRUE;
			break;
		}

		case SV_POTION_SELF_KNOWLEDGE:
		{
			msg_print(Ind, "You begin to know yourself a little better...");
			msg_print(Ind, NULL);
			self_knowledge(Ind);
			ident = TRUE;
			break;
		}

		case SV_POTION_EXPERIENCE:
		{
			if (p_ptr->exp < PY_MAX_EXP)
			{
				s32b ee = (p_ptr->exp / 2) + 10;
				if (ee > 100000L) ee = 100000L;
				msg_print(Ind, "\377GYou feel more experienced.");
				gain_exp(Ind, ee);
				ident = TRUE;
			}
			break;
		}

		/* additions from PernA */
		case SV_POTION_CURING:
		{
			if (hp_player(Ind, 50)) ident = TRUE;
			if (set_blind(Ind, 0)) ident = TRUE;
			if (set_poisoned(Ind, 0)) ident = TRUE;
			if (set_confused(Ind, 0)) ident = TRUE;
			if (set_stun(Ind, 0)) ident = TRUE;
			if (set_cut(Ind, 0)) ident = TRUE;
			if (set_image(Ind, 0)) ident = TRUE;
                        if (heal_insanity(Ind, 50)) ident = TRUE;
			break;
		}

		case SV_POTION_INVULNERABILITY:
		{
			ident = set_invuln(Ind, p_ptr->invuln + randint(7) + 7);
			break;
		}

		case SV_POTION_RESISTANCE:
		{
			ident = 
				set_oppose_acid(Ind, p_ptr->oppose_acid + randint(20) + 20) |
				set_oppose_elec(Ind, p_ptr->oppose_elec + randint(20) + 20) |
				set_oppose_fire(Ind, p_ptr->oppose_fire + randint(20) + 20) |
				set_oppose_cold(Ind, p_ptr->oppose_cold + randint(20) + 20) |
				set_oppose_pois(Ind, p_ptr->oppose_pois + randint(20) + 20);
			break;
		}


	}
	}
	else
		/* POTION2 */
	{
		switch (o_ptr->sval)
		{
			case SV_POTION2_CURE_LIGHT_SANITY:
				if (heal_insanity(Ind, damroll(4,8))) ident = TRUE;
				(void)set_image(Ind, 0);
				break;
			case SV_POTION2_CURE_SERIOUS_SANITY:
				if (heal_insanity(Ind, damroll(8,8))) ident = TRUE;
				(void)set_image(Ind, 0);
				break;
			case SV_POTION2_CURE_CRITICAL_SANITY:
				if (heal_insanity(Ind, damroll(12,8))) ident = TRUE;
				(void)set_image(Ind, 0);
				break;
			case SV_POTION2_CURE_SANITY:
//				if (heal_insanity(Ind, damroll(10,100))) ident = TRUE;
				if (heal_insanity(Ind, damroll(10,20))) ident = TRUE;
				(void)set_image(Ind, 0);
				break;
			case SV_POTION2_CHAUVE_SOURIS:
//				apply_morph(Ind, 100, "Potion of Chauve-Souris");
				if (!p_ptr->fruit_bat)
				{
					/* FRUIT BAT!!!!!! */

					msg_print(Ind, "You have been turned into a fruit bat!");				
					strcpy(p_ptr->died_from,"Potion of Chauve-Souris");
					p_ptr->fruit_bat = -1;
					player_death(Ind);
				}
				else				
				{	/* no saving throw for being restored..... */
					msg_print(Ind, "You have been restored!");
					p_ptr->fruit_bat = 0;
					p_ptr->update |= (PU_BONUS | PU_HP);
				}
								
				break;
		}
	}


	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* The item has been tried */
	object_tried(Ind, o_ptr);

	/* An identification was made */
	if (ident && !object_aware_p(Ind, o_ptr))
	{
		object_aware(Ind, o_ptr);
		gain_exp(Ind, (lev + (p_ptr->lev >> 1)) / p_ptr->lev);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Potions can feed the player */
	(void)set_food(Ind, p_ptr->food + o_ptr->pval);


	/* Destroy a potion in the pack */
	if (item >= 0)
	{
		inven_item_increase(Ind, item, -1);
		inven_item_describe(Ind, item);
		inven_item_optimize(Ind, item);
	}

	/* Destroy a potion on the floor */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}
}


/*
 * Curse the players armor
 */
bool curse_armor(int Ind)
{
	player_type *p_ptr = Players[Ind];

	object_type *o_ptr;

	char o_name[160];


	/* Curse the body armor */
	o_ptr = &p_ptr->inventory[INVEN_BODY];

	/* Nothing to curse */
	if (!o_ptr->k_idx) return (FALSE);


	/* Describe */
	object_desc(Ind, o_name, o_ptr, FALSE, 3);

	/* Attempt a saving throw for artifacts */
	if (artifact_p(o_ptr) && (rand_int(100) < 30))
	{
		/* Cool */
		msg_format(Ind, "A %s tries to %s, but your %s resists the effects!",
		           "terrible black aura", "surround your armor", o_name);
	}

	/* not artifact or failed save... */
	else
	{
		/* Oops */
		msg_format(Ind, "A terrible black aura blasts your %s!", o_name);

		if (true_artifact_p(o_ptr))
		{
			a_info[o_ptr->name1].cur_num = 0;
			a_info[o_ptr->name1].known = FALSE;
		}

		/* Blast the armor */
		o_ptr->name1 = 0;
		o_ptr->name2 = EGO_BLASTED;
		o_ptr->name3 = 0;
		o_ptr->to_a = 0 - randint(5) - randint(5);
		o_ptr->to_h = 0;
		o_ptr->to_d = 0;
		o_ptr->ac = 0;
		o_ptr->dd = 0;
		o_ptr->ds = 0;

		/* Curse it */
		o_ptr->ident |= ID_CURSED;

		/* Break it */
		o_ptr->ident |= ID_BROKEN;

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate mana */
		p_ptr->update |= (PU_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
	}

	return (TRUE);
}


/*
 * Curse the players weapon
 */
bool curse_weapon(int Ind)
{
	player_type *p_ptr = Players[Ind];

	object_type *o_ptr;

	char o_name[160];


	/* Curse the weapon */
	o_ptr = &p_ptr->inventory[INVEN_WIELD];

	/* Nothing to curse */
	if (!o_ptr->k_idx) return (FALSE);


	/* Describe */
	object_desc(Ind, o_name, o_ptr, FALSE, 3);

	/* Attempt a saving throw */
	if (artifact_p(o_ptr) && (rand_int(100) < 30))
	{
		/* Cool */
		msg_format(Ind, "A %s tries to %s, but your %s resists the effects!",
		           "terrible black aura", "surround your weapon", o_name);
	}

	/* not artifact or failed save... */
	else
	{
		/* Oops */
		msg_format(Ind, "A terrible black aura blasts your %s!", o_name);

		if (true_artifact_p(o_ptr))
		{
			a_info[o_ptr->name1].cur_num = 0;
			a_info[o_ptr->name1].known = FALSE;
		}

		/* Shatter the weapon */
		o_ptr->name1 = 0;
		o_ptr->name2 = EGO_SHATTERED;
		o_ptr->name3 = 0;
		o_ptr->to_h = 0 - randint(5) - randint(5);
		o_ptr->to_d = 0 - randint(5) - randint(5);
		o_ptr->to_a = 0;
		o_ptr->ac = 0;
		o_ptr->dd = 0;
		o_ptr->ds = 0;

		/* Curse it */
		o_ptr->ident |= ID_CURSED;

		/* Break it */
		o_ptr->ident |= ID_BROKEN;

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate mana */
		p_ptr->update |= (PU_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
	}

	/* Notice */
	return (TRUE);
}



/*
 * Curse the players equipment in general	- Jir -
 */
#if 0	// let's use this for Hand-o-Doom :)
bool curse_an_item(int Ind, int slot)
{
	player_type *p_ptr = Players[Ind];

	object_type *o_ptr;

	char o_name[160];


	/* Curse the body armor */
	o_ptr = &p_ptr->inventory[slot];

	/* Nothing to curse */
	if (!o_ptr->k_idx) return (FALSE);


	/* Describe */
	object_desc(Ind, o_name, o_ptr, FALSE, 3);

	/* Attempt a saving throw for artifacts */
	if (artifact_p(o_ptr) && (rand_int(100) < 50))
	{
		/* Cool */
		msg_format(Ind, "A %s tries to %s, but your %s resists the effects!",
		           "terrible black aura", "surround you", o_name);
	}

	/* not artifact or failed save... */
	else
	{
		/* Oops */
		msg_format(Ind, "A terrible black aura blasts your %s!", o_name);

		if (true_artifact_p(o_ptr))
		{
			a_info[o_ptr->name1].cur_num = 0;
			a_info[o_ptr->name1].known = FALSE;
		}

		/* Blast the armor */
		o_ptr->name1 = 0;
		o_ptr->name2 = EGO_BLASTED;	// ?
		o_ptr->name3 = 0;
		o_ptr->to_a = 0 - randint(5) - randint(5);
		o_ptr->to_h = 0 - randint(5) - randint(5);
		o_ptr->to_d = 0 - randint(5) - randint(5);
		o_ptr->ac = 0;
		o_ptr->dd = 0;
		o_ptr->ds = 0;

		/* Curse it */
		o_ptr->ident |= ID_CURSED;

		/* Break it */
		o_ptr->ident |= ID_BROKEN;

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate mana */
		p_ptr->update |= (PU_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
	}

	return (TRUE);
}
#endif	// 0


/*
 * Read a scroll (from the pack or floor).
 *
 * Certain scrolls can be "aborted" without losing the scroll.  These
 * include scrolls with no effects but recharge or identify, which are
 * cancelled before use.  XXX Reading them still takes a turn, though.
 */
 
 /*
 
 Added scroll of Life... uses vars x,y
-AD-
 */
 
/*
 * NOTE: seemingly, 'used_up' flag is used in a strange way to allow
 * item specification.  'keep' flag should be used for non-consuming
 * scrolls instead.		- Jir -
 */
void do_cmd_read_scroll(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];
	cave_type * c_ptr;

	int		k, ident, lev, x,y;
	bool	used_up, keep = FALSE;

	object_type	*o_ptr;


	/* Check some conditions */
	if (p_ptr->blind)
	{
		msg_print(Ind, "You can't see anything.");
		return;
	}
	if (no_lite(Ind))
	{
		msg_print(Ind, "You have no light to read by.");
		return;
	}
	if (p_ptr->confused)
	{
		msg_print(Ind, "You are too confused!");
		return;
	}


	/* Restrict choices to scrolls */
	item_tester_tval = TV_SCROLL;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	if( check_guard_inscription( o_ptr->note, 'r' )) {
		msg_print(Ind, "The item's inscription prevents it");
		return;
	};

	if (o_ptr->tval != TV_SCROLL && o_ptr->tval != TV_PARCHEMENT)
	{
		msg_print(Ind, "SERVER ERROR: Tried to read non-scroll!");
		return;
	}

	if (!can_use(Ind, o_ptr))
	{
		msg_print(Ind, "You are not high level enough.");
		return;
	}

	/* Take a turn */
	p_ptr->energy -= level_speed(&p_ptr->wpos);

	/* Not identified yet */
	ident = FALSE;

	/* Object level */
	lev = k_info[o_ptr->k_idx].level;
	
	process_hooks(HOOK_READ, "d", Ind);

	/* Assume the scroll will get used up */
	used_up = TRUE;

	/* Analyze the scroll */
	if (o_ptr->tval == TV_SCROLL)
	{
		switch (o_ptr->sval)
		{
			case SV_SCROLL_HOUSE:
			{
				//			unsigned char *ins=quark_str(o_ptr->note);
				cptr ins=quark_str(o_ptr->note);
				bool floor=TRUE;
				bool jail=FALSE;
				msg_print(Ind, "This is a house creation scroll.");
				ident = TRUE;
				if(ins){
					while((*ins!='\0')){
						if(*ins=='@'){
							ins++;
							if(*ins=='F'){
								floor=FALSE;
							}
							if(*ins=='J'){
								jail=TRUE;
							}
						}
						ins++;
					}
				}
				house_creation(Ind, floor, jail);
				break;
			}

			case SV_SCROLL_GOLEM:
			{	    
				msg_print(Ind, "This is a golem creation scroll.");
				ident = TRUE;
				golem_creation(Ind, 1);
				break;
			}

			case SV_SCROLL_BLOOD_BOND:
			{

				msg_print(Ind, "This is a blood bond scroll.");
				ident = TRUE;
				blood_bond(Ind, o_ptr);
				break;
			}

			case SV_SCROLL_ARTIFACT_CREATION:
			{

				msg_print(Ind, "This is an artifact creation scroll.");
				ident = TRUE;
				(void)create_artifact(Ind);
				used_up = FALSE;
				break;
			}

			case SV_SCROLL_DARKNESS:
			{
				if (unlite_area(Ind, 10, 3)) ident = TRUE;
				if (!p_ptr->resist_dark)
				{
					(void)set_blind(Ind, p_ptr->blind + 3 + randint(5));
				}
				break;
			}

			case SV_SCROLL_AGGRAVATE_MONSTER:
			{
				msg_print(Ind, "There is a high pitched humming noise.");
				aggravate_monsters(Ind, 1);
				ident = TRUE;
				break;
			}

			case SV_SCROLL_CURSE_ARMOR:
			{
				if (curse_armor(Ind)) ident = TRUE;
				break;
			}

			case SV_SCROLL_CURSE_WEAPON:
			{
				if (curse_weapon(Ind)) ident = TRUE;
				break;
			}

			case SV_SCROLL_SUMMON_MONSTER:
			{
				for (k = 0; k < randint(3); k++)
				{
					if (summon_specific(&p_ptr->wpos, p_ptr->py, p_ptr->px, getlevel(&p_ptr->wpos), 0))
					{
						ident = TRUE;
					}
				}
				break;
			}

			/* not adding bounds checking now... because of perma walls
			   hope that I don't need to...... 

			   OK, modified so you cant ressurect ghosts in walls......
			   to prevent bad things from happening in town.
			   */
			case SV_SCROLL_LIFE:
			{/*
				for (y = -1; y <= 1; y++)
				{
				for (x = -1; x <= 1; x++)
				{				
				cave_type **zcave;
				zcave=getcave(&p_ptr->wpos);
				c_ptr = &zcave[p_ptr->py+y][p_ptr->px+x];

				if ((c_ptr->m_idx < 0) && (cave_floor_bold(zcave, p_ptr->py+y, p_ptr->px+x)))
				{
				if (Players[0 - c_ptr->m_idx]->ghost)
				{
				resurrect_player(0 - c_ptr->m_idx);
				break;
				}
				}

				}
				}
				*/
				restore_level(Ind);
				do_scroll_life(Ind);
				break;

			}

			case SV_SCROLL_SUMMON_UNDEAD:
			{
				for (k = 0; k < randint(3); k++)
				{
					if (summon_specific(&p_ptr->wpos, p_ptr->py, p_ptr->px, getlevel(&p_ptr->wpos), SUMMON_UNDEAD))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_SCROLL_TRAP_CREATION:
			{
				if (trap_creation(Ind, 5, 1)) ident = TRUE;
				break;
			}

			case SV_SCROLL_PHASE_DOOR:
			{
				teleport_player(Ind, 10);
				ident = TRUE;
				break;
			}

			case SV_SCROLL_TELEPORT:
			{
				teleport_player(Ind, 100);
				ident = TRUE;
				break;
			}

			case SV_SCROLL_TELEPORT_LEVEL:
			{
				teleport_player_level(Ind);
				ident = TRUE;
				break;
			}

			case SV_SCROLL_WORD_OF_RECALL:
			{
				if (p_ptr->word_recall == 0)
				{
					set_recall_depth(p_ptr, o_ptr);
					p_ptr->word_recall = randint(20) + 15;
					msg_print(Ind, "\377oThe air about you becomes charged...");
				}
				else
				{
					p_ptr->word_recall = 0;
					msg_print(Ind, "\377oA tension leaves the air around you...");
				}
				ident = TRUE;
				break;
			}

			case SV_SCROLL_IDENTIFY:
			{
				msg_print(Ind, "This is an identify scroll.");
				ident = TRUE;
				(void)ident_spell(Ind);
				used_up = FALSE;
				break;
			}

			case SV_SCROLL_STAR_IDENTIFY:
			{
				msg_print(Ind, "This is an *identify* scroll.");
				ident = TRUE;
				(void)identify_fully(Ind);
				used_up = FALSE;
				break;
			}

			case SV_SCROLL_REMOVE_CURSE:
			{
				if (remove_curse(Ind))
				{
					msg_print(Ind, "\377GYou feel as if someone is watching over you.");
					ident = TRUE;
				}
				break;
			}

			case SV_SCROLL_STAR_REMOVE_CURSE:
			{
				remove_all_curse(Ind);
				ident = TRUE;
				break;
			}

			case SV_SCROLL_ENCHANT_ARMOR:
			{
				msg_print(Ind, "This is a scroll of enchant armor.");
				ident = TRUE;
				(void)enchant_spell(Ind, 0, 0, 1, 0);
				used_up = FALSE;
				break;
			}

			case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
			{
				msg_print(Ind, "This is a scroll of enchant weapon to-hit.");
				(void)enchant_spell(Ind, 1, 0, 0, 0);
				used_up = FALSE;
				ident = TRUE;
				break;
			}

			case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
			{
				msg_print(Ind, "This is a scroll of enchant weapon to-dam.");
				(void)enchant_spell(Ind, 0, 1, 0, 0);
				used_up = FALSE;
				ident = TRUE;
				break;
			}

			case SV_SCROLL_STAR_ENCHANT_ARMOR:
			{
				msg_print(Ind, "This is a scroll of *enchant* armor.");
				(void)enchant_spell(Ind, 0, 0, randint(3) + 2, 0);
				used_up = FALSE;
				ident = TRUE;
				break;
			}

			case SV_SCROLL_STAR_ENCHANT_WEAPON:
			{
				msg_print(Ind, "This is a scroll of *enchant* weapon.");
				(void)enchant_spell(Ind, randint(3), randint(3), 0, 0);
				used_up = FALSE;
				ident = TRUE;
				break;
			}

			case SV_SCROLL_RECHARGING:
			{
				msg_print(Ind, "This is a scroll of recharging.");
				(void)recharge(Ind, 60);
				used_up = FALSE;
				ident = TRUE;
				break;
			}

			case SV_SCROLL_LIGHT:
			{
				if (lite_area(Ind, damroll(2, 8), 2)) ident = TRUE;
				break;
			}

			case SV_SCROLL_MAPPING:
			{
				map_area(Ind);
				ident = TRUE;
				break;
			}

			case SV_SCROLL_DETECT_GOLD:
			{
				if (detect_treasure(Ind, DEFAULT_RADIUS)) ident = TRUE;
				break;
			}

			case SV_SCROLL_DETECT_ITEM:
			{
				if (detect_object(Ind, DEFAULT_RADIUS)) ident = TRUE;
				break;
			}

			case SV_SCROLL_DETECT_TRAP:
			{
				if (detect_trap(Ind, DEFAULT_RADIUS)) ident = TRUE;
				break;
			}

			case SV_SCROLL_DETECT_DOOR:
			{
				if (detect_sdoor(Ind, DEFAULT_RADIUS)) ident = TRUE;
				break;
			}

			case SV_SCROLL_DETECT_INVIS:
			{
				if (detect_invisible(Ind)) ident = TRUE;
				break;
			}

			case SV_SCROLL_SATISFY_HUNGER:
			{
				if (set_food(Ind, PY_FOOD_MAX - 1)) ident = TRUE;
				break;
			}

			case SV_SCROLL_BLESSING:
			{
				if (set_blessed(Ind, p_ptr->blessed + randint(12) + 6)) ident = TRUE;
				break;
			}

			case SV_SCROLL_HOLY_CHANT:
			{
				if (set_blessed(Ind, p_ptr->blessed + randint(24) + 12)) ident = TRUE;
				break;
			}

			case SV_SCROLL_HOLY_PRAYER:
			{
				if (set_blessed(Ind, p_ptr->blessed + randint(48) + 24)) ident = TRUE;
				break;
			}

			case SV_SCROLL_MONSTER_CONFUSION:
			{
				if (p_ptr->confusing == 0)
				{
					msg_print(Ind, "Your hands begin to glow.");
					p_ptr->confusing = TRUE;
					ident = TRUE;
				}
				break;
			}

			case SV_SCROLL_PROTECTION_FROM_EVIL:
			{
				k = 3 * p_ptr->lev;
				if (set_protevil(Ind, p_ptr->protevil + randint(25) + k)) ident = TRUE;
				break;
			}

			case SV_SCROLL_RUNE_OF_PROTECTION:
			{
				warding_glyph(Ind);
				ident = TRUE;
				break;
			}

			case SV_SCROLL_TRAP_DOOR_DESTRUCTION:
			{
				if (destroy_doors_touch(Ind)) ident = TRUE;
				break;
			}

			case SV_SCROLL_STAR_DESTRUCTION:
			{
				destroy_area(&p_ptr->wpos, p_ptr->py, p_ptr->px, 15, TRUE, FEAT_FLOOR);
				ident = TRUE;
				break;
			}

			case SV_SCROLL_DISPEL_UNDEAD:
			{
				if (dispel_undead(Ind, 60)) ident = TRUE;
				break;
			}

			case SV_SCROLL_GENOCIDE:
			{
				msg_print(Ind, "This is a genocide scroll.");
				(void)genocide(Ind);
				ident = TRUE;
				break;
			}

			case SV_SCROLL_MASS_GENOCIDE:
			{
				msg_print(Ind, "This is a mass genocide scroll.");
				(void)mass_genocide(Ind);
				ident = TRUE;
				break;
			}

			case SV_SCROLL_ACQUIREMENT:
			{
				acquirement(&p_ptr->wpos, p_ptr->py, p_ptr->px, 1, TRUE);
				ident = TRUE;
				break;
			}

			case SV_SCROLL_STAR_ACQUIREMENT:
			{
				acquirement(&p_ptr->wpos, p_ptr->py, p_ptr->px, randint(2) + 1, TRUE);
				ident = TRUE;
				break;
			}

			/* New Zangband scrolls */
			case SV_SCROLL_FIRE:
			{
				fire_ball(Ind, GF_FIRE, 0, 150, 4);
				/* Note: "Double" damage since it is centered on the player ... */
				if (!(p_ptr->oppose_fire || p_ptr->resist_fire || p_ptr->immune_fire))
					//                                take_hit(Ind, 50+randint(50)+(p_ptr->sensible_fire)?20:0, "a Scroll of Fire");
					take_hit(Ind, 50+randint(50), "a Scroll of Fire");
				ident = TRUE;
				break;
			}


			case SV_SCROLL_ICE:
			{
				fire_ball(Ind, GF_ICE, 0, 175, 4);
				if (!(p_ptr->oppose_cold || p_ptr->resist_cold || p_ptr->immune_cold))
					take_hit(Ind, 100+randint(100), "a Scroll of Ice");
				ident = TRUE;
				break;
			}

			case SV_SCROLL_CHAOS:
			{
				fire_ball(Ind, GF_CHAOS, 0, 222, 4);
				if (!p_ptr->resist_chaos)
					take_hit(Ind, 111+randint(111), "a Scroll of Chaos");
				ident = TRUE;
				break;
			}

			case SV_SCROLL_RUMOR:
			{
				msg_print(Ind, "You read the scroll:");
				fortune(Ind, magik(40) ? TRUE : FALSE);
				ident = TRUE;
				break;
			}

			case SV_SCROLL_LOTTERY:
			{
				int i = k_info[o_ptr->k_idx].cost, j = rand_int(10000);

				i -= i * o_ptr->discount / 100;

				if (j)
				{
					msg_print(Ind, "\377WYou draw a blank :-P");
				}
				else
				{
					if (p_ptr->au < i * 10000 / 5)
					{
						msg_broadcast_format(Ind, "\377B%s seems to hit the big time!", p_ptr->name);
#if 0
						char temp[80];
						sprintf(temp, "\377B%s seems to hit the big time!", p_ptr->name);
						msg_broadcast(Ind, temp);
#endif	// 0
						set_confused(Ind, p_ptr->confused + rand_int(10) + 10);
						set_image(Ind, p_ptr->image + rand_int(10) + 10);
					}

					msg_print(Ind, "\377BYou won the first prize!");
					p_ptr->au += i * 10000;

					/* Redraw gold */
					p_ptr->redraw |= (PR_GOLD);

					/* Window stuff */
					p_ptr->window |= (PW_PLAYER);
				}
				ident = TRUE;
				break;
			}

#if 0	// implement them whenever you feel like :)
			case SV_SCROLL_BACCARAT:
			case SV_SCROLL_BLACK_JACK:
			case SV_SCROLL_ROULETTE:
			case SV_SCROLL_SLOT_MACHINE:
			case SV_SCROLL_BACK_GAMMON:
			{
				break;
			}
#endif	// 0

			case SV_SCROLL_ID_ALL:
			{
				identify_pack(Ind);
				break;
			}

			case SV_SCROLL_VERMIN_CONTROL:
			{
				dun_level *l_ptr = getfloor(&p_ptr->wpos);
				if(l_ptr && !(l_ptr->flags1 & LF1_NO_MULTIPLY))
				{
					l_ptr->flags1 |= LF1_NO_MULTIPLY;
					msg_print(Ind, "You feel less itchy.");
					ident = TRUE;
				}
				break;
			}
		}
	}
	else if (o_ptr->tval == TV_PARCHEMENT)
	{
#if 0
		/* Maps */
		if (o_ptr->sval >= 200)
		{
			int i, n;
			char buf[80], fil[20];

			strnfmt(fil, 20, "book-%d.txt",o_ptr->sval);

			n = atoi(get_line(fil, ANGBAND_DIR_FILE, buf, -1));

			/* Parse all the fields */
			for (i = 0; i < n; i += 4)
			{
				/* Grab the fields */
				int x = atoi(get_line(fil, ANGBAND_DIR_FILE, buf, i + 0));
				int y = atoi(get_line(fil, ANGBAND_DIR_FILE, buf, i + 1));
				int w = atoi(get_line(fil, ANGBAND_DIR_FILE, buf, i + 2));
				int h = atoi(get_line(fil, ANGBAND_DIR_FILE, buf, i + 3));

				reveal_wilderness_around_player(y, x, h, w);
			}
		}

		/* Normal parchements */
		else
#endif	// 0
		{
			/* Get the filename */
			char    path[MAX_PATH_LENGTH];
			cptr q = format("book-%d.txt",o_ptr->sval);

			path_build(path, MAX_PATH_LENGTH, ANGBAND_DIR_TEXT, q);
			do_cmd_check_other_prepare(Ind, path);

//			used_up = FALSE;
			keep = TRUE;
		}
	}


	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* The item was tried */
	object_tried(Ind, o_ptr);

	/* An identification was made */
	if (ident && !object_aware_p(Ind, o_ptr))
	{
		object_aware(Ind, o_ptr);
		gain_exp(Ind, (lev + (p_ptr->lev >> 1)) / p_ptr->lev);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Hack -- really allow certain scrolls to be "preserved" */
	if (keep) return;

	/* Destroy a scroll in the pack */
	if (item >= 0)
	{
		inven_item_increase(Ind, item, -1);

		/* Hack -- allow certain scrolls to be "preserved" */
		if (!used_up) return;

		inven_item_describe(Ind, item);
		inven_item_optimize(Ind, item);
	}

	/* Destroy a scroll on the floor */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}
}







/*
 * Use a staff.			-RAK-
 *
 * One charge of one staff disappears.
 *
 * Hack -- staffs of identify can be "cancelled".
 */
void do_cmd_use_staff(int Ind, int item)
{
        u32b f1, f2, f3, f4, f5, esp;
	player_type *p_ptr = Players[Ind];

	int	ident, chance, lev, k, rad = DEFAULT_RADIUS_DEV(p_ptr);

	object_type		*o_ptr;

	/* Hack -- let staffs of identify get aborted */
	bool use_charge = TRUE;

#if 0	
	if (p_ptr->anti_magic)
	{
		msg_print(Ind, "An anti-magic shield disrupts your attempts.");	
		return;
	}
#endif
	if (get_skill(p_ptr, SKILL_ANTIMAGIC))
	{
		msg_print(Ind, "You don't believe in magic.");	
		return;
	}

	/* Restrict choices to wands */
	item_tester_tval = TV_STAFF;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	if( check_guard_inscription( o_ptr->note, 'u' )) {
                msg_print(Ind, "The item's inscription prevents it");
                return;
        }; 

	if (o_ptr->tval != TV_STAFF)
	{
		msg_print(Ind, "SERVER ERROR: Tried to use non-staff!");
		return;
	}

        if (!can_use(Ind, o_ptr))
        {
                msg_print(Ind, "You are not high level enough.");
		return;
        }

	/* Mega-Hack -- refuse to use a pile from the ground */
	if ((item < 0) && (o_ptr->number > 1))
	{
		msg_print(Ind, "You must first pick up the staffs.");
		return;
	}

	/* Verify potential overflow */
	/*if ((inven_cnt >= INVEN_PACK) &&
	    (o_ptr->number > 1))
	{*/
		/* Verify with the player */
		/*if (other_query_flag &&
		    !get_check("Your pack might overflow.  Continue? ")) return;
	}*/


	/* Take a turn */
	p_ptr->energy -= level_speed(&p_ptr->wpos);

	/* Not identified yet */
	ident = FALSE;

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - ((lev > 50) ? 50 : lev) - (p_ptr->antimagic * 2);

        /* Extract object flags */
        object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &esp);

        /* Is it simple to use ? */
        if (f4 & TR4_EASY_USE)
        {
                chance *= 10;
        }

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && (rand_int(USE_DEVICE - chance + 1) == 0))
	{
		chance = USE_DEVICE;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint(chance) < USE_DEVICE))
	{
		msg_print(Ind, "You failed to use the staff properly.");
		return;
	}

	/* Notice empty staffs */
	if (o_ptr->pval <= 0)
	{
		msg_print(Ind, "The staff has no charges left.");
		o_ptr->ident |= ID_EMPTY;
		
		/* Redraw */
		p_ptr->window |= (PW_INVEN);

		return;
	}


	/* Analyze the staff */
	switch (o_ptr->sval)
	{
		case SV_STAFF_DARKNESS:
		{
			if (unlite_area(Ind, 10, 3)) ident = TRUE;
			if (!p_ptr->resist_blind)
			{
				if (set_blind(Ind, p_ptr->blind + 3 + randint(5) - get_skill_scale(p_ptr, SKILL_DEVICE, 3))) ident = TRUE;
			}
			break;
		}

		case SV_STAFF_SLOWNESS:
		{
			if (set_slow(Ind, p_ptr->slow + randint(30) + 15 - get_skill_scale(p_ptr, SKILL_DEVICE, 15))) ident = TRUE;
			break;
		}

		case SV_STAFF_HASTE_MONSTERS:
		{
			if (speed_monsters(Ind)) ident = TRUE;
			break;
		}

		case SV_STAFF_SUMMONING:
		{
			for (k = 0; k < randint(4); k++)
			{
				if (summon_specific(&p_ptr->wpos, p_ptr->py, p_ptr->px, getlevel(&p_ptr->wpos), 0))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_STAFF_TELEPORTATION:
		{
			msg_format_near(Ind, "%s teleports away!", p_ptr->name);
			teleport_player(Ind, 100 + get_skill_scale(p_ptr, SKILL_DEVICE, 100));
			ident = TRUE;
			break;
		}

		case SV_STAFF_IDENTIFY:
		{
			if (!ident_spell(Ind)) use_charge = FALSE;
			ident = TRUE;
			break;
		}

		case SV_STAFF_REMOVE_CURSE:
		{
			if (remove_curse(Ind))
			{
				if (!p_ptr->blind)
				{
					msg_print(Ind, "The staff glows blue for a moment...");
				}
				ident = TRUE;
			}
			break;
		}

		case SV_STAFF_STARLITE:
		{
			if (!p_ptr->blind)
			{
				msg_print(Ind, "The end of the staff glows brightly...");
			}
			for (k = 0; k < 8; k++) lite_line(Ind, ddd[k]);
			ident = TRUE;
			break;
		}

		case SV_STAFF_LITE:
		{
			msg_format_near(Ind, "%s calls light.", p_ptr->name);
			if (lite_area(Ind, damroll(2 + get_skill_scale(p_ptr, SKILL_DEVICE, 10), 8), 2)) ident = TRUE;
			break;
		}

		case SV_STAFF_MAPPING:
		{
			map_area(Ind);
			ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_GOLD:
		{
			if (detect_treasure(Ind, rad)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_ITEM:
		{
			if (detect_object(Ind, rad)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_TRAP:
		{
			if (detect_trap(Ind, rad)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_DOOR:
		{
			if (detect_sdoor(Ind, rad)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_INVIS:
		{
			if (detect_invisible(Ind)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_EVIL:
		{
			if (detect_evil(Ind)) ident = TRUE;
			break;
		}

		case SV_STAFF_CURE_LIGHT:
		{
			if (hp_player(Ind, randint(8 + get_skill_scale(p_ptr, SKILL_DEVICE, 10)))) ident = TRUE;
			break;
		}

		case SV_STAFF_CURING:
		{
			if (set_blind(Ind, 0)) ident = TRUE;
			if (set_poisoned(Ind, 0)) ident = TRUE;
			if (set_confused(Ind, 0)) ident = TRUE;
			if (set_stun(Ind, 0)) ident = TRUE;
			if (set_cut(Ind, 0)) ident = TRUE;
			break;
		}

		case SV_STAFF_HEALING:
		{
			if (hp_player(Ind, 300 + get_skill_scale(p_ptr, SKILL_DEVICE, 100))) ident = TRUE;
			if (set_stun(Ind, 0)) ident = TRUE;
			if (set_cut(Ind, 0)) ident = TRUE;
			break;
		}

		case SV_STAFF_THE_MAGI:
		{
			if (do_res_stat(Ind, A_INT)) ident = TRUE;
			if (p_ptr->csp < p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
				ident = TRUE;
				msg_print(Ind, "Your feel your head clear.");
				p_ptr->redraw |= (PR_MANA);
				p_ptr->window |= (PW_PLAYER);
			}
			break;
		}

		case SV_STAFF_SLEEP_MONSTERS:
		{
			if (sleep_monsters(Ind)) ident = TRUE;
			break;
		}

		case SV_STAFF_SLOW_MONSTERS:
		{
			if (slow_monsters(Ind)) ident = TRUE;
			break;
		}

		case SV_STAFF_SPEED:
		{
			if (!p_ptr->fast)
			{
				if (set_fast(Ind, randint(30) + 15 + get_skill_scale(p_ptr, SKILL_DEVICE, 10))) ident = TRUE;
			}
			else
			{
				(void)set_fast(Ind, p_ptr->fast + 5);
			}
			break;
		}

		case SV_STAFF_PROBING:
		{
			probing(Ind);
			ident = TRUE;
			break;
		}

		case SV_STAFF_DISPEL_EVIL:
		{
			if (dispel_evil(Ind, 60 + get_skill_scale(p_ptr, SKILL_DEVICE, 70))) ident = TRUE;
			break;
		}

		case SV_STAFF_POWER:
		{
			if (dispel_monsters(Ind, 120 + get_skill_scale(p_ptr, SKILL_DEVICE, 120))) ident = TRUE;
			break;
		}

		case SV_STAFF_HOLINESS:
		{
			if (dispel_evil(Ind, 120 + get_skill_scale(p_ptr, SKILL_DEVICE, 120))) ident = TRUE;
			k = get_skill_scale(p_ptr, SKILL_DEVICE, 150);
			if (set_protevil(Ind, p_ptr->protevil + randint(25) + k)) ident = TRUE;
			if (set_poisoned(Ind, 0)) ident = TRUE;
			if (set_afraid(Ind, 0)) ident = TRUE;
			if (hp_player(Ind, 50)) ident = TRUE;
			if (set_stun(Ind, 0)) ident = TRUE;
			if (set_cut(Ind, 0)) ident = TRUE;
			break;
		}

		case SV_STAFF_GENOCIDE:
		{
			(void)genocide(Ind);
			ident = TRUE;
			break;
		}

		case SV_STAFF_EARTHQUAKES:
		{
			msg_format_near(Ind, "%s causes the ground to shake!", p_ptr->name);
			earthquake(&p_ptr->wpos, p_ptr->py, p_ptr->px, 10);
			ident = TRUE;
			break;
		}

		case SV_STAFF_DESTRUCTION:
		{
			msg_format_near(Ind, "%s unleashes great power!", p_ptr->name);
			destroy_area(&p_ptr->wpos, p_ptr->py, p_ptr->px, 15, TRUE, FEAT_FLOOR);
			ident = TRUE;
			break;
		}

		case SV_STAFF_STAR_IDENTIFY:
		{
			if (!identify_fully(Ind)) use_charge = FALSE;
			ident = TRUE;
			break;
		}

	}


	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Tried the item */
	object_tried(Ind, o_ptr);

	/* An identification was made */
	if (ident && !object_aware_p(Ind, o_ptr))
	{
		object_aware(Ind, o_ptr);
		gain_exp(Ind, (lev + (p_ptr->lev >> 1)) / p_ptr->lev);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Hack -- some uses are "free" */
	if (!use_charge) return;


	/* Use a single charge */
	o_ptr->pval--;

	/* XXX Hack -- unstack if necessary */
	if ((item >= 0) && (o_ptr->number > 1))
	{
		/* Make a fake item */
		object_type tmp_obj;
		tmp_obj = *o_ptr;
		tmp_obj.number = 1;

		/* Restore the charges */
		o_ptr->pval++;

		/* Unstack the used item */
		o_ptr->number--;
		p_ptr->total_weight -= tmp_obj.weight;
		item = inven_carry(Ind, &tmp_obj);

		/* Message */
		msg_print(Ind, "You unstack your staff.");
	}

	/* Describe charges in the pack */
	if (item >= 0)
	{
		inven_item_charges(Ind, item);
	}

	/* Describe charges on the floor */
	else
	{
		floor_item_charges(0 - item);
	}
}


/*
 * Aim a wand (from the pack or floor).
 *
 * Use a single charge from a single item.
 * Handle "unstacking" in a logical manner.
 *
 * For simplicity, you cannot use a stack of items from the
 * ground.  This would require too much nasty code.
 *
 * There are no wands which can "destroy" themselves, in the inventory
 * or on the ground, so we can ignore this possibility.  Note that this
 * required giving "wand of wonder" the ability to ignore destruction
 * by electric balls.
 *
 * All wands can be "cancelled" at the "Direction?" prompt for free.
 *
 * Note that the basic "bolt" wands do slightly less damage than the
 * basic "bolt" rods, but the basic "ball" wands do the same damage
 * as the basic "ball" rods.
 */
void do_cmd_aim_wand(int Ind, int item, int dir)
{
        u32b f1, f2, f3, f4, f5, esp;
	player_type *p_ptr = Players[Ind];

	int			lev, ident, chance, sval;

	object_type		*o_ptr;

#if 0
	if (p_ptr->anti_magic)
	{
		msg_print(Ind, "An anti-magic shield disrupts your attempts.");	
		return;
	}
#endif	// 0
	if (get_skill(p_ptr, SKILL_ANTIMAGIC))
	{
		msg_print(Ind, "You don't believe in magic.");	
		return;
	}

	/* Restrict choices to wands */
	item_tester_tval = TV_WAND;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}
	if( check_guard_inscription( o_ptr->note, 'a' )) {
                msg_print(Ind, "The item's inscription prevents it");
                return;
        }; 

	if (o_ptr->tval != TV_WAND)
	{
		msg_print(Ind, "SERVER ERROR: Tried to use non-wand!");
		return;
	}

        if (!can_use(Ind, o_ptr))
        {
                msg_print(Ind, "You are not high level enough.");
		return;
        }

	/* Mega-Hack -- refuse to aim a pile from the ground */
	if ((item < 0) && (o_ptr->number > 1))
	{
		msg_print(Ind, "You must first pick up the wands.");
		return;
	}

	/* Hack -- verify potential overflow */
	/*if ((inven_cnt >= INVEN_PACK) &&
	    (o_ptr->number > 1))
	{*/
		/* Verify with the player */
	/*	if (other_query_flag &&
		    !get_check("Your pack might overflow.  Continue? ")) return;
	}*/


	/* Allow direction to be cancelled for free */
	/*if (!get_aim_dir(&dir)) return;*/


	/* Take a turn */
	p_ptr->energy -= level_speed(&p_ptr->wpos);

	/* Not identified yet */
	ident = FALSE;

	/* Get the level */
	lev = k_info[o_ptr->k_idx].level;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - ((lev > 50) ? 50 : lev) - (p_ptr->antimagic * 2);

        /* Extract object flags */
        object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &esp);

        /* Is it simple to use ? */
        if (f4 & TR4_EASY_USE)
        {
                chance *= 10;
        }

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && (rand_int(USE_DEVICE - chance + 1) == 0))
	{
		chance = USE_DEVICE;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint(chance) < USE_DEVICE))
	{
		msg_print(Ind, "You failed to use the wand properly.");
		return;
	}

	/* The wand is already empty! */
	if (o_ptr->pval <= 0)
	{
		msg_print(Ind, "The wand has no charges left.");
		o_ptr->ident |= ID_EMPTY;

		/* Redraw */
		p_ptr->window |= (PW_INVEN);

		return;
	}



	/* XXX Hack -- Extract the "sval" effect */
	sval = o_ptr->sval;

	/* XXX Hack -- Wand of wonder can do anything before it */
	if (sval == SV_WAND_WONDER) sval = rand_int(SV_WAND_WONDER);

	/* Analyze the wand */
	switch (sval)
	{
		case SV_WAND_HEAL_MONSTER:
		{
			if (heal_monster(Ind, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_HASTE_MONSTER:
		{
			if (speed_monster(Ind, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_CLONE_MONSTER:
		{
			if (clone_monster(Ind, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_TELEPORT_AWAY:
		{
			if (teleport_monster(Ind, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_DISARMING:
		{
			if (disarm_trap(Ind, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_TRAP_DOOR_DEST:
		{
			if (destroy_door(Ind, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_STONE_TO_MUD:
		{
			if (wall_to_mud(Ind, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_LITE:
		{
			msg_print(Ind, "A line of blue shimmering light appears.");
			lite_line(Ind, dir);
			ident = TRUE;
			break;
		}

		case SV_WAND_SLEEP_MONSTER:
		{
			if (sleep_monster(Ind, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_SLOW_MONSTER:
		{
			if (slow_monster(Ind, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_CONFUSE_MONSTER:
		{
			if (confuse_monster(Ind, dir, 10 + get_skill_scale(p_ptr, SKILL_DEVICE, 50))) ident = TRUE;
			break;
		}

		case SV_WAND_FEAR_MONSTER:
		{
			if (fear_monster(Ind, dir, 10 + get_skill_scale(p_ptr, SKILL_DEVICE, 50))) ident = TRUE;
			break;
		}

		case SV_WAND_DRAIN_LIFE:
		{
			if (drain_life(Ind, dir, 75 + get_skill_scale(p_ptr, SKILL_DEVICE, 75))) ident = TRUE;
			break;
		}

		case SV_WAND_POLYMORPH:
		{
			if (poly_monster(Ind, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_STINKING_CLOUD:
		{
			msg_format_near(Ind, "%s fires a stinking cloud.", p_ptr->name);
			fire_ball(Ind, GF_POIS, dir, 12 + get_skill_scale(p_ptr, SKILL_DEVICE, 50), 2);
			ident = TRUE;
			break;
		}

		case SV_WAND_MAGIC_MISSILE:
		{
			msg_format_near(Ind, "%s fires a magic missile.", p_ptr->name);
			fire_bolt_or_beam(Ind, 20, GF_MISSILE, dir, damroll(2 + get_skill_scale(p_ptr, SKILL_DEVICE, 10), 6));
			ident = TRUE;
			break;
		}

		case SV_WAND_ACID_BOLT:
		{
			msg_format_near(Ind, "%s fires an acid bolt.", p_ptr->name);
			fire_bolt_or_beam(Ind, 20, GF_ACID, dir, damroll(5 + get_skill_scale(p_ptr, SKILL_DEVICE, 10), 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_ELEC_BOLT:
		{
			msg_format_near(Ind, "%s fires a lightning bolt.", p_ptr->name);
			fire_bolt_or_beam(Ind, 20, GF_ELEC, dir, damroll(3 + get_skill_scale(p_ptr, SKILL_DEVICE, 10), 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_FIRE_BOLT:
		{
			msg_format_near(Ind, "%s fires a fire bolt.", p_ptr->name);
			fire_bolt_or_beam(Ind, 20, GF_FIRE, dir, damroll(6 + get_skill_scale(p_ptr, SKILL_DEVICE, 10), 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_COLD_BOLT:
		{
			msg_format_near(Ind, "%s fires a frost bolt.", p_ptr->name);
			fire_bolt_or_beam(Ind, 20, GF_COLD, dir, damroll(3 + get_skill_scale(p_ptr, SKILL_DEVICE, 10), 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_ACID_BALL:
		{
			msg_format_near(Ind, "%s fires a ball of acid.", p_ptr->name);
			fire_ball(Ind, GF_ACID, dir, 60 + get_skill_scale(p_ptr, SKILL_DEVICE, 60), 2);
			ident = TRUE;
			break;
		}

		case SV_WAND_ELEC_BALL:
		{
			msg_format_near(Ind, "%s fires a ball of electricity.", p_ptr->name);
			fire_ball(Ind, GF_ELEC, dir, 32 + get_skill_scale(p_ptr, SKILL_DEVICE, 50), 2);
			ident = TRUE;
			break;
		}

		case SV_WAND_FIRE_BALL:
		{
			msg_format_near(Ind, "%s fires a fire ball.", p_ptr->name);
			fire_ball(Ind, GF_FIRE, dir, 72 + get_skill_scale(p_ptr, SKILL_DEVICE, 50), 2);
			ident = TRUE;
			break;
		}

		case SV_WAND_COLD_BALL:
		{
			msg_format_near(Ind, "%s fires a frost ball.", p_ptr->name);
			fire_ball(Ind, GF_COLD, dir, 48 + get_skill_scale(p_ptr, SKILL_DEVICE, 50), 2);
			ident = TRUE;
			break;
		}

		case SV_WAND_WONDER:
		{
			msg_print(Ind, "SERVER ERROR: Oops.  Wand of wonder activated.");
			break;
		}

		case SV_WAND_DRAGON_FIRE:
		{
			msg_format_near(Ind, "%s shoots dragon fire!", p_ptr->name);
			fire_ball(Ind, GF_FIRE, dir, 100 + get_skill_scale(p_ptr, SKILL_DEVICE, 100), 3);
			ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_COLD:
		{
			msg_format_near(Ind, "%s shoots dragon frost!", p_ptr->name);
			fire_ball(Ind, GF_COLD, dir, 80 + get_skill_scale(p_ptr, SKILL_DEVICE, 80), 3);
			ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_BREATH:
		{
			switch (randint(5))
			{
				case 1:
				{
					msg_format_near(Ind, "%s shoots dragon acid!", p_ptr->name);
					fire_ball(Ind, GF_ACID, dir, 100 + get_skill_scale(p_ptr, SKILL_DEVICE, 100), 3);
					break;
				}

				case 2:
				{
					msg_format_near(Ind, "%s shoots dragon lightning!", p_ptr->name);
					fire_ball(Ind, GF_ELEC, dir, 80 + get_skill_scale(p_ptr, SKILL_DEVICE, 100), 3);
					break;
				}

				case 3:
				{
					msg_format_near(Ind, "%s shoots dragon fire!", p_ptr->name);
					fire_ball(Ind, GF_FIRE, dir, 100 + get_skill_scale(p_ptr, SKILL_DEVICE, 100), 3);
					break;
				}

				case 4:
				{
					msg_format_near(Ind, "%s shoots dragon frost!", p_ptr->name);
					fire_ball(Ind, GF_COLD, dir, 80 + get_skill_scale(p_ptr, SKILL_DEVICE, 100), 3);
					break;
				}

				default:
				{
					msg_format_near(Ind, "%s shoots dragon poison!", p_ptr->name);
					fire_ball(Ind, GF_POIS, dir, 60 + get_skill_scale(p_ptr, SKILL_DEVICE, 100), 3);
					break;
				}
			}

			ident = TRUE;
			break;
		}

		case SV_WAND_ANNIHILATION:
		{
			if (drain_life(Ind, dir, 125 + get_skill_scale(p_ptr, SKILL_DEVICE, 125))) ident = TRUE;
			break;
		}

		/* Additions from PernAngband	- Jir - */
		case SV_WAND_ROCKETS:
		{
			msg_print(Ind, "You launch a rocket!");
			fire_ball(Ind, GF_ROCKET, dir, 75 + (randint(50) + get_skill_scale(p_ptr, SKILL_DEVICE, 100)), 2);
			ident = TRUE;
			break;
		}
#if 0
		/* Hope we can port this someday.. */
		case SV_WAND_CHARM_MONSTER:
		{
			if (charm_monster(dir, 45))
			ident = TRUE;
			break;
		}

#endif	// 0

                case SV_WAND_WALL_CREATION:
		{
                        project_hook(Ind, GF_STONE_WALL, dir, 1, PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID);
                        ident = TRUE;
			break;
		}

	}


	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Mark it as tried */
	object_tried(Ind, o_ptr);

	/* Apply identification */
	if (ident && !object_aware_p(Ind, o_ptr))
	{
		object_aware(Ind, o_ptr);
		gain_exp(Ind, (lev + (p_ptr->lev >> 1)) / p_ptr->lev);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Use a single charge */
	o_ptr->pval--;

	/* Hack -- unstack if necessary */
	if ((item >= 0) && (o_ptr->number > 1))
	{
		/* Make a fake item */
		object_type tmp_obj;
		tmp_obj = *o_ptr;
		tmp_obj.number = 1;

		/* Restore the charges */
		o_ptr->pval++;

		/* Unstack the used item */
		o_ptr->number--;
		p_ptr->total_weight -= tmp_obj.weight;
		item = inven_carry(Ind, &tmp_obj);

		/* Message */
		msg_print(Ind, "You unstack your wand.");
	}

	/* Describe the charges in the pack */
	if (item >= 0)
	{
		inven_item_charges(Ind, item);
	}

	/* Describe the charges on the floor */
	else
	{
		floor_item_charges(0 - item);
	}
}





/*
 * Activate (zap) a Rod
 *
 * Unstack fully charged rods as needed.
 *
 * Hack -- rods of perception/genocide can be "cancelled"
 * All rods can be cancelled at the "Direction?" prompt
 */
void do_cmd_zap_rod(int Ind, int item)
{
        u32b f1, f2, f3, f4, f5, esp;
	player_type *p_ptr = Players[Ind];

	int                 ident, chance, lev, rad = DEFAULT_RADIUS_DEV(p_ptr);

	object_type		*o_ptr;

	/* Hack -- let perception get aborted */
	bool use_charge = TRUE;
#if 0
	if (p_ptr->anti_magic)
	{
		msg_print(Ind, "An anti-magic shield disrupts your attempts.");	
		return;
	}
#endif	// 0
	if (get_skill(p_ptr, SKILL_ANTIMAGIC))
	{
		msg_print(Ind, "You don't believe in magic.");	
		return;
	}

	/* Restrict choices to rods */
	item_tester_tval = TV_ROD;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}
	if( check_guard_inscription( o_ptr->note, 'z' )) {
                msg_print(Ind, "The item's inscription prevents it");
                return;
        }; 


	if (o_ptr->tval != TV_ROD)
	{
		msg_print(Ind, "SERVER ERROR: Tried to zap non-rod!");
		return;
	}

	/* Mega-Hack -- refuse to zap a pile from the ground */
	if ((item < 0) && (o_ptr->number > 1))
	{
		msg_print(Ind, "You must first pick up the rods.");
		return;
	}

        if (!can_use(Ind, o_ptr))
        {
                msg_print(Ind, "You are not high level enough.");
		return;
        }

	/* Get a direction (unless KNOWN not to need it) */
	/* Pfft, dirty, dirty, diiirrrrtie!! (FIXME) */
//	if ((o_ptr->sval >= SV_ROD_MIN_DIRECTION) || !object_aware_p(Ind, o_ptr))
        if (((o_ptr->sval >= SV_ROD_MIN_DIRECTION) &&
			!(o_ptr->sval == SV_ROD_DETECT_TRAP) &&
//			!(o_ptr->sval == SV_ROD_HAVOC) &&
			!(o_ptr->sval == SV_ROD_HOME)) ||
		     !object_aware_p(Ind, o_ptr))
	{
		/* Get a direction, then return */
		p_ptr->current_rod = item;
		get_aim_dir(Ind);
		return;
	}

	/* Extract object flags */
	object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &esp);


	/* Take a turn */
	p_ptr->energy -= level_speed(&p_ptr->wpos) /
	        ((f4 & TR4_FAST_CAST)?2:1);

	/* Not identified yet */
	ident = FALSE;

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - ((lev > 50) ? 50 : lev) - (p_ptr->antimagic * 2);

        /* Is it simple to use ? */
        if (f4 & TR4_EASY_USE)
        {
                chance *= 10;
        }

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && (rand_int(USE_DEVICE - chance + 1) == 0))
	{
		chance = USE_DEVICE;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint(chance) < USE_DEVICE))
	{
		msg_print(Ind, "You failed to use the rod properly.");
		return;
	}

	/* Still charging */
	if (o_ptr->pval)
	{
		msg_print(Ind, "The rod is still charging.");
		return;
	}

	process_hooks(HOOK_ZAP, "d", Ind);

	/* Analyze the rod */
	switch (o_ptr->sval)
	{
		case SV_ROD_DETECT_TRAP:
		{
			if (detect_trap(Ind, rad)) ident = TRUE;
			o_ptr->pval = 50;
			break;
		}

		case SV_ROD_DETECT_DOOR:
		{
			if (detect_sdoor(Ind, rad)) ident = TRUE;
			o_ptr->pval = 70;
			break;
		}

		case SV_ROD_IDENTIFY:
		{
			ident = TRUE;
			if (!ident_spell(Ind)) use_charge = FALSE;
			o_ptr->pval = 10;
			break;
		}

		case SV_ROD_RECALL:
		{
			if (p_ptr->word_recall == 0)
			{
				set_recall_depth(p_ptr, o_ptr);
				msg_print(Ind, "\377oThe air about you becomes charged...");
				p_ptr->word_recall = 15 + randint(20);
			}
			else
			{
				msg_print(Ind, "\377oA tension leaves the air around you...");
				p_ptr->word_recall = 0;
			}
			ident = TRUE;
			o_ptr->pval = 60;
			break;
		}

		case SV_ROD_ILLUMINATION:
		{
			msg_format_near(Ind, "%s calls light.", p_ptr->name);
			if (lite_area(Ind, damroll(2, 8), 2)) ident = TRUE;
			o_ptr->pval = 30;
			break;
		}

		case SV_ROD_MAPPING:
		{
			map_area(Ind);
			ident = TRUE;
			o_ptr->pval = 99;
			break;
		}

		case SV_ROD_DETECTION:
		{
			detection(Ind, rad);
			ident = TRUE;
			o_ptr->pval = 99;
			break;
		}

		case SV_ROD_PROBING:
		{
			probing(Ind);
			ident = TRUE;
			o_ptr->pval = 50;
			break;
		}

		case SV_ROD_CURING:
		{
			if (set_blind(Ind, 0)) ident = TRUE;
			if (set_poisoned(Ind, 0)) ident = TRUE;
			if (set_confused(Ind, 0)) ident = TRUE;
			if (set_stun(Ind, 0)) ident = TRUE;
			if (set_cut(Ind, 0)) ident = TRUE;
			o_ptr->pval = 999;
			break;
		}

		case SV_ROD_HEALING:
		{
			if (hp_player(Ind, 500)) ident = TRUE;
			if (set_stun(Ind, 0)) ident = TRUE;
			if (set_cut(Ind, 0)) ident = TRUE;
			o_ptr->pval = 999;
			break;
		}

		case SV_ROD_RESTORATION:
		{
			if (restore_level(Ind)) ident = TRUE;
			if (do_res_stat(Ind, A_STR)) ident = TRUE;
			if (do_res_stat(Ind, A_INT)) ident = TRUE;
			if (do_res_stat(Ind, A_WIS)) ident = TRUE;
			if (do_res_stat(Ind, A_DEX)) ident = TRUE;
			if (do_res_stat(Ind, A_CON)) ident = TRUE;
			if (do_res_stat(Ind, A_CHR)) ident = TRUE;
			o_ptr->pval = 999;
			break;
		}

		case SV_ROD_SPEED:
		{
			if (!p_ptr->fast)
			{
				if (set_fast(Ind, randint(30) + 15)) ident = TRUE;
			}
			else
			{
				(void)set_fast(Ind, p_ptr->fast + 5);
			}
			o_ptr->pval = 99;
			break;
		}

		case SV_ROD_NOTHING:
		{
			break;
		}

		default:
		{
			msg_print(Ind, "SERVER ERROR: Directional rod zapped in non-directional function!");
			return;
		}
	}
	if(f4 & TR4_CHARGING) o_ptr->pval/=3;

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Tried the object */
	object_tried(Ind, o_ptr);

	/* Successfully determined the object function */
	if (ident && !object_aware_p(Ind, o_ptr))
	{
		object_aware(Ind, o_ptr);
		gain_exp(Ind, (lev + (p_ptr->lev >> 1)) / p_ptr->lev);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Hack -- deal with cancelled zap */
	if (!use_charge)
	{
		o_ptr->pval = 0;
		return;
	}


	/* XXX Hack -- unstack if necessary */
	if ((item >= 0) && (o_ptr->number > 1))
	{
		/* Make a fake item */
		object_type tmp_obj;
		tmp_obj = *o_ptr;
		tmp_obj.number = 1;

		/* Restore "charge" */
		o_ptr->pval = 0;

		/* Unstack the used item */
		o_ptr->number--;
		p_ptr->total_weight -= tmp_obj.weight;
		item = inven_carry(Ind, &tmp_obj);

		/* Message */
		msg_print(Ind, "You unstack your rod.");
	}
}



/*
 * Activate (zap) a Rod
 *
 * Unstack fully charged rods as needed.
 *
 * Hack -- rods of perception/genocide can be "cancelled"
 * All rods can be cancelled at the "Direction?" prompt
 */
void do_cmd_zap_rod_dir(int Ind, int dir)
{
        u32b f1, f2, f3, f4, f5, esp;
	player_type *p_ptr = Players[Ind];

	int	item, ident, chance, lev, rad = DEFAULT_RADIUS_DEV(p_ptr);

	object_type		*o_ptr;

	/* Hack -- let perception get aborted */
	bool use_charge = TRUE;


	item = p_ptr->current_rod;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}
	if( check_guard_inscription( o_ptr->note, 'z' )) {
                msg_print(Ind, "The item's inscription prevents it");
                return;
        }; 

	if (o_ptr->tval != TV_ROD)
	{
		msg_print(Ind, "SERVER ERROR: Tried to zap non-rod!");
		return;
	}

	/* Mega-Hack -- refuse to zap a pile from the ground */
	if ((item < 0) && (o_ptr->number > 1))
	{
		msg_print(Ind, "You must first pick up the rods.");
		return;
	}

        if (!can_use(Ind, o_ptr))
        {
                msg_print(Ind, "You are not high level enough.");
		return;
        }

	/* Hack -- verify potential overflow */
	/*if ((inven_cnt >= INVEN_PACK) &&
	    (o_ptr->number > 1))
	{*/
		/* Verify with the player */
		/*if (other_query_flag &&
		    !get_check("Your pack might overflow.  Continue? ")) return;
	}*/

	/* Extract object flags */
	object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &esp);


	/* Take a turn */
	p_ptr->energy -= level_speed(&p_ptr->wpos) /
	        ((f4 & TR4_FAST_CAST)?2:1);

	/* Not identified yet */
	ident = FALSE;

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - ((lev > 50) ? 50 : lev) - p_ptr->antimagic;

        /* Is it simple to use ? */
        if (f4 & TR4_EASY_USE)
        {
                chance *= 10;
        }

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && (rand_int(USE_DEVICE - chance + 1) == 0))
	{
		chance = USE_DEVICE;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint(chance) < USE_DEVICE))
	{
		msg_print(Ind, "You failed to use the rod properly.");
		return;
	}

	/* Still charging */
	if (o_ptr->pval)
	{
		msg_print(Ind, "The rod is still charging.");
		return;
	}

	process_hooks(HOOK_ZAP, "d", Ind);

	/* Analyze the rod */
	switch (o_ptr->sval)
	{
		case SV_ROD_TELEPORT_AWAY:
		{
			if (teleport_monster(Ind, dir)) ident = TRUE;
			o_ptr->pval = 25;
			break;
		}

		case SV_ROD_DISARMING:
		{
			if (disarm_trap(Ind, dir)) ident = TRUE;
			o_ptr->pval = 30;
			break;
		}

		case SV_ROD_LITE:
		{
			msg_print(Ind, "A line of blue shimmering light appears.");
			lite_line(Ind, dir);
			ident = TRUE;
			o_ptr->pval = 9;
			break;
		}

		case SV_ROD_SLEEP_MONSTER:
		{
			if (sleep_monster(Ind, dir)) ident = TRUE;
			o_ptr->pval = 18;
			break;
		}

		case SV_ROD_SLOW_MONSTER:
		{
			if (slow_monster(Ind, dir)) ident = TRUE;
			o_ptr->pval = 20;
			break;
		}

		case SV_ROD_DRAIN_LIFE:
		{
			if (drain_life(Ind, dir, 75)) ident = TRUE;
			o_ptr->pval = 23;
			break;
		}

		case SV_ROD_POLYMORPH:
		{
			if (poly_monster(Ind, dir)) ident = TRUE;
			o_ptr->pval = 25;
			break;
		}

		case SV_ROD_ACID_BOLT:
		{
			msg_format_near(Ind, "%s fires an acid bolt.", p_ptr->name);
			fire_bolt_or_beam(Ind, 10, GF_ACID, dir, damroll(6, 8));
			ident = TRUE;
			o_ptr->pval = 12;
			break;
		}

		case SV_ROD_ELEC_BOLT:
		{
			msg_format_near(Ind, "%s fires a lightning bolt.", p_ptr->name);
			fire_bolt_or_beam(Ind, 10, GF_ELEC, dir, damroll(3, 8));
			ident = TRUE;
			o_ptr->pval = 11;
			break;
		}

		case SV_ROD_FIRE_BOLT:
		{
			msg_format_near(Ind, "%s fires a fire bolt.", p_ptr->name);
			fire_bolt_or_beam(Ind, 10, GF_FIRE, dir, damroll(8, 8));
			ident = TRUE;
			o_ptr->pval = 15;
			break;
		}

		case SV_ROD_COLD_BOLT:
		{
			msg_format_near(Ind, "%s fires a frost bolt.", p_ptr->name);
			fire_bolt_or_beam(Ind, 10, GF_COLD, dir, damroll(5, 8));
			ident = TRUE;
			o_ptr->pval = 13;
			break;
		}

		case SV_ROD_ACID_BALL:
		{
			msg_format_near(Ind, "%s fires an acid ball.", p_ptr->name);
			fire_ball(Ind, GF_ACID, dir, 60, 2);
			ident = TRUE;
			o_ptr->pval = 27;
			break;
		}

		case SV_ROD_ELEC_BALL:
		{
			msg_format_near(Ind, "%s fires a lightning ball.", p_ptr->name);
			fire_ball(Ind, GF_ELEC, dir, 32, 2);
			ident = TRUE;
			o_ptr->pval = 23;
			break;
		}

		case SV_ROD_FIRE_BALL:
		{
			msg_format_near(Ind, "%s fires a fire ball.", p_ptr->name);
			fire_ball(Ind, GF_FIRE, dir, 72, 2);
			ident = TRUE;
			o_ptr->pval = 30;
			break;
		}

		case SV_ROD_COLD_BALL:
		{
			msg_format_near(Ind, "%s fires a frost ball.", p_ptr->name);
			fire_ball(Ind, GF_COLD, dir, 48, 2);
			ident = TRUE;
			o_ptr->pval = 25;
			break;
		}

		/* All of the following are needed if we tried zapping one of */
		/* these but we didn't know what it was. */
		case SV_ROD_DETECT_TRAP:
		{
			if (detect_trap(Ind, rad)) ident = TRUE;
			o_ptr->pval = 50;
			break;
		}

		case SV_ROD_DETECT_DOOR:
		{
			if (detect_sdoor(Ind, rad)) ident = TRUE;
			o_ptr->pval = 70;
			break;
		}

		case SV_ROD_IDENTIFY:
		{
			ident = TRUE;
			if (!ident_spell(Ind)) use_charge = FALSE;
			o_ptr->pval = 10;
			break;
		}

		case SV_ROD_RECALL:
		{
			if (p_ptr->word_recall == 0)
			{
				set_recall_depth(p_ptr, o_ptr);
				msg_print(Ind, "\377oThe air about you becomes charged...");
				p_ptr->word_recall = 15 + randint(20);
			}
			else
			{
				msg_print(Ind, "\377oA tension leaves the air around you...");
				p_ptr->word_recall = 0;
			}
			ident = TRUE;
			o_ptr->pval = 60;
			break;
		}

		case SV_ROD_ILLUMINATION:
		{
			msg_format_near(Ind, "%s calls light.", p_ptr->name);
			if (lite_area(Ind, damroll(2, 8), 2)) ident = TRUE;
			o_ptr->pval = 30;
			break;
		}

		case SV_ROD_MAPPING:
		{
			map_area(Ind);
			ident = TRUE;
			o_ptr->pval = 99;
			break;
		}

		case SV_ROD_DETECTION:
		{
			detection(Ind, rad);
			ident = TRUE;
			o_ptr->pval = 99;
			break;
		}

		case SV_ROD_PROBING:
		{
			probing(Ind);
			ident = TRUE;
			o_ptr->pval = 50;
			break;
		}

		case SV_ROD_CURING:
		{
			if (set_blind(Ind, 0)) ident = TRUE;
			if (set_poisoned(Ind, 0)) ident = TRUE;
			if (set_confused(Ind, 0)) ident = TRUE;
			if (set_stun(Ind, 0)) ident = TRUE;
			if (set_cut(Ind, 0)) ident = TRUE;
			o_ptr->pval = 999;
			break;
		}

		case SV_ROD_HEALING:
		{
			if (hp_player(Ind, 500)) ident = TRUE;
			if (set_stun(Ind, 0)) ident = TRUE;
			if (set_cut(Ind, 0)) ident = TRUE;
			o_ptr->pval = 999;
			break;
		}

		case SV_ROD_RESTORATION:
		{
			if (restore_level(Ind)) ident = TRUE;
			if (do_res_stat(Ind, A_STR)) ident = TRUE;
			if (do_res_stat(Ind, A_INT)) ident = TRUE;
			if (do_res_stat(Ind, A_WIS)) ident = TRUE;
			if (do_res_stat(Ind, A_DEX)) ident = TRUE;
			if (do_res_stat(Ind, A_CON)) ident = TRUE;
			if (do_res_stat(Ind, A_CHR)) ident = TRUE;
			o_ptr->pval = 999;
			break;
		}

		case SV_ROD_SPEED:
		{
			if (!p_ptr->fast)
			{
				if (set_fast(Ind, randint(30) + 15)) ident = TRUE;
			}
			else
			{
				(void)set_fast(Ind, p_ptr->fast + 5);
			}
			o_ptr->pval = 99;
			break;
		}

		case SV_ROD_HAVOC:
		{
			call_chaos(Ind, dir);
			ident = TRUE;
			o_ptr->pval = 90;
			break;
		}
		case SV_ROD_NOTHING:
		{
			break;
		}

		default:
		{
			msg_print(Ind, "SERVER ERROR: Tried to zap non-directional rod in directional function!");
			return;
		}
	}


	/* Clear the current rod */
	p_ptr->current_rod = -1;

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Tried the object */
	object_tried(Ind, o_ptr);

	/* Successfully determined the object function */
	if (ident && !object_aware_p(Ind, o_ptr))
	{
		object_aware(Ind, o_ptr);
		gain_exp(Ind, (lev + (p_ptr->lev >> 1)) / p_ptr->lev);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Hack -- deal with cancelled zap */
	if (!use_charge)
	{
		o_ptr->pval = 0;
		return;
	}


	/* XXX Hack -- unstack if necessary */
	if ((item >= 0) && (o_ptr->number > 1))
	{
		/* Make a fake item */
		object_type tmp_obj;
		tmp_obj = *o_ptr;
		tmp_obj.number = 1;

		/* Restore "charge" */
		o_ptr->pval = 0;

		/* Unstack the used item */
		o_ptr->number--;
		p_ptr->total_weight -= tmp_obj.weight;
		item = inven_carry(Ind, &tmp_obj);

		/* Message */
		msg_print(Ind, "You unstack your rod.");
	}
}



/*
 * Hook to determine if an object is activatable
 */
static bool item_tester_hook_activate(int Ind, object_type *o_ptr)
{
			  u32b f1, f2, f3, f4, f5, esp;

	/* Not known */
	if (!object_known_p(Ind, o_ptr)) return (FALSE);

			  /* Extract the flags */
			  object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &esp);

	/* Check activation flag */
	if (f3 & TR3_ACTIVATE) return (TRUE);

	/* Assume not */
	return (FALSE);
}



/*
 * Hack -- activate the ring of power
 */
static void ring_of_power(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];

	/* Pick a random effect */
	switch (randint(10))
	{
		case 1:
		case 2:
		{
			/* Message */
			msg_print(Ind, "You are surrounded by a malignant aura.");

			/* Decrease all stats (permanently) */
			(void)dec_stat(Ind, A_STR, 50, STAT_DEC_NORMAL);
			(void)dec_stat(Ind, A_INT, 50, STAT_DEC_NORMAL);
			(void)dec_stat(Ind, A_WIS, 50, STAT_DEC_NORMAL);
			(void)dec_stat(Ind, A_DEX, 50, STAT_DEC_NORMAL);
			(void)dec_stat(Ind, A_CON, 50, STAT_DEC_NORMAL);
			(void)dec_stat(Ind, A_CHR, 50, STAT_DEC_NORMAL);

			/* Lose some experience (permanently) */
			take_xp_hit(Ind, p_ptr->exp / 4, "Ring of Power", TRUE, FALSE);
#if 0
			p_ptr->exp -= (p_ptr->exp / 4);
			p_ptr->max_exp -= (p_ptr->exp / 4);
			check_experience(Ind);
#endif	// 0

			break;
		}

		case 3:
		{
			/* Message */
			msg_print(Ind, "You are surrounded by a powerful aura.");

			/* Dispel monsters */
			dispel_monsters(Ind, 1000);

			break;
		}

		case 4:
		case 5:
		case 6:
		{
			/* Mana Ball */
			fire_ball(Ind, GF_MANA, dir, 300, 3);

			break;
		}

		case 7:
		case 8:
		case 9:
		case 10:
		{
			/* Mana Bolt */
			fire_bolt(Ind, GF_MANA, dir, 250);

			break;
		}
	}
}




/*
 * Enchant some bolts
 */
static bool brand_bolts(int Ind)
{
	player_type *p_ptr = Players[Ind];
	int i;

	/* Use the first (XXX) acceptable bolts */
	for (i = 0; i < INVEN_PACK; i++)
	{
		object_type *o_ptr = &p_ptr->inventory[i];

		/* Skip non-bolts */
		if (o_ptr->tval != TV_BOLT) continue;

		/* Skip artifacts and ego-items */
		if (artifact_p(o_ptr) || ego_item_p(o_ptr)) continue;

		/* Skip cursed/broken items */
		if (cursed_p(o_ptr) || broken_p(o_ptr)) continue;

		/* Randomize */
		if (rand_int(100) < 75) continue;

		/* Message */
		msg_print(Ind, "Your bolts are covered in a fiery aura!");

		/* Ego-item */
		o_ptr->name2 = EGO_FLAME;

		/* Enchant */
		enchant(Ind, o_ptr, rand_int(3) + 4, ENCH_TOHIT | ENCH_TODAM);

		/* Hack -- you don't sell the wep blessed by your god, do you? :) */
		o_ptr->discount = 100;

		/* Notice */
		return (TRUE);
	}

	/* Fail */
	msg_print(Ind, "The fiery enchantment failed.");

	/* Notice */
	return (TRUE);
}


/*
 * Activate a wielded object.  Wielded objects never stack.
 * And even if they did, activatable objects never stack.
 *
 * Currently, only (some) artifacts, and Dragon Scale Mail, can be activated.
 * But one could, for example, easily make an activatable "Ring of Plasma".
 *
 * Note that it always takes a turn to activate an artifact, even if
 * the user hits "escape" at the "direction" prompt.
 */
void do_cmd_activate(int Ind, int item)
{
        u32b f1, f2, f3, f4, f5, esp;
	player_type *p_ptr = Players[Ind];

	int         i, k, lev, chance;

	object_type *o_ptr;


	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	if( check_guard_inscription( o_ptr->note, 'A' ))
	{
		msg_print(Ind, "The item's inscription prevents it");
		return;
	} 

	if (!can_use(Ind, o_ptr))
	{
		msg_print(Ind, "You are not high level enough.");
		return;
	}

	/* Test the item */
	if (!item_tester_hook_activate(Ind, o_ptr))
	{
		msg_print(Ind, "You cannot activate that item.");
		return;
	}

	/* Take a turn */
	p_ptr->energy -= level_speed(&p_ptr->wpos);

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;

	/* Hack -- use artifact level instead */
	if (artifact_p(o_ptr)) lev = a_info[o_ptr->name1].level;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - ((lev > 50) ? 50 : lev);

        /* Extract object flags */
        object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &esp);

        /* Is it simple to use ? */
        if (f4 & TR4_EASY_USE)
        {
                chance *= 10;
        }

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && (rand_int(USE_DEVICE - chance + 1) == 0))
	{
		chance = USE_DEVICE;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint(chance) < USE_DEVICE))
	{
		msg_print(Ind, "You failed to activate it properly.");
		return;
	}

	/* Check the recharge */
	if (o_ptr->timeout)
	{
		msg_print(Ind, "It whines, glows and fades...");
		return;
	}

	process_hooks(HOOK_ACTIVATE, "d", Ind);

	/* Wonder Twin Powers... Activate! */
	msg_print(Ind, "You activate it...");

	/* Hack -- Dragon Scale Mail can be activated as well */
	/* Yikes, hard-coded r_idx.. */
	if (o_ptr->tval == TV_DRAG_ARMOR)
	{
		switch (o_ptr->sval)
		{
			case SV_DRAGON_BLACK:
			{
				//			do_mimic_change(Ind, 429);
				do_mimic_change(Ind, race_index("Ancient black dragon"));
				break;
			}
			case SV_DRAGON_BLUE:
			{
				//		      do_mimic_change(Ind, 411);
				do_mimic_change(Ind, race_index("Ancient blue dragon"));
				break;
			}
			case SV_DRAGON_WHITE:
			{
				//			do_mimic_change(Ind, 424);
				do_mimic_change(Ind, race_index("Ancient white dragon"));
				break;
			}
			case SV_DRAGON_RED:
			{
				//			do_mimic_change(Ind, 444);
				do_mimic_change(Ind, race_index("Ancient red dragon"));
				break;
			}
			case SV_DRAGON_GREEN:
			{
				//			do_mimic_change(Ind, 425);
				do_mimic_change(Ind, race_index("Ancient green dragon"));
				break;
			}
			case SV_DRAGON_MULTIHUED:
			{
				//			do_mimic_change(Ind, 462);
				do_mimic_change(Ind, race_index("Ancient multi-hued dragon"));
				break;
			}
			case SV_DRAGON_SHINING:
			{
				//			do_mimic_change(Ind, 463);
				do_mimic_change(Ind, race_index("Ethereal dragon"));
				break;
			}
			case SV_DRAGON_LAW:
			{
				//			do_mimic_change(Ind, 520);
				do_mimic_change(Ind, race_index("Great Wyrm of Law"));
				break;
			}
			case SV_DRAGON_BRONZE:
			{
				//			do_mimic_change(Ind, 412);
				do_mimic_change(Ind, race_index("Ancient bronze dragon"));
				break;
			}
			case SV_DRAGON_GOLD:
			{
				//			do_mimic_change(Ind, 445);
				do_mimic_change(Ind, race_index("Ancient gold dragon"));
				break;
			}
			case SV_DRAGON_CHAOS:
			{
				//			do_mimic_change(Ind, 519);
				do_mimic_change(Ind, race_index("Great Wyrm of Chaos"));
				break;
			}
			case SV_DRAGON_BALANCE:
			{
				//			do_mimic_change(Ind, 521);
				do_mimic_change(Ind, race_index("Great Wyrm of Balance"));
				break;
			}
			case SV_DRAGON_POWER:
			{
				//			do_mimic_change(Ind, 549);
				do_mimic_change(Ind, race_index("Great Wyrm of Power"));
				break;
			}
		}
		o_ptr->timeout = 200 + rand_int(100);
		return;
	}

	if (o_ptr->tval == TV_GOLEM)
	{
		int m_idx = 0, k;
		monster_type *m_ptr;

		/* Process the monsters */
		for (k = m_top - 1; k >= 0; k--)
		{
			/* Access the index */
			i = m_fast[k];

			/* Access the monster */
			m_ptr = &m_list[i];

			/* Excise "dead" monsters */
			if (!m_ptr->r_idx) continue;

			if (m_ptr->owner != p_ptr->id) continue;

			m_idx = i;
			if (!m_idx) continue;
			m_ptr = &m_list[m_idx];

			if (!(m_ptr->r_ptr->extra & (1 << (o_ptr->sval - 200))))
			{
				msg_print(Ind, "I do not understand, master.");
				continue;
			}

			switch (o_ptr->sval)
			{
				case SV_GOLEM_ATTACK:
					if (m_ptr->mind & (1 << (o_ptr->sval - 200)))
					{
						msg_print(Ind, "I wont attack your target anymore, master.");
						m_ptr->mind &= ~(1 << (o_ptr->sval - 200));
					}
					else
					{
						msg_print(Ind, "I will attack your target, master.");
						m_ptr->mind |= (1 << (o_ptr->sval - 200));
					}
					break;
				case SV_GOLEM_FOLLOW:
					if (m_ptr->mind & (1 << (o_ptr->sval - 200)))
					{
						msg_print(Ind, "I wont follow you, master.");
						m_ptr->mind &= ~(1 << (o_ptr->sval - 200));
					}
					else
					{
						msg_print(Ind, "I will follow you, master.");
						m_ptr->mind |= (1 << (o_ptr->sval - 200));
					}
					break;
				case SV_GOLEM_GUARD:
					if (m_ptr->mind & (1 << (o_ptr->sval - 200)))
					{
						msg_print(Ind, "I wont guard my position anymore, master.");
						m_ptr->mind &= ~(1 << (o_ptr->sval - 200));
					}
					else
					{
						msg_print(Ind, "I will guard my position, master.");
						m_ptr->mind |= (1 << (o_ptr->sval - 200));
					}
					break;
			}
		}
		return;
	}

#if 0
	/* Artifacts activate by name */
	if (o_ptr->name1)
	{
		/* This needs to be changed */
		switch (o_ptr->name1)
		{
			case ART_NARTHANC:
			{
				msg_print(Ind, "Your dagger is covered in fire...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_NIMTHANC:
			{
				msg_print(Ind, "Your dagger is covered in frost...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_DETHANC:
			{
				msg_print(Ind, "Your dagger is covered in sparks...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_RILIA:
			{
				msg_print(Ind, "Your dagger throbs deep green...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_BELANGIL:
			{
				msg_print(Ind, "Your dagger is covered in frost...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_DAL:
			{
				msg_print(Ind, "\377GYou feel energy flow through your feet...");
				(void)set_afraid(Ind, 0);
				(void)set_poisoned(Ind, 0);
				o_ptr->timeout = 5;
				break;
			}

			case ART_RINGIL:
			{
				msg_print(Ind, "Your sword glows an intense blue...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_ANDURIL:
			{
				msg_print(Ind, "Your sword glows an intense red...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_FIRESTAR:
			{
				msg_print(Ind, "Your morningstar rages in fire...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_FEANOR:
			{
				if (!p_ptr->fast)
				{
					(void)set_fast(Ind, randint(20) + 20);
				}
				else
				{
					(void)set_fast(Ind, p_ptr->fast + 5);
				}
				o_ptr->timeout = 200;
				break;
			}

			case ART_THEODEN:
			{
				msg_print(Ind, "The blade of your axe glows black...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_TURMIL:
			{
				msg_print(Ind, "The head of your hammer glows white...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_CASPANION:
			{
				msg_print(Ind, "Your armor glows bright red...");
				destroy_doors_touch(Ind);
				o_ptr->timeout = 10;
				break;
			}

			case ART_AVAVIR:
			{
				if (p_ptr->word_recall == 0)
				{
					set_recall_depth(p_ptr, o_ptr);
					p_ptr->word_recall = randint(20) + 15;
					msg_print(Ind, "\377oThe air about you becomes charged...");
				}
				else
				{
					p_ptr->word_recall = 0;
					msg_print(Ind, "\377oA tension leaves the air around you...");
				}
				o_ptr->timeout = 200;
				break;
			}

			case ART_TARATOL:
			{
				if (!p_ptr->fast)
				{
					(void)set_fast(Ind, randint(20) + 20);
				}
				else
				{
					(void)set_fast(Ind, p_ptr->fast + 5);
				}
				o_ptr->timeout = rand_int(100) + 100;
				break;
			}

			case ART_ERIRIL:
			{
				/* Identify and combine pack */
				(void)ident_spell(Ind);
				/* XXX Note that the artifact is always de-charged */
				o_ptr->timeout = 10;
				break;
			}

			case ART_OLORIN:
			{
				probing(Ind);
				o_ptr->timeout = 20;
				break;
			}

			case ART_EONWE:
			{
				msg_print(Ind, "Your axe lets out a long, shrill note...");
				(void)mass_genocide(Ind);
				o_ptr->timeout = 1000;
				break;
			}

			case ART_LOTHARANG:
			{
				msg_print(Ind, "Your battle axe radiates deep purple...");
				hp_player(Ind, damroll(4, 8));
				(void)set_cut(Ind, (p_ptr->cut / 2) - 50);
				o_ptr->timeout = rand_int(3) + 3;
				break;
			}

			case ART_CUBRAGOL:
			{
				(void)brand_bolts(Ind);
				o_ptr->timeout = 999;
				break;
			}

			case ART_ARUNRUTH:
			{
				msg_print(Ind, "Your sword glows a pale blue...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_AEGLOS:
			{
				msg_print(Ind, "Your spear glows a bright white...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_OROME:
			{
				msg_print(Ind, "Your spear pulsates...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_SOULKEEPER:
			{
				msg_print(Ind, "Your armor glows a bright white...");
				msg_print(Ind, "\377GYou feel much better...");
				(void)hp_player(Ind, 1000);
				(void)set_cut(Ind, 0);
				o_ptr->timeout = 888;
				break;
			}

			case ART_BELEGENNON:
			{
				teleport_player(Ind, 10);
				o_ptr->timeout = 2;
				break;
			}

			case ART_CELEBORN:
			{
				(void)genocide(Ind);
				o_ptr->timeout = 500;
				break;
			}

			case ART_LUTHIEN:
			{
				restore_level(Ind);
				o_ptr->timeout = 450;
				break;
			}

			case ART_ULMO:
			{
				msg_print(Ind, "Your trident glows deep red...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_COLLUIN:
			{
				msg_print(Ind, "Your cloak glows many colours...");
				(void)set_oppose_acid(Ind, p_ptr->oppose_acid + randint(20) + 20);
				(void)set_oppose_elec(Ind, p_ptr->oppose_elec + randint(20) + 20);
				(void)set_oppose_fire(Ind, p_ptr->oppose_fire + randint(20) + 20);
				(void)set_oppose_cold(Ind, p_ptr->oppose_cold + randint(20) + 20);
				(void)set_oppose_pois(Ind, p_ptr->oppose_pois + randint(20) + 20);
				o_ptr->timeout = 111;
				break;
			}

			case ART_HOLCOLLETH:
			{
				msg_print(Ind, "Your cloak glows deep blue...");
				sleep_monsters_touch(Ind);
				o_ptr->timeout = 55;
				break;
			}

			case ART_THINGOL:
			{
				msg_print(Ind, "You hear a low humming noise...");
				recharge(Ind, 60);
				o_ptr->timeout = 70;
				break;
			}

			case ART_COLANNON:
			{
				teleport_player(Ind, 100);
				o_ptr->timeout = 45;
				break;
			}

			case ART_TOTILA:
			{
				msg_print(Ind, "Your flail glows in scintillating colours...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_CAMMITHRIM:
			{
				msg_print(Ind, "Your gloves glow extremely brightly...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_PAURHACH:
			{
				msg_print(Ind, "Your gauntlets are covered in fire...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_PAURNIMMEN:
			{
				msg_print(Ind, "Your gauntlets are covered in frost...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_PAURAEGEN:
			{
				msg_print(Ind, "Your gauntlets are covered in sparks...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_PAURNEN:
			{
				msg_print(Ind, "Your gauntlets look very acidic...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_FINGOLFIN:
			{
				msg_print(Ind, "Magical spikes appear on your cesti...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_HOLHENNETH:
			{
				msg_print(Ind, "An image forms in your mind...");
				detection(Ind, DEFAULT_RADIUS * 2);
				o_ptr->timeout = rand_int(55) + 55;
				break;
			}

			case ART_GONDOR:
			{
				msg_print(Ind, "\377GYou feel a warm tingling inside...");
				(void)hp_player(Ind, 500);
				(void)set_cut(Ind, 0);
				o_ptr->timeout = 500;
				break;
			}

			case ART_RAZORBACK:
			{
				msg_print(Ind, "You are surrounded by lightning!");
				for (i = 0; i < 8; i++) fire_ball(Ind, GF_ELEC, ddd[i], 150, 3);
				o_ptr->timeout = 1000;
				break;
			}

			case ART_BLADETURNER:
			{
				msg_print(Ind, "Your armor glows many colours...");
				(void)hp_player(Ind, 30);
				(void)set_afraid(Ind, 0);
				(void)set_shero(Ind, p_ptr->shero + randint(50) + 50);
				(void)set_blessed(Ind, p_ptr->blessed + randint(50) + 50);
				(void)set_oppose_acid(Ind, p_ptr->oppose_acid + randint(50) + 50);
				(void)set_oppose_elec(Ind, p_ptr->oppose_elec + randint(50) + 50);
				(void)set_oppose_fire(Ind, p_ptr->oppose_fire + randint(50) + 50);
				(void)set_oppose_cold(Ind, p_ptr->oppose_cold + randint(50) + 50);
				(void)set_oppose_pois(Ind, p_ptr->oppose_pois + randint(50) + 50);
				o_ptr->timeout = 400;
				break;
			}


			case ART_GALADRIEL:
			{
				msg_print(Ind, "The phial wells with clear light...");
				lite_area(Ind, damroll(2, 15), 3);
				o_ptr->timeout = rand_int(10) + 10;
				break;
			}

			case ART_ELENDIL:
			{
				msg_print(Ind, "The star shines brightly...");
				map_area(Ind);
				o_ptr->timeout = rand_int(50) + 50;
				break;
			}

			case ART_THRAIN:
			{
				msg_print(Ind, "The stone glows a deep green...");
				wiz_lite(Ind);
				(void)detect_sdoor(Ind, DEFAULT_RADIUS * 2);
				(void)detect_trap(Ind, DEFAULT_RADIUS * 2);
				o_ptr->timeout = rand_int(100) + 100;
				break;
			}


			case ART_INGWE:
			{
				msg_print(Ind, "An aura of good floods the area...");
				dispel_evil(Ind, p_ptr->lev * 5);
				o_ptr->timeout = rand_int(300) + 300;
				break;
			}

			case ART_CARLAMMAS:
			{
				msg_print(Ind, "The amulet lets out a shrill wail...");
				k = 3 * p_ptr->lev;
				(void)set_protevil(Ind, p_ptr->protevil + randint(25) + k);
				o_ptr->timeout = rand_int(225) + 225;
				break;
			}


			case ART_TULKAS:
			{
				msg_print(Ind, "The ring glows brightly...");
				if (!p_ptr->fast)
				{
					(void)set_fast(Ind, randint(75) + 75);
				}
				else
				{
					(void)set_fast(Ind, p_ptr->fast + 5);
				}
				o_ptr->timeout = rand_int(150) + 150;
				break;
			}

			case ART_NARYA:
			{
				msg_print(Ind, "The ring glows deep red...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_NENYA:
			{
				msg_print(Ind, "The ring glows bright white...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_VILYA:
			{
				msg_print(Ind, "The ring glows deep blue...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_POWER:
			{
				msg_print(Ind, "The ring glows intensely black...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}
		}

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return;
	}
#endif	// 0

	/* Artifacts */
	if (o_ptr->name1 && (o_ptr->name1 != ART_RANDART))
	{
		/* Choose effect */
		switch (o_ptr->name1)
		{
			case ART_NARTHANC:
			{
				msg_print(Ind, "Your dagger is covered in fire...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_NIMTHANC:
			{
				msg_print(Ind, "Your dagger is covered in frost...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_DETHANC:
			{
				msg_print(Ind, "Your dagger is covered in sparks...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_RILIA:
			{
				msg_print(Ind, "Your dagger throbs deep green...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_BELANGIL:
			{
				msg_print(Ind, "Your dagger is covered in frost...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_DAL:
			{
				msg_print(Ind, "\377GYou feel energy flow through your feet...");
				(void)set_afraid(Ind, 0);
				(void)set_poisoned(Ind, 0);
				o_ptr->timeout = 5;
				break;
			}

			case ART_RINGIL:
			{
				msg_print(Ind, "Your sword glows an intense blue...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_ANDURIL:
			{
				msg_print(Ind, "Your sword glows an intense red...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_FIRESTAR:
			{
				msg_print(Ind, "Your morningstar rages in fire...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_FEANOR:
			{
				if (!p_ptr->fast)
				{
					(void)set_fast(Ind, randint(20) + 20);
				}
				else
				{
					(void)set_fast(Ind, p_ptr->fast + 5);
				}
				o_ptr->timeout = 200;
				break;
			}

			case ART_THEODEN:
			{
				msg_print(Ind, "The blade of your axe glows black...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_TURMIL:
			{
				msg_print(Ind, "The head of your hammer glows white...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_CASPANION:
			{
				msg_print(Ind, "Your armor glows bright red...");
				destroy_doors_touch(Ind);
				o_ptr->timeout = 10;
				break;
			}

			case ART_AVAVIR:
			{
				if (p_ptr->word_recall == 0)
				{
					set_recall_depth(p_ptr, o_ptr);
					p_ptr->word_recall = randint(20) + 15;
					msg_print(Ind, "\377oThe air about you becomes charged...");
				}
				else
				{
					p_ptr->word_recall = 0;
					msg_print(Ind, "\377oA tension leaves the air around you...");
				}
				o_ptr->timeout = 200;
				break;
			}

			case ART_TARATOL:
			{
				if (!p_ptr->fast)
				{
					(void)set_fast(Ind, randint(20) + 20);
				}
				else
				{
					(void)set_fast(Ind, p_ptr->fast + 5);
				}
				o_ptr->timeout = rand_int(100) + 100;
				break;
			}

			case ART_ERIRIL:
			{
				/* Identify and combine pack */
				(void)ident_spell(Ind);
				/* XXX Note that the artifact is always de-charged */
				o_ptr->timeout = 10;
				break;
			}

			case ART_OLORIN:
			{
				probing(Ind);
				o_ptr->timeout = 20;
				break;
			}

			case ART_EONWE:
			{
				msg_print(Ind, "Your axe lets out a long, shrill note...");
				(void)mass_genocide(Ind);
				o_ptr->timeout = 1000;
				break;
			}

			case ART_LOTHARANG:
			{
				msg_print(Ind, "Your battle axe radiates deep purple...");
				hp_player(Ind, damroll(4, 8));
				(void)set_cut(Ind, (p_ptr->cut / 2) - 50);
				o_ptr->timeout = rand_int(3) + 3;
				break;
			}

			case ART_CUBRAGOL:
			{
				(void)brand_bolts(Ind);
				o_ptr->timeout = 999;
				break;
			}

			case ART_ARUNRUTH:
			{
				msg_print(Ind, "Your sword glows a pale blue...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_AEGLOS:
			{
				msg_print(Ind, "Your spear glows a bright white...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_OROME:
			{
				msg_print(Ind, "Your spear pulsates...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_SOULKEEPER:
			{
				msg_print(Ind, "Your armor glows a bright white...");
				msg_print(Ind, "\377GYou feel much better...");
				(void)hp_player(Ind, 1000);
				(void)set_cut(Ind, 0);
				o_ptr->timeout = 888;
				break;
			}

			case ART_BELEGENNON:
			{
				teleport_player(Ind, 10);
				o_ptr->timeout = 2;
				break;
			}

			case ART_CELEBORN:
			{
				(void)genocide(Ind);
				o_ptr->timeout = 500;
				break;
			}

			case ART_LUTHIEN:
			{
				restore_level(Ind);
				o_ptr->timeout = 450;
				break;
			}

			case ART_ULMO:
			{
				msg_print(Ind, "Your trident glows deep red...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_COLLUIN:
			{
				msg_print(Ind, "Your cloak glows many colours...");
				(void)set_oppose_acid(Ind, p_ptr->oppose_acid + randint(20) + 20);
				(void)set_oppose_elec(Ind, p_ptr->oppose_elec + randint(20) + 20);
				(void)set_oppose_fire(Ind, p_ptr->oppose_fire + randint(20) + 20);
				(void)set_oppose_cold(Ind, p_ptr->oppose_cold + randint(20) + 20);
				(void)set_oppose_pois(Ind, p_ptr->oppose_pois + randint(20) + 20);
				o_ptr->timeout = 111;
				break;
			}

			case ART_HOLCOLLETH:
			{
				msg_print(Ind, "Your cloak glows deep blue...");
				sleep_monsters_touch(Ind);
				o_ptr->timeout = 55;
				break;
			}

			case ART_THINGOL:
			{
				msg_print(Ind, "You hear a low humming noise...");
				recharge(Ind, 60);
				o_ptr->timeout = 70;
				break;
			}

			case ART_COLANNON:
			{
				teleport_player(Ind, 100);
				o_ptr->timeout = 45;
				break;
			}

			case ART_TOTILA:
			{
				msg_print(Ind, "Your flail glows in scintillating colours...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_CAMMITHRIM:
			{
				msg_print(Ind, "Your gloves glow extremely brightly...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_PAURHACH:
			{
				msg_print(Ind, "Your gauntlets are covered in fire...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_PAURNIMMEN:
			{
				msg_print(Ind, "Your gauntlets are covered in frost...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_PAURAEGEN:
			{
				msg_print(Ind, "Your gauntlets are covered in sparks...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_PAURNEN:
			{
				msg_print(Ind, "Your gauntlets look very acidic...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_FINGOLFIN:
			{
				msg_print(Ind, "Magical spikes appear on your cesti...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_HOLHENNETH:
			{
				msg_print(Ind, "An image forms in your mind...");
				detection(Ind, DEFAULT_RADIUS * 2);
				o_ptr->timeout = rand_int(55) + 55;
				break;
			}

			case ART_GONDOR:
			{
				msg_print(Ind, "\377GYou feel a warm tingling inside...");
				(void)hp_player(Ind, 500);
				(void)set_cut(Ind, 0);
				o_ptr->timeout = 500;
				break;
			}

			case ART_RAZORBACK:
			{
				msg_print(Ind, "You are surrounded by lightning!");
				for (i = 0; i < 8; i++) fire_ball(Ind, GF_ELEC, ddd[i], 150, 3);
				o_ptr->timeout = 1000;
				break;
			}

			case ART_BLADETURNER:
			{
				msg_print(Ind, "Your armor glows many colours...");
				(void)hp_player(Ind, 30);
				(void)set_afraid(Ind, 0);
				(void)set_shero(Ind, p_ptr->shero + randint(50) + 50);
				(void)set_blessed(Ind, p_ptr->blessed + randint(50) + 50);
				(void)set_oppose_acid(Ind, p_ptr->oppose_acid + randint(50) + 50);
				(void)set_oppose_elec(Ind, p_ptr->oppose_elec + randint(50) + 50);
				(void)set_oppose_fire(Ind, p_ptr->oppose_fire + randint(50) + 50);
				(void)set_oppose_cold(Ind, p_ptr->oppose_cold + randint(50) + 50);
				(void)set_oppose_pois(Ind, p_ptr->oppose_pois + randint(50) + 50);
				o_ptr->timeout = 400;
				break;
			}


			case ART_GALADRIEL:
			{
				msg_print(Ind, "The phial wells with clear light...");
				lite_area(Ind, damroll(2, 15), 3);
				o_ptr->timeout = rand_int(10) + 10;
				break;
			}

			case ART_ELENDIL:
			{
				msg_print(Ind, "The star shines brightly...");
				map_area(Ind);
				o_ptr->timeout = rand_int(50) + 50;
				break;
			}

			case ART_THRAIN:
			{
				msg_print(Ind, "The stone glows a deep green...");
				wiz_lite(Ind);
				(void)detect_sdoor(Ind, DEFAULT_RADIUS * 2);
				(void)detect_trap(Ind, DEFAULT_RADIUS * 2);
				o_ptr->timeout = rand_int(100) + 100;
				break;
			}


			case ART_INGWE:
			{
				msg_print(Ind, "An aura of good floods the area...");
				dispel_evil(Ind, p_ptr->lev * 5);
				o_ptr->timeout = rand_int(300) + 300;
				break;
			}

			case ART_CARLAMMAS:
			{
				msg_print(Ind, "The amulet lets out a shrill wail...");
				k = 3 * p_ptr->lev;
				(void)set_protevil(Ind, p_ptr->protevil + randint(25) + k);
				o_ptr->timeout = rand_int(225) + 225;
				break;
			}


			case ART_TULKAS:
			{
				msg_print(Ind, "The ring glows brightly...");
				if (!p_ptr->fast)
				{
					(void)set_fast(Ind, randint(75) + 75);
				}
				else
				{
					(void)set_fast(Ind, p_ptr->fast + 5);
				}
				o_ptr->timeout = rand_int(150) + 150;
				break;
			}

			case ART_NARYA:
			{
				msg_print(Ind, "The ring glows deep red...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_NENYA:
			{
				msg_print(Ind, "The ring glows bright white...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_VILYA:
			{
				msg_print(Ind, "The ring glows deep blue...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}

			case ART_POWER:
			{
				msg_print(Ind, "The ring glows intensely black...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}
			case ART_GILGALAD:
			{
				for (k = 1; k < 10; k++)
				{
					if (k - 5) fire_beam(Ind, GF_LITE, k, 75);
				}

				o_ptr->timeout = rand_int(75) + 75;
				break;
			}

			case ART_CELEBRIMBOR:
			{
				set_tim_esp(Ind, p_ptr->tim_esp + randint(20) + 20);

				o_ptr->timeout = rand_int(50) + 20;
				break;
			}

			case ART_SKULLCLEAVER:
			{
				destroy_area(&p_ptr->wpos, p_ptr->py, p_ptr->px, 15, TRUE, FEAT_FLOOR);
				o_ptr->timeout = rand_int(200) + 200;
				break;
			}

			case ART_HARADRIM:
			{
				set_afraid(Ind, 0);
				set_shero(Ind, p_ptr->shero + randint(25) + 25);
				hp_player(Ind, 30);
				o_ptr->timeout = rand_int(50) + 50;
				break;
			}

			case ART_FUNDIN:
			{
				dispel_evil(Ind, p_ptr->lev * 4);
				o_ptr->timeout = rand_int(100) + 100;
				break;
			}

			case ART_NAIN:
			case ART_EOL:
			case ART_UMBAR:
			{
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}


#if 0
			case ART_NUMENOR:
			{
				/* Give full knowledge */
				/* Hack -- Maximal info */
				monster_race *r_ptr;
				cave_type *c_ptr;
				int x, y, m;

				if (!tgt_pt(&x, &y)) break;

				c_ptr = &cave[y][x];
				if (!c_ptr->m_idx) break;

				r_ptr = &r_info[c_ptr->m_idx];

				/* Observe "maximal" attacks */
				for (m = 0; m < 4; m++)
				{
					/* Examine "actual" blows */
					if (r_ptr->blow[m].effect || r_ptr->blow[m].method)
					{
						/* Hack -- maximal observations */
						r_ptr->r_blows[m] = MAX_UCHAR;
					}
				}

				/* Hack -- maximal drops */
				r_ptr->r_drop_gold = r_ptr->r_drop_item =
					(((r_ptr->flags1 & (RF1_DROP_4D2)) ? 8 : 0) +
					 ((r_ptr->flags1 & (RF1_DROP_3D2)) ? 6 : 0) +
					 ((r_ptr->flags1 & (RF1_DROP_2D2)) ? 4 : 0) +
					 ((r_ptr->flags1 & (RF1_DROP_1D2)) ? 2 : 0) +
					 ((r_ptr->flags1 & (RF1_DROP_90))  ? 1 : 0) +
					 ((r_ptr->flags1 & (RF1_DROP_60))  ? 1 : 0));

				/* Hack -- but only "valid" drops */
				if (r_ptr->flags1 & (RF1_ONLY_GOLD)) r_ptr->r_drop_item = 0;
				if (r_ptr->flags1 & (RF1_ONLY_ITEM)) r_ptr->r_drop_gold = 0;

				/* Hack -- observe many spells */
				r_ptr->r_cast_inate = MAX_UCHAR;
				r_ptr->r_cast_spell = MAX_UCHAR;

				/* Hack -- know all the flags */
				r_ptr->r_flags1 = r_ptr->flags1;
				r_ptr->r_flags2 = r_ptr->flags2;
				r_ptr->r_flags3 = r_ptr->flags3;
				r_ptr->r_flags4 = r_ptr->flags4;
				r_ptr->r_flags5 = r_ptr->flags5;
				r_ptr->r_flags6 = r_ptr->flags6;
				r_ptr->r_flags7 = r_ptr->flags7;
				r_ptr->r_flags8 = r_ptr->flags8;
				r_ptr->r_flags9 = r_ptr->flags9;

				o_ptr->timeout = rand_int(200) + 500;
				break;
			}
#endif	// 0

			case ART_KNOWLEDGE:
			{
				identify_fully(Ind);
				//                                take_sanity_hit(damroll(10, 7), "the sounds of deads");
				take_hit(Ind, damroll(10, 7), "the sounds of deads");
				o_ptr->timeout = rand_int(200) + 100;
				break;
			}


			case ART_UNDEATH:
			{
				msg_print(Ind, "The phial wells with dark light...");
				unlite_area(Ind, damroll(2, 15), 3);
				take_hit(Ind, damroll(10, 10), "activating The Phial of Undeath");
				(void)dec_stat(Ind, A_DEX, 25, STAT_DEC_PERMANENT);
				(void)dec_stat(Ind, A_WIS, 25, STAT_DEC_PERMANENT);
				(void)dec_stat(Ind, A_CON, 25, STAT_DEC_PERMANENT);
				(void)dec_stat(Ind, A_STR, 25, STAT_DEC_PERMANENT);
				(void)dec_stat(Ind, A_CHR, 25, STAT_DEC_PERMANENT);
				(void)dec_stat(Ind, A_INT, 25, STAT_DEC_PERMANENT);
				o_ptr->timeout = rand_int(10) + 10;
				break;
			}

			case ART_HIMRING:
			{
				k = 3 * p_ptr->lev;
				(void)set_protevil(Ind, p_ptr->protevil + randint(25) + k);
				o_ptr->timeout = rand_int(225) + 225;
				break;
			}

#if 0
			case ART_FLAR:
			{
				if (dungeon_flags1 & LF1_NO_TELEPORT)
				{
					msg_print(Ind, "No teleport on special levels...");
					break;
				}

				if (dungeon_flags1 & LF1_NO_TELEPORT)
				{
					msg_print(Ind, "Not on special levels!");
					break;
				}

				msg_print(Ind, "You open a between gate. Choose a destination.");
				if (!tgt_pt(&ii,&ij)) return;
				p_ptr->energy -= 60 - plev;
				if (!cave_empty_bold(ij,ii) || (cave[ij][ii].info & CAVE_ICKY) ||
						(distance(ij,ii,py,px) > plev + 2) ||
						(!rand_int(plev * plev / 2)))
				{
					msg_print(Ind, "You fail to exit the between correctly!");
					p_ptr->energy -= 100;
					teleport_player(10);
				}
				else teleport_player_to(ij,ii);
				o_ptr->timeout = 100;
				break;
			}
#endif	// 0

			case ART_BARAHIR:
			{
				msg_print(Ind, "You exterminate small life.");
				(void)dispel_monsters(Ind, 4);
				o_ptr->timeout = rand_int(55) + 55;
				break;
			}


			/* The Stone of Lore is perilous, for the sake of game balance. */
			case ART_STONE_LORE:
			{
				msg_print(Ind, "The stone reveals hidden mysteries...");
				if (!ident_spell(Ind)) return;

				//                                if (!p_ptr->realm1)
				if (1)
				{
					/* Sufficient mana */
					if (20 <= p_ptr->csp)
					{
						/* Use some mana */
						p_ptr->csp -= 20;
					}

					/* Over-exert the player */
					else
					{
						int oops = 20 - p_ptr->csp;

						/* No mana left */
						p_ptr->csp = 0;
						p_ptr->csp_frac = 0;

						/* Message */
						msg_print(Ind, "You are too weak to control the stone!");

						/* Hack -- Bypass free action */
						(void)set_paralyzed(Ind, p_ptr->paralyzed +
											randint(5 * oops + 1));

						/* Confusing. */
						(void)set_confused(Ind, p_ptr->confused +
										   randint(5 * oops + 1));
					}

					/* Redraw mana */
					p_ptr->redraw |= (PR_MANA);
				}

				take_hit(Ind, damroll(1, 12), "perilous secrets");

				/* Confusing. */
				if (rand_int(5) == 0) (void)set_confused(Ind, p_ptr->confused +
						randint(10));

				/* Exercise a little care... */
				if (rand_int(20) == 0) take_hit(Ind, damroll(4, 10), "perilous secrets");
				o_ptr->timeout = 1;
				break;
			}



			case ART_MEDIATOR:
			{
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}


			case ART_DOR:
			case ART_GORLIM:
			{
				turn_monsters(Ind, 40 + p_ptr->lev);
				o_ptr->timeout = 3 * (p_ptr->lev + 10);
				break;
			}


			case ART_ANGUIREL:
			{
				switch(randint(13))
				{
					case 1: case 2: case 3: case 4: case 5:
						teleport_player(Ind, 10);
						break;
					case 6: case 7: case 8: case 9: case 10:
						teleport_player(Ind, 222);
						break;
					case 11: case 12:
					default:
						(void)stair_creation(Ind);
						break;
#if 0
					default:
						if(get_check("Leave this level? "))
						{
							if (autosave_l)
							{
								is_autosave = TRUE;
								msg_print(Ind, "Autosaving the game...");
								do_cmd_save_game();
								is_autosave = FALSE;
							}

							/* Leaving */
							p_ptr->leaving = TRUE;
						}
#endif	// 0
				}
				o_ptr->timeout = 35;
				break;
			}


			case ART_ERU:
			{
				msg_print(Ind, "Your sword glows an intense white...");
				hp_player(Ind, 7000);
				heal_insanity(Ind, 50);
				set_blind(Ind, 0);
				set_poisoned(Ind, 0);
				set_confused(Ind, 0);
				set_stun(Ind, 0);
				set_cut(Ind, 0);
				set_image(Ind, 0);
				o_ptr->timeout = 500;
				break;
			}

#if 0
			case ART_DAWN:
			{
				msg_print(Ind, "You summon the Legion of the Dawn.");
				(void)summon_specific_friendly(py, px, dlev, SUMMON_DAWN, TRUE);
				o_ptr->timeout = 500 + randint(500);
				break;
			}
#endif	// 0

#if 0
			case ART_AVAVIR:
			{
				if (dlev && (max_dlv[dungeon_type] > dlev))
				{
					if (get_check("Reset recall depth? "))
						max_dlv[dungeon_type] = dlev;
				}

				msg_print(Ind, "Your scythe glows soft white...");
				if (!p_ptr->word_recall)
				{
					p_ptr->word_recall = randint(20) + 15;
					msg_print(Ind, "The air about you becomes charged...");
				}
				else
				{
					p_ptr->word_recall = 0;
					msg_print(Ind, "A tension leaves the air around you...");
				}
				o_ptr->timeout = 200;
				break;
			}
#endif	// 0



			case ART_EVENSTAR:
			{
				restore_level(Ind);
				(void)do_res_stat(Ind, A_STR);
				(void)do_res_stat(Ind, A_DEX);
				(void)do_res_stat(Ind, A_CON);
				(void)do_res_stat(Ind, A_INT);
				(void)do_res_stat(Ind, A_WIS);
				(void)do_res_stat(Ind, A_CHR);
				o_ptr->timeout = 150;
				break;
			}

#if 1
			case ART_ELESSAR:
			{
				if (p_ptr->black_breath)
				{
					msg_print(Ind, "The hold of the Black Breath on you is broken!");
				}
				p_ptr->black_breath = FALSE;
				hp_player(Ind, 100);
				o_ptr->timeout = 200;
				break;
			}
#endif	// 0

			case ART_GANDALF:
			{
				msg_print(Ind, "Your mage staff glows deep blue...");
				if (p_ptr->csp < p_ptr->msp)
				{
					p_ptr->csp = p_ptr->msp;
					p_ptr->csp_frac = 0;
					msg_print(Ind, "Your feel your head clear.");
					p_ptr->redraw |= (PR_MANA);
					p_ptr->window |= (PW_PLAYER);
					p_ptr->window |= (PW_SPELL);
				}
				o_ptr->timeout = 666;
				break;
			}

#if 0
			case ART_MARDA:
			{
				if (randint(3) == 1)
				{
					if (summon_specific(py, px, ((plev * 3) / 2), SUMMON_DRAGONRIDER))
					{
						msg_print(Ind, "A DragonRider comes from the BETWEEN !");
						msg_print(Ind, "'I will burn you!'");
					}
				}
				else
				{
					if (summon_specific_friendly(py, px, ((plev * 3) / 2),
								SUMMON_DRAGONRIDER, (bool)(plev == 50 ? TRUE : FALSE)))
					{
						msg_print(Ind, "A DragonRider comes from the BETWEEN !");
						msg_print(Ind, "'I will help you in your difficult task.'");
					}
				}
				o_ptr->timeout = 1000;
				break;
			}
#endif	// 0

			case ART_PALANTIR_ITHIL:
			case ART_PALANTIR:
			{
				msg_print(Ind, "The stone glows a deep green...");
				wiz_lite_extra(Ind);
				(void)detect_trap(Ind, DEFAULT_RADIUS * 2);
				(void)detect_sdoor(Ind, DEFAULT_RADIUS * 2);
				//				(void)detect_stair(Ind);
				o_ptr->timeout = rand_int(100) + 100;
				break;
			}
#if 0
			case ART_ROBINTON:
			{
				msg_format(Ind, "Your instrument starts %s",music_info[3].desc);
				p_ptr->music = 3; /* Full ID */
				o_ptr->timeout = music_info[p_ptr->music].init_recharge;
				break;
			}
			case ART_PIEMUR:
			{
				msg_format(Ind, "Your instrument starts %s",music_info[9].desc);
				p_ptr->music = 9;
				o_ptr->timeout = music_info[p_ptr->music].init_recharge;
				break;
			}
			case ART_MENOLLY:
			{
				msg_format(Ind, "Your instrument starts %s",music_info[10].desc);
				p_ptr->music = 10;
				o_ptr->timeout = music_info[p_ptr->music].init_recharge;
				break;
			}
			case ART_EREBOR:
			{
				msg_print(Ind, "Your pick twists in your hands.");
				if (!get_aim_dir(Ind))
					return;
				if (passwall(dir, TRUE))
					msg_print(Ind, "A passage opens, and you step through.");
				else
					msg_print(Ind, "There is no wall there!");
				o_ptr->timeout = 75;
				break;
			}
			case ART_DRUEDAIN:
			{
				msg_print(Ind, "Your drum shows you the world.");
				detect_all();
				o_ptr->timeout = 99;
				break;
			}
			case ART_ROHAN:
			{
				msg_print(Ind, "Your horn glows deep red.");
				set_afraid(0);
				set_shero(Ind, p_ptr->shero + damroll(5,10) + 30);
				set_afraid(0);
				set_hero(Ind, p_ptr->hero + damroll(5,10) + 30);
				set_fast(Ind, p_ptr->fast + damroll(5,10) + 30);
				hp_player(30);
				o_ptr->timeout = 250;
				break;
			}
			case ART_HELM:
			{
				msg_print(Ind, "Your horn emits a loud sound.");
				if (!get_aim_dir(Ind)) return;
				fire_ball(GF_SOUND, dir, 300, 6);
				o_ptr->timeout = 300;
				break;
			}
			case ART_BOROMIR:
			{
				msg_print(Ind, "Your horn calls for help.");
				for(i = 0; i < 15; i++)
					summon_specific_friendly(py, px, ((plev * 3) / 2),SUMMON_HUMAN, TRUE);
				o_ptr->timeout = 1000;
				break;
			}
#endif	// 0
			case ART_HURIN:
			{
				if (!p_ptr->fast)
				{
					(void)set_fast(Ind, randint(50) + 50);
				}
				else
				{
					(void)set_fast(Ind, p_ptr->fast + 5);
				}
				hp_player(Ind, 30);
				set_afraid(Ind, 0);
				set_shero(Ind, p_ptr->shero + randint(50) + 50);
				o_ptr->timeout = rand_int(200) + 100;
				break;
			}
			case ART_AXE_GOTHMOG:
			{
				msg_print(Ind, "Your lochaber axe erupts in fire...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}
			case ART_MELKOR:
			{
				msg_print(Ind, "Your spear is covered of darkness...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}
#if 0
			case ART_GROND:
			{
				msg_print(Ind, "Your hammer hits the floor...");
				alter_reality();
				o_ptr->timeout = 100;
				break;
			}
			case ART_NATUREBANE:
			{
				msg_print(Ind, "Your axe glows blood red...");
				dispel_monsters(300);
				o_ptr->timeout = 200 + randint(200);
				break;
			}
#endif	// 0
			case ART_NIGHT:
			{
				msg_print(Ind, "Your axe emits a black aura...");
				p_ptr->current_activation = item;
				get_aim_dir(Ind);
				return;
			}
#if 0
			case ART_ORCHAST:
			{
				msg_print(Ind, "Your weapon glows brightly...");
				(void)detect_monsters_xxx(Ind, RF3_ORC);
				o_ptr->timeout = 10;
				break;
			}
#endif	// 0
			/* ToNE-NET additions */
			case ART_BILBO:
			{
				msg_print(Ind, "Your picklock flashes...");
				destroy_doors_touch(Ind);
				o_ptr->timeout = 30 + randint(30);
				return;
			}
		}

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

                if (o_ptr->timeout) return;
	}

	/* ego activation etc */
	else if (is_ego_p(o_ptr, EGO_DRAGON))
	{
		teleport_player(Ind, 100);
		o_ptr->timeout = 50 + randint(50);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return;
	}
	else if (is_ego_p(o_ptr, EGO_JUMP))
	{
		teleport_player(Ind, 10);
		o_ptr->timeout = 10 + randint(10);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return;
	}
	else if (is_ego_p(o_ptr, EGO_SPINING))
	{
		do_spin(Ind);
		o_ptr->timeout = 50 + randint(25);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return;
	}
	else if (is_ego_p(o_ptr, EGO_NOLDOR))
	{
		detect_treasure(Ind, DEFAULT_RADIUS);
		o_ptr->timeout = 10 + randint(20);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return;
	}
	else if (is_ego_p(o_ptr, EGO_SPECTRAL))
	{
		//                if (!p_ptr->wraith_form)
		if (!p_ptr->tim_wraith)
			//                        set_shadow(Ind, 20 + randint(20));
			set_tim_wraith(Ind, 20 + randint(20));
		else
			//                       set_shadow(Ind, p_ptr->tim_wraith + randint(20));
			set_tim_wraith(Ind, p_ptr->tim_wraith + randint(20));
		o_ptr->timeout = 50 + randint(50);

		/* Window stuff */
		p_ptr->window |= PW_INVEN | PW_EQUIP;

		/* Done */
		return;
	}
	/* Hack -- Amulet of the Serpents can be activated as well */
	if ((o_ptr->tval == TV_AMULET) && (o_ptr->sval == SV_AMULET_SERPENT))
	{
		/* Get a direction for breathing (or abort) */
		p_ptr->current_activation = item;
		get_aim_dir(Ind);
		return;
	}


	if (o_ptr->tval == TV_RING)
	{
		switch (o_ptr->sval)
		{
			case SV_RING_ELEC:
			case SV_RING_ACID:
			case SV_RING_ICE:
			case SV_RING_FLAMES:
				{
					/* Get a direction for breathing (or abort) */
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}


				/* Yes, this can be activated but at the cost of it's destruction */
			case SV_RING_TELEPORTATION:
				{
					//                                if(get_check("This will destroy the ring, do you want to continue ?"))
					{
						msg_print(Ind, "The ring explode into a space distorsion.");
						teleport_player(Ind, 200);

						/* It explodes, doesnt it ? */
						take_hit(Ind, damroll(2, 10), "an exploding ring");

						inven_item_increase(Ind, item, -255);
						inven_item_optimize(Ind, item);
					}
					break;
				}
			case SV_RING_POLYMORPH:
				{
					/* Mimics only */
					if (!get_skill(p_ptr, SKILL_MIMIC)) return;

					if(!(item==INVEN_LEFT || item==INVEN_RIGHT)){
						msg_print(Ind, "You must be wearing the ring!");
						return;
					}

                                        /* If never used before, then set to the player form, otherwise set the player form*/
					if (!o_ptr->pval)
					{
						if (p_ptr->r_killed[p_ptr->body_monster] < r_info[p_ptr->body_monster].level)
							msg_print(Ind, "Nothing happens");
						else{
							msg_format(Ind, "The form of the ring seems to change to a small %s.", r_info[p_ptr->body_monster].name + r_name);
							o_ptr->pval = p_ptr->body_monster;
							p_ptr->r_killed[p_ptr->body_monster]=0;
						}
					}
					else
					{
						monster_race *r_ptr = &r_info[o_ptr->pval];

#if 0
						if ((r_ptr->level > p_ptr->lev * 2) || (p_ptr->r_killed[o_ptr->pval] < r_ptr->level))
						{
							msg_print(Ind, "You dont match the ring yet.");
							return;
						}
#endif

						msg_print(Ind, "You polymorph !");
						p_ptr->body_monster = o_ptr->pval;
						p_ptr->body_changed = TRUE;

						p_ptr->update |= (PU_BONUS);

						/* Recalculate mana */
						p_ptr->update |= (PU_MANA | PU_HP);

						/* Window stuff */
						p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
					}
				}
		}


		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Success */
		return;

	}
#if 1
	/* Some ego items can be activated */
	else if (o_ptr->name2)
	{
		switch (o_ptr->name2)
		{
			case EGO_CLOAK_LORDLY_RES:
				{
					msg_print(Ind, "Your cloak flashes many colors...");

					(void)set_oppose_acid(Ind, p_ptr->oppose_acid + randint(40) + 40);
					(void)set_oppose_elec(Ind, p_ptr->oppose_elec + randint(40) + 40);
					(void)set_oppose_fire(Ind, p_ptr->oppose_fire + randint(40) + 40);
					(void)set_oppose_cold(Ind, p_ptr->oppose_cold + randint(40) + 40);
					(void)set_oppose_pois(Ind, p_ptr->oppose_pois + randint(40) + 40);

					o_ptr->timeout = rand_int(50) + 150;
					break;
				}
		}
		/* Done ego item activation */
		return;
	}

	/* Amulets of the moon can be activated for sleep monster */
	if ((o_ptr->tval == TV_AMULET) && (o_ptr->sval == SV_AMULET_THE_MOON))
	{
		msg_print(Ind, "Your amulet glows a deep blue...");
		sleep_monsters(Ind);

		o_ptr->timeout = rand_int(100) + 100;
		return;
	}
#endif	// 0

	/* Mistake */
	msg_print(Ind, "That object cannot be activated.");
}


void do_cmd_activate_dir(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];
	object_type *o_ptr;

	int item;

	item = p_ptr->current_activation;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	if( check_guard_inscription( o_ptr->note, 'A' )) {
                msg_print(Ind, "The item's inscription prevents it");
                return;
        }; 

        if (!can_use(Ind, o_ptr))
        {
                msg_print(Ind, "You are not high level enough.");
		return;
        }

	/* Artifacts activate by name */
	if (o_ptr->name1)
	{
		/* This needs to be changed */
		switch (o_ptr->name1)
		{
			case ART_NARTHANC:
			{
				fire_bolt(Ind, GF_FIRE, dir, damroll(9, 8));
				o_ptr->timeout = rand_int(8) + 8;
				break;
			}

			case ART_NIMTHANC:
			{
				fire_bolt(Ind, GF_COLD, dir, damroll(6, 8));
				o_ptr->timeout = rand_int(7) + 7;
				break;
			}

			case ART_DETHANC:
			{
				fire_bolt(Ind, GF_ELEC, dir, damroll(4, 8));
				o_ptr->timeout = rand_int(6) + 6;
				break;
			}

			case ART_RILIA:
			{
				fire_ball(Ind, GF_POIS, dir, 12, 3);
				o_ptr->timeout = rand_int(4) + 4;
				break;
			}

			case ART_BELANGIL:
			{
				fire_ball(Ind, GF_COLD, dir, 48, 2);
				o_ptr->timeout = rand_int(5) + 5;
				break;
			}

			case ART_RINGIL:
			{
				fire_ball(Ind, GF_COLD, dir, 100, 2);
				o_ptr->timeout = 300;
				break;
			}

			case ART_ANDURIL:
			{
				fire_ball(Ind, GF_FIRE, dir, 72, 2);
				o_ptr->timeout = 400;
				break;
			}

			case ART_FIRESTAR:
			{
				fire_ball(Ind, GF_FIRE, dir, 72, 3);
				o_ptr->timeout = 100;
				break;
			}

			case ART_THEODEN:
			{
				drain_life(Ind, dir, 120);
				o_ptr->timeout = 400;
				break;
			}

			case ART_TURMIL:
			{
				drain_life(Ind, dir, 90);
				o_ptr->timeout = 70;
				break;
			}

			case ART_ARUNRUTH:
			{
				fire_bolt(Ind, GF_COLD, dir, damroll(12, 8));
				o_ptr->timeout = 500;
				break;
			}

			case ART_AEGLOS:
			{
				fire_ball(Ind, GF_COLD, dir, 100, 2);
				o_ptr->timeout = 500;
				break;
			}

			case ART_OROME:
			{
				wall_to_mud(Ind, dir);
				o_ptr->timeout = 5;
				break;
			}

			case ART_ULMO:
			{
				teleport_monster(Ind, dir);
				o_ptr->timeout = 150;
				break;
			}

			case ART_TOTILA:
			{
				confuse_monster(Ind, dir, 20);
				o_ptr->timeout = 15;
				break;
			}

			case ART_CAMMITHRIM:
			{
				fire_bolt(Ind, GF_MISSILE, dir, damroll(2, 6));
				o_ptr->timeout = 2;
				break;
			}

			case ART_PAURHACH:
			{
				fire_bolt(Ind, GF_FIRE, dir, damroll(9, 8));
				o_ptr->timeout = rand_int(8) + 8;
				break;
			}

			case ART_PAURNIMMEN:
			{
				fire_bolt(Ind, GF_COLD, dir, damroll(6, 8));
				o_ptr->timeout = rand_int(7) + 7;
				break;
			}

			case ART_PAURAEGEN:
			{
				fire_bolt(Ind, GF_ELEC, dir, damroll(4, 8));
				o_ptr->timeout = rand_int(6) + 6;
				break;
			}

			case ART_PAURNEN:
			{
				fire_bolt(Ind, GF_ACID, dir, damroll(5, 8));
				o_ptr->timeout = rand_int(5) + 5;
				break;
			}

			case ART_FINGOLFIN:
			{
				fire_bolt(Ind, GF_ARROW, dir, 150);
				o_ptr->timeout = rand_int(90) + 90;
				break;
			}

			case ART_NARYA:
			{
				fire_ball(Ind, GF_FIRE, dir, 120, 3);
				o_ptr->timeout = rand_int(225) + 225;
				break;
			}

			case ART_NENYA:
			{
				fire_ball(Ind, GF_COLD, dir, 200, 3);
				o_ptr->timeout = rand_int(325) + 325;
				break;
			}

			case ART_VILYA:
			{
				fire_ball(Ind, GF_ELEC, dir, 250, 3);
				o_ptr->timeout = rand_int(425) + 425;
				break;
			}

			case ART_POWER:
			{
				ring_of_power(Ind, dir);
				o_ptr->timeout = rand_int(450) + 450;
				break;
			}

                        case ART_MEDIATOR:
			{
				if (!get_aim_dir(Ind)) return;
				msg_print(Ind, "You breathe the elements.");
				fire_ball(Ind, GF_MISSILE, dir, 300, 4);
				msg_print(Ind, "Your armor glows many colours...");
				(void)set_afraid(Ind, 0);
				(void)set_shero(Ind, p_ptr->shero + randint(50) + 50);
				(void)hp_player(Ind, 30);
				(void)set_blessed(Ind, p_ptr->blessed + randint(50) + 50);
				(void)set_oppose_acid(Ind, p_ptr->oppose_acid + randint(50) + 50);
				(void)set_oppose_elec(Ind, p_ptr->oppose_elec + randint(50) + 50);
				(void)set_oppose_fire(Ind, p_ptr->oppose_fire + randint(50) + 50);
				(void)set_oppose_cold(Ind, p_ptr->oppose_cold + randint(50) + 50);
				(void)set_oppose_pois(Ind, p_ptr->oppose_pois + randint(50) + 50);
				o_ptr->timeout = 400;
				break;
			}

                        case ART_AXE_GOTHMOG:
                        {
                                fire_ball(Ind, GF_FIRE, dir, 300, 4);
                                o_ptr->timeout = 200+rand_int(200);
                                break;
                        }
                        case ART_MELKOR:
                        {
                                fire_ball(Ind, GF_DARK, dir, 150, 3);
                                o_ptr->timeout = 100;
                                break;
                        }
			case ART_NIGHT:
			{
				int i;
				for (i = 0; i < 3; i++)
				{
					if (drain_life(Ind, dir, 100))
						hp_player(Ind, 100);
				}
				o_ptr->timeout = 250;
				break;
			}
                        case ART_NAIN:
                        {
								wall_to_mud(Ind, dir);
                                o_ptr->timeout = rand_int(5) + 7;
                                break;
                        }

                        case ART_EOL:
                        {
                                fire_bolt(Ind, GF_MANA, dir, damroll(9, 8));
                                o_ptr->timeout = rand_int(7) + 7;
                                break;
                        }

                        case ART_UMBAR:
                        {
                                fire_bolt(Ind, GF_MISSILE, dir, damroll(10, 10));
                                o_ptr->timeout = rand_int(20) + 20;
                                break;
                        }

		}

		/* Clear activation */
		p_ptr->current_activation = -1;

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return;
	}

        /* Hack -- Amulet of the Serpents can be activated as well */
	else if ((o_ptr->tval == TV_AMULET) && (o_ptr->sval == SV_AMULET_SERPENT))
        {
                msg_print(Ind, "You breathe venom...");
                fire_ball(Ind, GF_POIS, dir, 100, 2);
                o_ptr->timeout = rand_int(60) + 40;
        }

	else if (o_ptr->tval == TV_RING)
	{
		switch (o_ptr->sval)
		{
                        case SV_RING_ELEC:
			{
                                /* Get a direction for breathing (or abort) */
                                fire_ball(Ind, GF_ELEC, dir, 50, 2);
                                (void)set_oppose_elec(Ind, p_ptr->oppose_elec + randint(20) + 20);
				o_ptr->timeout = rand_int(50) + 50;
				break;
			}

			case SV_RING_ACID:
			{
                                /* Get a direction for breathing (or abort) */
				fire_ball(Ind, GF_ACID, dir, 50, 2);
				(void)set_oppose_acid(Ind, p_ptr->oppose_acid + randint(20) + 20);
				o_ptr->timeout = rand_int(50) + 50;
				break;
			}

			case SV_RING_ICE:
			{
                                /* Get a direction for breathing (or abort) */
				fire_ball(Ind, GF_COLD, dir, 50, 2);
				(void)set_oppose_cold(Ind, p_ptr->oppose_cold + randint(20) + 20);
				o_ptr->timeout = rand_int(50) + 50;
				break;
			}

			case SV_RING_FLAMES:
			{
                                /* Get a direction for breathing (or abort) */
				fire_ball(Ind, GF_FIRE, dir, 50, 2);
				(void)set_oppose_fire(Ind, p_ptr->oppose_fire + randint(20) + 20);
				o_ptr->timeout = rand_int(50) + 50;
				break;
			}
		}

#if 0
		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Success */
		return;
#endif
        }

	/* Clear current activation */
	p_ptr->current_activation = -1;

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	/* Success */
	return;
}

bool unmagic(int Ind)
{
	player_type *p_ptr = Players[Ind];
	bool ident;
	if (
			set_adrenaline(Ind, 0) |
			set_biofeedback(Ind, 0) |
			set_tim_esp(Ind, 0) |
			set_st_anchor(Ind, 0) |
			set_prob_travel(Ind, 0) |
			set_bow_brand(Ind, 0, 0, 0) |
			set_mimic(Ind, 0, 0) |
			set_tim_manashield(Ind, 0) |
			set_tim_traps(Ind, 0) |
			set_invis(Ind, 0, 0) |
			set_furry(Ind, 0) |
			set_tim_meditation(Ind, 0) |
			//				set_tim_wraith(Ind, 0) |
			set_fast(Ind, 0) |
			set_shield(Ind, 0) |
			set_blessed(Ind, 0) |
			set_hero(Ind, 0) |
			set_shero(Ind, 0) |
			set_protevil(Ind, 0) |
			set_invuln(Ind, 0) |
			set_tim_invis(Ind, 0) |
			set_tim_infra(Ind, 0) |
			set_oppose_acid(Ind, 0) |
			set_oppose_elec(Ind, 0) |
			set_oppose_fire(Ind, 0) |
			set_oppose_cold(Ind, 0) |
			set_oppose_pois(Ind, 0)
			) ident = TRUE;

	/* In town it only runs out if you are not on a wall
	 * To prevent breaking into houses */
	if (players_on_depth(&p_ptr->wpos)!= 0)
	{
			/* important! check for illegal spaces */
		cave_type **zcave;
		zcave=getcave(&p_ptr->wpos);
		if (in_bounds(p_ptr->py, p_ptr->px))
		{
			if ((p_ptr->wpos.wz) || (cave_floor_bold(zcave, p_ptr->py, p_ptr->px)))
			{
				if (set_tim_wraith(Ind, 0)) ident = TRUE;
			}
		}
	}

	return (ident);
}

/*
 * Displays random fortune/rumour.
 * Thanks Mihi!		- Jir -
 */
void fortune(int Ind, bool broadcast)
{
	char Rumor[80];

	msg_print(Ind, NULL);
//	switch(randint(20))
	switch(randint(80))
	{
		case 1:
			get_rnd_line("chainswd.txt",0 , Rumor);
			break;
		case 2:
			get_rnd_line("error.txt",0 , Rumor);
			break;
		case 3:
		case 4:
		case 5:
			get_rnd_line("death.txt",0 , Rumor);
			break;
		default:
			get_rnd_line("rumors.txt",0 , Rumor);
	}
	bracer_ff(Rumor);
//	msg_format(Ind, "%s", Rumor);
	msg_format(Ind, Rumor);
	msg_print(Ind, NULL);

	if (broadcast)
	{
		msg_broadcast(Ind, "Suddenly a thought comes to your mind:");
		msg_broadcast(Ind, Rumor);
	}

}

char random_colour()
{
//	char tmp[] = "wWrRbBgGdDuUoyvs";
	char tmp[] = "dwsorgbuDWvyRGBU";

	return(tmp[randint(15)]);	// never 'd'
}


