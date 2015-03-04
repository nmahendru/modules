#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x3c5a2042, "module_layout" },
	{ 0x52dc4c15, "genl_unregister_ops" },
	{ 0xba35c725, "genl_unregister_family" },
	{ 0x3291a5ee, "genl_register_ops" },
	{ 0x70f20fc3, "genl_register_family" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xc92bf12f, "netlink_unicast" },
	{ 0x757775ba, "nla_put" },
	{ 0xde0bdcff, "memset" },
	{ 0x212fafe6, "skb_put" },
	{ 0x626db2a6, "__alloc_skb" },
	{ 0x999e8297, "vfree" },
	{ 0xe914e41e, "strcpy" },
	{ 0x42224298, "sscanf" },
	{ 0x25ec1b28, "strlen" },
	{ 0x3950e170, "my_char_dev_read_k" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x27e1a049, "printk" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=my_char_dev";


MODULE_INFO(srcversion, "D941D6B97FD4E7C59CF4D20");
