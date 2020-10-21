#include "monitor.h"
#include "watchpoint.h"
#include "temu.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

void display_reg();
void display_memory(uint32_t addr,size_t size);
void display_watchpoint();
bool delete_watchpoint(int NO);
uint32_t expr(char *e, bool *success);
WP *new_wp();
/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(temu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_info(char *args){
	if(args[0] == 'r'){
		display_reg();
		return 0;
	}
	if(args[0] == 'w'){
		//display_memory(0,30);
		display_watchpoint();
		return 0;
	}
	return -1;
}

static int cmd_p(char *args){
	bool suc;
	int ans = expr(args,&suc);
	if(suc){
		printf("%x\n", ans);
		return 0;
	}else{
		return -1;
	}
}

static int cmd_si(char *args){
	int steps;
	sscanf(args, "%d", &steps);
	cpu_exec(steps);
	return 0;
}

static int cmd_w(char *args){
	WP *wp = new_wp();
	memcpy(wp->express, args, sizeof(args));
	bool suc;
	uint32_t value = expr(args, &suc);
	if(!suc)
		return -1;
	wp->value = value;
	return 0;
}

static int cmd_d(char *args){
	int NO = atoi(args);
	if(delete_watchpoint(NO)){
		return 0;
	}else{
		return -1;
	}
}

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table[] = {
	{"help", "Display informations about all supported commands", cmd_help},
	{"c", "Continue the execution of the program", cmd_c},
	{"q", "Exit TEMU", cmd_q},
	{"si", "Exec n steps.", cmd_si},
	{"info", "Display Information", cmd_info},
	{"p", "Display expression value", cmd_p},
	{"w", "Add watchpoint", cmd_w},
	{"d", "Delete watchpoint", cmd_d}

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

		int i;

		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
