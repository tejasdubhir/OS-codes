//apt-get install gcc-multilib

#include "thread.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


// thread metadata
struct thread {
	void *esp;
	struct thread *next;
	struct thread *prev;
};

struct thread *ready_list = NULL;     // ready list
struct thread *cur_thread = NULL;     // current thread
struct thread *ready_list_end = NULL;     // ready list

// defined in context.s
void context_switch(struct thread *prev, struct thread *next);

// insert the input thread to the end of the ready list.
static void push_back(struct thread *t)
{
	if(ready_list == NULL){
		ready_list = t;
		//printf("pehla elem \n");
		return;
	}
	struct thread* ptr = ready_list;
	//printf("push try karu \n");
	while(ptr->next !=NULL)
		ptr = ptr->next;
	//printf("push karu \n");
	t->prev = ptr;
	//printf("piche laga \n");
	ptr->next = t;
	//printf("aage laga\n");
	t->next = NULL;
	// if(cur_thread == NULL)
	// 	cur_thread = t;
	//printf("push hou \n");
}

// remove the first thread from the ready list and return to caller.
static struct thread *pop_front()
{
	if(ready_list ==NULL){
		return NULL;
	}
	// if(ready_list -> next == NULL){
	// 	struct thread* ptr = ready_list;
	// 	ready_list = NULL;
	// 	return ptr;
	// }
	//printf("pop karu \n");
	struct thread* ptr = ready_list;
	ready_list ->prev = NULL;
	// ready_list->next ->prev = NULL;
	//printf("poop karu \n");
	ready_list = ready_list->next;
	ptr ->next = NULL;
	ptr->prev = NULL;
	//printf("pop ho gya \n");
	return ptr;
}

// the next thread to schedule is the first thread in the ready list.
// obtain the next thread from the ready list and call context_switch.
static void schedule()
{
	//printf("shcedule karu \n");
	struct thread *prev = cur_thread;
	struct thread *next = pop_front();
	cur_thread = next;
	//printf("CS karu \n");
	context_switch(prev, next);
	// //printf("CS ho gya \n");
}

// push the cur_thread to the end of the ready list and call schedule
// if cur_thread is null, allocate struct thread for cur_thread
static void schedule1()
{
	// //printf("shcedule1 karu \n");
	if(cur_thread == NULL){
			// //printf("null mil gya shcedule1 karu \n");
			cur_thread = (struct thread*)malloc(sizeof(struct thread));
			// unsigned *ptr_stack  = malloc(4096);
			// ptr_stack+= 1024;
			// struct thread* t = malloc(sizeof(struct thread  ));
			// unsigned *stack  = ptr_stack;
			// //printf("number 5 \n");
			// ptr_stack--;
			// *ptr_stack = 0;
			// //printf("number 4 \n");
			// ptr_stack--;
			// *ptr_stack = 0;
			// ptr_stack--;
			// *ptr_stack = 0;
			// //printf("number 1 \n");
			// ptr_stack--;
			// *ptr_stack = 0;
			// //printf("number 2 \n");
			// ptr_stack--;
			// *ptr_stack = 0;
			// //printf("number 3 \n");	
			// ptr_stack--;
			// *ptr_stack = 0;
			// //printf("number 6 \n");
			// ptr_stack--;
			// *ptr_stack = 0;
			// //printf("number 17 \n");
			// cur_thread->esp = (void*)ptr_stack;		
	}
	push_back(cur_thread);
	schedule();
	// //printf("schedule1 kho gya \n");
}

// allocate stack and struct thread for new thread
// save the callee-saved registers and parameters on the stack
// set the return address to the target thread
// save the stack pointer in struct thread
// push the current thread to the end of the ready list
void create_thread(func_t func, void *param)
{
	unsigned *ptr_stack  = malloc(4096);
	ptr_stack+= 1024;
	struct thread* t = malloc(sizeof(struct thread  ));
	// //printf("number 5 \n");
	*ptr_stack = (unsigned)param;
	// //printf("number 4 \n");
	ptr_stack--;
	*ptr_stack = 0;
	// //printf("number 1 \n");

	ptr_stack--;
	*ptr_stack = (unsigned)func;
	ptr_stack--;
	*ptr_stack = 0;
	// //printf("number 2 \n");
	ptr_stack--;
	*ptr_stack = 0;
	// //printf("number 3 \n");
	ptr_stack--;
	*ptr_stack = 0;
	// //printf("number 6 \n");
	ptr_stack--;
	*ptr_stack = 0;
	// //printf("number 17 \n");
	t->esp = (void*)ptr_stack;
	// //printf("number 1 \n");
	push_back(t);
	//printf("\n");
}
	// if(cur_thread== NULL)
	// 	cur_thread = t;

// call schedule1
void thread_yield()
{
	schedule1();
}

// call schedule
void thread_exit()
{
	schedule();
}

// call schedule1 until ready_list is null
void wait_for_all()
{
	while(ready_list != NULL){
		schedule1();
	}
}
