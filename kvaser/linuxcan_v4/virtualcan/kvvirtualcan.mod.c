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
	{ 0x9c72b530, "autoremove_wake_function" },
	{ 0xc045a20c, "remove_wait_queue" },
	{ 0x47d400c9, "schedule_work" },
	{ 0xdfb8b6d0, "remove_proc_entry" },
	{ 0xa03d6a57, "__get_user_4" },
	{ 0x2d0ec62d, "queue_work" },
	{ 0x1d26aa98, "sprintf" },
	{ 0xda02d67, "jiffies" },
	{ 0x9925ce9c, "__might_sleep" },
	{ 0xd7474566, "__copy_to_user_ll" },
	{ 0xe8cbee2a, "__create_workqueue" },
	{ 0xa26ce134, "default_wake_function" },
	{ 0x80eca12a, "wait_for_completion" },
	{ 0x1af40e18, "__copy_from_user_ll" },
	{ 0x1075bf0, "panic" },
	{ 0x27a09877, "destroy_workqueue" },
	{ 0x49e79940, "__cond_resched" },
	{ 0x17d59d01, "schedule_timeout" },
	{ 0x4292364c, "schedule" },
	{ 0x799aca4, "local_bh_enable" },
	{ 0x6506d836, "register_chrdev" },
	{ 0x66a49e22, "create_proc_entry" },
	{ 0x23b324a4, "__module_put_and_exit" },
	{ 0x8351fe8e, "wake_up_process" },
	{ 0xab821cad, "__wake_up" },
	{ 0x72270e35, "do_gettimeofday" },
	{ 0x29a04e4e, "add_wait_queue" },
	{ 0x37a0cba, "kfree" },
	{ 0xf5f41dc5, "kthread_create" },
	{ 0x2e60bace, "memcpy" },
	{ 0xe419f205, "prepare_to_wait" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0x1105e73c, "finish_wait" },
	{ 0x6a414324, "complete" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

