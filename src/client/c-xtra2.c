/* $Id$ */
/* More extra client things */

#include "angband.h"

/*
 * Show previous messages to the user   -BEN-
 *
 * The screen format uses line 0 and 23 for headers and prompts,
 * skips line 1 and 22, and uses line 2 thru 21 for old messages.
 *
 * This command shows you which commands you are viewing, and allows
 * you to "search" for strings in the recall.
 *
 * Note that messages may be longer than 80 characters, but they are
 * displayed using "infinite" length, with a special sub-command to
 * "slide" the virtual display to the left or right.
 *
 * Attempt to only hilite the matching portions of the string.
 */
/* TODO: those functions should be bandled in one or two generic function(s).
 */
void do_cmd_messages(void)
{
	int i, j, k, n, nn, q, r, s, t=0;

	char shower[80] = "";
	char finder[80] = "";

	cptr message_recall[MESSAGE_MAX] = {0};
	cptr msg = "", msg2;

	/* Display messages in different colors -Zz */
	char nameA[20];
	char nameB[20];
	cptr nomsg_target = "Target Selected.";
	cptr nomsg_map = "Map sector ";

	strcpy(nameA, "[");  strcat(nameA, cname);  strcat(nameA, ":");
	strcpy(nameB, ":");  strcat(nameB, cname);  strcat(nameB, "]");


	/* Total messages */
	n = message_num();
	nn = 0;  /* number of new messages */

	/* Filter message buffer for "unimportant messages" add to message_recall
	 * "Target Selected" messages are too much clutter for archers to remove
	 * from msg recall
	 */

	j = 0;
	msg = NULL;
	for (i = 0; i < n; i++)
	{
		msg = message_str(i);

		if (strstr(msg, nomsg_target) ||
				strstr(msg, nomsg_map))
			continue;

		message_recall[nn] = msg;
		nn++;
	}


	/* Start on first message */
	i = 0;

	/* Start at leftmost edge */
	q = 0;

	/* Save the screen */
	Term_save();

	/* Process requests until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		n = nn;  /* new total # of messages in our new array */
		r = 0;	/* how many times the message is Repeated */
		s = 0;	/* how many lines Saved */
		k = 0;	/* end of buffer flag */


		/* Dump up to 20 lines of messages */
		for (j = 0; (j < 20) && (i + j + s < n); j++)
		{
			byte a = TERM_WHITE;

			msg2 = msg;
			msg = message_recall[i+j+s];

			/* Handle repeated messages */
			if (msg == msg2)
			{
				r++;
				j--;
				s++;
				if (i + j + s < n - 1) continue;
				k = 1;
			}

			if (r)
			{
				Term_putstr(t < 72 ? t : 72, 21-j+1-k, -1, a, format(" (x%d)", r + 1));
				r = 0;
			}

			/* Apply horizontal scroll */
			msg = (strlen(msg) >= q) ? (msg + q) : "";

			/* Handle "shower" */
			if (shower[0] && strstr(msg, shower)) a = TERM_YELLOW;

			/* Dump the messages, bottom to top */
			Term_putstr(0, 21-j, -1, a, (char*)msg);
			t = strlen(msg);
		}

		/* Display header XXX XXX XXX */
		prt(format("Message Recall (%d-%d of %d), Offset %d",
					i, i+j-1, n, q), 0, 0);

		/* Display prompt (not very informative) */
		prt("[Press 'p' for older, 'n' for newer, 'f' for filedump, ..., or ESCAPE]", 23, 0);

		/* Get a command */
		k = inkey();

		/* Exit on Escape */
		if (k == ESCAPE || k == KTRL('X')) break;

		/* Hack -- Save the old index */
		j = i;

		/* Hack -- go to a specific line */
		if (k == '#')
		{
			char tmp[80];
			prt(format("Goto Line(max %d): ", n), 23, 0);
			strcpy(tmp, "0");
			if (askfor_aux(tmp, 80, 0, 0))
			{
				i = atoi(tmp);
				i = i > 0 ? (i < n ? i : n - 1) : 0;
			}
		}

		/* Horizontal scroll */
		if (k == '4' || k == '<')
		{
			/* Scroll left */
			q = (q >= 40) ? (q - 40) : 0;

			/* Success */
			continue;
		}

		/* Horizontal scroll */
		if (k == '6' || k == '>')
		{
			/* Scroll right */
			q = q + 40;

			/* Success */
			continue;
		}

		/* Hack -- handle show */
		if (k == '=')
		{
			/* Prompt */
			prt("Show: ", 23, 0);

			/* Get a "shower" string, or continue */
			if (!askfor_aux(shower, 80, 0, 0)) continue;

			/* Okay */
			continue;
		}

		/* Hack -- handle find */
		/* FIXME -- (x4) compressing seems to ruin it */
		if (k == '/')
		{
			int z;

			/* Prompt */
			prt("Find: ", 23, 0);

			/* Get a "finder" string, or continue */
			if (!askfor_aux(finder, 80, 0, 0)) continue;

			/* Scan messages */
			for (z = i + 1; z < n; z++)
			{
				cptr str = message_str(z);

				/* Handle "shower" */
				if (strstr(str, finder))
				{
					/* New location */
					i = z;

					/* Hack -- also show */
					strcpy(shower, str);

					/* Done */
					break;
				}
			}
		}

		/* Recall 1 older message */
		if ((k == '8') || (k == '\b') || k=='k')
		{
			/* Go newer if legal */
			if (i + 1 < n) i += 1;
		}

		/* Recall 10 older messages */
		if (k == '+')
		{
			/* Go older if legal */
			if (i + 10 < n) i += 10;
		}

		/* Recall 20 older messages */
		if ((k == 'p') || (k == KTRL('P')) || (k == 'b') || k == KTRL('U'))
		{
			/* Go older if legal */
			if (i + 20 < n) i += 20;
		}

		/* Recall 20 newer messages */
		if ((k == 'n') || (k == KTRL('N')) || k==' ')
		{
			/* Go newer (if able) */
			i = (i >= 20) ? (i - 20) : 0;
		}

		/* Recall 10 newer messages */
		if (k == '-')
		{
			/* Go newer (if able) */
			i = (i >= 10) ? (i - 10) : 0;
		}

		/* Recall 1 newer messages */
		if (k == '2' || k=='j' || (k == '\n') || (k == '\r'))
		{
			/* Go newer (if able) */
			i = (i >= 1) ? (i - 1) : 0;
		}

		/* Recall the oldest messages */
		if (k == 'g')
		{
			/* Go oldest */
			i = n - 20;
		}

		/* Recall the newest messages */
		if (k == 'G')
		{
			/* Go newest */
			i = 0;
		}

		/* Dump */
		if ((k == 'f') || (k == 'F'))
		{
			char tmp[80];
			strnfmt(tmp, 160, "%s-msg.txt", cname);
			if (get_string("Filename: ", tmp, 80))
			{
				if (tmp[0] && (tmp[0] != ' '))
				{
					dump_messages(tmp, MESSAGE_MAX, 0);
					continue;
				}
			}
		}

		if (k == KTRL('T'))
		{
			/* Take a screenshot */
			xhtml_screenshot("screenshotXXXX");
			continue;
		}

		/* Hack -- Error of some kind */
		if (i == j) bell();
	}

	/* Restore the screen */
	Term_load();

	/* Flush any queued events */
	Flush_queue();
}


