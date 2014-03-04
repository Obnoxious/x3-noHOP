/* proto-p10.c - IRC protocol output
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

#include "nickserv.h"
#include "chanserv.h"
#include "hash.h"
#include "helpfile.h"
#include "hosthiding.h"
#include "proto-common.c"
#include "opserv.h"

/* Full commands. */
#define CMD_ACCOUNT		"ACCOUNT"
#define CMD_ADMIN               "ADMIN"
#define CMD_ALIST               "ALIST"
#define CMD_ASLL		"ASLL"
#define CMD_AWAY                "AWAY"
#define CMD_BURST               "BURST"
#define CMD_CLEARMODE           "CLEARMODE"
#define CMD_CLOSE               "CLOSE"
#define CMD_CNOTICE             "CNOTICE"
#define CMD_CONNECT             "CONNECT"
#define CMD_CPRIVMSG            "CPRIVMSG"
#define CMD_CREATE              "CREATE"
#define CMD_DESTRUCT            "DESTRUCT"
#define CMD_DESYNCH             "DESYNCH"
#define CMD_DIE                 "DIE"
#define CMD_DNS                 "DNS"
#define CMD_EOB                 "END_OF_BURST"
#define CMD_EOB_ACK             "EOB_ACK"
#define CMD_ERROR               "ERROR"
#define CMD_FAKEHOST            "FAKE"
#define CMD_GET			"GET"
#define CMD_GLINE               "GLINE"
#define CMD_HASH                "HASH"
#define CMD_HELP                "HELP"
#define CMD_INFO                "INFO"
#define CMD_INVITE              "INVITE"
#define CMD_ISON                "ISON"
#define CMD_JOIN                "JOIN"
#define CMD_JUPE                "JUPE"
#define CMD_KICK                "KICK"
#define CMD_KILL                "KILL"
#define CMD_LINKS               "LINKS"
#define CMD_LIST                "LIST"
#define CMD_LUSERS              "LUSERS"
#define CMD_MAP                 "MAP"
#define CMD_MARK                "MARK"
#define CMD_MODE                "MODE"
#define CMD_MOTD                "MOTD"
#define CMD_NAMES               "NAMES"
#define CMD_NICK                "NICK"
#define CMD_NOTICE              "NOTICE"
#define CMD_OPER                "OPER"
#define CMD_OPMODE              "OPMODE"
#define CMD_PART                "PART"
#define CMD_PASS                "PASS"
#define CMD_PING                "PING"
#define CMD_PONG                "PONG"
#define CMD_POST                "POST"
#define CMD_PRIVMSG             "PRIVMSG"
#define CMD_PRIVS		"PRIVS"
#define CMD_PROTO               "PROTO"
#define CMD_QUIT                "QUIT"
#define CMD_REHASH              "REHASH"
#define CMD_REMOVE		"REMOVE"
#define CMD_RESET		"RESET"
#define CMD_RESTART             "RESTART"
#define CMD_RPING               "RPING"
#define CMD_RPONG               "RPONG"
#define CMD_SASL                "SASL"
#define CMD_SERVER              "SERVER"
#define CMD_SERVLIST            "SERVLIST"
#define CMD_SERVSET             "SERVSET"
#define CMD_SET			"SET"
#define CMD_SETTIME             "SETTIME"
#define CMD_SGLINE              "SGLINE"
#define CMD_SHUN		"SHUN"
#define CMD_SILENCE             "SILENCE"
#define CMD_SMO                 "SMO"
#define CMD_SNO                 "SNO"
#define CMD_SSHUN		"SSHUN"
#define CMD_SPAMFILTER          "SPAMFILTER"
#define CMD_SQUERY              "SQUERY"
#define CMD_SQUIT               "SQUIT"
#define CMD_STATS               "STATS"
#define CMD_SVSJOIN             "SVSJOIN"
#define CMD_SVSNICK             "SVSNICK"
#define CMD_SVSPART             "SVSPART"
#define CMD_SVSQUIT             "SVSQUIT"
#define CMD_SWHOIS              "SWHOIS"
#define CMD_TIME                "TIME"
#define CMD_TOPIC               "TOPIC"
#define CMD_TRACE               "TRACE"
#define CMD_UPING               "UPING"
#define CMD_USER                "USER"
#define CMD_USERHOST            "USERHOST"
#define CMD_USERIP              "USERIP"
#define CMD_VERSION             "VERSION"
#define CMD_WALLCHOPS           "WALLCHOPS"
#define CMD_WALLOPS             "WALLOPS"
#define CMD_WALLHOPS            "WALLHOPS"
#define CMD_WALLUSERS           "WALLUSERS"
#define CMD_WALLVOICES          "WALLVOICES"
#define CMD_WALLHOPS            "WALLHOPS"
#define CMD_WHO                 "WHO"
#define CMD_WHOIS               "WHOIS"
#define CMD_WHOWAS              "WHOWAS"
#define CMD_ZLINE		"ZLINE"

/* Tokenized commands. */
#define TOK_ACCOUNT		"AC"
#define TOK_ADMIN               "AD"
#define TOK_ALIST               "AL"
#define TOK_ASLL		"LL"
#define TOK_AWAY                "A"
#define TOK_BURST               "B"
#define TOK_CLEARMODE           "CM"
#define TOK_CLOSE               "CLOSE"
#define TOK_CNOTICE             "CN"
#define TOK_CONNECT             "CO"
#define TOK_CPRIVMSG            "CP"
#define TOK_CREATE              "C"
#define TOK_DESTRUCT            "DE"
#define TOK_DESYNCH             "DS"
#define TOK_DIE                 "DIE"
#define TOK_DNS                 "DNS"
#define TOK_EOB                 "EB"
#define TOK_EOB_ACK             "EA"
#define TOK_ERROR               "Y"
#define TOK_EXEMPT		"EX"
#define TOK_FAKEHOST            "FA"
#define TOK_GET			"GET"
#define TOK_GLINE               "GL"
#define TOK_HASH                "HASH"
#define TOK_HELP                "HELP"
#define TOK_INFO                "F"
#define TOK_INVITE              "I"
#define TOK_ISON                "ISON"
#define TOK_JOIN                "J"
#define TOK_JUPE                "JU"
#define TOK_KICK                "K"
#define TOK_KILL                "D"
#define TOK_LINKS               "LI"
#define TOK_LIST                "LIST"
#define TOK_LUSERS              "LU"
#define TOK_MAP                 "MAP"
#define TOK_MARK                "MK"
#define TOK_MODE                "M"
#define TOK_MOTD                "MO"
#define TOK_NAMES               "E"
#define TOK_NICK                "N"
#define TOK_NOTICE              "O"
#define TOK_OPER                "OPER"
#define TOK_OPMODE              "OM"
#define TOK_PART                "L"
#define TOK_PASS                "PA"
#define TOK_PING                "G"
#define TOK_PONG                "Z"
#define TOK_POST                "POST"
#define TOK_PRIVMSG             "P"
#define TOK_PRIVS		"PRIVS"
#define TOK_PROTO               "PROTO"
#define TOK_QUIT                "Q"
#define TOK_REHASH              "REHASH"
#define TOK_REMOVE		"RM"
#define TOK_RESET		"RESET"
#define TOK_RESTART             "RESTART"
#define TOK_RPING               "RI"
#define TOK_RPONG               "RO"
#define TOK_SASL                "SASL"
#define TOK_SERVER              "S"
#define TOK_SERVLIST            "SERVSET"
#define TOK_SERVSET             "SERVSET"
#define TOK_SET			"SET"
#define TOK_SETTIME             "SE"
#define TOK_SGLINE              "SGL"
#define TOK_SHUN		"SU"
#define TOK_SILENCE             "U"
#define TOK_SMO                 "SMO"
#define TOK_SNO                 "SNO"
#define TOK_SSHUN		"SSU"
#define TOK_SPAMFILTER          "SF"
#define TOK_SQUERY              "SQUERY"
#define TOK_SQUIT               "SQ"
#define TOK_STATS               "R"
#define TOK_SVSJOIN             "SJ"
#define TOK_SVSNICK             "SN"
#define TOK_SVSPART             "SP"
#define TOK_SVSQUIT             "SX"
#define TOK_SWHOIS              "SW"
#define TOK_TIME                "TI"
#define TOK_TOPIC               "T"
#define TOK_TRACE               "TR"
#define TOK_UPING               "UP"
#define TOK_USER                "USER"
#define TOK_USERHOST            "USERHOST"
#define TOK_USERIP              "USERIP"
#define TOK_VERSION             "V"
#define TOK_WALLCHOPS           "WC"
#define TOK_WALLOPS             "WA"
#define TOK_WALLHOPS            "WH"
#define TOK_WALLUSERS           "WU"
#define TOK_WALLVOICES          "WV"
#define TOK_WALLHOPS            "WH"
#define TOK_WHO                 "H"
#define TOK_WHOIS               "W"
#define TOK_WHOWAS              "X"
#define TOK_ZLINE		"ZL"

/* Protocol messages; aliased to full commands or tokens depending
   on compile-time configuration. ircu prefers tokens WITH THE
   EXCEPTION OF THE SERVER AND PASS COMMANDS, which cannot be
   tokenized, because clients' (ie. a linking server) commands are
   only checked against the full command list.
*/
#if defined(ENABLE_TOKENS)
#define TYPE(NAME)              TOK_ ## NAME
#else /* !ENABLE_TOKENS */
#define TYPE(NAME)              CMD_ ## NAME
#endif /* ENABLE_TOKENS */

#define P10_ACCOUNT		TYPE(ACCOUNT)
#define P10_ADMIN               TYPE(ADMIN)
#define P10_ASLL		TYPE(ASLL)
#define P10_AWAY                TYPE(AWAY)
#define P10_BURST               TYPE(BURST)
#define P10_CLEARMODE           TYPE(CLEARMODE)
#define P10_CLOSE               TYPE(CLOSE)
#define P10_CNOTICE             TYPE(CNOTICE)
#define P10_CONNECT             TYPE(CONNECT)
#define P10_CPRIVMSG            TYPE(CPRIVMSG)
#define P10_CREATE              TYPE(CREATE)
#define P10_DESTRUCT            TYPE(DESTRUCT)
#define P10_DESYNCH             TYPE(DESYNCH)
#define P10_DIE                 TYPE(DIE)
#define P10_DNS                 TYPE(DNS)
#define P10_EOB                 TYPE(EOB)
#define P10_EOB_ACK             TYPE(EOB_ACK)
#define P10_ERROR               TYPE(ERROR)
#define P10_FAKEHOST            TYPE(FAKEHOST)
#define P10_GET			TYPE(GET)
#define P10_GLINE               TYPE(GLINE)
#define P10_HASH                TYPE(HASH)
#define P10_HELP                TYPE(HELP)
#define P10_INFO                TYPE(INFO)
#define P10_INVITE              TYPE(INVITE)
#define P10_ISON                TYPE(ISON)
#define P10_JOIN                TYPE(JOIN)
#define P10_JUPE                TYPE(JUPE)
#define P10_KICK                TYPE(KICK)
#define P10_KILL                TYPE(KILL)
#define P10_LINKS               TYPE(LINKS)
#define P10_LIST                TYPE(LIST)
#define P10_LUSERS              TYPE(LUSERS)
#define P10_MAP                 TYPE(MAP)
#define P10_MARK                TYPE(MARK)
#define P10_MODE                TYPE(MODE)
#define P10_MOTD                TYPE(MOTD)
#define P10_NAMES               TYPE(NAMES)
#define P10_NICK                TYPE(NICK)
#define P10_NOTICE              TYPE(NOTICE)
#define P10_OPER                TYPE(OPER)
#define P10_OPMODE              TYPE(OPMODE)
#define P10_PART                TYPE(PART)
#define P10_PASS                CMD_PASS
#define P10_PING                TYPE(PING)
#define P10_PONG                TYPE(PONG)
#define P10_POST                TYPE(POST)
#define P10_PRIVMSG             TYPE(PRIVMSG)
#define P10_PRIVS		TYPE(PRIVS)
#define P10_PROTO               TYPE(PROTO)
#define P10_QUIT                TYPE(QUIT)
#define P10_REHASH              TYPE(REHASH)
#define P10_REMOVE		TYPE(REMOVE)
#define P10_RESET		TYPE(RESET)
#define P10_RESTART             TYPE(RESTART)
#define P10_RPING               TYPE(RPING)
#define P10_RPONG               TYPE(RPONG)
#define P10_SASL                TYPE(SASL)
#define P10_SERVER              CMD_SERVER
#define P10_SERVLIST            TYPE(SERVLIST)
#define P10_SERVSET             TYPE(SERVSET)
#define P10_SET			TYPE(SET)
#define P10_SETTIME             TYPE(SETTIME)
#define P10_SGLINE              TYPE(SGLINE)
#define P10_SHUN		TYPE(SHUN)
#define P10_SILENCE             TYPE(SILENCE)
#define P10_SMO                 TYPE(SMO)
#define P10_SNO                 TYPE(SNO)
#define P10_SSHUN		TYPE(SSHUN)
#define P10_SPAMFILTER          TYPE(SPAMFILTER)
#define P10_SQUERY              TYPE(SQUERY)
#define P10_SQUIT               TYPE(SQUIT)
#define P10_STATS               TYPE(STATS)
#define P10_SVSJOIN             TYPE(SVSJOIN)
#define P10_SVSNICK             TYPE(SVSNICK)
#define P10_SVSPART             TYPE(SVSPART)
#define P10_SVSQUIT		TYPE(SVSQUIT)
#define P10_SWHOIS              TYPE(SWHOIS)
#define P10_TIME                TYPE(TIME)
#define P10_TOPIC               TYPE(TOPIC)
#define P10_TRACE               TYPE(TRACE)
#define P10_UPING               TYPE(UPING)
#define P10_USER                TYPE(USER)
#define P10_USERHOST            TYPE(USERHOST)
#define P10_USERIP              TYPE(USERIP)
#define P10_VERSION             TYPE(VERSION)
#define P10_WALLCHOPS           TYPE(WALLCHOPS)
#define P10_WALLOPS             TYPE(WALLOPS)
#define P10_WALLHOPS            TYPE(WALLHOPS)
#define P10_WALLUSERS           TYPE(WALLUSERS)
#define P10_WALLVOICES          TYPE(WALLVOICES)
#define P10_WHO                 TYPE(WHO)
#define P10_WHOIS               TYPE(WHOIS)
#define P10_WHOWAS              TYPE(WHOWAS)
#define P10_ZLINE		TYPE(ZLINE)
#define P10_EXEMPT		TYPE(EXEMPT)

/* Servers claiming to have a boot or link time before PREHISTORY
 * trigger errors to the log.  We hope no server has been running
 * constantly since September 1994.  :)
 */
#define PREHISTORY 780000000

#define MODELEN                 40 + KEYLEN

static struct server *servers_num[64*64];
static privmsg_func_t *privmsg_funcs;
static unsigned int num_privmsg_funcs;
static privmsg_func_t *notice_funcs;
static unsigned int num_notice_funcs;
static struct dict *unbursted_channels;
static const char *his_servername;
static const char *his_servercomment;
static int extended_accounts;

/* These correspond to 1 << X:      01234567890123456789012345 */
const char irc_user_mode_chars[] = "oOiw dkgh    x  BnIX  azDRWHLq";

static struct userNode *AddUser(struct server* uplink, const char *nick, const char *ident, const char *hostname, const char *modes, const char *numeric, const char *userinfo, time_t timestamp, const char *realip);

extern int off_channel;
extern int DefConLevel;
extern int DefConTimeOut;
extern char *DefConChanModes;

static int parse_oplevel(char *str);

char privbuf[512] = "";

/* Numerics can be XYY, XYYY, or XXYYY; with X's identifying the
 * server and Y's indentifying the client on that server. */
struct server*
GetServerN(const char *numeric)
{
    switch (strlen(numeric)) {
    default:
        return servers_num[base64toint(numeric, 2)];
    case 4:
    case 3:
    case 1:
        return servers_num[base64toint(numeric, 1)];
    case 0:
        return NULL;
    }
}

struct userNode*
GetUserN(const char *numeric) /* using numeric */
{
    struct userNode *un;
    struct server *s;
    int n, slen, ulen;

    switch (strlen(numeric)) {
    default:
        log_module(MAIN_LOG, LOG_WARNING, "GetUserN(%s): numeric too long!", numeric);
        return NULL;
    case 5: slen = 2; ulen = 3; break;
    case 4: slen = 1; ulen = 3; break;
    case 3: slen = 1; ulen = 2; break;
    case 2: case 1: case 0:
        log_module(MAIN_LOG, LOG_WARNING, "GetUserN(%s): numeric too short!", numeric);
        return NULL;
    }
    if (!(s = servers_num[base64toint(numeric, slen)])) {
        log_module(MAIN_LOG, LOG_WARNING, "GetUserN(%s): couldn't find server (len=%d)!", numeric, slen);
        return NULL;
    }
    n = base64toint(numeric+slen, ulen) & s->num_mask;
    if (!(un = s->users[n])) {
        log_module(MAIN_LOG, LOG_WARNING, "GetUserN(%s) couldn't find user!", numeric);
    }
    return un;
}

extern struct userNode *opserv;
static void
check_ctcp(struct userNode *user, struct userNode *bot, char *text, UNUSED_ARG(int server_qualified))
{
    char *cmd;
    /* if its a version reply, do an alert check (only alerts with version=something) */
    if(bot == opserv) {
        if(text[0] == '\001') {
            text++;
            cmd = mysep(&text, " ");
            if(!irccasecmp(cmd, "VERSION")) {
                char *version = mysep(&text, "\n");
                if(!version)
                    version = "";
                /* opserv_debug("Opserv got CTCP VERSION Notice from %s: %s", user->nick, version); */
                /* TODO: setup a ctcp_funcs thing to handle this and other CTCPS properly */
                user->version_reply = strdup(version);
                /* TODO: put this in the db */
                if(match_ircglob(version, "WebTV;*"))
                    user->no_notice = true; /* webbies cant see notices */
            }
        }
    }
}


