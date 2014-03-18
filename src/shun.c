/* shun.c - Shun database
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

#include "heap.h"
#include "helpfile.h"
#include "log.h"
#include "saxdb.h"
#include "timeq.h"
#include "shun.h"

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#define KEY_REASON "reason"
#define KEY_EXPIRES "expires"
#define KEY_ISSUER "issuer"
#define KEY_ISSUED "issued"

static heap_t shun_heap; /* key: expiry time, data: struct shun_entry* */
static dict_t shun_dict; /* key: target, data: struct shun_entry* */

static int
shun_comparator(const void *a, const void *b)
{
    const struct shun *ga=a, *gb=b;
    return ga->expires - gb->expires;
}

static void
free_shun_from_dict(void *data)
{
    struct shun *ent = data;
    free(ent->issuer);
    free(ent->target);
    free(ent->reason);
    free(ent);
}

static void
free_shun(struct shun *ent)
{
    dict_remove(shun_dict, ent->target);
}

static int
shun_for_p(UNUSED_ARG(void *key), void *data, void *extra)
{
    struct shun *ge = data;
    return !irccasecmp(ge->target, extra);
}

static int
delete_shun_for_p(UNUSED_ARG(void *key), void *data, void *extra)
{
    struct shun *ge = data;

    if (!irccasecmp(ge->target, extra)) {
        free_shun(ge);
        return 1;
    } else {
        return 0;
    }
}

static void
shun_expire(UNUSED_ARG(void *data))
{
    time_t stopped;
    void *wraa;

    stopped = 0;
    while (heap_size(shun_heap)) {
        heap_peek(shun_heap, 0, &wraa);
        stopped = ((struct shun*)wraa)->expires;
        if (stopped > now)
            break;
        heap_pop(shun_heap);
        free_shun(wraa);
    }
    if (heap_size(shun_heap))
        timeq_add(stopped, shun_expire, NULL);
}

int
shun_remove(const char *target, int announce)
{
    int res = dict_find(shun_dict, target, NULL) ? 1 : 0;
    if (heap_remove_pred(shun_heap, delete_shun_for_p, (char*)target)) {
        void *argh;
        struct shun *new_first;
        heap_peek(shun_heap, 0, &argh);
        if (argh) {
            new_first = argh;
            timeq_del(0, shun_expire, 0, TIMEQ_IGNORE_WHEN|TIMEQ_IGNORE_DATA);
            timeq_add(new_first->expires, shun_expire, 0);
        }
    }
    if (announce)
        irc_unshun(target);
    return res;
}

struct shun *
shun_add(const char *issuer, const char *target, unsigned long duration, const char *reason, time_t issued, int announce)
{
    struct shun *ent;
    struct shun *prev_first;
    void *argh;

    heap_peek(shun_heap, 0, &argh);
    prev_first = argh;
    ent = dict_find(shun_dict, target, NULL);
    if (ent) {
        heap_remove_pred(shun_heap, shun_for_p, (char*)target);
        if (ent->expires < (time_t)(now + duration))
            ent->expires = now + duration;
    } else {
        ent = malloc(sizeof(*ent));
        ent->issued = issued;
        ent->issuer = strdup(issuer);
        ent->target = strdup(target);
        ent->expires = now + duration;
        ent->reason = strdup(reason);
        dict_insert(shun_dict, ent->target, ent);
    }
    heap_insert(shun_heap, ent, ent);
    if (!prev_first || (ent->expires < prev_first->expires)) {
	timeq_del(0, shun_expire, 0, TIMEQ_IGNORE_WHEN|TIMEQ_IGNORE_DATA);
	timeq_add(ent->expires, shun_expire, 0);
    }
    if (announce)
        irc_shun(NULL, ent);
    return ent;
}

static char *
shun_alternate_target(const char *target)
{
    const char *hostname;

    /* If no host part, bail. */
    if (!(hostname = strchr(target, '@')))
        return NULL;
    /* If host part contains wildcards, bail. */
    if (hostname[strcspn(hostname, "*?/")])
        return NULL;
    /* Get parsed address and canonical name for host. */
#if 0
    irc_in_addr_t in; /* move this to the right place */
    if (irc_pton(&in, NULL, hostname+1)) {
        if (getnameinfo(/*TODO*/))
              return NULL;
    } else if (!getaddrinfo(/*TODO*/)) {
    } else return NULL;
#else
    return NULL;
#endif
}

