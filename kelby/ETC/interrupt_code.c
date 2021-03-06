{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fscript\fprq2\fcharset0 Comic Sans MS;}{\f1\fswiss\fcharset0 Arial;}}
{\*\generator Msftedit 5.41.15.1507;}\viewkind4\uc1\pard\f0\fs28 #include \ldblquote vme_rcc.h\rdblquote\par
#include <signal.h>\par
/*...*/\par
int global_interrupt;\par
/*...*/\par
void my_interrupt_handler(int sig) \{\par
\b\i /* interrupt handler function */\par
\b0\i0 static VME_InterruptInfo_t interrupt_info;\par
static u_int errod_code;\par
if(error_code = VME_InterruptInfoGet(&interrupt_info) \{\par
\b\i /* get information on interrupt */\par
\b0\i0 VME_ErrorPrint(error_code);\par
return(error_code);\par
\}\par
printf(\ldblquote INTERRUPT in example program: vector =%02x, multiple =\par
%d\\n\rdblquote , interrupt_info.vector,\par
interrupt.multiple);\par
if(interrupt_info.level == 1) \{\par
\b\i /* interrupt from a level assigned to RORA interrupters? */\par
\b0\i0 if(error_code = VME_InterruptRenable(global_interrupt) \{\par
\b\i /* re-enable interrupt => can wait again on interrupt */\par
\b0\i0 VME_ErrorPrint(error_code);\par
return(error_code);\par
\}\par
\}\par
/*...*/\par
\}\par
/*...*/\par
VME_InterruptList_t interrupt_list;\par
VME_InterruptInfo_t interrupt_info;\par
int time_out = 100000;\par
\b\i /* time-out about 100 sec */\par
\b0\i0 VME_ErrorCode_t error_code;\par
u_int error_number;\par
/*...*/\par
interrupt_list.list_of_items[0].vector = 0x11;\par
interrupt_list.list_of_items[0].level = 1;\par
interrupt_list.list_of_items[0].type = VME_INT_RORA;\par
\b\i /* fill parameters for first interrupt */\par
\b0\i0 interrupt_list.list_of_items[1].vector = 0x22;\par
interrupt_list.list_of_items[1].level = 2;\par
interrupt_list.list_of_items[1].type = VME_INT_ROAK;\par
\b\i /* fill parameters for second interrupt */\par
\b0\i0 interrupt_list.number_of_items = 2;\par
\b\i /* total number of interrupts */\par
\b0\i0 if(error_code = VME_InterruptLink(&interrupt_list, &global_interrupt) \{\par
\b\i /* link VMEbus interrupt list to application program */\par
\b0\i0 VME_ErrorPrint(error_code);\par
return(error_code);\par
\}\par
/*...*/\par
if(error_code = VME_InterruptRegisterSignal(global_interrupt,SIGBUS) \{\par
\b\i /* register SIGBUS signal for VMEbus interrupt list */\par
\b0\i0 VME_ErrorPrint(error_code);\par
return(error_code);\par
\}\par
/*...*/\par
\b\i /* install example interrupt handler for SIGBUS signal,\par
not part of this API, see function sigaction() */\par
/* VMEbus interrupts will be caught asynchronously by\par
example interrupt handler */\par
\b0\i0 /*...*/\par
if(error_code = VME_InterruptRegisterSignal(global_interrupt,0) \{\par
\b\i /* (un-)register signal for VMEbus interrupt list */\par
\b0\i0 VME_ErrorPrint(error_code);\par
return(error_code);\par
\}\par
if(error_code = VME_InterruptUnlink(global_interrupt) \{\par
\b\i /* unlink VMEbus interrupt list from application program */\par
\b0\i0 VME_ErrorPrint(error_code);\par
return(error_code);\par
\}\par
\f1\fs20\par
}
 