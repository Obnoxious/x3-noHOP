#include "chanserv.h"
#include "conf.h"
#include "modcmd.h"
#include "nickserv.h"
#include "opserv.h"
#include "timeq.h"

static const struct message_entry msgtab[] = {
	{ "MSMSG_BAR",             "----------------------------------------"},
	{ NULL, NULL }

};

struct userNode *adminserv;

#define ADMINSERV_FUNC(NAME)	MODCMD_FUNC(NAME)
#define ADMINSERV_SYNTAX()	svscmd_send_help_brief(user, adminserv, cmd)
#define ADMINSERV_MIN_PARAMS(N)	if(argc < (N)) {	\
						reply("MSG_MISSING_PARAMS", argv[0]); \
						ADMINSERV_SYNTAX(); \
						return 0; } 

static struct {
	struct userNode *bot;
} adminserv_conf;

extern struct string_list *autojoin_channels;
const char *adminserv_module_deps[] = { NULL };
static struct module *adminserv_module;
static struct log_type *AS_LOG;

static void
adminserv_conf_read(void)
{
	dict_t conf_node;
	const char *str;

	str = "modules/adminserv";
	if (!(conf_node = conf_get_data(str, RECDB_OBJECT))) {
		log_module(AS_LOG, LOG_ERROR, "config node `%s' is missing or has wrong type.", str);
		return;
	}
}

int
adminserv_init(void)
{
	AS_LOG = log_register_type("AdminServ", "file:adminserv.log");
	conf_register_reload(adminserv_conf_read);

	adminserv_module = module_register("AdminServ", AS_LOG, "mod-adminserv.help", NULL);

	message_register_table(msgtab);

	return 1;
}

int
adminserv_finalize(void) {
	struct chanNode *chan;
	unsigned int i;
	dict_t conf_node;
	const char *str;

	str = "modules/adminserv";
	if (!(conf_node = conf_get_data(str, RECDB_OBJECT))) {
		log_module(AS_LOG, LOG_ERROR, "config node `%s' is missing or has the wrong type.", str);
		return 0;
	};

	str = database_get_data(conf_node, "bot", RECDB_QSTRING);
	if (str) {
		adminserv = adminserv_conf.bot;
		const char *modes = conf_get_data("modules/adminserv/modes", RECDB_QSTRING);
		adminserv = AddLocalUser(str, str, NULL, "Helper Service", modes);
		service_register(adminserv);
	} else {
		log_module(AS_LOG, LOG_ERROR, "database_get_data for adminserv_conf.bot failed!");
		exit(1);
	}

	if (autojoin_channels && adminserv) {
		for (i = 0; i < autojoin_channels->used; i++) {
			chan = AddChannel(autojoin_channels->list[i], now, "+nt", NULL, NULL);
			AddChannelUser(adminserv, chan)->modes |= MODE_CHANOP;
		}
	}

	return 1;
}