static void
privmsg_user_helper(struct userNode *un, void *data)
{
    struct privmsg_desc *pd = data;
    unsigned int num = un->num_local;
    if (!pd->is_notice) {
        if ((num < num_privmsg_funcs) && privmsg_funcs[num]) {
            privmsg_funcs[num](pd->user, un, pd->text, pd->is_qualified);
        }
    } else {
        if ((num < num_notice_funcs) && notice_funcs[num]) {
            check_ctcp(pd->user, un, pd->text, pd->is_qualified);
            notice_funcs[num](pd->user, un, pd->text, pd->is_qualified);
        }
    }
}

/* equiv to user doing /connect server port target */
void irc_connect(struct userNode *user, char *server, unsigned int port, struct server *target)
{
    putsock("%s " P10_CONNECT " %s %d %s", user->numeric, server, port, target->numeric);
}

void
irc_squit_route(struct server *srv, const char *message, ...)
{
    va_list arg_list;
    char buffer[MAXLEN];
    va_start(arg_list, message);
    vsnprintf(buffer, MAXLEN-2, message, arg_list);
    buffer[MAXLEN-1] = 0;

    /* When would we squit ourselves exactly?? -Rubin */
    if(srv == self && cManager.uplink->state == CONNECTED ) {
        unsigned int i;

        /* Quit all clients linked to me. */
        for(i = 0; i <= self->num_mask; i++) {
            if(!self->users[i])
                continue;
            irc_quit(self->users[i], buffer);
        }
    }

    putsock("%s " P10_SQUIT " %s %d :%s", self->numeric, srv->name, 0, buffer);

    if(srv == self) {
        /* Force a reconnect to the currently selected server. */
        cManager.uplink->tries = 0;
        log_module(MAIN_LOG, LOG_INFO, "Squitting from uplink: %s", buffer);
        close_socket();
    }
}

void
irc_server(struct server *srv)
{
    char extranum[COMBO_NUMERIC_LEN+1];

    inttobase64(extranum, srv->num_mask, (srv->numeric[1] || (srv->num_mask >= 64*64)) ? 3 : 2);
    if (srv == self) {
        putsock(P10_SERVER " %s %d " FMT_TIME_T " " FMT_TIME_T " J10 %s%s +s6o :%s",
                srv->name, srv->hops+1, srv->boot, srv->link_time, srv->numeric, extranum, srv->description);
    } else {
        putsock("%s " P10_SERVER " %s %d " FMT_TIME_T " " FMT_TIME_T " %c10 %s%s +s6o :%s",
                self->numeric, srv->name, srv->hops+1, srv->boot, srv->link_time, (srv->self_burst ? 'J' : 'P'), srv->numeric, extranum, srv->description);
    }
}

void
irc_p10_pton(irc_in_addr_t *ip, const char *input)
{
    if (strlen(input) == 6) {
        unsigned int value;
        memset(ip, 0, 6 * sizeof(ip->in6[0]));
        value = base64toint(input, 6);
        if (value)
            ip->in6[5] = htons(65535);
        ip->in6[6] = htons(value >> 16);
        ip->in6[7] = htons(value & 65535);
    } else {
        unsigned int pos = 0;
        do {
            if (*input == '_') {
                unsigned int left;
                for (left = (25 - strlen(input)) / 3 - pos; left; left--)
                    ip->in6[pos++] = 0;
                input++;
            } else {
                ip->in6[pos++] = ntohs(base64toint(input, 3));
                input += 3;
            }
        } while (pos < 8);
    }
}

void
irc_p10_ntop(char *output, const irc_in_addr_t *ip)
{
    if (!irc_in_addr_is_valid(*ip)) {
        strcpy(output, "AAAAAA");
    } else if (irc_in_addr_is_ipv4(*ip)) {
        unsigned int in4;
        in4 = (ntohs(ip->in6[6]) << 16) | ntohs(ip->in6[7]);
        inttobase64(output, in4, 6);
        output[6] = '\0';
    } else if (irc_in_addr_is_ipv6(*ip)) {
        unsigned int max_start, max_zeros, curr_zeros, zero, ii;
        /* Can start by printing out the leading non-zero parts. */
        for (ii = 0; (ip->in6[ii]) && (ii < 8); ++ii) {
            inttobase64(output, ntohs(ip->in6[ii]), 3);
            output += 3;
        }
        /* Find the longest run of zeros. */
        for (max_start = zero = ii, max_zeros = curr_zeros = 0; ii < 8; ++ii) {
            if (!ip->in6[ii])
                curr_zeros++;
            else if (curr_zeros > max_zeros) {
                max_start = ii - curr_zeros;
                max_zeros = curr_zeros;
                curr_zeros = 0;
            }
        }
        if (curr_zeros > max_zeros) {
            max_start = ii - curr_zeros;
            max_zeros = curr_zeros;
            curr_zeros = 0;
        }
        /* Print the rest of the address */
        for (ii = zero; ii < 8; ) {
            if ((ii == max_start) && max_zeros) {
                *output++ = '_';
                ii += max_zeros;
            } else {
                inttobase64(output, ntohs(ip->in6[ii]), 3);
                output += 3;
            }
        }
        *output = '\0';
    } else {
        strcpy(output, "???");
    }
}

void
irc_user(struct userNode *user)
{
    char b64ip[25];
    if (!user || IsDummy(user))
        return;
    irc_p10_ntop(b64ip, &user->ip);
    if (user->modes) {
        char modes[32];

        irc_user_modes(user, modes, sizeof(modes));
        /* we don't need to put the + in modes because it's in the format string. */
        putsock("%s " P10_NICK " %s %d " FMT_TIME_T " %s %s +%s %s %s :%s",
                user->uplink->numeric, user->nick, user->uplink->hops+1, user->timestamp, user->ident, user->hostname, modes, b64ip, user->numeric, user->info);
    } else {
        putsock("%s " P10_NICK " %s %d " FMT_TIME_T " %s %s %s %s :%s",
                user->uplink->numeric, user->nick, user->uplink->hops+1, user->timestamp, user->ident, user->hostname, b64ip, user->numeric, user->info);
    }
}

void
irc_rename(struct userNode *user, const char *new_handle)
{
    if(extended_accounts)
        putsock("%s " P10_ACCOUNT " %s M %s", self->numeric, user->numeric, new_handle);
}

void
irc_delete(struct userNode *user)
{
    if(extended_accounts)
        putsock("%s " P10_ACCOUNT " %s U", self->numeric, user->numeric);
}

void
irc_account(struct userNode *user, const char *stamp, time_t timestamp)
{
    if(extended_accounts)
        putsock("%s " P10_ACCOUNT " %s R %s "FMT_TIME_T, self->numeric, user->numeric, stamp, timestamp); 
    else
        putsock("%s " P10_ACCOUNT " %s %s "FMT_TIME_T, self->numeric, user->numeric, stamp, timestamp);
}

void
irc_fakehost(struct userNode *user, const char *host)
{
    putsock("%s " P10_FAKEHOST " %s %s", self->numeric, user->numeric, host);
}

void
irc_regnick(UNUSED_ARG(struct userNode *user))
{
    /* no operation here */
}

void
irc_nick(struct userNode *user, UNUSED_ARG(const char *old_nick))
{
    putsock("%s " P10_NICK " %s "FMT_TIME_T, user->numeric, user->nick, now);
}

void
irc_fetchtopic(struct userNode *from, const char *to)
{
    if (!from || !to)
        return;
    putsock("%s " P10_TOPIC " %s", from->numeric, to);
}

void
irc_squit(struct server *srv, const char *message, const char *service_message)
{
    if (!service_message)
        service_message = message;

    /* Are we leaving the network? */
    if (srv == self && cManager.uplink->state == CONNECTED) {
        unsigned int i;

        /* Quit all clients linked to me. */
        for (i = 0; i <= self->num_mask; i++) {
            if (!self->users[i])
                continue;
            irc_quit(self->users[i], service_message);
        }
    }

    putsock("%s " P10_SQUIT " %s %d :%s", self->numeric, srv->name, 0, message);

    if (srv == self) {
        /* Force a reconnect to the currently selected server. */
        cManager.uplink->tries = 0;
        log_module(MAIN_LOG, LOG_INFO, "Squitting from uplink: %s", message);
        close_socket();
    }
}

void
irc_wallchops(struct userNode *from, const char *to, const char *message)
{
    putsock("%s " P10_WALLCHOPS " %s :%s", from->numeric, to, message);
}

void
irc_wallops(const char *format, ...)
{
    va_list arg_list;
    char buffer[MAXLEN];
    va_start(arg_list, format);
    vsnprintf(buffer, MAXLEN-2, format, arg_list);
    buffer[MAXLEN-1] = 0;
    putsock("%s " P10_WALLOPS " :%s", self->numeric, buffer);
}

static int
deliver_to_dummy(struct userNode *source, struct userNode *dest, const char *message, int type)
{
    unsigned int num;

    if (!dest || !IsDummy(dest) || !IsLocal(dest))
        return 0;
    num = dest->num_local;
    switch (type) {
    default:
        if ((num < num_notice_funcs) && notice_funcs[num])
            notice_funcs[num](source, dest, message, 0);
        break;
    case 1:
        if ((num < num_privmsg_funcs) && privmsg_funcs[num])
            privmsg_funcs[num](source, dest, message, 0);
        break;
    }
    return 1;
}

void
irc_notice(struct userNode *from, const char *to, const char *message)
{
    if (to[0] == '#' || to[0] == '$'
        || !deliver_to_dummy(from, GetUserN(to), message, 0))
        putsock("%s " P10_NOTICE " %s :%s", from->numeric, to, message);
}

void
irc_notice_user(struct userNode *from, struct userNode *to, const char *message)
{
    if (!deliver_to_dummy(from, to, message, 0))
        putsock("%s " P10_NOTICE " %s :%s", from->numeric, to->numeric, message);
}

void
irc_privmsg(struct userNode *from, const char *to, const char *message)
{
    if (to[0] == '#' || to[0] == '$'
        || !deliver_to_dummy(from, GetUserN(to), message, 1))
        putsock("%s " P10_PRIVMSG " %s :%s", from->numeric, to, message);
}

void
irc_privmsg_user(struct userNode *from, struct userNode *to, const char *message)
{
    putsock("%s " P10_PRIVMSG " %s :%s", from->numeric, to->numeric, message);
}

void 
irc_version_user(struct userNode *from, struct userNode *to)
{
    irc_privmsg_user(from, to, "\001VERSION\001");
}

void
irc_eob(void)
{
    putsock("%s " P10_EOB, self->numeric);
}

void
irc_eob_ack(void)
{
    putsock("%s " P10_EOB_ACK, self->numeric);

    char *nick;
    const char *str;
    str = conf_get_data("services/opserv/nick", RECDB_QSTRING);
    nick = strdup(str);

    if (nick && (DefConLevel < 5)) {
        DefConProcess(GetUserH(nick));

        if (DefConTimeOut > 0)
            timeq_add(now + DefConTimeOut, defcon_timeout, NULL);
    }
}

void
irc_rpong(const char *from1, const char *from2, const char *pingtime, const char *clientinfo)
{
    putsock("%s " P10_RPONG " %s %s %s :%s", self->numeric, from1, from2, pingtime, clientinfo);
}

void
irc_ping(const char *payload)
{
    putsock("%s " P10_PING " :%s", self->numeric, payload);
}

void
irc_pong(const char *who, const char *data)
{
    putsock("%s " P10_PONG " %s :%s", self->numeric, who, data);
}

void
irc_pong_asll(const char *who, const char *orig_ts)
{
    char *delim;
    struct timeval orig;
    struct timeval sys_now;
    int diff;

    orig.tv_sec = strtoul(orig_ts, &delim, 10);
    orig.tv_usec = (*delim == '.') ? strtoul(delim + 1, NULL, 10) : 0;
    gettimeofday(&sys_now, NULL);
    diff = (sys_now.tv_sec - orig.tv_sec) * 1000 + (sys_now.tv_usec - orig.tv_usec) / 1000;
    putsock("%s " P10_PONG " %s %s %d %lu.%06lu", self->numeric, who, orig_ts, diff, (unsigned long)sys_now.tv_sec, (unsigned long)sys_now.tv_usec);
}

void
irc_pass(const char *passwd)
{
    putsock(P10_PASS " :%s", passwd);
}

void
irc_introduce(const char *passwd)
{
    void timed_send_ping(void *data);

    self->self_burst = self->burst = 1;
    irc_pass(passwd);
    irc_server(self);
    burst_length = 0;
    timeq_add(now + ping_freq, timed_send_ping, 0);
}

void
irc_gline(struct server *srv, struct gline *gline, int silent)
{
    putsock("%s " P10_GLINE " %s +%s " FMT_TIME_T " " FMT_TIME_T " :%s<%s> %s",
            self->numeric, (srv ? srv->numeric : "*"), gline->target, gline->expires-now, now, silent ? "AUTO " : "", gline->issuer, gline->reason);
}

void
irc_shun(struct server *srv, struct shun *shun)
{
    putsock("%s " P10_SHUN " %s +%s " FMT_TIME_T " " FMT_TIME_T " :%s<%s> %s",
            self->numeric, (srv ? srv->numeric : "*"), shun->target, shun->expires-now, shun->issuer, shun->reason);
}

void
irc_settime(const char *srv_name_mask, time_t new_time)
{
    ioset_set_time(new_time);
    if (!strcmp(srv_name_mask, "*"))
        srv_name_mask = "";
    putsock("%s " P10_SETTIME " " FMT_TIME_T " %s", self->numeric, new_time, srv_name_mask);
}

void
irc_ungline(const char *mask)
{
    putsock("%s " P10_GLINE " * -%s " FMT_TIME_T, self->numeric, mask, now);
}

void
irc_unshun(const char *mask)
{
    putsock("%s " P10_SHUN " * -%s", self->numeric, mask);
}

void
irc_burst(struct chanNode *chan)
{
    char burst_line[512];
    int pos, base_len, len;
    struct modeNode *mn;
    struct banNode *bn;
    struct exemptNode *en;
    long last_mode=-1;
    unsigned int first_ban;
    unsigned int first_exempt;
    unsigned int n;

    base_len = sprintf(burst_line, "%s " P10_BURST " %s " FMT_TIME_T " ",
                       self->numeric, chan->name, chan->timestamp);
    len = irc_make_chanmode(chan, burst_line+base_len);
    pos = base_len + len;
    if (len > 0 && chan->members.used > 0)
        burst_line[pos++] = ' ';

    if(chan->members.used < 1)
        return; /* dont burst empty channels (created by discrims) */
    /* dump the users */
    for (n=0; n<chan->members.used; n++) {
        mn = chan->members.list[n];
        if (pos > 500) {
            burst_line[pos-1] = 0; /* -1 to back up over the space or comma */
            putsock("%s", burst_line);
            pos = base_len;
            last_mode = -1;
        }

        memcpy(burst_line+pos, mn->user->numeric, strlen(mn->user->numeric));
        pos += strlen(mn->user->numeric);
        if (mn->modes && (mn->modes != last_mode)) {
            last_mode = mn->modes;
            burst_line[pos++] = ':';
            if (last_mode & MODE_CHANOP)
                burst_line[pos++] = 'o';
            if (last_mode & MODE_VOICE)
                burst_line[pos++] = 'v';
        }
        if ((n+1)<chan->members.used)
            burst_line[pos++] = ',';
    }

    if (len > 0 && (chan->banlist.used > 0 || chan->exemptlist.used > 0))
        burst_line[pos++] = ' ';

    if (chan->banlist.used) {
        /* dump the bans */
        first_ban = 1;

        for (n=0; n<chan->banlist.used; ) {
            if (first_ban && (pos < 500)) {
                burst_line[pos++] = ':';
                burst_line[pos++] = '%';
            }
            bn = chan->banlist.list[n];
            len = strlen(bn->ban);
            if (pos + 2 + len < 505) {
                memcpy(burst_line + pos, bn->ban, len);
                pos += len;
                burst_line[pos++] = ' ';
                first_ban = 0;
                n++;
            } else {
                burst_line[pos-1] = 0;
                putsock("%s", burst_line);
                pos = base_len;
                first_ban = 1;
            }
        }
    }

    if (chan->exemptlist.used) {
        /* dump the exempts */
        first_exempt = 1;

        for (n=0; n<chan->exemptlist.used; ) {
            if (first_exempt && (pos < 500)) {
                if (chan->banlist.used < 1) {
                    burst_line[pos++] = ':';
                    burst_line[pos++] = '%';
                    burst_line[pos++] = ' ';
                }
                burst_line[pos++] = '~';
                burst_line[pos++] = ' ';
            }
            en = chan->exemptlist.list[n];
            len = strlen(en->exempt);
            if (pos + 2 + len < 505) {
                memcpy(burst_line + pos, en->exempt, len);
                pos += len;
                burst_line[pos++] = ' ';
                first_exempt = 0;
                n++;
            } else {
                burst_line[pos-1] = 0;
                putsock("%s", burst_line);
                pos = base_len;
                first_exempt = 1;
            }
        }
    }
    /* print the last line */
    burst_line[pos] = 0;
    putsock("%s", burst_line);
}

void
irc_quit(struct userNode *user, const char *message)
{
    putsock("%s " P10_QUIT " :%s", user->numeric, message);
}

void
irc_error(const char *to, const char *message)
{
    if (to) {
        putsock("%s " P10_ERROR " :%s", to, message);
    } else {
        putsock(":%s " P10_ERROR " :%s", self->name, message);
    }
}

void
irc_kill(struct userNode *from, struct userNode *target, const char *message)
{
    if (from) {
        putsock("%s " P10_KILL " %s :%s!%s (%s)",
                from->numeric, target->numeric, self->name, from->nick, message);
    } else {
        putsock("%s " P10_KILL " %s :%s (%s)",
                self->numeric, target->numeric, self->name, message);
    }
}

void
irc_mode(struct userNode *from, struct chanNode *target, const char *modes)
{
    call_channel_mode_funcs(from, target, (char **)&modes, 0);
    putsock("%s " P10_MODE " %s %s "FMT_TIME_T,
            (from ? from->numeric : self->numeric),
            target->name, modes, target->timestamp);
}

/* Added to allow services to mode users
   2005 - 8 - 10 by Life4Christ
*/
void
irc_umode(struct userNode *target, const char *modes)
{
    putsock("%s " P10_MODE " %s %s ",self->numeric,target->nick, modes);
}


void
irc_invite(struct userNode *from, struct userNode *who, struct chanNode *to)
{
    putsock("%s " P10_INVITE " %s %s", from->numeric, who->nick, to->name);
}

