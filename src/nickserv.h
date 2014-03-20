/* nickserv.h - Nick/authentiction service
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

#ifndef _nickserv_h
#define _nickserv_h

#include "hash.h"   /* for NICKLEN, etc., and common.h */
#include "dict.h"
#include <tre/regex.h> /* for regex in nickserv_config */
struct svccmd;

#define NICKSERV_HANDLE_LEN ACCOUNTLEN
#define COOKIELEN 10

/* HI_FLAG_* go into handle_info.flags */
#define HI_FLAG_OPER_SUSPENDED 0x00000001
#define HI_FLAG_USE_PRIVMSG    0x00000002
#define HI_FLAG_SUPPORT_HELPER 0x00000004
#define HI_FLAG_HELPING        0x00000008
#define HI_FLAG_SUSPENDED      0x00000010
#define HI_FLAG_MIRC_COLOR     0x00000020
#define HI_FLAG_FROZEN         0x00000040
#define HI_FLAG_NODELETE       0x00000080
#define HI_FLAG_NETWORK_HELPER 0x00000100
#define HI_FLAG_BOT            0x00000200
#define HI_FLAG_AUTOHIDE       0x00000400
#define HI_FLAG_ADVANCED       0x00000800
#define HI_FLAG_ADMINSERV_OPER        0x00001000
#define HI_FLAG_ADMINSERV_ADMIN       0x00002000
/* Flag characters for the above.  First char is LSB, etc. */
#define HANDLE_FLAGS "SphgscfnHbxoa"

/* HI_STYLE_* go into handle_info.userlist_style */
#define HI_STYLE_NORMAL	       'n'
#define HI_STYLE_CLEAN	       'c'
#define HI_STYLE_ADVANCED      'a'
#define HI_STYLE_CLASSIC       'k'

#define HI_DEFAULT_FLAGS       (HI_FLAG_MIRC_COLOR)

/* This is overridden by conf file */
#define HI_DEFAULT_STYLE       HI_STYLE_NORMAL

#define HANDLE_FLAGGED(hi, tok) ((hi)->flags & HI_FLAG_##tok)
#define HANDLE_SET_FLAG(hi, tok) ((hi)->flags |= HI_FLAG_##tok)
#define HANDLE_TOGGLE_FLAG(hi, tok) ((hi)->flags ^= HI_FLAG_##tok)
#define HANDLE_CLEAR_FLAG(hi, tok) ((hi)->flags &= ~HI_FLAG_##tok)

#define IsAdminServOper(user) (user->handle_info && HANDLE_FLAGGED(user->handle_info, ADMINSERV_OPER))
#define IsAdminServAdmin(user) (user->handle_info && HANDLE_FLAGGED(user->handle_info, ADMINSERV_ADMIN))
#define IsSupportHelper(user) (user->handle_info && HANDLE_FLAGGED(user->handle_info, SUPPORT_HELPER))
#define IsNetworkHelper(user) (user->handle_info && HANDLE_FLAGGED(user->handle_info, NETWORK_HELPER))
#define IsHelper(user) (IsSupportHelper(user) || IsNetworkHelper(user))
#define IsHelping(user) (user->handle_info && HANDLE_FLAGGED(user->handle_info, HELPING))
#define IsStaff(user) (IsOper(user) || IsAdminServOper(user) || IsAdminServAdmin(user) || IsSupportHelper(user) || IsNetworkHelper(user))
#define IsBot(user) (user->handle_info && HANDLE_FLAGGED(user->handle_info, BOT))

enum cookie_type {
    ACTIVATION,
    PASSWORD_CHANGE,
    EMAIL_CHANGE,
    ALLOWAUTH
};

struct handle_cookie {
    struct handle_info *hi;
    char *data;
    enum cookie_type type;
    time_t expires;
    char cookie[COOKIELEN+1];
};

struct handle_note {
    char            setter[NICKSERV_HANDLE_LEN+1];
    time_t          date;
    char            note[1];
};

struct handle_info {
    struct nick_info *nicks;
    struct string_list *masks;
    struct string_list *sslfps;
    struct string_list *ignores;
    struct userNode *users;
    struct userData *channels;
    struct handle_cookie *cookie;
    struct handle_note *note;
    struct language *language;
    char *email_addr;
    char *epithet;
    char *infoline;
    char *handle;
    char *fakehost;
    time_t registered;
    time_t lastseen;
    int karma;
    unsigned short flags;
    unsigned short opserv_level;
    unsigned short screen_width;
    unsigned short table_width;
    unsigned char userlist_style;
    unsigned char announcements;
    unsigned char maxlogins;
    char passwd[MD5_CRYPT_LENGTH+1];
    char last_quit_host[USERLEN+HOSTLEN+2];
};

struct nick_info {
    struct handle_info *owner;
    struct nick_info *next; /* next nick owned by same handle */
    char nick[NICKLEN+1];
    time_t registered;
    time_t lastseen;
};

struct handle_info_list {
    unsigned int used, size;
    struct handle_info **list;
    char *tag; /* e.g. email address */
};

extern const char *handle_flags;

enum reclaim_action {
    RECLAIM_NONE,
    RECLAIM_WARN,
    RECLAIM_SVSNICK,
    RECLAIM_KILL
};

