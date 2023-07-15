#include "fault.h"



static __attribute__((section(".noinit")))  fault_log_t fault_log;


static bool is_fault = false;




bool faultInit(void)
{

  if (fault_log.magic_number == 0x5555AAAA)
  {
    fault_log.magic_number = 0;

    logPrintf("Fault Message\n");
    logPrintf("  Type : %d\n",     fault_log.type);
    logPrintf("  Msg  : %s\n",     fault_log.msg);

    if (fault_log.is_reg == true)
    {
      logPrintf("  R0   : 0x%08X\n", fault_log.REG_R0);
      logPrintf("  R1   : 0x%08X\n", fault_log.REG_R1);
      logPrintf("  R2   : 0x%08X\n", fault_log.REG_R2);
      logPrintf("  R3   : 0x%08X\n", fault_log.REG_R3);
      logPrintf("  R12  : 0x%08X\n", fault_log.REG_R12);
      logPrintf("  LR   : 0x%08X\n", fault_log.REG_LR);
      logPrintf("  PC   : 0x%08X\n", fault_log.REG_PC);
      logPrintf("  PSR  : 0x%08X\n", fault_log.REG_PSR);
    }
    logPrintf("\n");

    is_fault = true;
  }

  return true;
}

bool faultIsReady(void)
{
  return is_fault;
}

bool faultReset(const char *p_msg, uint32_t *p_stack)
{
  fault_log.magic_number = 0x5555AAAA;
  fault_log.type = 0;

  if (p_stack != NULL)
  {
    fault_log.is_reg  = true; 
    fault_log.REG_R0  = p_stack[0];  // Register R0
    fault_log.REG_R1  = p_stack[1];  // Register R1
    fault_log.REG_R2  = p_stack[2];  // Register R2
    fault_log.REG_R3  = p_stack[3];  // Register R3
    fault_log.REG_R12 = p_stack[4];  // Register R12
    fault_log.REG_LR  = p_stack[5];  // Link register LR
    fault_log.REG_PC  = p_stack[6];  // Program counter PC
    fault_log.REG_PSR = p_stack[7];  // Program status word PSR
  }
  else
  {
    fault_log.is_reg = false;
  }

  uint32_t i;
  bool last = false;
  for (i=0; i<sizeof(fault_log.msg)-1; i++)
  {
    
    if (p_msg[i] != 0 && last == false)
    {
      fault_log.msg[i] = p_msg[i];
    }
    else
    {
      last = true;
      fault_log.msg[i] = 0;
    }
  }


  NVIC_SystemReset();

  return true;
}