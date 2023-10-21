#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// トークンの最大長（コマンド、オプションを含む）
#define TOKEN_LEN 20
#define MAX_TOK 20
typedef struct node{
	char token[TOKEN_LEN];
	struct node *next;
	struct node *prev;
}NODE;

void insert(NODE *node, char *token){
	NODE *newNode = (NODE *)malloc(sizeof(NODE));
	strcpy(newNode->token, token);
	newNode->next = NULL;
	newNode->prev = NULL;

	if(node==NULL){
		node = newNode;
		return;
	}

	NODE *_node = node;
	while(_node->next!=NULL){
		_node = _node->next;
	}
	_node->next = newNode;
	newNode->prev = _node;
	return;
}

int main(void){
	NODE* nodes = (NODE *)malloc(sizeof(NODE));
	insert(nodes, "1");
	insert(nodes, "2");
	insert(nodes, "3");
	insert(nodes, "4");
	insert(nodes, "5");
	insert(nodes, "6");
	insert(nodes, "7");
	insert(nodes, "8");
	insert(nodes, "9");
	insert(nodes, "10");

	while ((nodes = nodes->next)) {
		printf("curr:%s | prev: %s | next: %s\n",
		nodes->token,
		nodes->prev->token,
		nodes->next->token);

	}
	nodes = nodes->prev;
	while ((nodes = nodes->prev)) {
		printf("%s\n",nodes->token);
	}
	return 0;
}