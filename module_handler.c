/*
 *  intrpt.c - Обработчик прерываний.
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <asm/io.h>
MODULE_LICENSE("GPL");
#define KBD_IRQ            1    
#define KBD_DATA        0x60    
#define KBD_SCAN_CODE_MASK    0x7f
#define KBD_STATUS_MASK        0x80
static char scancode;
static irqreturn_t kbd_isr(int irq, void *data)
{
    char *sc = (char *)data;

    *sc = inb(KBD_DATA);

    return IRQ_WAKE_THREAD;
}
static irqreturn_t kbd_thread(int irq, void *data)
{
    char sc = *(char *)data;

    pr_alert("### Scan Code %#x %s\n",
            sc & KBD_SCAN_CODE_MASK,
            sc & KBD_STATUS_MASK ? "Released" : "Pressed");

    return IRQ_HANDLED;
}
static int __init kbd_init(void)
{
    return request_threaded_irq(KBD_IRQ, kbd_isr, kbd_thread,
            IRQF_SHARED, "test_keyb", &scancode);
}
static void __exit kbd_clean(void)
{
    free_irq(KBD_IRQ, &scancode);
}

module_init(kbd_init);
module_exit(kbd_clean);
