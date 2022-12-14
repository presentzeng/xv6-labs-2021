#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  uint64 addr;
  if(argaddr(0, &addr) < 0)
    return -1;
  char *buf = (char*)addr;

  int n;
  if(argint(1, &n) < 0)
    return -1;

  uint64 ubit;
  uint64 bit = 0;
  if(argaddr(2, &ubit) < 0)
    return -1;
  unsigned int * upbit =(unsigned int *) ubit;
  //unsigned int * pbit = (unsigned int *) bit;

  for(int i = 1; i < n; i++)
  {
    //walk(myproc()->pagetable, (uint64)&buf[i*PGSIZE], 0);
    pte_t *pte = walk(myproc()->pagetable, (uint64)&buf[i*PGSIZE], 0);
    if(*pte & PTE_A)
    {
        //*pbit = *pbit & (1 << i);
        bit = bit | (1 << i);
        printf("%d\n", bit);
        *pte = *pte & ~(1 << i);
    }
  }
  if(copyout(myproc()->pagetable, (uint64) upbit, (char *)&bit, sizeof(bit)) < 0)
    return -1;
  // lab pgtbl: your code here.
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