void
irc_silence(struct userNode *who, const char *mask, int add)
{
    putsock("%s " P10_SILENCE " %s %s%s", self->numeric, who->numeric, add ? "+" : "-", mask);
}

void
irc_join(struct userNode *who, struct chanNode *what)
{
    if (what->members.used == 1) {
        putsock("%s " P10_CREATE " %s "FMT_TIME_T,
                who->numeric, what->name, what->timestamp);
    } else {
        putsock("%s " P10_JOIN " %s "FMT_TIME_T, who->numeric, what->name, what->timestamp);
    }
}

void
irc_svsjoin(struct userNode *from, struct userNode *who, struct chanNode *to)
{
    putsock("%s " P10_SVSJOIN " %s %s "FMT_TIME_T, from->uplink->numeric, who->numeric, to->name, now);
}

void
irc_svspart(struct userNode *from, struct userNode *who, struct chanNode *to)
{
    putsock("%s " P10_SVSPART " %s %s", from->uplink->numeric, who->numeric, to->name);
}

void
irc_svsquit(struct userNode *from, struct userNode *who, char const *reason)
{
    putsock("%s " P10_SVSQUIT " %s :%s", from->uplink->numeric, who->numeric, reason);
}

void
irc_kick(struct userNode *who, struct userNode *target, struct chanNode *channel, const char *msg)
{
    const char *numeric;
    struct modeNode *mn = GetUserMode(channel, who);
    numeric = ((mn && (mn->modes & MODE_CHANOP)) || off_channel) ? who->numeric : self->numeric;
    putsock("%s " P10_KICK " %s %s :%s",
            numeric, channel->name, target->numeric, msg);
}

void
irc_stats(struct userNode *from, struct server *target, char type)
{
    putsock("%s " P10_STATS " %c :%s", from->numeric, type, target->numeric);
}

void
irc_svsnick(struct userNode *from, struct userNode *target, const char *newnick)
{
    putsock("%s " P10_SVSNICK " %s %s "FMT_TIME_T, from->uplink->numeric, target->numeric, newnick, now);
}

void
irc_swhois(struct userNode *from, struct userNode *target, const char *message)
{
    putsock("%s " P10_SWHOIS " %s %s%s", from->uplink->numeric, target->numeric, message ? ":" : "",
                                         message ? message : "");

}

void
irc_part(struct userNode *who, struct chanNode *what, const char *reason)
{
    if (reason) {
        putsock("%s " P10_PART " %s :%s", who->numeric, what->name, reason);
    } else {
        putsock("%s " P10_PART " %s", who->numeric, what->name);
    }
}

void
irc_topic(struct userNode *service, struct userNode *who, struct chanNode *what, const char *topic)
{

   int type = 4, host_in_topic = 0, hasident = 0, hhtype = 0;
   const char *hstr, *tstr, *hhstr, *htstr;
   char *host, *hostmask;
   char shost[MAXLEN];
   char sident[MAXLEN];

   tstr = conf_get_data("server/type", RECDB_QSTRING);
   hstr = conf_get_data("server/host_in_topic", RECDB_QSTRING);
   hhstr = conf_get_data("server/hidden_host", RECDB_QSTRING);
   htstr = conf_get_data("server/hidden_host_type", RECDB_QSTRING);
   if(tstr)
     type = atoi(tstr);
   else
     type = 4;/* default to 040 style topics */
   if (htstr)
     hhtype = atoi(htstr);

   if (hstr) {
      if (IsHiddenHost(who) && IsFakeHost(who))
          safestrncpy(shost, who->fakehost, sizeof(shost));
      else if (IsHiddenHost(who) && IsSetHost(who)) {
          hostmask = strdup(who->sethost);
          if ((host = (strrchr(hostmask, '@')))) {
              hasident = 1;
              *host++ = '\0';
          } else
              host = hostmask;

          if (hasident)
              safestrncpy(sident, hostmask, sizeof(shost));
          else
              safestrncpy(sident, who->ident, sizeof(shost));

          safestrncpy(shost, host, sizeof(shost));
      } else if (IsHiddenHost(who) && ((hhtype == 1) || (hhtype == 3)) && who->handle_info && hhstr) {
          snprintf(shost, sizeof(shost), "%s.%s", who->handle_info->handle, hhstr);
      } else if (IsHiddenHost(who) && ((hhtype == 2) || (hhtype == 3)) && who->crypthost[0]) {
          safestrncpy(shost, who->crypthost, sizeof(shost));
      } else
          safestrncpy(shost, who->hostname, sizeof(shost));

      host_in_topic = atoi(hstr);
   }

   if (type >= 5) {
     putsock("%s " P10_TOPIC " %s %s%s%s%s%s " FMT_TIME_T " " FMT_TIME_T " :%s", service->numeric, what->name,
             who->nick, host_in_topic ? "!" : "", host_in_topic ? (IsSetHost(who) ? sident : who->ident) : "", 
             host_in_topic ? "@" : "", host_in_topic ? shost : "", what->timestamp, now, topic);
   } else {
     who = service;
     putsock("%s " P10_TOPIC " %s :%s", who->numeric, what->name, topic);
   }
}

void
irc_raw(const char *what)
{
    putsock("%s", what);
}

void
irc_numeric(struct userNode *user, unsigned int num, const char *format, ...)
{
    va_list arg_list;
    char buffer[MAXLEN];
    va_start(arg_list, format);
    vsnprintf(buffer, MAXLEN-2, format, arg_list);
    buffer[MAXLEN-1] = 0;
    putsock(":%s %03d %s %s", self->name, num, user->nick, buffer);
}

void
irc_mark(struct userNode *user, char *mark)
{
    char *host = user->hostname;

    /* TODO: Allow mark overwrite. If they are marked, and their fakehost is oldmark.hostname, update it to newmark.hostname so mark can be called multiple times. Probably requires ircd modification also */
    if(user->mark)
        return;

    /* if the mark will put us over the  host length, clip some off the left hand side
     * to make room...
     */
    if(strlen(host) + 1 + strlen(mark) > HOSTLEN)
        host += 1 + ( (strlen(host) + 1 + strlen(mark)) - HOSTLEN );
    putsock("%s " CMD_MARK " %s DNSBL +m %s.%s", self->numeric, user->nick, mark, host);
    putsock("%s " CMD_MARK " %s DNSBL_DATA %s", self->numeric, user->nick, mark);

    /* Save it in the user */
    user->mark = strdup(mark);

    /* If they are not otherwise marked, mark their host with fakehost */
    if(!IsFakeHost(user) && !IsSetHost(user) && !(IsHiddenHost(user) && user->handle_info) )
    {
        struct modeNode *mn = NULL;
        char fakehost[HOSTLEN];
        unsigned int count = 0;
        unsigned int n = 0;

        putsock("%s " CMD_FAKEHOST " %s %s.%s", self->numeric, user->numeric, mark, host);
        putsock("%s " CMD_MODE " %s +x", self->numeric, user->nick);
        
        snprintf(fakehost, sizeof(fakehost), "%s.%s", mark, host);
        safestrncpy(user->fakehost, fakehost, sizeof(user->fakehost));

        for (n=count=0; n<user->channels.used; n++) {
            mn = user->channels.list[n];
            if (strlen(mn->channel->name) >= 1) /* Sanity */
                check_bans(user, mn->channel->name);
        }
    }
}

void irc_sno(unsigned int mask, char const* format, ...) {
    va_list arg_list;
    char buffer[MAXLEN];
    va_start(arg_list, format);
    vsnprintf(buffer, MAXLEN-2, format, arg_list);
    buffer[MAXLEN-1] = 0;
    putsock("%s " CMD_SNO " %d :%s", self->numeric, mask, buffer);
}

void
irc_sasl(struct server* dest, const char *identifier, const char *subcmd, const char *data)
{
    putsock("%s " P10_SASL " %s %s %s %s", self->numeric, dest->numeric, identifier, subcmd, data);
}

static void send_burst(void);

static void
change_nicklen(int new_nicklen)
{
    unsigned int nn;
    char new_nick[NICKLEN+1];
    struct userNode *user;

    nicklen = new_nicklen;
    /* fix up any users we have here */
    for (nn=0; nn<=self->num_mask; nn++) {
        if (!(user = self->users[nn]))
            continue;
        safestrncpy(new_nick, user->nick, sizeof(new_nick));
        new_nick[nicklen] = 0;
        NickChange(user, new_nick, 1);
    }
}

static CMD_FUNC(cmd_whois)
{
    struct userNode *from;
    struct userNode *who;
    char buf[MAXLEN];
    unsigned int i, mlen, len;

    if (argc < 3)
        return 0;
    if (!(from = GetUserH(origin))) {
        log_module(MAIN_LOG, LOG_ERROR, "Could not find WHOIS origin user %s", origin);
        return 0;
    }
    if (!(who = GetUserH(argv[2]))) {
        irc_numeric(from, ERR_NOSUCHNICK, "%s@%s :No such nick", argv[2], self->name);
        return 1;
    }

    if (IsFakeHost(who) && IsHiddenHost(who))
        irc_numeric(from, RPL_WHOISUSER, "%s %s %s * :%s", who->nick, who->ident, who->fakehost, who->info);
    else if (IsHiddenHost(who) && who->handle_info && hidden_host_suffix)
        irc_numeric(from, RPL_WHOISUSER, "%s %s %s.%s * :%s", who->nick, who->ident, who->handle_info->handle, hidden_host_suffix, who->info);
    else
        irc_numeric(from, RPL_WHOISUSER, "%s %s %s * :%s", who->nick, who->ident, who->hostname, who->info);

    if (!IsService(who) || (from == who)) {
        struct modeNode *mn;
        mlen = strlen(self->name) + strlen(from->nick) + 12 + strlen(who->nick);
        len = 0;
        *buf = '\0';
        for (i = who->channels.used; i > 0; )
        {
            mn = who->channels.list[--i];

            if (!IsOper(from) && (mn->channel->modes & (MODE_PRIVATE | MODE_SECRET)) && !GetUserMode(mn->channel, from))
                continue;

            if (len + strlen(mn->channel->name) + mlen > MAXLEN - 5)
            {
                irc_numeric(from, RPL_WHOISCHANNELS, "%s :%s", who->nick, buf);
                *buf = '\0';
                len = 0;
            }

            if (IsDeaf(who))
                *(buf + len++) = '-';
            if ((mn->channel->modes & (MODE_PRIVATE | MODE_SECRET)) && !GetUserMode(mn->channel, from))
                *(buf + len++) = '*';
            if (mn->modes & MODE_CHANOP)
                *(buf + len++) = '@';
            else if (mn->modes & MODE_VOICE)
                *(buf + len++) = '+';

            if (len)
                *(buf + len) = '\0';
            strcpy(buf + len, mn->channel->name);
            len += strlen(mn->channel->name);
            strcat(buf + len, " ");
            len++;
        }
        if (buf[0] != '\0')
            irc_numeric(from, RPL_WHOISCHANNELS, "%s :%s", who->nick, buf);
    }

    if (his_servername && his_servercomment && !IsOper(from) && from != who)
        irc_numeric(from, RPL_WHOISSERVER, "%s %s :%s", who->nick, his_servername, his_servercomment);
    else
        irc_numeric(from, RPL_WHOISSERVER, "%s %s :%s", who->nick, who->uplink->name, who->uplink->description);

    if (IsAway(who))
        irc_numeric(from, RPL_AWAY, "%s :Away", who->nick);
    if (IsOper(who))
        irc_numeric(from, RPL_WHOISOPERATOR, "%s :%s", who->nick, IsLocal(who) ? "is a megalomaniacal power hungry tyrant" : "is an IRC Operator");
    if (who->handle_info)
        irc_numeric(from, RPL_WHOISACCOUNT, "%s %s :is logged in as", who->nick, who->handle_info->handle);
    if (IsHiddenHost(who) && who->handle_info && (IsOper(from) || from == who))
        irc_numeric(from, RPL_WHOISACTUALLY, "%s %s@%s %s :Actual user@host, Actual IP", who->nick, who->ident, who->hostname, irc_ntoa(&who->ip));
    if (IsLocal(who) && !IsService(who) && (IsOper(from) || from == who))
        irc_numeric(from, RPL_WHOISIDLE, "%s %ld %ld :seconds idle, signon time", who->nick, now - who->idle_since, who->timestamp);

    irc_numeric(from, RPL_ENDOFWHOIS, "%s :End of /WHOIS list", who->nick);
    return 1;
}

static CMD_FUNC(cmd_server)
{
    struct server *srv;
    const char *str;

    if (argc < 8)
        return 0;
    if (self->uplink) {
        /* another server introduced us */
        srv = AddServer(GetServerH(origin), argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[6], argv[argc-1]);
        if (!srv)
            return 0;
        srv->self_burst = argv[5][0] == 'J';
        srv->burst = 1;
    } else {
        /* this must be our uplink */
        srv = self->uplink = AddServer(self, argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[6], argv[argc-1]);
        if (!srv)
            return 0;
        srv->self_burst = argv[5][0] == 'J';
        srv->burst = 1;
        if ((argv[7][0] == '+') && !force_n2k) {
            log_module(MAIN_LOG, LOG_WARNING, "Got Undernet-style SERVER message but \"force_n2k\" not on.");
        }
        send_burst();
    }

    /* Fix up our timestamps if necessary. */
    if (srv->boot <= PREHISTORY) {
        /* Server from the mists of time.. */
        if (srv->hops == 1) {
            log_module(MAIN_LOG, LOG_ERROR, "Server %s claims to have booted at time "FMT_TIME_T".  This is absurd.", srv->name, srv->boot);
        }
    } else if ((str = conf_get_data("server/reliable_clock", RECDB_QSTRING))
               && enabled_string(str)) {
        /* If we have a reliable clock, we just keep our current time. */
    } else {
        if (srv->boot <= self->boot) {
            /* The other server is older than us.  Accept their timestamp.
             * Alternately, we are same age, but we accept their time
             * since we are linking to them. */
            self->boot = srv->boot;
            ioset_set_time(srv->link_time
);
        }
    }
    if (srv == self->uplink) {
        extern time_t burst_begin;
        burst_begin = now;
    }
    return 1;
}


static CMD_FUNC(cmd_eob)
{
    struct server *sender;
    dict_iterator_t it;

    if (!(sender = GetServerH(origin)))
        return 0;
    if (sender == self->uplink) {
        cManager.uplink->state = CONNECTED;
        for (it = dict_first(unbursted_channels); it; it = iter_next(it))
            irc_burst(iter_data(it));
        dict_delete(unbursted_channels);
        unbursted_channels = NULL;
        irc_eob();
        irc_eob_ack();

        /* now that we know who our uplink is,
         * we can center the routing map and activate auto-routing.
         */
        //activate_routing(NULL, NULL, NULL);
        routing_init();
    }
    sender->self_burst = 0;
    recalc_bursts(sender);
    call_server_link_funcs(sender);
    /* let auto-routing figure out if we were
     * wating on this server to link a child to it */
    /* DONT call this if uplink is _US_ */
    if(sender->uplink != self)
        routing_handle_connect(sender->name, sender->uplink->name);
    return 1;
}

static CMD_FUNC(cmd_eob_ack)
{
    extern time_t burst_begin;

    if (GetServerH(origin) == self->uplink) {
        burst_length = now - burst_begin;
        self->self_burst = self->burst = 0;
    }
    cManager.uplink->state = CONNECTED;
    return 1;
}

static CMD_FUNC(cmd_rping)
{
    struct server *dest;

    if (!(dest = GetServerN(argv[1])))
        return 1; /* if its a jupe or something, return 1 so its silently ignored */

    if (dest == self)
        irc_rpong(argv[2], argv[3], argv[4], argv[5]);

    return 1;
}

static CMD_FUNC(cmd_sasl)
{
    struct server *serv;

    if (argc < 5)
        return 0;

    serv = GetServerH(origin);
    if (!serv)
      return 0;

    call_sasl_input_func(serv, argv[2], argv[3], argv[4], (argc>5 ? argv[5] : NULL));

    return 1;
}

static CMD_FUNC(cmd_ping)
{
    struct server *srv;
    struct userNode *un;

    if (argc > 3)
        irc_pong_asll(argv[2], argv[3]);
    else if ((srv = GetServerH(origin)))
        irc_pong(self->name, srv->numeric);
    else if ((un = GetUserH(origin)))
        irc_pong(self->name, un->numeric);
    else
        irc_pong(self->name, origin);

    timeq_del(0, timed_send_ping, 0, TIMEQ_IGNORE_WHEN|TIMEQ_IGNORE_DATA);
    timeq_del(0, timed_ping_timeout, 0, TIMEQ_IGNORE_WHEN|TIMEQ_IGNORE_DATA);
    timeq_add(now + ping_freq, timed_send_ping, 0);
    received_ping();
    return 1;
}

static CMD_FUNC(cmd_error_nick)
{
    /* Go back to original IRC length .. and try to reconnect :/ */
    change_nicklen(9);
    irc_squit(self, "Got erroneous nickname, truncating nicks.", NULL);
    return 1;
}

struct create_desc {
    struct userNode *user;
    time_t when;
};

static void
join_helper(struct chanNode *chan, void *data)
{
    struct create_desc *cd = data;
    AddChannelUser(cd->user, chan);
}

static void
create_helper(char *name, void *data)
{
    struct create_desc *cd = data;

    if (!strcmp(name, "0")) {
        while (cd->user->channels.used > 0)
            DelChannelUser(cd->user, cd->user->channels.list[0]->channel, 0, 0);
        return;
    }

    AddChannelUser(cd->user, AddChannel(name, cd->when, NULL, NULL, NULL));
}

static CMD_FUNC(cmd_create)
{
    struct create_desc cd;
    struct userNode *user;

    if ((argc < 3) || !(user = GetUserH(origin)))
        return 0;
    cd.user = user;
    cd.when = atoi(argv[2]);
    parse_foreach(argv[1], join_helper, create_helper, NULL, NULL, &cd);
    return 1;
}

static CMD_FUNC(cmd_join)
{
    struct create_desc cd;

    if (!(cd.user = GetUserH(origin)))
        return 0;
    if (argc < 2)
        return 0;
    else if (argc < 3)
        cd.when = now;
    else
        cd.when = atoi(argv[2]);
    parse_foreach(argv[1], join_helper, create_helper, NULL, NULL, &cd);
    return 1;
}