struct nickserv_config {
    unsigned int disable_nicks : 1;
    unsigned int valid_handle_regex_set : 1;
    unsigned int valid_nick_regex_set : 1;
    unsigned int valid_fakehost_regex_set : 1;
    unsigned int autogag_enabled : 1;
    unsigned int email_enabled : 1;
    unsigned int email_required : 1;
    unsigned int default_hostmask : 1;
    unsigned int warn_nick_owned : 1;
    unsigned int warn_clone_auth : 1;
    unsigned int sync_log : 1;
    unsigned int expire_nicks : 1;
    unsigned long nicks_per_handle;
    unsigned long password_min_length;
    unsigned long password_min_digits;
    unsigned long password_min_upper;
    unsigned long password_min_lower;
    unsigned long db_backup_frequency;
    unsigned long handle_expire_frequency;
    unsigned long autogag_duration;
    unsigned long email_visible_level;
    unsigned long cookie_timeout;
    unsigned long handle_expire_delay;
    unsigned long nochan_handle_expire_delay;
    unsigned long modoper_level;
    unsigned long set_epithet_level;
    unsigned long set_title_level;
    unsigned long set_fakehost_level;
    unsigned long handles_per_email;
    unsigned long email_search_level;
    unsigned long nick_expire_frequency;
    unsigned long nick_expire_delay;
    const char *network_name;
    const char *titlehost_suffix;
    regex_t valid_handle_regex;
    regex_t valid_nick_regex;
    regex_t valid_fakehost_regex;
    dict_t weak_password_dict;
    struct policer_params *auth_policer_params;
    enum reclaim_action reclaim_action;
    enum reclaim_action auto_reclaim_action;
    unsigned long auto_reclaim_delay;
    unsigned char default_maxlogins;
    unsigned char hard_maxlogins;
    unsigned long ounregister_inactive;
    unsigned long ounregister_flags;
    const char *auto_oper;
    const char *auto_admin;
    const char *auto_oper_privs;
    const char *auto_admin_privs;
    char default_style;
    struct string_list *denied_fakehost_words;
    unsigned int force_handles_lowercase;
    unsigned int ldap_enable;
#ifdef WITH_LDAP
    const char *ldap_uri;
    const char *ldap_base;
    const char *ldap_dn_fmt;
    unsigned int ldap_version;
    unsigned int ldap_autocreate;

    const char *ldap_admin_dn;
    const char *ldap_admin_pass;
    const char *ldap_field_account;
    const char *ldap_field_password;
    const char *ldap_field_email;
    const char *ldap_field_oslevel;
    struct string_list *ldap_object_classes;
    const char *ldap_oper_group_dn;
    unsigned int ldap_oper_group_level;
    const char *ldap_field_group_member;
    unsigned int ldap_timeout;
#endif
};

void init_nickserv(const char *nick);
struct handle_info *get_handle_info(const char *handle);
struct handle_info *smart_get_handle_info(struct userNode *service, struct userNode *user, const char *name);
int oper_try_set_access(struct userNode *user, struct userNode *bot, struct handle_info *target, unsigned int new_level);
int oper_outranks(struct userNode *user, struct handle_info *hi);
struct nick_info *get_nick_info(const char *nick);
struct modeNode *find_handle_in_channel(struct chanNode *channel, struct handle_info *handle, struct userNode *except);
int nickserv_modify_handle_flags(struct userNode *user, struct userNode *bot, const char *str, unsigned long *add, unsigned long *remove);
int oper_has_access(struct userNode *user, struct userNode *bot, unsigned int min_level, unsigned int quiet);
void nickserv_show_oper_accounts(struct userNode *user, struct svccmd *cmd);

struct handle_info *get_victim_oper(struct userNode *user, const char *target);
struct handle_info *loc_auth(char *sslfp, char *handle, char *password, char *userhost);

typedef void (*user_mode_func_t)(struct userNode *user, const char *mode_change, void *extra);
void reg_user_mode_func(user_mode_func_t func, void *extra);
typedef void (*channel_mode_func_t)(struct userNode *who, struct chanNode *channel, char **mode, unsigned int argc, void *extra);
void reg_channel_mode_func(channel_mode_func_t func, void *extra);

/* auth_funcs are called when a user gets a new handle_info.  They are
 * called *after* user->handle_info has been updated.  */
typedef void (*auth_func_t)(struct userNode *user, struct handle_info *old_handle, void *extra);
void reg_auth_func(auth_func_t func, void *extra);

/* Called just after a handle is renamed. */
typedef void (*handle_rename_func_t)(struct handle_info *handle, const char *old_handle, void *extra);
void reg_handle_rename_func(handle_rename_func_t func, void *extra);

/* unreg_funcs are called right before a handle is unregistered.
 * `user' is the person who caused the handle to be unregistered (either a
 * client authed to the handle, or an oper). */
typedef void (*unreg_func_t)(struct userNode *user, struct handle_info *handle, void *extra);
void reg_unreg_func(unreg_func_t func, void *extra);

/* Called just before a handle is merged */
typedef void (*handle_merge_func_t)(struct userNode *user, struct handle_info *handle_to, struct handle_info *handle_from, void *extra);
void reg_handle_merge_func(handle_merge_func_t, void *extra);

/* Called after an allowauth. handle is null if allowauth authorization was
 * removed */
typedef void (*allowauth_func_t)(struct userNode *user, struct userNode *target, struct handle_info *handle, void *extra);
void reg_allowauth_func(allowauth_func_t func, void *extra);

/* Called when an auth attempt fails because of a bad password */
typedef void (*failpw_func_t)(struct userNode *user, struct handle_info *handle, void *extra);
void reg_failpw_func(failpw_func_t func, void *extra);

void send_func_list(struct userNode *user);

extern dict_t nickserv_handle_dict;

#endif
