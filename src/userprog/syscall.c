#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#define LOADER_PHYS_BASE 0xc0000000     /* 3 GB. */
#define	PHYS_BASE ((void *) LOADER_PHYS_BASE)

static void syscall_handler (struct intr_frame *);
void* get_arg(void *ptr);

/*
void
check_ptr_validity(void *ptr)
{
  if(!(ptr>0x08048000 && ptr< PHYS_BASE )) exit(-1);
}
*/


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int syscall_num = (int)get_arg(f->esp);

  //int syscall_num = *(int*)(f->esp);
  printf ("system call! [%d]\n", syscall_num);

  switch(syscall_num){
	case SYS_HALT:
	  halt();
	  break;
	case SYS_EXIT:
	  exit((int)get_arg(f->esp+4));
	  break;
	case SYS_EXEC:
	  f->eax = (uint32_t) exec((char *) get_arg(f->esp+4));
	  break;
	case SYS_WAIT:
	  f->eax = (uint32_t) process_wait((int)get_arg(f->esp+4));
	  break;
	case SYS_READ:
	  f->eax = read((int)get_arg(f->esp+4), (void *)get_arg(f->esp+8), (unsigned int)get_arg(f->esp+12));
	  break;
	case SYS_WRITE:
	  f->eax = write((int)get_arg(f->esp+4), (void *)get_arg(f->esp+8), (unsigned int)get_arg(f->esp+12));
	  break;
	case SYS_FIBO:
	  break;
	case SYS_SUMINT:
	  break;

	default:
	  break;
  
  }

  thread_exit ();
}

void 
halt (void)
{
  shutdown_power_off();
}

void  
exit (int status)
{

}

pid_t 
exec (const char *cmd_line)
{
  return process_execute(cmd_line);
}

int 
wait (pid_t pid)
{
  return process_wait(pid);
}

read (int fd, void *buffer, unsigned size)
{
  
}

int 
write (int fd, const void *buffer, unsigned size)
{
}

int 
fibonacci (int n)
{
  int n1 = 1, n2 = 1;
  int i, temp;

  if( n ==1 || n == 2 ){
	return 1;
  }
  else{

	for( i = 3; i <= n; i++ ){
	  temp = n1;
	  n1 = n2;
	  n2 = n1 + temp;
	}
	return n2;		
  }
}

int 
sum_of_four_integers (int a, int b, int c, int d)
{
  return a+b+c+d;
}

void *
get_arg(void *ptr)
{
  void *temp;

  //check_ptr_validity(ptr);
  memcpy(&temp, ptr, 4);
  return  temp;

}