static CMD_FUNC(cmd_svsjoin)
{
    struct create_desc cd;

    if (!(cd.user = GetUserH(argv[1])))
        return 0;
    if (argc < 3)
        return 0;
    else if (argc < 4)
        cd.when = now;
    else
        cd.when = atoi(argv[3]);
    parse_foreach(argv[2], join_helper, create_helper, NULL, NULL, &cd);
    return 1;
}

static CMD_FUNC(cmd_pong)
{
    if (argc < 3)
        return 0;
    if (!strcmp(argv[2], self->name)) {
        timeq_del(0, timed_send_ping, 0, TIMEQ_IGNORE_WHEN|TIMEQ_IGNORE_DATA);
        timeq_del(0, timed_ping_timeout, 0, TIMEQ_IGNORE_WHEN|TIMEQ_IGNORE_DATA);
        timeq_add(now + ping_freq, timed_send_ping, 0);
        received_ping();
    }
    return 1;
}

static CMD_FUNC(cmd_nick)
{
    struct userNode *user;
    if ((user = GetUserH(origin))) {
        /* nick change (since the source is a user numeric) */
        if (argc < 2)
            return 0;
        NickChange(user, argv[1], 1);
    } else {
        struct server *serv;
        struct userNode *nuser;
        char modes[MAXLEN];
        /* new nick */
        if (argc < 9)
            return 0;
        serv = GetServerH(origin);
        if (argc > 9)
            unsplit_string(argv+6, argc-9, modes);
        else
            strcpy(modes, "+");
        nuser = AddUser(serv, argv[1], argv[4], argv[5], modes, argv[argc-2], argv[argc-1], atoi(argv[3]), argv[argc-3]);
    }
    return 1;
}

static CMD_FUNC(cmd_account)
{
    struct userNode *user;
    struct server *server;
    struct handle_info *hi;

    if ((argc < 3) || !origin || !(server = GetServerH(origin)))
        return 0; /* Origin must be server. */
    
    /* This next line appears to tremple origin.. why? */
    user = GetUserN(argv[1]);
    if (!user)
        return 1; /* A QUIT probably passed the ACCOUNT. */

    if(!extended_accounts) /* any need for this function without? */
        return 1;
    
    if(!strcmp(argv[2],"C"))
    {
        if((hi = loc_auth(NULL, argv[4], argv[5], NULL)))
        {
            /* Return a AC A */
            putsock("%s " P10_ACCOUNT " %s A %s "FMT_TIME_T, self->numeric, server->numeric , argv[3], hi->registered);

			log_module(MAIN_LOG, LOG_DEBUG, "loc_auth: %s\n", user->nick);
         }
        else
        {
            /* Return a AC D */
            putsock("%s " P10_ACCOUNT " %s D %s", self->numeric, server->numeric , argv[3]);
        }
        return 1;
    }
    else if(!strcmp(argv[2],"H")) /* New enhanced (host) version of C */
    {
        if((hi = loc_auth(NULL, argv[5], argv[6], argv[4] )))
        {
            /* Return a AC A */
            putsock("%s " P10_ACCOUNT " %s A %s "FMT_TIME_T, self->numeric, server->numeric , argv[3], hi->registered);
        }
        else
        {
            /* Return a AC D */
            putsock("%s " P10_ACCOUNT " %s D %s", self->numeric, server->numeric , argv[3]);
        }
        return 1;
    }
    else if(!strcmp(argv[2],"S"))
    {
        if((hi = loc_auth(argv[5], argv[6], argv[7], argv[4])))
        {
            /* Return a AC A */
            putsock("%s " P10_ACCOUNT " %s A %s "FMT_TIME_T, self->numeric, server->numeric , argv[3], hi->registered);
        }
        else
        {
            /* Return a AC D */
            putsock("%s " P10_ACCOUNT " %s D %s", self->numeric, server->numeric , argv[3]);
        }
        return 1;
    }
    else if(!strcmp(argv[2],"R"))
       call_account_func(user, argv[3]);
    else
        call_account_func(user, argv[2]); /* For backward compatability */
    return 1;
}

static CMD_FUNC(cmd_fakehost)
{
    struct userNode *user;

    if ((argc < 3) || !origin || !GetServerH(origin))
        return 0;
    if (!(user = GetUserN(argv[1])))
        return 1;
    assign_fakehost(user, argv[2], 0);
    return 1;
}

static struct {
  char        *name;
  unsigned int priv;
} privtab[] = {
#define P(priv)         { #priv, PRIV_ ## priv }
  P(CHAN_LIMIT),     P(MODE_LCHAN),     P(WALK_LCHAN),    P(DEOP_LCHAN),
  P(SHOW_INVIS),     P(SHOW_ALL_INVIS), P(UNLIMIT_QUERY), P(KILL),
  P(LOCAL_KILL),     P(REHASH),         P(RESTART),       P(DIE),
  P(GLINE),          P(LOCAL_GLINE),    P(JUPE),          P(LOCAL_JUPE),
  P(OPMODE),         P(LOCAL_OPMODE),   P(SET),           P(WHOX),
  P(BADCHAN),        P(LOCAL_BADCHAN),  P(SEE_CHAN),      P(PROPAGATE),
  P(DISPLAY),        P(SEE_OPERS),      P(WIDE_GLINE),    P(FORCE_OPMODE),
  P(FORCE_LOCAL_OPMODE), P(REMOTEREHASH), P(CHECK), P(SEE_SECRET_CHAN),
  P(SHUN),           P(LOCAL_SHUN),     P(WIDE_SHUN),     P(ZLINE),
  P(LOCAL_ZLINE),    P(WIDE_ZLINE),     P(LIST_CHAN),     P(WHOIS_NOTICE),
  P(HIDE_IDLE),      P(XTRAOP),         P(HIDE_CHANNELS), P(DISPLAY_MODE),
  P(FREEFORM),       P(REMOVE),         P(SPAMFILTER),
#undef P
  { 0, 0 }
};

char *client_report_privs(struct userNode *client)
{
  int i;

  privbuf[0] = '\0';
  for (i = 0; privtab[i].name; i++) {
    if (HasPriv(client, privtab[i].priv)) {
      strcat(privbuf, privtab[i].name);
      strcat(privbuf, " ");
    }
  }

  privbuf[strlen(privbuf)] = 0;

  return privbuf;
}

int clear_privs(struct userNode *who) {
  int i = 0;
  assert(0 != who);

  for (i = 0; privtab[i].name; i++)
    RevokePriv(who, privtab[i].priv);

  return 0;
}

int client_modify_priv_by_name(struct userNode *who, char *priv, int what) {
 int i = 0;
 assert(0 != priv);
 assert(0 != who);

  for (i = 0; privtab[i].name; i++) {
    if (0 == strcmp(privtab[i].name, priv)) {
      if (what == PRIV_ADD)
        GrantPriv(who, privtab[i].priv);
      else if (what == PRIV_DEL) {
        RevokePriv(who, privtab[i].priv);
      }
    }
  }
  return 0;
}

int check_priv(char *priv)
{
  int i;

  for (i = 0; privtab[i].name; i++) {
    if (0 == strcmp(privtab[i].name, priv)) {
      return 1;
    }
  }
  return 0;
}

void
irc_privs(struct userNode *target, char *flag, int add)
{
    client_modify_priv_by_name(target, flag, add);
    putsock("%s " P10_PRIVS " %s %s%s", self->numeric, target->numeric, (add == PRIV_ADD) ? "+" : "-", flag);
}

void
irc_raw_privs(struct userNode *target, const char *privs)
{
    putsock("%s " P10_PRIVS " %s %s", self->numeric, target->numeric,  privs);
}

static CMD_FUNC(cmd_privs)
{
    char *tstr = NULL;
    char buf[512] = "";
    char *p = 0;
    char *tmp;

    int what = PRIV_ADD;
    int type = 0;

    unsigned int i;

    struct server *sender;
    struct userNode *user;


    tstr = conf_get_data("server/type", RECDB_QSTRING);
    if(tstr)
        type = atoi(tstr);


    if (!(sender = GetServerH(origin))) { /* from oper */
        return 1; /* ignore as no services have privs set */
    } else { /* from server */
        if (type < 5)
            return 1; /* silently ignore */

        user = argc > 1 ? GetUserN(argv[1]) : NULL;

        if (!user)
            return 0;

        for (i=1; i<argc; i++) {
            strcat(buf, argv[i]);
            strcat(buf, " ");
        }

        for (i = 2; i < argc; i++) {
            if (*argv[i] == '+') { what = PRIV_ADD; argv[i]++; }
            if (*argv[i] == '-') { what = PRIV_DEL; argv[i]++; }
            for (tmp = x3_strtok(&p, argv[i], ","); tmp;
                 tmp = x3_strtok(&p, NULL, ",")) {
                if (!strcmp(tmp, "PRIV_NONE")) {
                    if (now > user->timestamp+5)
                        clear_privs(user);
                    break;
                } else
                    client_modify_priv_by_name(user, tmp, what);
            }
        }
    }
    return 1;
}

static CMD_FUNC(cmd_burst)
{
    extern int rel_age;
    char modes[MAXLEN], *members = "";
    static char exemptlist[MAXLEN], banlist[MAXLEN];
    unsigned int next = 3, res = 1;
    int ctype = 0, echeck = 0, bcheck = 0;
    struct chanData *cData;
    struct chanNode *cNode;
    struct userNode *un;
    struct modeNode *mNode;
    long mode;
    int oplevel = -1;
    int type = 0;
    char *user, *end, sep;
    char *tstr;
    time_t in_timestamp;
    char* parm = NULL;

    if (argc < 3)
        return 0;
    modes[0] = 0;

    tstr = conf_get_data("server/type", RECDB_QSTRING);
    if(tstr) {
        type = atoi(tstr);
    }

    exemptlist[0] = 0;
    banlist[0] = 0;

    while (next < argc) {
        switch (argv[next][0]) {
        case '+': {
            const char *pos;
            int n_modes;
            for (pos=argv[next], n_modes = 1; *pos; pos++)
                if ((*pos == 'k') || (*pos == 'l') || (*pos == 'A')
                    || (*pos == 'U') || ((type > 7) && (*pos == 'L')))
                    n_modes++;
            if (next + n_modes > argc)
                n_modes = argc - next;
            unsplit_string(argv+next, n_modes, modes);
            next += n_modes;
            break;
        }
        case '%': {
            ctype = 1;
            for(parm = mysep(&argv[next], " "); /* parm = first param */
                  parm;   /* While param is not null */
                  parm = mysep(&argv[next], " ")  /* parm = next param */
               )
            {
              if (0 == strcmp("~", parm))
                ctype = 2;
              if (ctype == 1) {
                if (bcheck == 0) {
                  /* strip % char off start of very first ban */
                  if (strlen(parm) > 1) {
                    strncat(banlist, strtok(parm, "%"), sizeof(banlist) - 1 - strlen(banlist));
                    strncat(banlist, " ", sizeof(banlist) - 1 - strlen(banlist));
                  }
                  bcheck = 1;
                } else {
                  strncat(banlist, parm, sizeof(banlist) - 1 - strlen(banlist));
                  strncat(banlist, " ", sizeof(banlist) - 1 - strlen(banlist));
                }
              } else if (ctype == 2) {
                if (echeck == 0) {
                   echeck = 1;
                } else {
                  strncat(exemptlist, parm, sizeof(exemptlist) - 1 - strlen(exemptlist));
                  strncat(exemptlist, " ", sizeof(exemptlist) - 1 - strlen(exemptlist));
                }
              }
            }
            next++;
            break;
        }
        default: members = argv[next++]; break;
      }
    }

    in_timestamp = atoi(argv[2]);
    if ((cNode = dict_find(unbursted_channels, argv[1], NULL))) {
        cNode->timestamp = in_timestamp;
        dict_remove(unbursted_channels, cNode->name);
        irc_burst(cNode);
    }
    cNode = AddChannel(argv[1], in_timestamp, modes, banlist, exemptlist);
    cData = cNode->channel_info;

    if (!cData) {
        if (cNode->modes & MODE_REGISTERED) {
            irc_join(opserv, cNode);
            irc_mode(opserv, cNode, "-z");
            irc_part(opserv, cNode, "");
        }
    }

    /* Burst channel members in now. */
    for (user = members, sep = *members, mode = 0; sep; user = end) {
        for (end = user + 3; isalnum(*end) || *end == '[' || *end == ']'; end++) ;
        sep = *end++; end[-1] = 0;
        if (sep == ':') {
            mode = 0;
            while ((sep = *end++)) {
                if (sep == 'o') {
                    mode |= MODE_CHANOP;
                    oplevel = -1;
                } else if (sep == 'v') {
                    mode |= MODE_VOICE;
                    oplevel = -1;
                } else if (isdigit(sep)) {
                    mode |= MODE_CHANOP;
                    if (oplevel >= 0)
                        oplevel += parse_oplevel(end);
                    else
                        oplevel = parse_oplevel(end);
                    while (isdigit(*end)) end++;
                } else
                    break;
            }
            if (rel_age < 0)
                mode = 0;
        }
        if (!(un = GetUserN(user))) {
            res = 0;
            continue;
        }
        if ((mNode = AddChannelUser(un, cNode))) {
            mNode->modes = mode;
            mNode->oplevel = oplevel;
        }
    }

    return res;
}

/* TODO: 
 * This is a stub that doesn't actually do anything. It should be completed
 * so that bans on *!*@markname.* match users as it does in nefarious
 */
static CMD_FUNC(cmd_mark)
{
    struct userNode *target;
    /* 
     * log_module(MAIN_LOG, LOG_ERROR, "DEBUG: mark, user %s, type %s, arg %s", argv[1], argv[2], argv[3]);
     */

    if(argc < 4)
        return 0;
    if(!strcasecmp(argv[2], "DNSBL")) {
        /* DNSBL <modes> */
        return 1;
    }
    else if(!strcasecmp(argv[2], "DNSBL_DATA")) {
        /* DNSBL_DATA name */
        target = GetUserH(argv[1]);
        if(!target) {
            log_module(MAIN_LOG, LOG_ERROR, "Unable to find user %s whose dnsbl mark is changing.", argv[1]);
            return 0;
        }
        target->mark = strdup(argv[3]);
        return 1;
        
    }
    else if(!strcasecmp(argv[2], "CVERSION")) {
        /* CTCP VERSION mark */
        target = GetUserH(argv[1]);
        if(!target) {
            log_module(MAIN_LOG, LOG_ERROR, "Unable to find user %s whose version mark is changing.", argv[1]);
            return 0;
        }

        char *version = unsplit_string(argv + 3, argc - 3, NULL);
        if(!version)
            version = "";

        target->version_reply = strdup(version);

        if(match_ircglob(version, "WebTV;*"))
            target->no_notice = true; /* webbies cant see notices */

        return 1;
    }
    else if(!strcasecmp(argv[2], "SSLCLIFP")) {
        /* SSL fingerprint mark */
        target = GetUserH(argv[1]);
        if(!target) {
            log_module(MAIN_LOG, LOG_ERROR, "Unable to find user %s whose SSL fingerprint mark is changing.", argv[1]);
            return 0;
        }

        char *sslfp = unsplit_string(argv + 3, argc - 3, NULL);
        if(!sslfp)
            sslfp = "";

        target->sslfp = strdup(sslfp);

        return 1;
    }
    /* unknown type of mark */
    return 1;
}

static CMD_FUNC(cmd_mode)
{
    struct chanNode *cn;
    struct userNode *un;
    char *sethost; // sethost - reed/apples
    int i; // sethost - reed/apples

    if (argc < 3)
        return 0;
    if (!IsChannelName(argv[1])) {
        un = GetUserH(argv[1]);
        if (!un) {
            log_module(MAIN_LOG, LOG_ERROR, "Unable to find user %s whose mode is changing.", argv[1]);
            return 0;
        }
        // sethost - reed/apples
        if (argc == 3)
        mod_usermode(un, argv[2]);
        else {
          sethost = malloc(strlen(argv[2]) + 1 + strlen(argv[3]) + 1);
          i = 0;
          while((sethost[i++] = *argv[2]++));
          i--;
         sethost[i++] = ' ';
          while((sethost[i++] = *argv[3]++));
          mod_usermode(un, sethost); // sethost - reed/apples
        }

        return 1;
    }

    if (!(cn = GetChannel(argv[1]))) {
        log_module(MAIN_LOG, LOG_ERROR, "Unable to find channel %s whose mode is changing.", argv[1]);
        return 0;
    }
    if ((un = GetUserH(origin))) {
        struct modeNode *mn;
        /* Update idle time for person setting the mode */
        if ((mn = GetUserMode(cn, un)))
            mn->idle_since = now;
    } else {
        /* If it came from a server, reset timestamp to re-sync. */
        cn->timestamp = atoi(argv[argc-1]);
    }

    if (checkDefCon(DEFCON_NO_MODE_CHANGE) && !IsOper(un)) {
        const char *str;
        str = conf_get_data("services/opserv/nick", RECDB_QSTRING);
        if (str) {
            char modes[MODELEN];
            struct userNode *opserv = GetUserH(str);
 
            send_message_type(4, un, opserv, "Channel modes cannot be changed due to DefCon level %d in effect, please try again soon", DefConLevel);
            irc_make_chanmode(cn, modes);
            irc_mode(opserv, cn, modes);
            irc_mode(opserv, cn, DefConChanModes);
        }
        return 1;
    }


    return mod_chanmode(un, cn, argv+2, argc-2, MCP_ALLOW_OVB|MCP_FROM_SERVER|(un ? MC_NOTIFY : 0));
}

static CMD_FUNC(cmd_opmode)
{
    struct chanNode *cn;
    struct userNode *un;

    if (argc < 3)
        return 0;

    if (!(cn = GetChannel(argv[1]))) {
        log_module(MAIN_LOG, LOG_ERROR, "Unable to find channel %s whose mode is changing.", argv[1]);
        return 0;
    }
    if (!(un = GetUserH(origin))) {
        log_module(MAIN_LOG, LOG_ERROR, "Unable to find user %s requesting OPMODE.", origin);
        return 0;
    }
    if (!IsOper(un)) {
        log_module(MAIN_LOG, LOG_ERROR, "Non-privileged user %s using OPMODE.", un->nick);
        return 0;
    }

    return mod_chanmode(un, cn, argv+2, argc-2, MCP_ALLOW_OVB|MCP_FROM_SERVER); /* do NOT announce opmode locally */
}