struct shun *
shun_find(const char *target)
{
    struct shun *res;
    dict_iterator_t it;
    char *alt_target;

    res = dict_find(shun_dict, target, NULL);
    if (res)
        return res;
    else if (target[strcspn(target, "*?")]) {
        /* Wildcard: do an obnoxiously long search. */
        for (it = dict_first(shun_dict); it; it = iter_next(it)) {
            res = iter_data(it);
            if (match_ircglob(target, res->target))
                return res;
        }
    }
    /* See if we can resolve the hostname part of the mask. */
    if ((alt_target = shun_alternate_target(target))) {
        res = shun_find(alt_target);
        free(alt_target);
        return res;
    }
    return NULL;
}

static int
shun_refresh_helper(UNUSED_ARG(void *key), void *data, void *extra)
{
    struct shun *ge = data;
    irc_shun(extra, ge);
    return 0;
}

void
shun_refresh_server(struct server *srv)
{
    heap_remove_pred(shun_heap, shun_refresh_helper, srv);
}

void
shun_refresh_all(void)
{
    heap_remove_pred(shun_heap, shun_refresh_helper, 0);
}

unsigned int
shun_count(void)
{
    return dict_size(shun_dict);
}

static int
shun_add_record(const char *key, void *data, UNUSED_ARG(void *extra))
{
    struct record_data *rd = data;
    const char *issuer, *reason, *dstr;
    time_t issued, expiration;

    if (!(reason = database_get_data(rd->d.object, KEY_REASON, RECDB_QSTRING))) {
	log_module(MAIN_LOG, LOG_ERROR, "Missing reason for shun %s", key);
	return 0;
    }
    if (!(dstr = database_get_data(rd->d.object, KEY_EXPIRES, RECDB_QSTRING))) {
	log_module(MAIN_LOG, LOG_ERROR, "Missing expiration for shun %s", key);
	return 0;
    }
    expiration = strtoul(dstr, NULL, 0);
    if ((dstr = database_get_data(rd->d.object, KEY_ISSUED, RECDB_QSTRING))) {
        issued = strtoul(dstr, NULL, 0);
    } else {
        issued = now;
    }
    if (!(issuer = database_get_data(rd->d.object, KEY_ISSUER, RECDB_QSTRING))) {
        issuer = "<unknown>";
    }
    if (expiration > now)
        shun_add(issuer, key, expiration - now, reason, issued, 0);
    return 0;
}

static int
shun_saxdb_read(struct dict *db)
{
    return dict_foreach(db, shun_add_record, 0) != NULL;
}

static int
shun_write_entry(UNUSED_ARG(void *key), void *data, void *extra)
{
    struct shun *ent = data;
    struct saxdb_context *ctx = extra;

    saxdb_start_record(ctx, ent->target, 0);
    saxdb_write_int(ctx, KEY_EXPIRES, ent->expires);
    saxdb_write_int(ctx, KEY_ISSUED, ent->issued);
    saxdb_write_string(ctx, KEY_REASON, ent->reason);
    saxdb_write_string(ctx, KEY_ISSUER, ent->issuer);
    saxdb_end_record(ctx);
    return 0;
}

static int
shun_saxdb_write(struct saxdb_context *ctx)
{
    heap_remove_pred(shun_heap, shun_write_entry, ctx);
    return 0;
}

static void
shun_db_cleanup(UNUSED_ARG(void *extra))
{
    heap_delete(shun_heap);
    dict_delete(shun_dict);
}

void
shun_init(void)
{
    shun_heap = heap_new(shun_comparator);
    shun_dict = dict_new();
    dict_set_free_data(shun_dict, free_shun_from_dict);
    saxdb_register("shun", shun_saxdb_read, shun_saxdb_write);
    reg_exit_func(shun_db_cleanup, NULL);
}

