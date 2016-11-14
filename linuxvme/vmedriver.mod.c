#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

#undef unix
struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = __stringify(KBUILD_MODNAME),
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0x65b4e613, "struct_module" },
	{ 0x7da8156e, "__kmalloc" },
	{ 0xf34350ab, "__mod_timer" },
	{ 0x89b301d4, "param_get_int" },
	{ 0x1683aa15, "del_timer" },
	{ 0x9c72b530, "autoremove_wake_function" },
	{ 0x6c3397fb, "malloc_sizes" },
	{ 0xb7c82d3b, "pci_disable_device" },
	{ 0xdfb8b6d0, "remove_proc_entry" },
	{ 0xa03d6a57, "__get_user_4" },
	{ 0x98bd6f46, "param_set_int" },
	{ 0x1d26aa98, "sprintf" },
	{ 0xb3a307c6, "si_meminfo" },
	{ 0xda02d67, "jiffies" },
	{ 0x9925ce9c, "__might_sleep" },
	{ 0xd7474566, "__copy_to_user_ll" },
	{ 0x9ef719e7, "remap_page_range" },
	{ 0xaa136450, "param_get_charp" },
	{ 0x1af40e18, "__copy_from_user_ll" },
	{ 0x1b7d4074, "printk" },
	{ 0x3412acda, "pci_find_device" },
	{ 0x9eac042a, "__ioremap" },
	{ 0x49e79940, "__cond_resched" },
	{ 0x7dceceac, "capable" },
	{ 0x123d3b6a, "kmem_cache_alloc" },
	{ 0xbf758c44, "pci_bus_read_config_dword" },
	{ 0x26e96637, "request_irq" },
	{ 0x4292364c, "schedule" },
	{ 0x8a7d1c31, "high_memory" },
	{ 0x6506d836, "register_chrdev" },
	{ 0x66a49e22, "create_proc_entry" },
	{ 0x8351fe8e, "wake_up_process" },
	{ 0xa7deb079, "proc_root" },
	{ 0x2cd7da6c, "param_set_charp" },
	{ 0xab821cad, "__wake_up" },
	{ 0xa1e60194, "pci_bus_write_config_byte" },
	{ 0x37a0cba, "kfree" },
	{ 0x2e60bace, "memcpy" },
	{ 0xe419f205, "prepare_to_wait" },
	{ 0xedc03953, "iounmap" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0x1105e73c, "finish_wait" },
	{ 0xd22b546, "__up_wakeup" },
	{ 0xf2520b76, "__down_failed" },
	{ 0x7a93221b, "pci_enable_device" },
	{ 0x954cbb26, "vsprintf" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