static int clear_chanmode(struct chanNode *channel, const char *modes);

static CMD_FUNC(cmd_clearmode)
{
    struct chanNode *cn;
    struct userNode *un;

    if (argc < 3)
        return 0;

    if (!(cn = GetChannel(argv[1]))) {
        log_module(MAIN_LOG, LOG_ERROR, "Unable to find channel %s whose mode is changing.", argv[1]);
        return 0;
    }
    if (!(un = GetUserH(origin))) {
        log_module(MAIN_LOG, LOG_ERROR, "Unable to find user %s requesting CLEARMODE.", origin);
        return 0;
    }
    if (!IsOper(un)) {
        log_module(MAIN_LOG, LOG_ERROR, "Non-privileged user %s using CLEARMODE.", un->nick);
        return 0;
    }

    return clear_chanmode(cn, argv[2]);
}

static CMD_FUNC(cmd_topic)
{
    struct chanNode *cn;
    time_t chan_ts, topic_ts;
    struct userNode *user;

    if (argc < 3)
        return 0;
    if (!(cn = GetChannel(argv[1]))) {
        log_module(MAIN_LOG, LOG_ERROR, "Unable to find channel %s whose topic is being set", argv[1]);
        return 0;
    }


    if (argc == 5) {              /* Asuka / Topic Bursting IRCu's */
        user = GetUserH(origin);
        chan_ts = atoi(argv[2]);
        topic_ts = atoi(argv[3]);
    } else if (argc >= 6) {       /* Nefarious 0.5.0 */
        user = GetUserH(strtok(argv[2], "!"));
        chan_ts = atoi(argv[3]);
        topic_ts = atoi(argv[4]);
    } else {                      /* Regular IRCu (No Topic Bursting)*/
        user = GetUserH(origin);
        chan_ts = cn->timestamp;
        topic_ts = now;
    }

    SetChannelTopic(cn, user, user, argv[argc-1], 0);
    cn->topic_time = topic_ts;
    return 1;
}

static CMD_FUNC(cmd_num_topic)
{
    struct chanNode *cn;

    if (!argv[0])
        return 0; /* huh? */
    if (argv[2]) {
        cn = GetChannel(argv[2]);
        if (!cn) {
            log_module(MAIN_LOG, LOG_ERROR, "Unable to find channel %s in topic reply", argv[2]);
            return 0;
        }
    } else
        return 0;

    switch (atoi(argv[0])) {
    case 331:
        cn->topic_time = 0;
        break;  /* no topic */
    case 332:
        if (argc < 4)
            return 0;
        safestrncpy(cn->topic, unsplit_string(argv+3, argc-3, NULL), sizeof(cn->topic));
        break;
    case 333:
        if (argc < 5)
            return 0;
        safestrncpy(cn->topic_nick, argv[3], sizeof(cn->topic_nick));
        cn->topic_time = atoi(argv[4]);
        break;
    default:
        return 0; /* should never happen */
    }
    return 1;
}

static CMD_FUNC(cmd_num_gline)
{
    if (argc < 7) {
        if (argc < 6)
            return 0;
        else
            gline_add(origin, argv[3], atoi(argv[4])-now, argv[5], now, 0, 0);
    } else {
        if (!strcmp(argv[5], "+"))
          gline_add(origin, argv[3], atoi(argv[4])-now, argv[6], now, 0, 0);
    }
    return 1;
}

static CMD_FUNC(cmd_num_shun)
{
    if (argc < 7) {
        if (argc < 6)
            return 0;
        else
            shun_add(origin, argv[3], atoi(argv[4])-now, argv[5], now, 0);
    } else {
        if (!strcmp(argv[5], "+"))
            shun_add(origin, argv[3], atoi(argv[4])-now, argv[6], now, 0);
    }
    return 1;
}

static CMD_FUNC(cmd_quit)
{
    struct userNode *user;
    if (argc < 2)
        return 0;
    /* Sometimes we get a KILL then a QUIT or the like, so we don't want to
     * call DelUser unless we have the user in our grasp. */
    if ((user = GetUserH(origin)))
        DelUser(user, NULL, false, argv[1]);
    return 1;
}

static CMD_FUNC(cmd_kill)
{
    struct userNode *user;
    if (argc < 2)
        return 0;
    user = GetUserN(argv[1]);
    if (!user) {
        /* If we get a KILL for a non-existent user, it could be a
         * Ghost response to a KILL we sent out earlier.  So we only
         * whine if the target is local.
         */
        if (!strncmp(argv[1], self->numeric, strlen(self->numeric))) {
            log_module(MAIN_LOG, LOG_ERROR, "Unable to find kill victim %s", argv[1]);
            return 0;
        }
        return 1;
    }

    if (IsLocal(user) && IsService(user)) {
        /* TODO: rate limit this so silly things don't happen. */
        ReintroduceUser(user);
        return 1;
    }

    DelUser(user, NULL, false, argv[2]);
    return 1;
}

static CMD_FUNC(cmd_svspart)
{
    struct userNode *user;

    if (argc < 3)
        return 0;
    user = GetUserN(argv[1]);
    if (!user)
        return 0;
    parse_foreach(argv[2], part_helper, NULL, NULL, NULL, user);
    return 1;
}

static CMD_FUNC(cmd_silence)
{
    struct userNode *user;
    char *mask;
    char *new_mask;
    unsigned int i;

    if (argc < 2)
        return 0;

    user = GetUserN(argv[1]);

    /* Sanity, go nuts if this happens */
    if (!user)
        return 0;

    /*  We can safely ignore this if a user adding a silence is not
     *  ignored. However this brings up a TODO. If a user logs in and
     *  they have silences on the IRCd then we need to set them here
     *  somehow
     */
    if (!user->handle_info)
        return 1;

    mask = argv[2];

    if (*mask == '-') {
        for (i=0; i<user->handle_info->ignores->used; i++) {
            if (!irccasecmp(mask+1, user->handle_info->ignores->list[i]))
                user->handle_info->ignores->list[i] = user->handle_info->ignores->list[--user->handle_info->ignores->used];
        }
    } else {
        for (i=0; i<user->handle_info->ignores->used; i++) {
            if (!strcmp(mask+1, user->handle_info->ignores->list[i]))
                return 1; /* Already on the users NickServ ignore list, safely ignore */
        }

        new_mask = strdup(mask+1);
        string_list_append(user->handle_info->ignores, new_mask);
    }
    return 1;
}

static CMD_FUNC(cmd_kick)
{
    if (argc < 3)
        return 0;
    ChannelUserKicked(GetUserH(origin), GetUserN(argv[2]), GetChannel(argv[1]));
    return 1;
}

static CMD_FUNC(cmd_squit)
{
    struct server *server;
    char *uplink;

    if (argc < 4)
        return 0;
    if (!(server = GetServerH(argv[1])))
        return 0;

    if (server == self->uplink) {
        /* Force a reconnect to the currently selected server. */
        cManager.uplink->tries = 0;
        log_module(MAIN_LOG, LOG_INFO, "Squitting from uplink: %s", argv[3]);
        close_socket();
        return 1;
    }

    uplink = strdup(server->uplink->name);
    DelServer(server, 0, argv[3]);
    /* if its a pingout and pingout connecting is enabled
               or its a read error and readerror connecting is enabled
               or were doing a "N" and i need to connect that server somewhere */
    routing_handle_squit(argv[1], uplink, argv[3]);
    free(uplink);
    return 1;
}

static CMD_FUNC(cmd_privmsg)
{
    struct privmsg_desc pd;
    if (argc < 3)
        return 0;
    pd.user = GetUserH(origin);
    if (!pd.user || (IsGagged(pd.user) && !IsOper(pd.user)))
        return 1;

    if (checkDefCon(DEFCON_OPER_ONLY) && !IsOper(pd.user)) {
        const char *str;
        str = conf_get_data("services/opserv/nick", RECDB_QSTRING);
        if (str)
            send_message_type(4, pd.user, GetUserH(str), "Services are currently not available, please try again soon");
        return 1;
    }

    if (checkDefCon(DEFCON_SILENT_OPER_ONLY) && !IsOper(pd.user))
        return 1; /* Silently Ignore */

    pd.is_notice = 0;
    pd.text = argv[argc - 1];
    parse_foreach(argv[1], privmsg_chan_helper, NULL, privmsg_user_helper, privmsg_invalid, &pd);

    return 1;
}

static CMD_FUNC(cmd_notice)
{
    struct privmsg_desc pd;
    struct server *srv;
    int nuser = 0;

    if (argc != 3)
        return 0;

    pd.user = GetUserH(origin);
    if(!pd.user)
        nuser = 1;
    if (!pd.user || (IsGagged(pd.user) && !IsOper(pd.user))) {
    }
    else {
        pd.is_notice = 1;
        pd.text = argv[2];
        parse_foreach(argv[1], privmsg_chan_helper, NULL, privmsg_user_helper, privmsg_invalid, &pd);
    }

    srv = GetServerH(origin);
    if(srv) {
        char *sargv[MAXNUMPARAMS];
        int sargc;

        sargc = split_line(argv[2], true, MAXNUMPARAMS, sargv);

        if(!strcasecmp(sargv[0], "Connect:")) {
            /* :Connect: Host shoe.loxxin.net not listed in ircd.conf */
            if(!strcasecmp(sargv[3], "not") && !strcasecmp(sargv[4], "listed")) {
                routing_handle_connect_failure(srv, sargv[2], unsplit_string(sargv+3, sargc-3, NULL));
            }
        }
        else if(!strcasecmp(sargv[0], "Link")) {
            /* :Link with mephisto.etheria.cx cancelled: Server mephisto.etheria.cx[216.46.33.71]
             *
             * :Link with laptop.afternet.org cancelled: Connection refused
             */
            if(!strcasecmp(sargv[3], "cancelled:")) {
                routing_handle_connect_failure(srv, sargv[2], unsplit_string(sargv+4, sargc-4, NULL));
            }
        }
    }
    return 1;
}

static CMD_FUNC(cmd_away)
{
    struct userNode *uNode;

    uNode = GetUserH(origin);
    if (!uNode)
        return 1;
    if (argc < 2)
        uNode->modes &= ~FLAGS_AWAY;
    else
        uNode->modes |= FLAGS_AWAY;
    return 1;
}

static CMD_FUNC(cmd_gline)
{
    if (argc < 3)
        return 0;
    if (argv[2][0] == '+') {
        if (argc < 5)
            return 0;
        gline_add(origin, argv[2]+1, strtoul(argv[3], NULL, 0), argv[argc-1], now, 0, 0);
        return 1;
    } else if (argv[2][0] == '-') {
        gline_remove(argv[2]+1, 0);
        return 1;
    } else
        return 0;
}

static CMD_FUNC(cmd_sgline)
{
    struct server *sender;

    if (argc < 4)
        return 0;

    if (!(sender = GetServerH(origin)))
        return 0;

    gline_add(origin, argv[1], strtoul(argv[2], NULL, 0), argv[argc-1], now, 1, 0);
    return 1;
}

static CMD_FUNC(cmd_sshun)
{
    struct server *sender;

    if (argc < 4)
        return 0;

    if (!(sender = GetServerH(origin)))
        return 0;

    shun_add(origin, argv[1], strtoul(argv[2], NULL, 0), argv[argc-1], now, 1);
    return 1;
}

static CMD_FUNC(cmd_shun)
{
    if (argc < 3)
        return 0;
    if (argv[2][0] == '+') {
        if (argc < 5)
            return 0;
        shun_add(origin, argv[2]+1, strtoul(argv[3], NULL, 0), argv[argc-1], now, 0);
        return 1;
    } else if (argv[2][0] == '-') {
        shun_remove(argv[2]+1, 0);
        return 1;
    } else
        return 0;
}

static CMD_FUNC(cmd_svsnick)
{
    struct userNode *target, *dest;
    if ((argc < 4)
        || !(target = GetUserN(argv[1]))
        || !IsLocal(target)
        || (dest = GetUserH(argv[2])))
        return 0;
    NickChange(target, argv[2], 0);
    return 1;
}

void
free_user(struct userNode *user)
{
    free(user->nick);
    free(user);
}

static void
parse_cleanup(UNUSED_ARG(void *extra))
{
    unsigned int nn;
    free(of_list);
    free(of_list_extra);
    free(privmsg_funcs);
    num_privmsg_funcs = 0;
    free(notice_funcs);
    num_notice_funcs = 0;
    free(mcf_list);
    dict_delete(irc_func_dict);
    for (nn=0; nn<dead_users.used; nn++)
        free_user(dead_users.list[nn]);
    userList_clean(&dead_users);
}

static void
p10_conf_reload(void) {
    hidden_host_suffix = conf_get_data("server/hidden_host", RECDB_QSTRING);
    his_servername = conf_get_data("server/his_servername", RECDB_QSTRING);
    his_servercomment = conf_get_data("server/his_servercomment", RECDB_QSTRING);
}

static void
remove_unbursted_channel(struct chanNode *cNode, UNUSED_ARG(void *extra)) {
    if (unbursted_channels)
        dict_remove(unbursted_channels, cNode->name);
}