struct shun_discrim *
shun_discrim_create(struct userNode *user, struct userNode *src, unsigned int argc, char *argv[])
{
    unsigned int i;
    struct shun_discrim *discrim;

    discrim = calloc(1, sizeof(*discrim));
    discrim->max_issued = now;
    discrim->limit = 50;

    for (i=0; i<argc; i++) {
        if (i + 2 > argc) {
            send_message(user, src, "MSG_MISSING_PARAMS", argv[i]);
            goto fail;
        } else if (!irccasecmp(argv[i], "mask") || !irccasecmp(argv[i], "host")) {
            if (!irccasecmp(argv[++i], "exact"))
                discrim->target_mask_type = SEXACT;
            else if (!irccasecmp(argv[i], "subset"))
                discrim->target_mask_type = SSUBSET;
            else if (!irccasecmp(argv[i], "superset"))
                discrim->target_mask_type = SSUPERSET;
            else
                discrim->target_mask_type = SSUBSET, i--;
            if (++i == argc) {
                send_message(user, src, "MSG_MISSING_PARAMS", argv[i-1]);
                goto fail;
            }
            if (!is_shun(argv[i]) && !IsChannelName(argv[i])) {
                send_message(user, src, "MSG_INVALID_SHUN", argv[i]);
                goto fail;
            }
            discrim->target_mask = argv[i];
            discrim->alt_target_mask = shun_alternate_target(discrim->target_mask);
        } else if (!irccasecmp(argv[i], "limit"))
            discrim->limit = strtoul(argv[++i], NULL, 0);
        else if (!irccasecmp(argv[i], "reason"))
            discrim->reason_mask = argv[++i];
        else if (!irccasecmp(argv[i], "issuer"))
            discrim->issuer_mask = argv[++i];
        else if (!irccasecmp(argv[i], "after"))
            discrim->min_expire = now + ParseInterval(argv[++i]);
        else if (!irccasecmp(argv[i], "before"))
            discrim->max_issued = now - ParseInterval(argv[++i]);
        else {
            send_message(user, src, "MSG_INVALID_CRITERIA", argv[i]);
            goto fail;
        }
    }
    return discrim;
  fail:
    free(discrim->alt_target_mask);
    free(discrim);
    return NULL;
}

struct shun_search {
    struct shun_discrim *discrim;
    shun_search_func func;
    void *data;
    unsigned int hits;
};

static int
shun_discrim_match(struct shun *shun, struct shun_discrim *discrim)
{
    if ((discrim->issuer_mask && !match_ircglob(shun->issuer, discrim->issuer_mask))
        || (discrim->reason_mask && !match_ircglob(shun->reason, discrim->reason_mask))
        || (discrim->target_mask
            && (((discrim->target_mask_type == SSUBSET)
                 && !match_ircglobs(discrim->target_mask, shun->target)
                 && (!discrim->alt_target_mask
                     || !match_ircglobs(discrim->alt_target_mask, shun->target)))
                || ((discrim->target_mask_type == SEXACT)
                    && irccasecmp(discrim->target_mask, shun->target)
                    && (!discrim->alt_target_mask
                        || !irccasecmp(discrim->alt_target_mask, shun->target)))
                || ((discrim->target_mask_type == SSUPERSET)
                    && !match_ircglobs(shun->target, discrim->target_mask)
                    && (!discrim->alt_target_mask
                        || !match_ircglobs(discrim->alt_target_mask, shun->target)))))
        || (discrim->max_issued < shun->issued)
        || (discrim->min_expire > shun->expires)) {
        return 0;
    }
    return 1;
}

static int
shun_search_helper(UNUSED_ARG(void *key), void *data, void *extra)
{
    struct shun *shun = data;
    struct shun_search *search = extra;

    if (shun_discrim_match(shun, search->discrim)
        && (search->hits++ < search->discrim->limit)) {
        search->func(shun, search->data);
    }
    return 0;
}

unsigned int
shun_discrim_search(struct shun_discrim *discrim, shun_search_func gsf, void *data)
{
    struct shun_search search;
    search.discrim = discrim;
    search.func = gsf;
    search.data = data;
    search.hits = 0;
    heap_remove_pred(shun_heap, shun_search_helper, &search);
    return search.hits;
}

