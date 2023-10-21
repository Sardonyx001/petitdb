/*

Created by Jamel Eddine Lassoued

参考サイト：
	https://stackoverflow.com/questions/66108949/how-can-i-implement-a-function-lookup-table-in-c
	https://stackoverflow.com/questions/266357/tokenizing-strings-in-c
	https://stackoverflow.com/questions/1693853/copying-arrays-of-structs-in-c
	https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
	https://stackoverflow.com/questions/13372688/sorting-members-of-structure-array 
	https://stackoverflow.com/questions/9594427/why-am-i-getting-arithmetic-of-pointer-type-is-required
	
苦労点：
	+ strtokは「\n」ごとにトークンを読み込むのでこれを配慮しないといけない。
		-> 解決策：strtokの「sep」パラメータに「\n」を追加する：　strtok(cmd, " \n")

	+ 処理を続くためにparsedTokensを毎回初期化しないといけない
		-> 解決策：　parseCmdでNULLに戻す。
		if(parsed!=NULL)　parsed = NULL;

	+ switch文に各case文の最後にbreakをいれないといけない

	+ q_sortを利用するときに、配列の範囲に注意
		->（末尾のインデックスはdb->sizeではなく、db->size-1である。）

	+ do_select関数（select処理を行う関数）では、関数参照テーブル（LUT）を利用しようとしたが、
	select関数は該当属性の値そのものを検索している一方、cmpFuncsのLUTはレコード同士を比較している。
	そのため、検索したい値が該当しているレコードを特定するための作業は、単純に検索の作業になるため意味ないかと判断した。

TODO: Ideas to improve this
	+ Refactor into multiple files 
	+ Use B+ trees for the database repressentation.
	+ Employ better search functionality
	+ Add more commands 
	+ Add better syntax for commands 
	+ 
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define CMD_LEN 100		// 処理できるコマンド行（文字単位で）の最大長
#define CMD_NUM 4		// 利用できるコマンドの数
#define TOKEN_LEN 20	// トークンの最大長（それぞれのコマンド、オプションの最大長）
#define ATTR_NUM 4		// 属性の数と
#define ATTR_LEN 20		// 属性名の最大長
#define NAME_LEN 20		// name属性の値の最大長
#define MAX_RECORD 100	// データベーステーブルのレコードの最大長

// 入力を処理するためのリスト構造
typedef struct node{
	char token[TOKEN_LEN];
	struct node *next;
}NODE;

// リスト構造への挿入関数
NODE *insert(NODE *node, char *token){
	// 新しいノードを初期化する
	NODE *newNode = (NODE *)malloc(sizeof(NODE));
	strcpy(newNode->token, token);
	newNode->next = NULL;

	// リストが空の場合、新しいノードをリストの先頭に設定して終了する
	if(node==NULL){
		node = newNode;
		return node;
	}

	// リストの最後のノードまで移動する
	NODE *_node = node;
	while(_node->next!=NULL){
		_node = _node->next;
	}

	// 最後のノードのnextポインタを新しいノードに設定してリストに追加する
	_node->next = newNode;
	return node;
}

// データベースのレコード構造
typedef struct record {
	int id;
	char name[NAME_LEN];
	int score;
}RECORD;

// メモリ内のデータベース構造（MAX_RECORDぐらいのレコードを含む）
typedef struct database {
	int size;
	RECORD records[MAX_RECORD];
}DATABASE;

// ユーザー入力からコマンドを読み込む関数
char *readCmd(char cmd[CMD_LEN]){
	printf("command> ");
	fgets(cmd, CMD_LEN, stdin);
	return cmd;
}
// 入力されたコマンドが正当かどうかを確認する関数
int validateCmd(char cmd[CMD_LEN], char commands[CMD_NUM][CMD_LEN]){
	int flag = 0; //　不正コマンドと仮定
	for (int i = 0; i < CMD_NUM; i++) {
		if(strcmp(cmd, commands[i]) == 0){
			flag = 1;
			return flag;
		}
	}
	return flag;
}

// 入力された属性が正当かどうかを確認する関数
int validateAttr(char attr[ATTR_LEN], char attributes[ATTR_NUM][ATTR_LEN]){
	int flag = 0; //　不正と仮定
	for (int i = 0; i < ATTR_NUM; i++) {
		if(strcmp(attr, attributes[i]) == 0){
			flag = 1;
			return flag;
		}
	}
	return flag;
}

// 入力されたコマンドを解析して、リストに格納する関数
NODE* parseCmdList(char cmd[CMD_LEN], char commands[CMD_NUM][CMD_LEN],NODE* parsed){
	if(parsed!=NULL){
		parsed = NULL;
	}
	//コマンド読み込み
	readCmd(cmd);	
	// 一番目のトークンを取得（これは実際のコマンド名になる）
	char* token = strtok(cmd, " \n"); 
	// 一番目のトークンが妥当かどうかを調べる
	int is_valid = validateCmd(token, commands); 
	if(is_valid){
		while (token) {
			parsed = insert(parsed, token);
			token = strtok(NULL, " \n");
		}
	}else{
		printf("正しいコマンドを入力してください：\n");
		for(int i = 0; i<CMD_NUM;i++){printf("|%s",commands[i]);}printf("|\n");
	}
	return parsed;
}

// データベースを初期化する関数
void init(DATABASE *db){
	db->size = 10;
    char names[10][NAME_LEN] = {"Tanaka",
						"Kato", 
						"Nakata", 
						"Fujita", 
						"Manabe", 
						"Aoki", 
						"Morimoto", 
						"Uehara", 
						"Yamada", 
						"Watanabe"};
	int ids[10] = {115,3,54,23,11,62,49,97,76,37};
	int scores[10] = {64,92,85,44,95,67,88,72,56,74};

	for (int i = 0; i < db->size; i++) {
		db->records[i].id = ids[i];
		strcpy(db->records[i].name,names[i]);  
		db->records[i].score = scores[i];
	}
}

// 名前でselectする
void select_by_name(DATABASE *db, char *val) {
	for (int i = 0; i < db->size; i++) {
		if (strcmp(db->records[i].name, val) == 0) {
			printf("%.3d %.10s %.2d\n",
				db->records[i].id, db->records[i].name, db->records[i].score);
		}
	}
}

// IDでselectする
void select_by_id(DATABASE *db, char *val) {
	int id = atoi(val);
	for (int i = 0; i < db->size; i++) {
		if (db->records[i].id == id) {
			printf("%.3d %.10s %.2d\n",
				db->records[i].id, db->records[i].name, db->records[i].score);
		}
	}
}

// 成績でselectする
void select_by_score(DATABASE *db, char *val) {
	int score = atoi(val);
	for (int i = 0; i < db->size; i++) {
		if (db->records[i].score == score) {
			printf("%.3d %.10s %.2d\n",
				db->records[i].id, db->records[i].name, db->records[i].score);
		}
	}
}

// 全レコードを抽出する
void select_all(DATABASE *db) {
	for (int i = 0; i < db->size; i++) {
		printf("%.3d %.10s %.2d\n",
			db->records[i].id, db->records[i].name, db->records[i].score);
	}
}


void do_select(NODE* tokenList, DATABASE *db, char attributes[ATTR_NUM][ATTR_LEN]){
	
    if (tokenList == NULL) {
		printf("selectしたい属性を入力してください\n");
		return;
	}

	char *attr = tokenList->token;
	int is_valid = validateAttr(attr, attributes);
	if(is_valid!=1){
		printf("Value Error. この属性が存在しない。\n");
		return;
	}

	// selectする属性と値を取得する
	char *val = NULL;
	if (tokenList->next != NULL) {
		val = tokenList->next->token;
	}

	// select処理を実行する
	if (strcmp(attr, "name") == 0) {
		select_by_name(db, val);
	} else if (strcmp(attr, "id") == 0) {
		select_by_id(db, val);
	} else if (strcmp(attr, "score") == 0) {
		select_by_score(db, val);
	} else if (strcmp(attr, "*") == 0) {
		select_all(db);
	}
}


// レコード比較関数テーブル用
typedef int (*funptr)(const void*,const void*);

//IDで比較する
int compareRecordsById(const void* a, const void* b) {
	RECORD* recordA = (RECORD*)a;
	RECORD* recordB = (RECORD*)b;
	return recordA->id - recordB->id;
}
//氏名で比較する
int compareRecordsByName(const void* a, const void* b) {
	RECORD* recordA = (RECORD*)a;
	RECORD* recordB = (RECORD*)b;
	return strcmp(recordA->name, recordB->name);
}
//成績で比較する
int compareRecordsByScore(const void* a, const void* b) {
	RECORD* recordA = (RECORD*)a;
	RECORD* recordB = (RECORD*)b;
	return recordA->score - recordB->score;
}

int compareRecords(const char attr[], const void*a, const void* b){
	RECORD* recordA = (RECORD*)a;
	RECORD* recordB = (RECORD*)b;
	funptr cmpFuncs[] = {
		compareRecordsById,
		compareRecordsByName,
		compareRecordsByScore
	};
	const char* funcNames[] = {
		"id",
		"name",
		"score"
	};
	int index = -1;
	int numFuctions = sizeof(cmpFuncs)/sizeof(cmpFuncs[0]);
	for (int i = 0; i < numFuctions; i++) {
		if (strcmp(attr, funcNames[i])==0) {
			index = i;
			break;
		}
	}
	funptr cmpFunc = cmpFuncs[index];
	return cmpFunc(recordA,recordB);
}

void swap(RECORD* arr, int i, int j) {
		RECORD* temp = (RECORD *)malloc(sizeof(RECORD));
		*temp = arr[i];
		arr[i] = arr[j];
		arr[j] = *temp;
}

int partition(RECORD *recordList, int low , int high, const char attr[]){
	RECORD pivot = recordList[(high+low)/2];
	low--;
	high++;
	while(1){
		do{low++;}while(compareRecords(attr,&recordList[low], &pivot)<0);
		do{high--;}while(compareRecords(attr,&recordList[high], &pivot)>0);
		if (low >= high) return high;
		swap(recordList, low, high);
	}
}

void q_sort(RECORD *recordList, int low, int high, const char attr[]) {
	if (low >=0 && high >=0 && low < high) {
		int pivot_index = partition(recordList,low,high,attr);
		q_sort(recordList, low, pivot_index,attr);
		q_sort(recordList, pivot_index + 1, high,attr);
	}
}

void do_sort(NODE * parsedTokens, DATABASE *db,char attributes[ATTR_NUM][ATTR_LEN]){
	printf("sort処理\n");

	char attr[TOKEN_LEN];
	if (parsedTokens==NULL) {
		printf("Syntax Error。正しいsyntaxは: sort [属性名]\n");
		return;
	}

	strcpy(attr, parsedTokens->token);
	int is_valid = validateAttr(attr, attributes);
	if(is_valid!=1){
		printf("Value Error. この属性が存在しない。\n");
		return;
	}

	RECORD *records = (RECORD *)malloc(sizeof(RECORD)*MAX_RECORD);
	for (int i = 0; i < db->size; i++) {
		records[i] = db->records[i];
	}
	printf("DB size: %d\n",db->size);
	q_sort(records, 0, db->size-1, attr);

	for (int i = 0; i < db->size; i++) {
		printf("%.3d %.10s %.2d\n",\
		records[i].id,\
		records[i].name,\
		records[i].score );
	}
}

void do_exit(){
	printf("exit処理\n");
	exit(0);
}

void do_insert(NODE *tokenList, DATABASE *db){
	printf("insert処理\n");
	RECORD newRecord;

	// トークンリストがNULLかどうかをチェックする
	if (tokenList==NULL) {
		printf("Syntax Error. 正しいinsertのsyntaxは\"insert [id] [name] [score]\"です。\n");
		return;
	}
	// 新規レコードのIDを取得する
	newRecord.id = atoi(tokenList->token);

	if (tokenList->next==NULL) {
		printf("Syntax Error. 正しいinsertのsyntaxは\"insert [id] [name] [score]\"です。\n");
		return;
	}
	// トークンリストの次のトークンを取得する
	tokenList = tokenList->next;
	// 新規レコードの氏名を取得する
	strcpy(newRecord.name,tokenList->token);

	if (tokenList->next==NULL) {
		printf("Syntax Error. 正しいinsertのsyntaxは\"insert [id] [name] [score]\"です。\n");
		return;
	}
	// トークンリストの次のトークンを取得する
	tokenList = tokenList->next;
	// 新規レコードの得点を取得する
	newRecord.score = atoi(tokenList->token);
	// データベースのサイズが最大記録数に達したかどうかをチェックする
	if(db->size+1>=MAX_RECORD){
		printf("データベースには空きがないです。削除機能もまだ未導入です。\n");
		return;
	}
	// 新規レコードをデータベースに追加する
	db->records[db->size] = newRecord;
	db->size++;

	// データベースの全レコードを表示する
	for (int i = 0; i < db->size; i++) {
		printf("%.3d %.10s %.2d\n",\
		db->records[i].id,\
		db->records[i].name,\
		db->records[i].score );
	}
}

// コマンドのコマンド配列のインデックスを取得する関数
// コマンドがNULLの場合は-1を返す
// オプションコードが見つかればそのインデックスを返す
// 見つからなければ-1を返す
int getOptCode(char cmd[CMD_LEN], char commands[CMD_NUM][CMD_LEN]){
	if (cmd==NULL) {
		return -1;
	}
	for (int i = 0; i < CMD_NUM; i++) {
		if(strcmp(cmd, commands[i]) == 0){
			return i;
		}
	}
	return -1;
}

int main(int argc, char* argv[])
{	
	if(argc>1){ // argc is at least 1 since the first argument is always the filepath
		return 1;
	}
	// コマンド名の定義
	char commands[CMD_NUM][CMD_LEN] = {"exit", "select", "sort", "insert"};
	// 属性名の定義
	char attributes[ATTR_NUM][ATTR_LEN] = {"id", "name", "score", "*"};
	// コマンド文字列の領域確保
	char *command = (char *)malloc(sizeof(char)*CMD_LEN);
	// データベースの領域確保
	DATABASE *db = (DATABASE *)malloc(sizeof(DATABASE));
	// データベースの初期化
	init(db);

	// トークンのリスト
	NODE *parsedTokens = NULL;
	// コマンド入力ループ
	while(1){
		// コマンドの解析
		parsedTokens = parseCmdList(command,commands,parsedTokens);
		// コマンドのオプションコードの取得
		int optcode = getOptCode(parsedTokens->token,commands);
		// コマンドの実行
		switch (optcode) {
			// 終了
			case 0: do_exit();break;
			// 選択
			case 1: do_select(parsedTokens->next, db, attributes);break;
			// ソート
			case 2: do_sort(parsedTokens->next, db, attributes);break;
			// 挿入
			case 3: do_insert(parsedTokens->next, db);break;
			// 構文エラー
			default: printf("Syntax Error.\n");break;
		}
	}

	// 解析済みトークンの解放
	free(parsedTokens);
	// データベースの解放
	free(db);
	// コマンド文字列の解放
	free(command);
	// 終了
	return 0;
}
