/*
 *	xt_iface - kernel module to match interface state flags
 *
 *	Original author: Gáspár Lajos <gaspar.lajos@glsys.eu>
 */

#define _KERNEL 1

#include <linux/if.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/netfilter/x_tables.h>
#include "xt_iface.h"

MODULE_AUTHOR("Gáspár Lajos <gaspar.lajos@glsys.eu>");
MODULE_DESCRIPTION("Xtables: iface match module");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_iface");
MODULE_ALIAS("ip6t_iface");
//MODULE_ALIAS("arpt_iface");

static struct xt_iface_flag_pairs xt_iface_lookup[XT_IFACE_FLAGCOUNT] =
{
	{.iface_flag = XT_IFACE_UP,		.iff_flag = IFF_UP},
	{.iface_flag = XT_IFACE_BROADCAST,	.iff_flag = IFF_BROADCAST},
	{.iface_flag = XT_IFACE_LOOPBACK,	.iff_flag = IFF_LOOPBACK},
	{.iface_flag = XT_IFACE_POINTOPOINT,	.iff_flag = IFF_POINTOPOINT},
	{.iface_flag = XT_IFACE_RUNNING,	.iff_flag = IFF_RUNNING},
	{.iface_flag = XT_IFACE_NOARP,		.iff_flag = IFF_NOARP},
	{.iface_flag = XT_IFACE_PROMISC,	.iff_flag = IFF_PROMISC},
	{.iface_flag = XT_IFACE_MULTICAST,	.iff_flag = IFF_MULTICAST},
	{.iface_flag = XT_IFACE_DYNAMIC,	.iff_flag = IFF_DYNAMIC},
	{.iface_flag = XT_IFACE_LOWER_UP,	.iff_flag = IFF_LOWER_UP},
	{.iface_flag = XT_IFACE_DORMANT,	.iff_flag = IFF_DORMANT},
};

static bool xt_iface_mt(const struct sk_buff *skb,
    const struct xt_match_param *par)
{
	const struct xt_iface_mtinfo *info = par->matchinfo;
	struct net_device *dev;
	bool retval;
	int i;
	DEBUGP("match...");
	DEBUGP("Interface: %s", info->ifname);
	retval =
	    ((dev = dev_get_by_name(&init_net, info->ifname)) != NULL);
	if (retval) {
#if DEBUG
		DEBUGP("dev->flags: %#8x", dev->flags);
		if (dev->flags & IFF_UP)
			DEBUGP("            %#8x (UP)", IFF_UP);
		if (dev->flags & IFF_BROADCAST)
			DEBUGP("            %#8x (BROADCAST)", IFF_BROADCAST);
		if (dev->flags & IFF_LOOPBACK)
			DEBUGP("            %#8x (LOOPBACK)", IFF_LOOPBACK);
		if (dev->flags & IFF_POINTOPOINT)
			DEBUGP("            %#8x (POINTOPOINT)", IFF_POINTOPOINT);
		if (dev->flags & IFF_RUNNING)
			DEBUGP("            %#8x (RUNNING)", IFF_RUNNING);
		if (dev->flags & IFF_NOARP)
			DEBUGP("            %#8x (NOARP)", IFF_NOARP);
		if (dev->flags & IFF_PROMISC)
			DEBUGP("            %#8x (PROMISC)", IFF_PROMISC);
		if (dev->flags & IFF_MULTICAST)
			DEBUGP("            %#8x (MULTICAST)", IFF_MULTICAST);
		if (dev->flags & IFF_DYNAMIC)
			DEBUGP("            %#8x (DYNAMIC)", IFF_DYNAMIC);
		if (dev->flags & IFF_LOWER_UP)
			DEBUGP("            %#8x (LOWER_UP)", IFF_LOWER_UP);
		if (dev->flags & IFF_DORMANT)
			DEBUGP("            %#8x (DORMANT)", IFF_DORMANT);
#endif
		for (i=0; (i<XT_IFACE_FLAGCOUNT) && (retval); i++)
			{
			if (info->flags & xt_iface_lookup[i].iface_flag)
				retval = retval && (dev->flags & xt_iface_lookup[i].iff_flag);
			if (info->invflags & xt_iface_lookup[i].iface_flag)
				retval = retval && !(dev->flags & xt_iface_lookup[i].iff_flag);
			}
		dev_put(dev);
	}
	return retval;
}

static bool xt_iface_mt_check(const struct xt_mtchk_param *par)
{
	DEBUGP("checkentry...");
	return true;
}

static void xt_iface_mt_destroy(const struct xt_mtdtor_param *par)
{
	DEBUGP("destroy...");
}

static struct xt_match xt_iface_mt_reg[] __read_mostly = {
	{
		.name       = _MODULE_NAME,
		.revision   = _MODULE_REVISION,
		.family     = AF_INET,
		.matchsize  = XT_ALIGN(sizeof(struct xt_iface_mtinfo)),
		.match      = xt_iface_mt,
		.checkentry = xt_iface_mt_check,
		.destroy    = xt_iface_mt_destroy,
		.data       = 0,
		.me         = THIS_MODULE,
	},
	{
		.name       = _MODULE_NAME,
		.revision   = _MODULE_REVISION,
		.family     = AF_INET6,
		.matchsize  = XT_ALIGN(sizeof(struct xt_iface_mtinfo)),
		.match      = xt_iface_mt,
		.checkentry = xt_iface_mt_check,
		.destroy    = xt_iface_mt_destroy,
		.data       = 0,
		.me         = THIS_MODULE,
	},
};

static int __init xt_iface_match_init(void)
{
	DEBUGP("init...\n");
	return xt_register_matches(xt_iface_mt_reg,
		ARRAY_SIZE(xt_iface_mt_reg));
}

static void __exit xt_iface_match_exit(void)
{
	DEBUGP("exit...\n");
	xt_unregister_matches(xt_iface_mt_reg, ARRAY_SIZE(xt_iface_mt_reg));
}

module_init(xt_iface_match_init);
module_exit(xt_iface_match_exit);
