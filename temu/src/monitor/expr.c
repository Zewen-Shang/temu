#include "temu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
	NOTYPE = 256, EQ,NEQ,NUM,REG,

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
	int priority;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +", NOTYPE,-1}, // spaces
	{"\\+", '+',4},	// plus
	{"-", '-',4},
	{"\\*", '*',3},
	{"/", '/',3},
	{"==", EQ,7}, // equal
	{"!=", NEQ,7}, 
	{"[0-9]+", NUM,-1},
	{"\\&\\&",'&',11},
	{"\\|\\|",'|',12},
	{"\\(", '(',-1},
	{"\\)", ')',-1},
	{"(\\$zero|\\$at|\\$v0|\\$v1|\\$a0|\\$a1|\\$a2|\\$a3|\\$t0|\\$t1|\\$t2|\\$t3|\\$t4|\\$t5|\\$t6|\\$t7|\\$s0|\\$s1|\\$s2|\\$s3|\\$s4|\\$s5|\\$s6|\\$s7|\\$t8|\\$t9|\\$k0|\\$k1|\\$gp|\\$sp|\\$fp|\\$ra)", REG,-1}};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
	int priority;
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				tokens[nr_token].type = rules[i].token_type;
				memcpy(tokens[nr_token].str,substr_start,substr_len);
				tokens[nr_token].str[substr_len] = '\0';
				tokens[nr_token].priority = rules[i].priority;
				nr_token++;

				switch(rules[i].token_type) {
					case NOTYPE:
						nr_token--;
						break;
					//default: panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

static bool check_parentheses(int start,int end){
	if(tokens[start].type == '(' && tokens[end].type == ')'){
		int i;
		int cnt = 0;
		bool flag = true;
		for (i = start; i <= end; i++)
		{
			if(tokens[i].type == '(')
				cnt++;
			if (tokens[i].type == ')')
				cnt--;
			if (cnt == 0 && i != end)
				flag = false;
			if(cnt < 0){
				panic("too more ')'.\n");
			}
		}
		if(cnt != 0)
			panic("'(' ')' don't match\n");
		return flag;
	}
	return false;
}

static int eval(int start,int end){
	if(start > end){
		panic("bad expression.");
	}else if(start == end){
		if(tokens[start].type == NUM)
			return atoi(tokens[start].str);
		else if(tokens[start].type == REG){
			int i;
			for (i = 0; i < 32;i++){
				if(strcmp(tokens[start].str,regfile[i]) == 0)
					break;
			}
			return reg_w(i);
		}
		else
			panic("The only token should be number.\n");
	}
	else if (check_parentheses(start, end))
		return eval(start + 1, end - 1);
	else
	{
		int i;//iterator
		int parCnt = 0;//parentheses
		//find dominant operator
		int dominant_index = -1, dominant_priority = -1;
		for (i = start; i <= end; i++)
		{
			switch (tokens[i].type)
			{
			case '(':
				parCnt++;
				break;
			case ')':
				parCnt--;
				break;
			case '+':
			case '-':
			case '*':
			case '/':
			case EQ:
			case NEQ:
			case '&':
			case '|':
			default:
				if(parCnt)
					continue;
				if(tokens[i].priority >= dominant_priority){
					dominant_priority = tokens[i].priority ;
					dominant_index = i;
				}
				break;
			}
		}
		switch (tokens[dominant_index].type)
		{
		case '+':
			return eval(start, dominant_index - 1) + eval(dominant_index + 1, end);
			break;
		case '-':
			return eval(start, dominant_index - 1) - eval(dominant_index + 1, end);
			break;
		case '*':
			return eval(start, dominant_index - 1) * eval(dominant_index + 1, end);
			break;
		case '/':
			return eval(start, dominant_index - 1) / eval(dominant_index + 1, end);
			break;
		case EQ:
			return eval(start, dominant_index - 1) == eval(dominant_index + 1, end);
			break;
		case NEQ:
			return eval(start, dominant_index - 1) != eval(dominant_index + 1, end);
			break;
		case '&':
			return eval(start, dominant_index - 1) && eval(dominant_index + 1, end);
			break;
		case '|':
			return eval(start, dominant_index - 1) || eval(dominant_index + 1, end);
			break;
		default:
			break;
		}
	}
	return 0;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	*success = true;
	return eval(0, nr_token - 1);
}