void
init_parse(void)
{
    const char *str, *desc;
    int numnick, usermask, max_users;
    char numer[COMBO_NUMERIC_LEN+1];

    /* read config items */
    str = conf_get_data("server/extended_accounts", RECDB_QSTRING);
    extended_accounts = str ? enabled_string(str) : 1;
    str = conf_get_data("server/ping_freq", RECDB_QSTRING);
    ping_freq = str ? ParseInterval(str) : 120;
    str = conf_get_data("server/ping_timeout", RECDB_QSTRING);
    ping_timeout = str ? ParseInterval(str) : 30;
    str = conf_get_data("server/force_n2k", RECDB_QSTRING);
    force_n2k = str ? enabled_string(str) : 1;
    str = conf_get_data("server/numeric", RECDB_QSTRING);
    if (!str) {
        log_module(MAIN_LOG, LOG_ERROR, "No server/numeric entry in config file.");
        exit(1);
    }
    numnick = atoi(str);
    str = conf_get_data("server/max_users", RECDB_QSTRING);
    max_users = str ? atoi(str) : 4096;
    for (usermask = 4; usermask < max_users; usermask <<= 1) ;
    usermask--;
    if ((numnick < 64) && (usermask < 4096) && !force_n2k)
        inttobase64(numer, (numnick << 12) + (usermask & 0x00fff), 3);
    else
        inttobase64(numer, (numnick << 18) + (usermask & 0x3ffff), 5);

    str = conf_get_data("server/hostname", RECDB_QSTRING);
    desc = conf_get_data("server/description", RECDB_QSTRING);
    if (!str || !desc) {
        log_module(MAIN_LOG, LOG_ERROR, "No server/hostname entry in config file.");
        exit(1);
    }
    self = AddServer(NULL, str, 0, boot_time, now, numer, desc);
    conf_register_reload(p10_conf_reload);

    irc_func_dict = dict_new();
    dict_insert(irc_func_dict, CMD_BURST, cmd_burst);
    dict_insert(irc_func_dict, TOK_BURST, cmd_burst);
    dict_insert(irc_func_dict, CMD_CREATE, cmd_create);
    dict_insert(irc_func_dict, TOK_CREATE, cmd_create);
    dict_insert(irc_func_dict, CMD_EOB, cmd_eob);
    dict_insert(irc_func_dict, TOK_EOB, cmd_eob);
    dict_insert(irc_func_dict, CMD_EOB_ACK, cmd_eob_ack);
    dict_insert(irc_func_dict, TOK_EOB_ACK, cmd_eob_ack);
    dict_insert(irc_func_dict, CMD_MODE, cmd_mode);
    dict_insert(irc_func_dict, TOK_MODE, cmd_mode);
    dict_insert(irc_func_dict, CMD_MARK, cmd_mark);
    dict_insert(irc_func_dict, TOK_MARK, cmd_mark);
    dict_insert(irc_func_dict, CMD_NICK, cmd_nick);
    dict_insert(irc_func_dict, TOK_NICK, cmd_nick);
    dict_insert(irc_func_dict, CMD_ACCOUNT, cmd_account);
    dict_insert(irc_func_dict, TOK_ACCOUNT, cmd_account);
    dict_insert(irc_func_dict, CMD_FAKEHOST, cmd_fakehost);
    dict_insert(irc_func_dict, TOK_FAKEHOST, cmd_fakehost);
    dict_insert(irc_func_dict, CMD_PASS, cmd_pass);
    dict_insert(irc_func_dict, TOK_PASS, cmd_pass);
    dict_insert(irc_func_dict, CMD_PING, cmd_ping);
    dict_insert(irc_func_dict, TOK_PING, cmd_ping);
    dict_insert(irc_func_dict, CMD_PRIVMSG, cmd_privmsg);
    dict_insert(irc_func_dict, TOK_PRIVMSG, cmd_privmsg);
    dict_insert(irc_func_dict, CMD_PONG, cmd_pong);
    dict_insert(irc_func_dict, TOK_PONG, cmd_pong);
    dict_insert(irc_func_dict, CMD_QUIT, cmd_quit);
    dict_insert(irc_func_dict, TOK_QUIT, cmd_quit);
    dict_insert(irc_func_dict, CMD_SERVER, cmd_server);
    dict_insert(irc_func_dict, TOK_SERVER, cmd_server);
    dict_insert(irc_func_dict, CMD_JOIN, cmd_join);
    dict_insert(irc_func_dict, TOK_JOIN, cmd_join);
    dict_insert(irc_func_dict, CMD_PART, cmd_part);
    dict_insert(irc_func_dict, TOK_PART, cmd_part);
    dict_insert(irc_func_dict, CMD_ERROR, cmd_error);
    dict_insert(irc_func_dict, TOK_ERROR, cmd_error);
    dict_insert(irc_func_dict, CMD_TOPIC, cmd_topic);
    dict_insert(irc_func_dict, TOK_TOPIC, cmd_topic);
    dict_insert(irc_func_dict, CMD_AWAY, cmd_away);
    dict_insert(irc_func_dict, TOK_AWAY, cmd_away);
    dict_insert(irc_func_dict, CMD_SILENCE, cmd_silence);
    dict_insert(irc_func_dict, TOK_SILENCE, cmd_silence);
    dict_insert(irc_func_dict, CMD_KICK, cmd_kick);
    dict_insert(irc_func_dict, TOK_KICK, cmd_kick);
    dict_insert(irc_func_dict, CMD_SQUIT, cmd_squit);
    dict_insert(irc_func_dict, TOK_SQUIT, cmd_squit);
    dict_insert(irc_func_dict, CMD_KILL, cmd_kill);
    dict_insert(irc_func_dict, TOK_KILL, cmd_kill);
    dict_insert(irc_func_dict, CMD_NOTICE, cmd_notice);
    dict_insert(irc_func_dict, TOK_NOTICE, cmd_notice);
    dict_insert(irc_func_dict, CMD_STATS, cmd_stats);
    dict_insert(irc_func_dict, TOK_STATS, cmd_stats);
    dict_insert(irc_func_dict, CMD_SVSJOIN, cmd_svsjoin);
    dict_insert(irc_func_dict, TOK_SVSJOIN, cmd_svsjoin);
    dict_insert(irc_func_dict, CMD_SVSNICK, cmd_svsnick);
    dict_insert(irc_func_dict, TOK_SVSNICK, cmd_svsnick);
    dict_insert(irc_func_dict, CMD_SVSPART, cmd_svspart);
    dict_insert(irc_func_dict, TOK_SVSPART, cmd_svspart);
    dict_insert(irc_func_dict, CMD_SWHOIS, cmd_dummy);
    dict_insert(irc_func_dict, TOK_SWHOIS, cmd_dummy);
    dict_insert(irc_func_dict, CMD_WHOIS, cmd_whois);
    dict_insert(irc_func_dict, TOK_WHOIS, cmd_whois);
    dict_insert(irc_func_dict, CMD_GLINE, cmd_gline);
    dict_insert(irc_func_dict, TOK_GLINE, cmd_gline);
    dict_insert(irc_func_dict, CMD_SGLINE, cmd_sgline);
    dict_insert(irc_func_dict, TOK_SGLINE, cmd_sgline);
    dict_insert(irc_func_dict, CMD_SHUN, cmd_shun);
    dict_insert(irc_func_dict, TOK_SHUN, cmd_shun);
    dict_insert(irc_func_dict, CMD_SSHUN, cmd_sshun);
    dict_insert(irc_func_dict, TOK_SSHUN, cmd_sshun);
    dict_insert(irc_func_dict, CMD_OPMODE, cmd_opmode);
    dict_insert(irc_func_dict, TOK_OPMODE, cmd_opmode);
    dict_insert(irc_func_dict, CMD_CLEARMODE, cmd_clearmode);
    dict_insert(irc_func_dict, TOK_CLEARMODE, cmd_clearmode);
    dict_insert(irc_func_dict, CMD_VERSION, cmd_version);
    dict_insert(irc_func_dict, TOK_VERSION, cmd_version);
    dict_insert(irc_func_dict, CMD_ADMIN, cmd_admin);
    dict_insert(irc_func_dict, TOK_ADMIN, cmd_admin);
    dict_insert(irc_func_dict, CMD_SMO, cmd_dummy);
    dict_insert(irc_func_dict, TOK_SMO, cmd_dummy);
    dict_insert(irc_func_dict, CMD_SNO, cmd_dummy);
    dict_insert(irc_func_dict, TOK_SNO, cmd_dummy);

    dict_insert(irc_func_dict, CMD_RPING, cmd_rping);
    dict_insert(irc_func_dict, TOK_RPING, cmd_rping);
    dict_insert(irc_func_dict, CMD_RPONG, cmd_dummy);
    dict_insert(irc_func_dict, TOK_RPONG, cmd_dummy);

    dict_insert(irc_func_dict, CMD_SASL, cmd_sasl);
    dict_insert(irc_func_dict, TOK_SASL, cmd_sasl);

    /* In P10, DESTRUCT doesn't do anything except be broadcast to servers.
     * Apparently to obliterate channels from any servers that think they
     * exist?
     */
    dict_insert(irc_func_dict, CMD_DESTRUCT, cmd_dummy);
    dict_insert(irc_func_dict, TOK_DESTRUCT, cmd_dummy);
    /* Ignore invites */
    dict_insert(irc_func_dict, CMD_INVITE, cmd_dummy);
    dict_insert(irc_func_dict, TOK_INVITE, cmd_dummy);
    /* DESYNCH is just informational, so ignore it */
    dict_insert(irc_func_dict, CMD_DESYNCH, cmd_dummy);
    dict_insert(irc_func_dict, TOK_DESYNCH, cmd_dummy);
    /* Ignore channel operator notices. */
    dict_insert(irc_func_dict, CMD_WALLCHOPS, cmd_dummy);
    dict_insert(irc_func_dict, TOK_WALLCHOPS, cmd_dummy);
    dict_insert(irc_func_dict, CMD_WALLVOICES, cmd_dummy);
    dict_insert(irc_func_dict, TOK_WALLVOICES, cmd_dummy);
    dict_insert(irc_func_dict, CMD_WALLHOPS, cmd_dummy);
    dict_insert(irc_func_dict, TOK_WALLHOPS, cmd_dummy);
    /* Ignore opers being silly. */
    dict_insert(irc_func_dict, CMD_WALLOPS, cmd_dummy);
    dict_insert(irc_func_dict, TOK_WALLOPS, cmd_dummy);
    dict_insert(irc_func_dict, CMD_WALLHOPS, cmd_dummy);
    dict_insert(irc_func_dict, TOK_WALLHOPS, cmd_dummy);
    dict_insert(irc_func_dict, TOK_WALLUSERS, cmd_dummy);
    /* Ignore dnsbl exemptions */
    dict_insert(irc_func_dict, TOK_EXEMPT, cmd_dummy);
    dict_insert(irc_func_dict, CMD_PRIVS, cmd_privs);
    dict_insert(irc_func_dict, TOK_PRIVS, cmd_privs);
    /* ignore ALIST for now */
    dict_insert(irc_func_dict, TOK_ALIST, cmd_dummy);
    dict_insert(irc_func_dict, CMD_ALIST, cmd_dummy);
    /* ignore SPAMFILTER */
    dict_insert(irc_func_dict, TOK_SPAMFILTER, cmd_dummy);
    dict_insert(irc_func_dict, CMD_SPAMFILTER, cmd_dummy);
    /* Ignore remote luser */
    dict_insert(irc_func_dict, TOK_LUSERS, cmd_dummy);
    /* We have reliable clock!  Always!  Wraaa! */
    dict_insert(irc_func_dict, CMD_SETTIME, cmd_dummy);
    dict_insert(irc_func_dict, TOK_SETTIME, cmd_dummy);
    /* Ignore ZLINE/ZL/REMOVE/RM */
    dict_insert(irc_func_dict, CMD_ZLINE, cmd_dummy);
    dict_insert(irc_func_dict, TOK_ZLINE, cmd_dummy);
    dict_insert(irc_func_dict, CMD_REMOVE, cmd_dummy);
    dict_insert(irc_func_dict, TOK_REMOVE, cmd_dummy);

    /* ignore /trace and /motd commands targetted at us */
    dict_insert(irc_func_dict, TOK_TRACE, cmd_dummy);
    dict_insert(irc_func_dict, TOK_MOTD, cmd_dummy);
    dict_insert(irc_func_dict, TOK_UPING, cmd_dummy);

    /* handle topics */
    dict_insert(irc_func_dict, "331", cmd_num_topic);
    dict_insert(irc_func_dict, "332", cmd_num_topic);
    dict_insert(irc_func_dict, "333", cmd_num_topic);
    dict_insert(irc_func_dict, "345", cmd_dummy); /* blah has been invited to blah */
    dict_insert(irc_func_dict, "432", cmd_error_nick); /* Erroneus [sic] nickname */
    /* ban list resetting */
    /* "stats g" responses */
    dict_insert(irc_func_dict, "230", cmd_dummy); /* ignore stats headers */
    dict_insert(irc_func_dict, "247", cmd_num_gline);
    dict_insert(irc_func_dict, "542", cmd_num_shun);
    dict_insert(irc_func_dict, "219", cmd_dummy); /* "End of /STATS report" */
    /* other numeric responses we might get */
    dict_insert(irc_func_dict, "401", cmd_dummy); /* target left network */
    dict_insert(irc_func_dict, "403", cmd_dummy); /* no such channel */
    dict_insert(irc_func_dict, "404", cmd_dummy); /* cannot send to channel */
    dict_insert(irc_func_dict, "439", cmd_dummy); /* target change too fast */
    dict_insert(irc_func_dict, "441", cmd_dummy); /* target isn't on that channel */
    dict_insert(irc_func_dict, "442", cmd_dummy); /* you aren't on that channel */
    dict_insert(irc_func_dict, "443", cmd_dummy); /* is already on channel (after invite?) */
    dict_insert(irc_func_dict, "461", cmd_dummy); /* Not enough parameters (after TOPIC w/ 0 args) */
    dict_insert(irc_func_dict, "467", cmd_dummy); /* Channel key already set */

    num_privmsg_funcs = 16;
    privmsg_funcs = malloc(sizeof(privmsg_func_t)*num_privmsg_funcs);
    memset(privmsg_funcs, 0, sizeof(privmsg_func_t)*num_privmsg_funcs);

    num_notice_funcs = 16;
    notice_funcs = malloc(sizeof(privmsg_func_t)*num_notice_funcs);
    memset(notice_funcs, 0, sizeof(privmsg_func_t)*num_notice_funcs);

    userList_init(&dead_users);
    reg_del_channel_func(remove_unbursted_channel, NULL);
    reg_exit_func(parse_cleanup, NULL);
    // reg_notice_func(opserv, check_ctcp);
}

int
parse_line(char *line, int recursive)
{
    char *argv[MAXNUMPARAMS], *origin;
    int argc, cmd, res=0;
    cmd_func_t *func;

    argc = split_line(line, true, MAXNUMPARAMS, argv);
    cmd = self->uplink || !argv[0][1] || !argv[0][2];
    if (argc > cmd) {
        if (cmd) {
            if (argv[0][0] == ':') {
                origin = argv[0]+1;
            } else if (!argv[0][1] || !argv[0][2]) {
                struct server *sNode = GetServerN(argv[0]);
                origin = sNode ? sNode->name : 0;
            } else {
                struct userNode *uNode = GetUserN(argv[0]);
                origin = uNode ? uNode->nick : 0;
            }
        } else
            origin = 0;
        if ((func = dict_find(irc_func_dict, argv[cmd], NULL)))
            res = func(origin, argc-cmd, argv+cmd);
    }
    if (!res) {
        log_module(MAIN_LOG, LOG_ERROR, "PARSE ERROR on line: %s", unsplit_string(argv, argc, NULL));
    } else if (!recursive) {
        unsigned int i;
        for (i=0; i<dead_users.used; i++)
            free_user(dead_users.list[i]);
        dead_users.used = 0;
    }
    return res;
}

static void
parse_foreach(char *target_list, foreach_chanfunc cf, foreach_nonchan nc, foreach_userfunc uf, foreach_nonuser nu, void *data)
{
    char *j, old;

    do {
        j = target_list;
        while (*j != 0 && *j != ',')
            j++;
        old = *j;
        *j = 0;

        if (IsChannelName(target_list)
            || (target_list[0] == '0' && target_list[1] == '\0')) {
            struct chanNode *chan = GetChannel(target_list);

            if (chan) {
                if (cf)
                    cf(chan, data);
            } else {
                if (nc)
                    nc(target_list, data);
            }
        } else {
            struct userNode *user;
            struct privmsg_desc *pd = data;

            pd->is_qualified = 0;
            if (*target_list == '@') {
                user = NULL;
            } else if (strchr(target_list, '@')) {
                struct server *server;

                pd->is_qualified = 1;
                user = GetUserH(strtok(target_list, "@"));
                server = GetServerH(strtok(NULL, "@"));

                if (user && (user->uplink != server)) {
                    /* Don't attempt to index into any arrays
                       using a user's numeric on another server. */
                    user = NULL;
                }
            } else {
                user = GetUserN(target_list);
            }

            if (user) {
                if (uf)
                    uf(user, data);
            } else {
                if (nu)
                    nu(target_list, data);
            }
        }
        target_list = j+1;
    } while (old == ',');
}

static int
get_local_numeric(void)
{
    static unsigned int next_numeric = 0;
    if (self->clients > self->num_mask)
        return -1;
    while (self->users[next_numeric])
        if (++next_numeric > self->num_mask)
            next_numeric = 0;
    return next_numeric;
}

static void
make_numeric(struct server *svr, int local_num, char *outbuf)
{
    int slen, llen;

    if (force_n2k || svr->numeric[1]) {
        slen = 2;
        llen = 3;
    } else {
        slen = 1;
        llen = (local_num < 64*64) ? 2 : 3;
    }
    strncpy(outbuf, svr->numeric, slen);
    inttobase64(outbuf+slen, local_num, llen);
    outbuf[slen+llen] = 0;
}

struct server *
AddServer(struct server *uplink, const char *name, int hops, time_t boot, time_t link_time, const char *numeric, const char *description)
{
    struct server* sNode;
    int slen, mlen;

    if ((sNode = GetServerN(numeric))) {
        /* This means we're trying to re-add an existant server.
         * To be safe, we should forget the previous incarnation.
         * (And all its linked servers.)
         *
         * It usually only happens in replays when the original
         * had a ping timeout and the replay didn't (because
         * replaying a ping timeout invariably gets things wrong).
         */
        DelServer(sNode, 0, NULL);
    }

    switch (strlen(numeric)) {
    case 5: slen = 2; mlen = 3; break;
    case 4: slen = 1; mlen = 3; break;
    case 3: slen = 1; mlen = 2; break;
    default:
        log_module(MAIN_LOG, LOG_ERROR, "AddServer(\"%s\", \"%s\", ...): Numeric %s has invalid length.", uplink->name, name, numeric);
        return NULL;
    }

    sNode = calloc(1, sizeof(*sNode));
    sNode->uplink = uplink;
    safestrncpy(sNode->name, name, sizeof(sNode->name));
    sNode->num_mask = base64toint(numeric+slen, mlen);
    sNode->hops = hops;
    sNode->boot = boot;
    sNode->link_time = link_time;
    strncpy(sNode->numeric, numeric, slen);
    safestrncpy(sNode->description, description, sizeof(sNode->description));
    sNode->users = calloc(sNode->num_mask+1, sizeof(*sNode->users));
    serverList_init(&sNode->children);
    if (sNode->uplink) {
        /* uplink may be NULL if we're just building ourself */
        serverList_append(&sNode->uplink->children, sNode);
    }
    servers_num[base64toint(numeric, slen)] = sNode;
    dict_insert(servers, sNode->name, sNode);
    return sNode;
}

void DelServer(struct server* serv, int announce, const char *message)
{
    unsigned int i;

    /* If we receive an ERROR command before the SERVER
     * command a NULL server can be passed */
    if (!serv)
        return;

    /* Hrm, what's the right way to SQUIT some other server?
     * (This code is only to handle killing juped servers.) */
    if (announce && (serv->uplink == self) && (serv != self->uplink))
        irc_squit(serv, message, NULL);

    /* must recursively remove servers linked to this one first */
    for (i=serv->children.used;i>0;)
        if (serv->children.list[--i] != self)
            DelServer(serv->children.list[i], false, NULL);

    /* clean up server's user hash tables */
    for (i=0;i<=serv->num_mask;i++)
        if (serv->users[i])
            DelUser(serv->users[i], NULL, false, "server delinked");

    /* delete server */
    if (serv->uplink)
        serverList_remove(&serv->uplink->children, serv);
    if (serv == self->uplink)
        self->uplink = NULL;
    servers_num[base64toint(serv->numeric, strlen(serv->numeric))] = NULL;
    dict_remove(servers, serv->name);
    serverList_clean(&serv->children);
    free(serv->users);
    free(serv);
}

struct userNode *
AddLocalUser(const char *nick, const char *ident, const char *hostname, const char *desc, const char *modes)
{
    char numeric[COMBO_NUMERIC_LEN+1];
    int local_num = get_local_numeric();
    time_t timestamp = now;
    struct userNode *old_user = GetUserH(nick);

    if (!modes)
        modes = "+oik";

    if (old_user) {
        if (IsLocal(old_user))
            return old_user;
        timestamp = old_user->timestamp - 1;
    }
    if (local_num == -1) {
        log_module(MAIN_LOG, LOG_ERROR, "Unable to allocate numnick for service %s", nick);
        return 0;
    }
    if (!hostname)
        hostname = self->name;
    make_numeric(self, local_num, numeric);
    return AddUser(self, nick, ident, hostname, modes, numeric, desc, now, "AAAAAA");
}

struct userNode *
AddClone(const char *nick, const char *ident, const char *hostname, const char *desc)
{
    char numeric[COMBO_NUMERIC_LEN+1];
    int local_num = get_local_numeric();
    time_t timestamp = now;
    struct userNode *old_user = GetUserH(nick);

    if (old_user) {
        if (IsLocal(old_user))
            return old_user;
        timestamp = old_user->timestamp - 1;
    }
    if (local_num == -1) {
        log_module(MAIN_LOG, LOG_ERROR, "Unable to allocate numnick for clone %s", nick);
        return 0;
    }
    make_numeric(self, local_num, numeric);
    return AddUser(self, nick, ident, hostname, "+i", numeric, desc, timestamp, "AAAAAA");
}

int
is_valid_nick(const char *nick) {
    unsigned int ii;
    /* IRC has some of The Most Fucked-Up ideas about character sets
     * in the world.. */
    if (!isalpha(*nick) && !strchr("{|}~[\\]^_`", *nick))
        return 0;
    for (ii = 0; nick[ii]; ++ii)
        if (!isalnum(nick[ii]) && !strchr("{|}~[\\]^-_`", nick[ii]))
            return 0;
    if (strlen(nick) > nicklen)
        return 0;
    return 1;
}

