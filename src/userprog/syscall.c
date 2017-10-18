#include <stdio.h>
#include <syscall-nr.h>
#include "userprog/syscall.h"
#include "userprog/process.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "devices/input.h"


static void syscall_handler (struct intr_frame *);
void check_memory_valid(void *addr);
void * get_arg (void *ptr );

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


static void
syscall_handler (struct intr_frame *f UNUSED) 
{

  void *syscall_num = (f->esp);
  void *arg1, *arg2, *arg3, *arg4;
  check_memory_valid(syscall_num+3);

  switch( *(int*)syscall_num ){
	case SYS_HALT:
	  halt();
	  break;
	case SYS_EXIT:
	  arg1 = f->esp + 4*1;
	  check_memory_valid(arg1+3);

	  // void exit(int status)
	  exit( *(int *)arg1 ); 

	  break;
	case SYS_EXEC:
	  arg1 = f->esp + 4*1;
	  check_memory_valid(arg1+3);
	  
	  //f->eax = (uint32_t)exec( (char *)arg1 );
	  f->eax = (uint32_t)exec( (char *)*(int*)arg1 );

	  break;
	case SYS_WAIT:
	  arg1 = f->esp + 4*1;
	  check_memory_valid(arg1+3);

	  // int wait(pid_t pid)
	  f->eax = (uint32_t)process_wait( *(int *)arg1 );
	  break;
	case SYS_READ:
	  arg1 = f->esp + 4*1;
	  arg2 = f->esp + 4*2;
	  arg3 = f->esp + 4*3;
	  check_memory_valid(arg3+3);

	  // int read(int fd, void *buffer, unsigned size)
	  f->eax = read( *(int *)arg1, *(void **)arg2, *(unsigned int*)arg3 );

	  break;
	case SYS_WRITE:
	  arg1 = f->esp + 4*1;
	  arg2 = f->esp + 4*2;
	  arg3 = f->esp + 4*3;
	  check_memory_valid(arg3+3);

	  // int write(int fd, void *buffer, unsigned size)
	  f->eax = write( *(int *)arg1, *(void **)arg2, *(unsigned int*)arg3 );

	  break;
	case SYS_FIBO:
	  arg1 = f->esp + 4*1;
	  check_memory_valid(arg1+3);

	  f->eax = fibonacci( *(int *)arg1 );
	  break;
	case SYS_SUMINT:
	  arg1 = f->esp + 4*1;
	  arg2 = f->esp + 4*2;
	  arg3 = f->esp + 4*3;
	  arg4 = f->esp + 4*4;
	  check_memory_valid(arg4+3);

	  f->eax = sum_of_four_integers( *(int *)arg1, *(int *)arg2, 
									 *(int *)arg3, *(int *)arg4);
	  break;

	default:
	  break;
  }

}

void 
halt (void)
{
  shutdown_power_off();
}

void  
exit (int status)
{

  struct thread *cthread = thread_current();
  cthread->exit_status = status;
  

  char cthread_name[16+1];
  int ii = 0;
  
  memset(cthread_name, 0x00, sizeof(cthread_name));
  for( ii = 0; ii< strlen(cthread->name); ii++ ){
	if( cthread->name[ii] == '\0' || cthread->name[ii] == ' '){
	  break;
	}
	cthread_name[ii] = cthread->name[ii];
  }
  cthread->name[ii] = '\0';

  printf("%s: exit(%d)\n", cthread_name, cthread->exit_status); 

  thread_exit();

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

int
read (int fd, void *buffer, unsigned size)
{
  int ii;
  char temp;

  if ( fd == 0 ){

	for ( ii = 0; ii< size; ii++){
	  temp = input_getc();
	  *(unsigned char*)(buffer+ii) = temp;

	  if ( temp == '\0' || temp == '\n'){
		*((char*)(buffer+ii)) = '\0';
		break;		  
	  }
	}
	return ii;
  }  
  else{
	return -1;
  }
}

int 
write (int fd, const void *buffer, unsigned size)
{
  if( fd == 1 ){
	putbuf(buffer, size);
	return size;
  }
  return -1;
}

int 
fibonacci (int n)
{
  int n1 = 1, n2 = 1;
  int i, temp;

  if( n ==1 || n == 2 ){
	return 1;
  }

  for( i = 3; i <= n; i++ ){
	temp = n1;
	n1 = n2;
	n2 = n1 + temp;
  }
  return n2;		
}

int 
sum_of_four_integers (int a, int b, int c, int d)
{
  return a+b+c+d;
}

void
check_memory_valid(void *addr)
{
  if( addr < 0x08048000 || addr >= PHYS_BASE ){
	exit(-1);
  }
}

