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
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x3ec8886f, "param_ops_int" },
	{ 0x25ec1b28, "strlen" },
	{ 0x813bb835, "__register_chrdev" },
	{ 0x4f8b5ddb, "_copy_to_user" },
	{ 0x27e1a049, "printk" },
	{ 0x3032758b, "__tracepoint_module_get" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0xb4390f9a, "mcount" },
	{ 0xa960db2a, "module_put" },
	{ 0x4f6b400b, "_copy_from_user" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "8DC924E4434132B47E2118A");
