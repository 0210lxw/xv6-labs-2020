#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
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

uint64 sys_sbrk(void)
{
  int addr;
  int n;

  // 获取系统调用的第一个参数，即需要增加或减少的字节数
  if(argint(0, &n) < 0)
    return -1;

  struct proc *p = myproc(); // 获取当前进程
  addr = p->sz;              // 当前进程的堆顶地址
  uint64 sz = p->sz;         // 当前进程的堆大小

  if(n > 0)
  {
    // 如果n大于0，表示需要增加堆的大小
    // 这里实现了懒分配，只是增加了堆的大小，但并没有立即分配物理内存
    p->sz += n;
  }
  else if(sz + n > 0)
  {
    // 如果n小于0，表示需要减少堆的大小
    // 检查减少后的堆大小是否仍然大于0
    sz = uvmdealloc(p->pagetable, sz, sz + n); // 释放多余的虚拟内存
    p->sz = sz; // 更新进程的堆大小
  }
  else
  {
    // 如果减少后的堆大小小于或等于0，返回-1表示错误
    return -1;
  }

  // 返回原始的堆顶地址
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