static struct userNode*
AddUser(struct server* uplink, const char *nick, const char *ident, const char *hostname, const char *modes, const char *numeric, const char *userinfo, time_t timestamp, const char *realip)
{
    struct userNode *oldUser, *uNode;
    unsigned int ignore_user, dummy;
    char *tstr;
    int type;

    if ((strlen(numeric) < 3) || (strlen(numeric) > 5)) {
        log_module(MAIN_LOG, LOG_WARNING, "AddUser(%p, %s, ...): numeric %s wrong length!", (void*)uplink, nick, numeric);
        return NULL;
    }

    if (!uplink) {
        log_module(MAIN_LOG, LOG_WARNING, "AddUser(%p, %s, ...): server for numeric %s doesn't exist!", (void*)uplink, nick, numeric);
        return NULL;
    }

    if (uplink != GetServerN(numeric)) {
        log_module(MAIN_LOG, LOG_WARNING, "AddUser(%p, %s, ...): server for numeric %s differs from nominal uplink %s.", (void*)uplink, nick, numeric, uplink->name);
        return NULL;
    }

    dummy = modes && modes[0] == '*';
    if (dummy) {
        ++modes;
    } else if (!is_valid_nick(nick)) {
        log_module(MAIN_LOG, LOG_WARNING, "AddUser(%p, %s, ...): invalid nickname detected.", (void*)uplink, nick);
        return NULL;
    }

    ignore_user = 0;
    if ((oldUser = GetUserH(nick))) {
        if (IsLocal(oldUser)
            && (IsService(oldUser) || IsPersistent(oldUser))) {
            /* The service should collide the new user off - but not
             * if the new user is coming in during a burst.  (During a
             * burst, the bursting server will kill either our user --
             * triggering a ReintroduceUser() -- or its own.)
             */
            oldUser->timestamp = timestamp - 1;
            ignore_user = 1;
            if (!uplink->burst)
                irc_user(oldUser);
        } else if (oldUser->timestamp > timestamp) {
            /* "Old" user is really newer; remove them */
            DelUser(oldUser, 0, 1, "Overruled by older nick");
        } else {
            /* User being added is too new; do not add them to
             * clients, but do add them to the server's list, since it
             * will send a KILL and QUIT soon. */
            ignore_user = 1;
        }
    }

    /* create new usernode and set all values */
    uNode = calloc(1, sizeof(*uNode));
    uNode->nick = strdup(nick);
    safestrncpy(uNode->ident, ident, sizeof(uNode->ident));
    safestrncpy(uNode->info, userinfo, sizeof(uNode->info));
    safestrncpy(uNode->hostname, hostname, sizeof(uNode->hostname));
    safestrncpy(uNode->numeric, numeric, sizeof(uNode->numeric));
    irc_p10_pton(&uNode->ip, realip);

    tstr = conf_get_data("server/type", RECDB_QSTRING);
    type = atoi(tstr);
    if (type == 7) {
      if (irc_in_addr_is_ipv4(uNode->ip)) {
        make_virtip((char*)irc_ntoa(&uNode->ip), (char*)irc_ntoa(&uNode->ip), uNode->cryptip);
        make_virthost((char*)irc_ntoa(&uNode->ip), uNode->hostname, uNode->crypthost);
      } else if (irc_in_addr_is_ipv6(uNode->ip)) {
        make_ipv6virthost((char*)irc_ntoa(&uNode->ip), uNode->hostname, uNode->crypthost);
      }
    }

    if (!uNode->crypthost && uNode->cryptip)
        snprintf(uNode->crypthost, sizeof(uNode->crypthost), "%s", strdup(uNode->cryptip));
    uNode->idle_since = timestamp;
    uNode->timestamp = timestamp;
    modeList_init(&uNode->channels);
    uNode->uplink = uplink;
    if (++uNode->uplink->clients > uNode->uplink->max_clients) {
        uNode->uplink->max_clients = uNode->uplink->clients;
    }
    uNode->num_local = base64toint(numeric+strlen(uNode->uplink->numeric), 3) & uNode->uplink->num_mask;
    uNode->uplink->users[uNode->num_local] = uNode;
    mod_usermode(uNode, modes);
    if (dummy)
        uNode->modes |= FLAGS_DUMMY;

    set_geoip_info(uNode);

    if (ignore_user)
        return uNode;

    dict_insert(clients, uNode->nick, uNode);
    if (dict_size(clients) > max_clients) {
        max_clients = dict_size(clients);
        max_clients_time = now;
    }
    if (IsLocal(uNode))
        irc_user(uNode);
    call_new_user_funcs(uNode);

    if ((uNode->loc == 1) && (uNode->handle_info))
        send_func_list(uNode);

    return uNode;
}

/* removes user from it's server's hash table and nick hash table */
void
DelUser(struct userNode* user, struct userNode *killer, int announce, const char *why)
{

    verify(user);

    /* mark them as dead, in case anybody cares */
    user->dead = 1;

    /* remove pending adduser commands */
    wipe_adduser_pending(NULL, user);

    /* remove user from all channels */
    while (user->channels.used > 0)
        DelChannelUser(user, user->channels.list[user->channels.used-1]->channel, NULL, 0);

    /* Call these in reverse order so ChanServ can update presence
       information before NickServ nukes the handle_info. */
    call_del_user_funcs(user, killer, why);

    user->uplink->clients--;
    user->uplink->users[user->num_local] = NULL;
    if (IsOper(user))
        userList_remove(&curr_opers, user);
    /* remove from global dictionary, but not if after a collide */
    if (user == dict_find(clients, user->nick, NULL))
        dict_remove(clients, user->nick);

    if (IsInvisible(user))
        invis_clients--;

    if (announce) {
        if (IsLocal(user))
            irc_quit(user, why);
        else
            irc_kill(killer, user, why);
    }

    if (IsLocal(user)) {
        unsigned int num = user->num_local;
        if (num < num_privmsg_funcs)
            privmsg_funcs[num] = NULL;
        if (num < num_notice_funcs)
            notice_funcs[num] = NULL;
    }

    modeList_clean(&user->channels);

    /* Clean up version data */
    if(user->version_reply) {
        free(user->version_reply);
        user->version_reply = NULL;
    }

    /* Clean up SSL fingerprint data */
    if(user->sslfp) {
        free(user->sslfp);
        user->sslfp = NULL;
    }

    /* clean up mark */
    if(user->mark) {
        free(user->mark);
        user->mark = NULL;
    }

    /* clean up geoip data if any */
    if(user->country_code) free(user->country_code);
    if(user->city) free(user->city);
    if(user->region) free(user->region);
    if(user->postal_code) free(user->postal_code);

    /* We don't free them, in case we try to privmsg them or something
     * (like when a stupid oper kills themself).  We just put them onto
     * a list of clients that get freed after processing each line.
     */
    if (dead_users.size)
        userList_append(&dead_users, user);
    else
        free_user(user);
}

static void call_oper_funcs(struct userNode *user);

void mod_usermode(struct userNode *user, const char *mode_change) {
    int add = 1;
    const char *word = mode_change;

    if (!user || !mode_change)
        return;

    call_user_mode_funcs(user, mode_change);

    while (*word != ' ' && *word) word++;
    while (*word == ' ') word++;
    while (1) {
#define do_user_mode(FLAG) do { if (add) user->modes |= FLAG; else user->modes &= ~FLAG; } while (0)
	switch (*mode_change++) {
	case 0: case ' ': return;
	case '+': add = 1; break;
	case '-': add = 0; break;
	case 'o':
	    do_user_mode(FLAGS_OPER);
            if (!add) {
                userList_remove(&curr_opers, user);
            } else if (!userList_contains(&curr_opers, user)) {
                userList_append(&curr_opers, user);
                call_oper_funcs(user);
	    }
	    break;
	case 'O': do_user_mode(FLAGS_LOCOP); break;
	case 'i': do_user_mode(FLAGS_INVISIBLE);
	    if (add)
                invis_clients++;
            else
                invis_clients--;
	    break;
	case 'w': do_user_mode(FLAGS_WALLOP); break;
	case 'd': do_user_mode(FLAGS_DEAF); break;
	case 'k': do_user_mode(FLAGS_SERVICE); break;
	case 'g': do_user_mode(FLAGS_GLOBAL); break;
	case 'B': do_user_mode(FLAGS_BOT); break;
	case 'n': do_user_mode(FLAGS_HIDECHANS); break;
	case 'I': do_user_mode(FLAGS_HIDEIDLE); break;
	case 'X': do_user_mode(FLAGS_XTRAOP); break;
	case 'C': do_user_mode(FLAGS_CLOAKHOST);
	    if (*word) {
		char cloakhost[MAXLEN];
		unsigned int ii;
		for (ii=0; (*word != ' ') && (*word != '\0'); )
		    cloakhost[ii++] = *word++;
		cloakhost[ii] = 0;
		while (*word == ' ')
		    word++;
		safestrncpy(user->crypthost, cloakhost, sizeof(user->crypthost));
	    }
	    break;
	case 'c': do_user_mode(FLAGS_CLOAKIP);
	    if (*word) {
		char cloakip[MAXLEN];
		unsigned int ii;
		for (ii=0; (*word != ' ') && (*word != '\0'); )
		    cloakip[ii++] = *word++;
		cloakip[ii] = 0;
		while (*word == ' ')
		    word++;
		safestrncpy(user->cryptip, cloakip, sizeof(user->cryptip));
	    }
	    break;
	// sethost - reed/apples
	// case 'h': do_user_mode(FLAGS_HELPER); break;
	// I check if there's an 'h' in the first part, and if there, 
	// then everything after the space becomes their new host.
	case 'h': do_user_mode(FLAGS_SETHOST);
	    if (*word) {
		char sethost[MAXLEN];
		unsigned int ii;
		for (ii=0; (*word != ' ') && (*word != '\0'); )
		    sethost[ii++] = *word++;
		sethost[ii] = 0;
		while (*word == ' ')
		    word++;
		safestrncpy(user->sethost, sethost, sizeof(user->sethost));
	    }
	    break;
        case 'x': do_user_mode(FLAGS_HIDDEN_HOST); break;
        case 'r':
            if (*word) {
                char tag[MAXLEN];
                unsigned int ii;
                for (ii=0; (*word != ' ') && (*word != '\0'); )
                    tag[ii++] = *word++;
                tag[ii] = 0;
                while (*word == ' ')
                    word++;
                call_account_func(user, tag);
            }
            break;
        case 'f':
            if (*word) {
                char host[MAXLEN];
                unsigned int ii;
                for (ii=0; (*word != ' ') && (*word != '\0'); )
                    host[ii++] = *word++;
                host[ii] = 0;
                while (*word == ' ')
                    word++;
                assign_fakehost(user, host, 0);
            }
            break;
        case 'a': do_user_mode(FLAGS_ADMIN); break;
        case 'z': do_user_mode(FLAGS_SSL); break;
        case 'D': do_user_mode(FLAGS_PRIVDEAF); break;
        case 'R': do_user_mode(FLAGS_ACCOUNTONLY); break;
        case 'W': do_user_mode(FLAGS_WHOIS); break;
        case 'H': do_user_mode(FLAGS_HIDEOPER); break;
        case 'L': do_user_mode(FLAGS_NOLINK); break;
        case 'q': do_user_mode(FLAGS_COMMONCHANSONLY); break;
	}
#undef do_user_mode
    }
}

static int
keyncpy(char output[], char input[], size_t output_size)
{
    size_t ii;

    if (input[0] == ':')
    {
        output[0] = '\0';
        return 1;
    }

    for (ii = 0; (ii + 1 < output_size) && (input[ii] != '\0'); ++ii)
    {
        output[ii] = input[ii];
    }

    output[ii] = '\0';
    return 0;
}

struct mod_chanmode *
mod_chanmode_parse(struct chanNode *channel, char **modes, unsigned int argc, unsigned int flags, short base_oplevel)
{
    struct mod_chanmode *change;
    unsigned int ii, in_arg, ch_arg, add;

    if (argc == 0)
        return NULL;
    if (!(change = mod_chanmode_alloc(argc - 1)))
        return NULL;

    for (ii = ch_arg = 0, in_arg = add = 1;
         (modes[0][ii] != '\0') && (modes[0][ii] != ' ');
         ++ii) {
        switch (modes[0][ii]) {
        case '+':
            add = 1;
            break;
        case '-':
            add = 0;
            break;
#define do_chan_mode(FLAG) do { if (add) change->modes_set |= (FLAG), change->modes_clear &= ~(FLAG); else change->modes_clear |= (FLAG), change->modes_set &= ~(FLAG); } while(0)
        case 'C': do_chan_mode(MODE_NOCTCPS); break;
        case 'D': do_chan_mode(MODE_DELAYJOINS); break;
        case 'c': do_chan_mode(MODE_NOCOLORS); break;
        case 'i': do_chan_mode(MODE_INVITEONLY); break;
        case 'm': do_chan_mode(MODE_MODERATED); break;
        case 'n': do_chan_mode(MODE_NOPRIVMSGS); break;
        case 'p': do_chan_mode(MODE_PRIVATE); break;
        case 'r': do_chan_mode(MODE_REGONLY); break;
        case 's': do_chan_mode(MODE_SECRET); break;
        case 't': do_chan_mode(MODE_TOPICLIMIT); break;
        case 'S': do_chan_mode(MODE_STRIPCOLOR); break;
        case 'M': do_chan_mode(MODE_MODUNREG); break;
        case 'N': do_chan_mode(MODE_NONOTICE); break;
        case 'Q': do_chan_mode(MODE_NOQUITMSGS); break;
        case 'T': do_chan_mode(MODE_NOAMSG); break;
        case 'O': do_chan_mode(MODE_OPERSONLY); break;
        case 'a': do_chan_mode(MODE_ADMINSONLY); break;
        case 'Z': do_chan_mode(MODE_SSLONLY); break;
	case 'L': do_chan_mode(MODE_HIDEMODE); break;
	case 'z':
	  if (!(flags & MCP_REGISTERED)) {
              do_chan_mode(MODE_REGISTERED);
	  } else {
              mod_chanmode_free(change);
              return NULL;
	  }
	  break;
#undef do_chan_mode
        case 'l':
            if (add) {
                if (in_arg >= argc)
                    goto error;
                change->modes_set |= MODE_LIMIT;
                change->new_limit = atoi(modes[in_arg++]);
            } else {
                change->modes_set &= ~MODE_LIMIT;
                change->modes_clear |= MODE_LIMIT;
            }
            break;
        case 'k':
            if (add) {
                if ((in_arg >= argc)
                    || keyncpy(change->new_key, modes[in_arg++], sizeof(change->new_key)))
                    goto error;
                change->modes_set |= MODE_KEY;
            } else {
                change->modes_clear |= MODE_KEY;
                if (!(flags & MCP_KEY_FREE)) {
                    if (in_arg >= argc)
                        goto error;
                    in_arg++;
                }
            }
            break;
        case 'U':
            if (add)
            {
                if ((in_arg >= argc)
                    || keyncpy(change->new_upass, modes[in_arg++], sizeof(change->new_upass)))
                    goto error;
                change->modes_set |= MODE_UPASS;
            } else {
                change->modes_clear |= MODE_UPASS;
                if (!(flags & MCP_UPASS_FREE)) {
                    if (in_arg >= argc)
                        goto error;
                    in_arg++;
                }
            }
            break;
        case 'A':
            if (add) {
                if ((in_arg >= argc)
                    || keyncpy(change->new_apass, modes[in_arg++], sizeof(change->new_apass)))
                    goto error;
                change->modes_set |= MODE_APASS;
            } else {
                change->modes_clear |= MODE_APASS;
                if (!(flags & MCP_APASS_FREE)) {
                    if (in_arg >= argc)
                      goto error;
                    in_arg++;
                }
            }
            break;
        case 'b':
            if (!(flags & MCP_ALLOW_OVB))
                goto error;
            if (in_arg >= argc)
                goto error;
            change->args[ch_arg].mode = MODE_BAN;
            if (!add)
                change->args[ch_arg].mode |= MODE_REMOVE;
            change->args[ch_arg++].u.hostmask = modes[in_arg++];
            break;
        case 'e':
            if (!(flags & MCP_ALLOW_OVB))
                goto error;
            if (in_arg >= argc)
                goto error;
            change->args[ch_arg].mode = MODE_EXEMPT;
            if (!add)
                change->args[ch_arg].mode |= MODE_REMOVE;
            change->args[ch_arg++].u.hostmask = modes[in_arg++];
            break;
        case 'o': case 'h': case 'v':
        {
            struct userNode *victim;
            char *oplevel_str;
            int oplevel;

            if (in_arg >= argc)
                goto error;
            oplevel_str = strchr(modes[in_arg], ':');
            if (oplevel_str)
            {
                /* XXYYY M #channel +o XXYYY:<oplevel> */
                *oplevel_str++ = '\0';
                oplevel = parse_oplevel(oplevel_str);
                if (oplevel <= base_oplevel && !(flags & MCP_FROM_SERVER))
                    oplevel = base_oplevel + 1;
            }
            else if (channel->modes & MODE_UPASS)
                oplevel = base_oplevel + 1;
            else
                oplevel = -1;

            /* Check that oplevel is within bounds. */
            if (oplevel > MAXOPLEVEL)
                oplevel = MAXOPLEVEL;

            if (!(flags & MCP_ALLOW_OVB))
                goto error;
            if (in_arg >= argc)
                goto error;

            if (modes[0][ii] == 'o')
                change->args[ch_arg].mode = MODE_CHANOP;
            else if (modes[0][ii] == 'v')
                change->args[ch_arg].mode = MODE_VOICE;

            if (!add)
                change->args[ch_arg].mode |= MODE_REMOVE;
            if (flags & MCP_FROM_SERVER)
                victim = GetUserN(modes[in_arg++]);
            else
                victim = GetUserH(modes[in_arg++]);
            if (!victim)
                continue;
            if ((change->args[ch_arg].u.member = GetUserMode(channel, victim)))
            {
                /* Apply the oplevel change */
                change->args[ch_arg].u.member->oplevel = oplevel;
                ch_arg++;
            }
            break;
        }
        default:
            if (!(flags & MCP_FROM_SERVER))
                goto error;
            break;
        }
    }
    change->argc = ch_arg; /* in case any turned out to be ignored */
    if (change->modes_set & MODE_SECRET) {
        change->modes_set &= ~(MODE_PRIVATE);
        change->modes_clear |= MODE_PRIVATE;
    } else if (change->modes_set & MODE_PRIVATE) {
        change->modes_set &= ~(MODE_SECRET);
        change->modes_clear |= MODE_SECRET;
    }
    return change;
  error:
    mod_chanmode_free(change);
    return NULL;
}

struct chanmode_buffer {
    char modes[MAXLEN];
    char args[MAXLEN];
    struct chanNode *channel;
    struct userNode *actor;
    unsigned int modes_used;
    unsigned int args_used;
    size_t chname_len;
    unsigned int is_add : 1;
    unsigned int is_chanop : 1;
};

