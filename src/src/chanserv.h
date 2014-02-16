/* chanserv.h - Channel service bot
 * Copyright 2000-2004 srvx Development Team
 *
 * This file is part of x3.
 *
 * x3 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with srvx; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#ifndef _chanserv_h
#define _chanserv_h

#include "nickserv.h"

enum UL_ALIASES {
    UL_PEON = 1,
    UL_OP = 200,
    UL_MANAGER = 300,
    UL_PRESENT = UL_MANAGER, /* Level needed to keep the channel active */
    UL_COOWNER = 400,
    UL_OWNER = 500,
    UL_HELPER = 600,
};

enum levelOption {
    lvlEnfOps,
    lvlEnfModes,
    lvlEnfTopic,
    lvlPubCmd,
    lvlSetters,
    lvlUserInfo,
    lvlInviteMe,
    lvlTopicSnarf,
    NUM_LEVEL_OPTIONS
};

enum charOption {
    chAutomode,
    chProtect,
    chToys,
    chTopicRefresh,
    chCTCPReaction,
    chBanTimeout,
    chResync,
    NUM_CHAR_OPTIONS
};

#define CHANNEL_NODELETE	0x00000001 /* (1 << 0) */
#define CHANNEL_SUSPENDED	0x00000002 /* (1 << 1) */
#define CHANNEL_INFO_LINES	0x00000004 /* (1 << 2) - DEPRECATED */
#define CHANNEL_VOICE_ALL    	0x00000008 /* (1 << 3) - DEPRECATED */

/* No longer used. */                      /* (1 << 4) */
#define CHANNEL_DYNAMIC_LIMIT	0x00000020 /* (1 << 5) */
#define CHANNEL_TOPIC_SNARF	0x00000040 /* (1 << 6) - DEPRECATED */
#define CHANNEL_PEON_INVITE     0x00000080 /* (1 << 7) - DEPRECATED */
#define CHANNEL_OFFCHANNEL      0x00000100 /* (1 << 8) */
#define CHANNEL_UNREVIEWED      0x00000400 /* (1 << 10) */

/* Flags with values over 0x20000000 or (1 << 29) will not work
 * because chanData.flags is a 30-bit field.
 */

/* how many seconds a pending adduser will wait for a user
 * to get auth or register 
 */
#define ADDUSER_PENDING_EXPIRE 7200  /* 2 hours */

#define IsProtected(x)		((x)->flags & CHANNEL_NODELETE)
#define IsSuspended(x)		((x)->flags & CHANNEL_SUSPENDED)
#define IsOffChannel(x)         (((x)->flags & CHANNEL_OFFCHANNEL) && (off_channel > 1))

struct chanData
{
    struct chanNode	*channel;
    struct mod_chanmode modes;

    time_t		registered;
    time_t		visited;
    time_t 		limitAdjusted;
    time_t              ownerTransfer;

    char		*topic;
    char		*greeting;
    char		*user_greeting;
    char		*registrar;
    char                *topic_mask;

    unsigned int        maxsetinfo;
    unsigned int	flags : 30;
    unsigned int        may_opchan : 1;
    unsigned int        max;
    unsigned int        last_refresh;
    unsigned int        last_resync;
    unsigned int        roulette_chamber;
    unsigned short      banCount; /* Lamers, really */
    unsigned short      userCount;
    unsigned short      lvlOpts[NUM_LEVEL_OPTIONS];
    unsigned char       chOpts[NUM_CHAR_OPTIONS];

    struct userData	*users;
    struct banData	*bans; /* Lamers, really */
    struct dict         *notes;
    struct suspended	*suspended;
    struct giveownership *giveownership;
    struct chanData	*prev;
    struct chanData	*next;
};

#define USER_NOAUTO_OP          0x00000001 /* OLD; Not used at all.. */
#define USER_SUSPENDED          0x00000002
#define USER_AUTO_INVITE        0x00000004
#define USER_AUTO_OP            0x00000008
#define USER_AUTO_JOIN          0x00000010
#define USER_FLAGS_SIZE         25
#define USER_FLAGS_DEFAULT      USER_AUTO_OP

#define IsUserAutoOp(USER)      ((USER)->flags & USER_AUTO_OP)
#define IsUserSuspended(USER)   ((USER)->flags & USER_SUSPENDED)
#define IsUserAutoInvite(USER)  ((USER)->flags & USER_AUTO_INVITE)
#define IsUserAutoJoin(USER)    ((USER)->flags & USER_AUTO_JOIN)

struct userData
{
    struct handle_info	*handle;
    struct chanData	*channel;

    char		*info;
    time_t 		seen;
    time_t              expires;
    time_t              accessexpiry;
    time_t              clvlexpiry;
    unsigned short      lastaccess;
    unsigned short      access;
    unsigned int	present : 1;
    unsigned int        flags : USER_FLAGS_SIZE;

    /* linked list of userDatas for a chanData */
    struct userData	*prev;
    struct userData	*next;
    /* linked list of userDatas for a handle_info */
    struct userData     *u_prev;
    struct userData     *u_next;
};

struct adduserPending
{
    struct chanNode *channel;
    struct userNode *user;
    int level;
    time_t created;

    struct adduserPending *prev;
    struct adduserPending *next;
};

struct banData
{
    char		mask[NICKLEN + USERLEN + HOSTLEN + 3];
    char		owner[NICKLEN+1];
    struct chanData     *channel;

    time_t		set;
    time_t		triggered;
    time_t              expires;

    char		*reason;

    struct banData	*prev;
    struct banData	*next;
};

struct suspended
{
    struct chanData	*cData;
    char		*suspender;
    char                *reason;
    time_t              issued, expires, revoked;
    struct suspended    *previous;
};

struct giveownership
{
    char                 *staff_issuer;
    char                  *old_owner;
    char                  *target;
    unsigned short        target_access;
    time_t                issued;
    char                  *reason;
    struct giveownership  *previous;
};

struct do_not_register
{
    char   chan_name[CHANNELLEN+1];
    char   setter[NICKSERV_HANDLE_LEN+1];
    time_t set, expires; 
    char   reason[1];
};

#define GetChannelUser(channel, handle) _GetChannelUser(channel, handle, 1, 0)
struct userData *_GetChannelUser(struct chanData *channel, struct handle_info *handle, int override, int allow_suspended);
struct banData *add_channel_ban(struct chanData *channel, const char *mask, char *owner, time_t set, time_t triggered, time_t expires, char *reason);

void init_chanserv(const char *nick);
void del_channel_user(struct userData *user, int do_gc);
struct channelList *chanserv_support_channels(void);
unsigned short user_level_from_name(const char *name, unsigned short clamp_level);
struct do_not_register *chanserv_is_dnr(const char *chan_name, struct handle_info *handle);
int check_user_level(struct chanNode *channel, struct userNode *user, enum levelOption opt, int allow_override, int exempt_owner);

void do_math(char *Buffer, char *Math);
char* user_level_name_from_level(int level);

void process_adduser_pending(struct userNode *user);
void wipe_adduser_pending(struct chanNode *channel, struct userNode *user);

int check_bans(struct userNode *user, const char *channel);
int trace_check_bans(struct userNode *user, struct chanNode *chan);

#endif