/* Show buffer of "important events" only via the CTRL-O command -Zz */
void do_cmd_messages_chatonly(void)
{

/* Note: This only serves for a bad hack here, condition to work is
   that no non-chat must begin its first line (if it's multi-lined)
   on a space char ' '. Should maybe be done by adding another array
   to message__buf that is set in c-util.c and is a flag to keep track
   of chat-buffer or non-chat-buffer property of each message - C. Blue */
	bool was_ctrlo_buffer = FALSE;

	int i, j, k, n, nn, q;

	char shower[80] = "";
	char finder[80] = "";

	/* Create array to store message buffer for important messags  */
	/* (This is an expensive hit, move to c-init.c?  But this only */
	/* occurs when user hits CTRL-O which is usally in safe place  */
	/* or after AFK) 					       */
	cptr message_chat[MESSAGE_MAX] = {0};

	/* Display messages in different colors */
	char nameA[20];
	char nameB[20];
	cptr msg_deadA = "You have been killed";
	cptr msg_deadB = "You die";
	cptr msg_unique = "was slain by ";
	cptr msg_killed = "was killed ";
	cptr msg_killed2 = "was annihilated ";
	cptr msg_killed3 = "was vaporized ";
	cptr msg_destroyed = "was destroyed ";
	cptr msg_killedF = "by Morgoth, Lord of Darkness"; /* for fancy death messages */
	cptr msg_suicide = "committed suicide.";
	cptr msg_entered = "has entered the game.";
	cptr msg_left = "has left the game.";
	cptr msg_quest = "has won the";
	cptr msg_dice = "dice and get";
	cptr msg_level = "Welcome to level";
	cptr msg_level2 = "has attained level";
	cptr msg_gained_ability = "\377G*";
	cptr msg_inven_destroy1 = "\377oYour ";
	cptr msg_inven_destroy2 = "\377oOne of your ";
	cptr msg_inven_destroy3 = "\377oSome of your ";
	cptr msg_inven_destroy4 = "\377oAll of your ";
/*	cptr msg_inven_destroy1 = "was destroyed!";
	cptr msg_inven_destroyx = "were destroyed!";*/
	cptr msg_inven_steal1 = " was stolen";
	cptr msg_inven_steal2 = " were stolen";
	cptr msg_nopkfight = "You have beaten";
	cptr msg_nopkfight2 = "has beaten you";
	cptr msg_bloodbond = "blood bonds";
	cptr msg_bloodbond2 = "ou blood bond";
	cptr msg_bloodbond3 = "won the blood bond";
	cptr msg_challenge = "challenges";
	cptr msg_defeat = "has defeated";
	cptr msg_retire = "has retired";
	cptr msg_fruitbat = "turned into a fruit bat";
        cptr msg_afk1 = "seems to be AFK now";
        cptr msg_afk2 = "has returned from AFK";

	strcpy(nameA, "[");  strcat(nameA, cname);  strcat(nameA, ":");
	strcpy(nameB, ":");  strcat(nameB, cname);  strcat(nameB, "]");


	/* Total messages */
	n = message_num();
	nn = 0;  /* number of new messages */

	/* Filter message buffer for "important messages" add to message_chat*/
//	for (i = 0; i < n; i++)
	for (i = n - 1; i >= 0; i--) /* traverse from oldest to newest message, for was_ctrlo_buf */
	{
		cptr msg = message_str(i);

		if ((strstr(msg, nameA) != NULL) || (strstr(msg, nameB) != NULL) || (msg[0] == '[') ||
		    (strstr(msg, msg_killed) != NULL) || (strstr(msg, msg_killed2) != NULL) ||
		    (strstr(msg, msg_killed3) != NULL) || (strstr(msg, msg_destroyed) != NULL) ||
		    (strstr(msg, msg_killedF) != NULL) ||
		    (strstr(msg, msg_unique) != NULL) || (strstr(msg, msg_suicide) != NULL) ||
		    (strstr(msg, msg_entered) != NULL) || (strstr(msg, msg_left) != NULL) ||
		    (strstr(msg, msg_quest) != NULL) || (strstr(msg, msg_dice) != NULL) ||
		    (strstr(msg, msg_level) != NULL) || (strstr(msg, msg_level2) != NULL) ||
		    (strstr(msg, msg_gained_ability) != NULL) ||
		    (strstr(msg, msg_deadA)  != NULL) || (strstr(msg, msg_deadB) != NULL) ||
		    (strstr(msg, msg_inven_destroy1) != NULL) || (strstr(msg, msg_inven_destroy2) != NULL) ||
		    (strstr(msg, msg_inven_destroy3) != NULL) || (strstr(msg, msg_inven_destroy4) != NULL) ||
/*		    (strstr(msg, msg_inven_destroy1) != NULL) || (strstr(msg, msg_inven_destroyx) != NULL) || */
		    (strstr(msg, msg_inven_steal1) != NULL) || (strstr(msg, msg_inven_steal2) != NULL) ||
		    (strstr(msg, msg_nopkfight) != NULL) || (strstr(msg, msg_nopkfight2) != NULL) ||
		    (strstr(msg, msg_bloodbond) != NULL) || (strstr(msg, msg_bloodbond2) != NULL) ||
		    (strstr(msg, msg_bloodbond3) != NULL) ||
		    (strstr(msg, msg_challenge) != NULL) || (strstr(msg, msg_defeat) != NULL) ||
		    (strstr(msg, msg_retire) != NULL) ||
		    (strstr(msg, msg_afk1) != NULL) || (strstr(msg, msg_afk2) != NULL) ||
		    (strstr(msg, msg_fruitbat) != NULL) || (msg[2] == '[') ||
		    (msg[0] == ' ' && was_ctrlo_buffer))
		{
			was_ctrlo_buffer = TRUE;
			message_chat[nn] = msg;
			nn++;
		} else {
			was_ctrlo_buffer = FALSE;
		}
	}

	/* Start on first message */
	i = 0;

	/* Start at leftmost edge */
	q = 0;


	/* Save the screen */
	Term_save();

	/* Process requests until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Use last element in message_chat as  message_num() */
		n = nn;
		/* Dump up to 20 lines of messages */
		for (j = 0; (j < 20) && (i + j < n); j++)
		{
			byte a = TERM_WHITE;
			cptr msg = message_chat[nn - 1 - (i+j)]; /* because of inverted traversal direction, see further above */
//			cptr msg = message_chat[i+j];

			/* Apply horizontal scroll */
			msg = (strlen(msg) >= q) ? (msg + q) : "";

			/* Handle "shower" */
			if (shower[0] && strstr(msg, shower)) a = TERM_YELLOW;

			/* Dump the messages, bottom to top */
			Term_putstr(0, 21-j, -1, a, (char*)msg);
		}

		/* Display header XXX XXX XXX */
		prt(format("Message Recall (%d-%d of %d), Offset %d",
					i, i+j-1, n, q), 0, 0);

		/* Display prompt (not very informative) */
		prt("[Press 'p' for older, 'n' for newer, 'f' for filedump, ..., or ESCAPE]", 23, 0);

		/* Get a command */
		k = inkey();

		/* Exit on Escape */
		if (k == ESCAPE || k == KTRL('X')) break;

		/* Hack -- Save the old index */
		j = i;

		/* Hack -- go to a specific line */
		if (k == '#')
		{
			char tmp[80];
			prt(format("Goto Line(max %d): ", n), 23, 0);
			strcpy(tmp, "0");
			if (askfor_aux(tmp, 80, 0, 0))
			{
				i = atoi(tmp);
				i = i > 0 ? (i < n ? i : n - 1) : 0;
			}
		}

		/* Horizontal scroll */
		if (k == '4' || k == '<')
		{
			/* Scroll left */
			q = (q >= 40) ? (q - 40) : 0;

			/* Success */
			continue;
		}

		/* Horizontal scroll */
		if (k == '6' || k == '>')
		{
			/* Scroll right */
			q = q + 40;

			/* Success */
			continue;
		}

		/* Hack -- handle show */
		if (k == '=')
		{
			/* Prompt */
			prt("Show: ", 23, 0);

			/* Get a "shower" string, or continue */
			if (!askfor_aux(shower, 80, 0, 0)) continue;

			/* Okay */
			continue;
		}

		/* Hack -- handle find */
		if (k == '/')
		{
			int z;

			/* Prompt */
			prt("Find: ", 23, 0);

			/* Get a "finder" string, or continue */
			if (!askfor_aux(finder, 80, 0, 0)) continue;

			/* Scan messages */
			for (z = i + 1; z < n; z++)
			{
				cptr str = message_str(z);

				/* Handle "shower" */
				if (strstr(str, finder))
				{
					/* New location */
					i = z;

					/* Hack -- also show */
					strcpy(shower, str);

					/* Done */
					break;
				}
			}
		}

		/* Recall 1 older message */
		if ((k == '8') || (k == '\b') || k=='k')
		{
			/* Go newer if legal */
			if (i + 1 < n) i += 1;
		}

		/* Recall 10 older messages */
		if (k == '+')
		{
			/* Go older if legal */
			if (i + 10 < n) i += 10;
		}

		/* Recall 20 older messages */
		if ((k == 'p') || (k == KTRL('P')) || (k == 'b') || (k == KTRL('O')) ||
				k == KTRL('U'))
		{
			/* Go older if legal */
			if (i + 20 < n) i += 20;
		}

		/* Recall 20 newer messages */
		if ((k == 'n') || (k == KTRL('N')) || k==' ')
		{
			/* Go newer (if able) */
			i = (i >= 20) ? (i - 20) : 0;
		}

		/* Recall 10 newer messages */
		if (k == '-')
		{
			/* Go newer (if able) */
			i = (i >= 10) ? (i - 10) : 0;
		}

		/* Recall 1 newer messages */
		if (k == '2' || k=='j' || (k == '\n') || (k == '\r'))
		{
			/* Go newer (if able) */
			i = (i >= 1) ? (i - 1) : 0;
		}

		/* Recall the oldest messages */
		if (k == 'g')
		{
			/* Go oldest */
			i = n - 20;
		}

		/* Recall the newest messages */
		if (k == 'G')
		{
			/* Go newest */
			i = 0;
		}

		/* Dump */
		if ((k == 'f') || (k == 'F'))
		{
			char tmp[80];
			strnfmt(tmp, 160, "%s-chat.txt", cname);
			if (get_string("Filename: ", tmp, 80))
			{
				if (tmp[0] && (tmp[0] != ' '))
				{
					dump_messages(tmp, MESSAGE_MAX, 1);
					continue;
				}
			}
		}

		if (k == KTRL('T'))
		{
			/* Take a screenshot */
			xhtml_screenshot("screenshotXXXX");
			continue;
		}

		/* Hack -- Error of some kind */
		if (i == j) bell();
	}

	/* Restore the screen */
	Term_load();

	/* Flush any queued events */
	Flush_queue();
}