static void
mod_chanmode_append(struct chanmode_buffer *buf, char ch, const char *arg)
{
    size_t arg_len = strlen(arg);
    if (buf->modes_used > (MAXMODEPARAMS) ||
        buf->modes_used + buf->args_used + buf->chname_len + arg_len > 450) {
        memcpy(buf->modes + buf->modes_used, buf->args, buf->args_used);
        buf->modes[buf->modes_used + buf->args_used] = '\0';
        irc_mode((buf->is_chanop ? buf->actor : NULL), buf->channel, buf->modes);
        buf->modes[0] = buf->is_add ? '+' : '-';
        buf->modes_used = 1;
        buf->args_used = 0;
    }
    buf->modes[buf->modes_used++] = ch;
    buf->args[buf->args_used++] = ' ';
    memcpy(buf->args + buf->args_used, arg, arg_len);
    buf->args_used += arg_len;
}

void
mod_chanmode_announce(struct userNode *who, struct chanNode *channel, struct mod_chanmode *change)
{
    struct chanmode_buffer chbuf;
    unsigned int arg;
    struct modeNode *mn;
    char int_buff[32], mode = '\0';

    assert(change->argc <= change->alloc_argc);
    memset(&chbuf, 0, sizeof(chbuf));
    chbuf.channel = channel;
    chbuf.actor = who;
    chbuf.chname_len = strlen(channel->name);

    mn = GetUserMode(channel, who);
    if ((mn && (mn->modes & MODE_CHANOP)) || off_channel)
        chbuf.is_chanop = 1;

    /* First remove modes */
    chbuf.is_add = 0;
    if (change->modes_clear) {
        if (mode != '-')
            chbuf.modes[chbuf.modes_used++] = mode = '-';
#define DO_MODE_CHAR(BIT, CHAR) if (change->modes_clear & MODE_##BIT) chbuf.modes[chbuf.modes_used++] = CHAR
        DO_MODE_CHAR(PRIVATE, 'p');
        DO_MODE_CHAR(SECRET, 's');
        DO_MODE_CHAR(MODERATED, 'm');
        DO_MODE_CHAR(TOPICLIMIT, 't');
        DO_MODE_CHAR(INVITEONLY, 'i');
        DO_MODE_CHAR(NOPRIVMSGS, 'n');
        DO_MODE_CHAR(LIMIT, 'l');
        DO_MODE_CHAR(DELAYJOINS, 'D');
        DO_MODE_CHAR(REGONLY, 'r');
        DO_MODE_CHAR(NOCOLORS, 'c');
        DO_MODE_CHAR(NOCTCPS, 'C');
        DO_MODE_CHAR(STRIPCOLOR, 'S');
        DO_MODE_CHAR(MODUNREG, 'M');
        DO_MODE_CHAR(NONOTICE, 'N');
        DO_MODE_CHAR(NOQUITMSGS, 'Q');
        DO_MODE_CHAR(NOAMSG, 'T');
        DO_MODE_CHAR(OPERSONLY, 'O');
        DO_MODE_CHAR(ADMINSONLY, 'a');
        DO_MODE_CHAR(REGISTERED, 'z');
        DO_MODE_CHAR(SSLONLY, 'Z');
	DO_MODE_CHAR(HIDEMODE, 'L');
#undef DO_MODE_CHAR
        if (change->modes_clear & channel->modes & MODE_KEY)
            mod_chanmode_append(&chbuf, 'k', channel->key);
        if (change->modes_clear & channel->modes & MODE_UPASS)
            mod_chanmode_append(&chbuf, 'U', channel->upass);
        if (change->modes_clear & channel->modes & MODE_APASS)
            mod_chanmode_append(&chbuf, 'A', channel->apass);
    }
    for (arg = 0; arg < change->argc; ++arg) {
        if (!(change->args[arg].mode & MODE_REMOVE))
            continue;
        if (mode != '-')
            chbuf.modes[chbuf.modes_used++] = mode = '-';
        switch (change->args[arg].mode & ~MODE_REMOVE) {
        case MODE_BAN:
            mod_chanmode_append(&chbuf, 'b', change->args[arg].u.hostmask);
            break;
        case MODE_EXEMPT:
            mod_chanmode_append(&chbuf, 'e', change->args[arg].u.hostmask);
            break;
        default:
            if (change->args[arg].mode & MODE_CHANOP)
                mod_chanmode_append(&chbuf, 'o', change->args[arg].u.member->user->numeric);
            if (change->args[arg].mode & MODE_VOICE)
                mod_chanmode_append(&chbuf, 'v', change->args[arg].u.member->user->numeric);
            break;
        }
    }

    /* Then set them */
    chbuf.is_add = 1;
    if (change->modes_set) {
        if (mode != '+')
            chbuf.modes[chbuf.modes_used++] = mode = '+';
#define DO_MODE_CHAR(BIT, CHAR) if (change->modes_set & MODE_##BIT) chbuf.modes[chbuf.modes_used++] = CHAR
        DO_MODE_CHAR(PRIVATE, 'p');
        DO_MODE_CHAR(SECRET, 's');
        DO_MODE_CHAR(MODERATED, 'm');
        DO_MODE_CHAR(TOPICLIMIT, 't');
        DO_MODE_CHAR(INVITEONLY, 'i');
        DO_MODE_CHAR(NOPRIVMSGS, 'n');
        DO_MODE_CHAR(DELAYJOINS, 'D');
        DO_MODE_CHAR(REGONLY, 'r');
        DO_MODE_CHAR(NOCOLORS, 'c');
        DO_MODE_CHAR(NOCTCPS, 'C');
        DO_MODE_CHAR(STRIPCOLOR, 'S');
        DO_MODE_CHAR(MODUNREG, 'M');
        DO_MODE_CHAR(NONOTICE, 'N');
        DO_MODE_CHAR(NOQUITMSGS, 'Q');
        DO_MODE_CHAR(NOAMSG, 'T');
        DO_MODE_CHAR(OPERSONLY, 'O');
        DO_MODE_CHAR(ADMINSONLY, 'a');
        DO_MODE_CHAR(REGISTERED, 'z');
        DO_MODE_CHAR(SSLONLY, 'Z');
	DO_MODE_CHAR(HIDEMODE, 'L');
#undef DO_MODE_CHAR
        if(change->modes_set & MODE_KEY)
            mod_chanmode_append(&chbuf, 'k', change->new_key);
        if (change->modes_set & MODE_UPASS)
            mod_chanmode_append(&chbuf, 'U', change->new_upass);
        if (change->modes_set & MODE_APASS)
            mod_chanmode_append(&chbuf, 'A', change->new_apass);
        if(change->modes_set & MODE_LIMIT) {
            sprintf(int_buff, "%d", change->new_limit);
            mod_chanmode_append(&chbuf, 'l', int_buff);
        }
    }
    for (arg = 0; arg < change->argc; ++arg) {
        if (change->args[arg].mode & MODE_REMOVE)
            continue;
        if (mode != '+')
            chbuf.modes[chbuf.modes_used++] = mode = '+';
        switch (change->args[arg].mode) {
        case MODE_BAN:
            mod_chanmode_append(&chbuf, 'b', change->args[arg].u.hostmask);
            break;
        case MODE_EXEMPT:
            mod_chanmode_append(&chbuf, 'e', change->args[arg].u.hostmask);
            break;
        default:
            if (change->args[arg].mode & MODE_CHANOP)
                mod_chanmode_append(&chbuf, 'o', change->args[arg].u.member->user->numeric);
            if (change->args[arg].mode & MODE_VOICE)
                mod_chanmode_append(&chbuf, 'v', change->args[arg].u.member->user->numeric);
            break;
        }
    }

    /* Flush the buffer and apply changes locally */
    if (chbuf.modes_used > 0) {
        memcpy(chbuf.modes + chbuf.modes_used, chbuf.args, chbuf.args_used);
        chbuf.modes[chbuf.modes_used + chbuf.args_used] = '\0';
        irc_mode((chbuf.is_chanop ? chbuf.actor : NULL), chbuf.channel, chbuf.modes);
    }
    mod_chanmode_apply(who, channel, change);
}

char *
mod_chanmode_format(struct mod_chanmode *change, char *outbuff)
{
    unsigned int used = 0;
    assert(change->argc <= change->alloc_argc);
    if (change->modes_clear) {
        outbuff[used++] = '-';
#define DO_MODE_CHAR(BIT, CHAR) if (change->modes_clear & MODE_##BIT) outbuff[used++] = CHAR
        DO_MODE_CHAR(PRIVATE, 'p');
        DO_MODE_CHAR(SECRET, 's');
        DO_MODE_CHAR(MODERATED, 'm');
        DO_MODE_CHAR(TOPICLIMIT, 't');
        DO_MODE_CHAR(INVITEONLY, 'i');
        DO_MODE_CHAR(NOPRIVMSGS, 'n');
        DO_MODE_CHAR(LIMIT, 'l');
        DO_MODE_CHAR(KEY, 'k');
        DO_MODE_CHAR(UPASS, 'U');
        DO_MODE_CHAR(APASS, 'A');
        DO_MODE_CHAR(DELAYJOINS, 'D');
        DO_MODE_CHAR(REGONLY, 'r');
        DO_MODE_CHAR(NOCOLORS, 'c');
        DO_MODE_CHAR(NOCTCPS, 'C');
        DO_MODE_CHAR(STRIPCOLOR, 'S');
        DO_MODE_CHAR(MODUNREG, 'M');
        DO_MODE_CHAR(NONOTICE, 'N');
        DO_MODE_CHAR(NOQUITMSGS, 'Q');
        DO_MODE_CHAR(NOAMSG, 'T');
        DO_MODE_CHAR(OPERSONLY, 'O');
        DO_MODE_CHAR(ADMINSONLY, 'a');
        DO_MODE_CHAR(REGISTERED, 'z');
        DO_MODE_CHAR(SSLONLY, 'Z');
	DO_MODE_CHAR(HIDEMODE, 'L');
#undef DO_MODE_CHAR
    }
    if (change->modes_set) {
        outbuff[used++] = '+';
#define DO_MODE_CHAR(BIT, CHAR) if (change->modes_set & MODE_##BIT) outbuff[used++] = CHAR
        DO_MODE_CHAR(PRIVATE, 'p');
        DO_MODE_CHAR(SECRET, 's');
        DO_MODE_CHAR(MODERATED, 'm');
        DO_MODE_CHAR(TOPICLIMIT, 't');
        DO_MODE_CHAR(INVITEONLY, 'i');
        DO_MODE_CHAR(NOPRIVMSGS, 'n');
        DO_MODE_CHAR(DELAYJOINS, 'D');
        DO_MODE_CHAR(REGONLY, 'r');
        DO_MODE_CHAR(NOCOLORS, 'c');
        DO_MODE_CHAR(NOCTCPS, 'C');
        DO_MODE_CHAR(STRIPCOLOR, 'S');
        DO_MODE_CHAR(MODUNREG, 'M');
        DO_MODE_CHAR(NONOTICE, 'N');
        DO_MODE_CHAR(NOQUITMSGS, 'Q');
        DO_MODE_CHAR(NOAMSG, 'T');
        DO_MODE_CHAR(OPERSONLY, 'O');
        DO_MODE_CHAR(ADMINSONLY, 'a');
        DO_MODE_CHAR(REGISTERED, 'z');
        DO_MODE_CHAR(SSLONLY, 'Z');
	DO_MODE_CHAR(HIDEMODE, 'L');

	DO_MODE_CHAR(KEY, 'k');
	DO_MODE_CHAR(LIMIT, 'l');
	DO_MODE_CHAR(APASS, 'A');
	DO_MODE_CHAR(UPASS, 'U');
#undef DO_MODE_CHAR

#define DO_MODE_PARM(BIT, PARM) if (change->modes_set & MODE_##BIT) used += sprintf(outbuff+used, " %s", PARM);
	DO_MODE_PARM(KEY, change->new_key);
	if (change->modes_set & MODE_LIMIT) used += sprintf(outbuff+used, " %d", change->new_limit);
	DO_MODE_PARM(APASS, change->new_apass);
	DO_MODE_PARM(UPASS, change->new_upass);
#undef DO_MODE_PARM
    }
    outbuff[used] = 0;
    return outbuff;
}

static int
clear_chanmode(struct chanNode *channel, const char *modes)
{
    unsigned int cleared;

    for (cleared = 0; *modes; modes++) {
        switch (*modes) {
        case 'o': cleared |= MODE_CHANOP; break;
        case 'v': cleared |= MODE_VOICE; break;
        case 'p': cleared |= MODE_PRIVATE; break;
        case 's': cleared |= MODE_SECRET; break;
        case 'm': cleared |= MODE_MODERATED; break;
        case 't': cleared |= MODE_TOPICLIMIT; break;
        case 'i': cleared |= MODE_INVITEONLY; break;
        case 'n': cleared |= MODE_NOPRIVMSGS; break;
        case 'k':
            cleared |= MODE_KEY;
            channel->key[0] = '\0';
            break;
        case 'A':
            cleared |= MODE_APASS;
            channel->apass[0] = '\0';
            break;
        case 'U':
            cleared |= MODE_UPASS;
            channel->upass[0] = '\0';
            break;
        case 'l':
            cleared |= MODE_LIMIT;
            channel->limit = 0;
            break;
        case 'b': cleared |= MODE_BAN; break;
        case 'e': cleared |= MODE_EXEMPT; break;
        case 'D': cleared |= MODE_DELAYJOINS; break;
        case 'r': cleared |= MODE_REGONLY; break;
        case 'c': cleared |= MODE_NOCOLORS; break;
        case 'C': cleared |= MODE_NOCTCPS; break;
        case 'S': cleared |= MODE_STRIPCOLOR; break;
        case 'M': cleared |= MODE_MODUNREG; break;
        case 'N': cleared |= MODE_NONOTICE; break;
        case 'Q': cleared |= MODE_NOQUITMSGS; break;
        case 'T': cleared |= MODE_NOAMSG; break;
        case 'O': cleared |= MODE_OPERSONLY; break;
        case 'a': cleared |= MODE_ADMINSONLY; break;
        case 'z': cleared |= MODE_REGISTERED; break;
        case 'Z': cleared |= MODE_SSLONLY; break;
	case 'L': cleared |= MODE_HIDEMODE; break;
        }
    }

    if (!cleared)
        return 1;

    /* Remove simple modes. */
    channel->modes &= ~cleared;

    /* If removing bans, kill 'em all. */
    if ((cleared & MODE_BAN) && channel->banlist.used) {
        unsigned int i;
        for (i=0; i<channel->banlist.used; i++)
            free(channel->banlist.list[i]);
        channel->banlist.used = 0;
    }

    /* If removing exempts, kill 'em all. */
    if ((cleared & MODE_EXEMPT) && channel->exemptlist.used) {
        unsigned int i;
        for (i=0; i<channel->exemptlist.used; i++)
            free(channel->exemptlist.list[i]);
        channel->exemptlist.used = 0;
    }

    /* Removed member modes. */
    if ((cleared & (MODE_CHANOP | MODE_VOICE)) && channel->members.used) {
        int mask = ~(cleared & (MODE_CHANOP | MODE_VOICE));
        unsigned int i;

        for (i = 0; i < channel->members.used; i++)
            channel->members.list[i]->modes &= mask;
    }

    return 1;
}

void
reg_privmsg_func(struct userNode *user, privmsg_func_t handler)
{
    unsigned int numeric = user->num_local;
    if (numeric >= num_privmsg_funcs) {
        int newnum = numeric + 8, ii;
        privmsg_funcs = realloc(privmsg_funcs, newnum*sizeof(privmsg_func_t));
        for (ii = num_privmsg_funcs; ii < newnum; ++ii)
            privmsg_funcs[ii] = NULL;
        num_privmsg_funcs = newnum;
    }
    if (privmsg_funcs[numeric])
        log_module(MAIN_LOG, LOG_WARNING, "re-registering new privmsg handler for numeric %d", numeric);
    privmsg_funcs[numeric] = handler;
}

void
unreg_privmsg_func(struct userNode *user)
{
    if (!IsLocal(user) || user->num_local >= num_privmsg_funcs)
        return; /* this really only works with users */

    privmsg_funcs[user->num_local] = NULL;
}


void
reg_notice_func(struct userNode *user, privmsg_func_t handler)
{
    unsigned int numeric = user->num_local;
    if (numeric >= num_notice_funcs) {
        int newnum = numeric + 8, ii;
        notice_funcs = realloc(notice_funcs, newnum*sizeof(privmsg_func_t));
        for (ii = num_privmsg_funcs; ii < newnum; ++ii)
            privmsg_funcs[ii] = NULL;
        num_notice_funcs = newnum;
    }
    if (notice_funcs[numeric])
        log_module(MAIN_LOG, LOG_WARNING, "re-registering new notice handler for numeric %d", numeric);
    notice_funcs[numeric] = handler;
}

void
unreg_notice_func(struct userNode *user)
{
    if (!IsLocal(user) || user->num_local >= num_privmsg_funcs)
        return; /* this really only works with users */

    notice_funcs[user->num_local] = NULL;
}

static void
send_burst(void)
{
    unsigned int i, hop, max_hop=1;
    dict_iterator_t it;

    /* burst (juped) servers, closest first (except self, which is sent already) */
    for (i=0; i<ArrayLength(servers_num); i++)
        if (servers_num[i] && servers_num[i]->hops > max_hop)
            max_hop = servers_num[i]->hops;
    for (hop=1; hop<=max_hop; hop++) {
        for (i=0;i<ArrayLength(servers_num);i++) {
            if (servers_num[i]
                && (servers_num[i]->hops == hop)
                && (servers_num[i] != self->uplink))
                irc_server(servers_num[i]);
        }
    }

    /* burst local nicks */
    for (i=0; i<=self->num_mask; i++)
        if (self->users[i])
            irc_user(self->users[i]);

    /* build dict of unbursted channel names (just copy existing channels) */
    unbursted_channels = dict_new();
    for (it = dict_first(channels); it; it = iter_next(it))
        dict_insert(unbursted_channels, iter_key(it), iter_data(it));
}

/*
 * Oplevel parsing
 */
static int
parse_oplevel(char *str)
{
    int oplevel = 0;
    while (isdigit(*str))
        oplevel = oplevel * 10 + *str++ - '0';
    return oplevel;
}

