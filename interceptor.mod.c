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
	{ 0xf9a482f9, "msleep" },
	{ 0xdcb0349b, "sys_close" },
	{ 0xe2d5255a, "strcmp" },
	{ 0xe914e41e, "strcpy" },
	{ 0x5a34a45c, "__kmalloc" },
	{ 0x25ec1b28, "strlen" },
	{ 0x7a22f2bc, "kmem_cache_alloc_trace" },
	{ 0xb2ac7ac1, "kmalloc_caches" },
	{ 0x152de1e4, "current_task" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x27e1a049, "printk" },
	{ 0x123e7740, "call_usermodehelper_exec" },
	{ 0xd7511e7d, "call_usermodehelper_setup" },
	{ 0x291d60bb, "my_char_dev_write_k" },
	{ 0x91715312, "sprintf" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=my_char_dev";


MODULE_INFO(srcversion, "8EAC3F340AD6A92446AD669");