/*
 * dump message history to a file.	- Jir -
 *
 * XXX The beginning of dump can be corrupted. FIXME
 */
/* FIXME: result can be garbled if contains '%' */
/* chatonly if mode != 0 */
void dump_messages_aux(FILE *fff, int lines, int mode, bool ignore_color)
{
	int i, j, k, n, nn, q, r, s, t=0;

	cptr message_recall[MESSAGE_MAX] = {0};
	cptr msg = "", msg2;

	cptr nomsg_target = "Target Selected.";
	cptr nomsg_map = "Map sector ";

	cptr msg_deadA = "You have been killed";
	cptr msg_deadB = "You die";
	cptr msg_unique = "was slain by";
	cptr msg_killed = "was killed by";
  cptr msg_killed2 = "was annihilated ";
  cptr msg_killed3 = "was vaporized ";
//	cptr msg_destroyed = "ghost was destroyed by";
	cptr msg_destroyed = "was destroyed by";
	cptr msg_suicide = "committed suicide.";
	cptr msg_killedF = "by Morgoth, Lord of Darkness"; /* for fancy death messages */


	char buf[160];

	/* Total messages */
	n = message_num();
	nn = 0;  /* number of new messages */

	/* Filter message buffer for "unimportant messages" add to message_recall
	 * "Target Selected" messages are too much clutter for archers to remove
	 * from msg recall
	 */

	j = 0;
	msg = NULL;
	for (i = 0; i < n; i++)
	{
		msg = message_str(i);

		if (!mode)
		{
			if (strstr(msg, nomsg_target) ||
					strstr(msg, nomsg_map))
				continue;

			message_recall[nn] = msg;
			nn++;
		}
		else
		{
			if (
				(msg[0] == '[') || (msg[2] == '[') ||
				(strstr(msg, msg_killed) != NULL) ||
				(strstr(msg, msg_killed2) != NULL) ||
				(strstr(msg, msg_killed3) != NULL) ||
				(strstr(msg, msg_destroyed) != NULL) ||
				(strstr(msg, msg_killedF) != NULL) ||
				(strstr(msg, msg_unique) != NULL) ||
				(strstr(msg, msg_suicide) != NULL) ||
				(strstr(msg, msg_deadA) != NULL) ||
				(strstr(msg, msg_deadB) != NULL)
				)
			{
				message_recall[nn] = msg;
				nn++;
			}
		}
	}


	/* Start on first message */
	i = nn - lines;
	if (i < 0) i = 0;

	/* Start at leftmost edge */
	q = 0;

	n = nn;  /* new total # of messages in our new array */
	r = 0;	/* how many times the message is Repeated */
	s = 0;	/* how many lines Saved */
	k = 0;	/* end of buffer flag */


	/* Dump up to 20 lines of messages */
	for (j = 0; j + s < MIN(n, lines); j++)
	{
		msg2 = msg;
		msg = message_recall[MIN(n, lines) - (j+s) - 1];

		/* Handle repeated messages */
		if (msg == msg2)
		{
			r++;
			j--;
			s++;
			if (j + s < MIN(n, lines) - 1) continue;
			k = 1;
		}

		if (r)
		{
			fprintf(fff, " (x%d)", r + 1);
			r = 0;
		}
		fprintf(fff, "\n");
		if (k) break;

		q=0;
		for(t=0; t<strlen(msg); t++){
                        if(msg[t]=='\377'){
                                if (!ignore_color)
                                        buf[q++]='{';
                                else
                                {
                                        t++;
                                        if (msg[t] == '\0')
                                                break;
                                }
				continue;
			}
			if(msg[t]=='\n'){
				buf[q++]='\n';
				continue;
			}
			if(msg[t]=='\r'){
                                buf[q++]='\n';
				continue;
			}
			buf[q++]=msg[t];
		}
		buf[q]='\0';

		/* Dump the messages, bottom to top */
		fputs(buf, fff);
	}
	fprintf(fff, "\n\n");
}

errr dump_messages(cptr name, int lines, int mode)
{
	int			fd = -1;
	FILE		*fff = NULL;
	char		buf[1024];

	cptr what = mode ? "Chat" : "Message";

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_USER, name);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the non-existing file */
	if (fd < 0) fff = my_fopen(buf, "w");


	/* Invalid file */
	if (!fff)
	{
		/* Message */
		c_msg_print(format("%s dump failed!", what));
		c_msg_print(NULL);

		/* Error */
		return (-1);
	}

	/* Begin dump */
	fprintf(fff, "  [TomeNET %d.%d.%d%s @ %s %s Dump]\n\n",
		VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, CLIENT_VERSION_TAG, server_name, what);

	/* Do it */
	dump_messages_aux(fff, lines, mode, FALSE);

	fprintf(fff, "\n\n");

	/* Close it */
	my_fclose(fff);


	/* Message */
	c_msg_print(format("%s dump successful.", what));
	c_msg_print(NULL);

	/* Success */
	return (0);
}
