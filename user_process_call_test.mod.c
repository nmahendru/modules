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
	{ 0xe17ada44, "read_char_dev_k" },
	{ 0x27e1a049, "printk" },
	{ 0x7a22f2bc, "kmem_cache_alloc_trace" },
	{ 0xb2ac7ac1, "kmalloc_caches" },
	{ 0x123e7740, "call_usermodehelper_exec" },
	{ 0xd7511e7d, "call_usermodehelper_setup" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=char_dev";


MODULE_INFO(srcversion, "20C9359C2714F17B1CC1C26");
