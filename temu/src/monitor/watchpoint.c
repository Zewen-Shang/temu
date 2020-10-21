#include "watchpoint.h"
#include "expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

void display_watchpoint(){
	WP *p = head;
	while (p)
	{
		printf("Watchpoint %d: %s\n",p->NO, p->express);
		p = p->next;
	}
}

WP* new_wp(){
	if(!head){
		head = free_;
		free_ = free_->next;
		head->next = NULL;
		return head;
	}
	WP *last_used = head,*pre;
	while (last_used)
	{
		pre = last_used;
		last_used = last_used->next;
	}
	pre->next = free_;
	free_ = free_->next;
	pre->next->next = NULL;
	return pre->next;
}

void free_wp(WP *wp){
	WP *last_used = head,*pre;
	while (last_used)
	{
		pre = last_used;
		last_used = last_used->next;
	}
	last_used = pre;
	last_used->next = wp;

	pre = free_;
	while (pre->next != wp)
	{
		pre = pre->next;
	}
	pre->next = wp->next;
	wp->next = NULL;
}

bool delete_watchpoint(int NO){
	WP *p = head;
	while (p)
	{
		if(p->NO == NO){
			break;
		}
		p = p->next;
	}
	if(p->NO != NO)
		return false;
	free_wp(p);
	return true;
}