/* x3ldap.h - LDAP functionality for x3, by Rubin
 * Copyright 2002-2007 X3 Development Team
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

#ifndef _x3ldap_h
#define _x3ldap_h

#include "nickserv.h"
int ldap_do_init();

unsigned int ldap_check_auth(const char *account, const char *pass);
int ldap_do_add(const char *account, const char *password, const char *email);
int ldap_rename_account(char *oldaccount, char *newaccount);
int ldap_delete_account(char *account);
int ldap_do_oslevel(const char *account, int level, int oldlevel);
int ldap_do_modify(const char *account, const char *password, const char *email);
int ldap_get_user_info(const char *account, char **email);
int ldap_delfromgroup(char *account, const char *group);
int ldap_add2group(char *account, const char *group);
int ldap_user_exists(const char *account);

void ldap_close();

#endif /* _x3ldap_h */
