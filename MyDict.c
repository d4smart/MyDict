#include <Python.h>	//使用python调用C
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "cJSON.h"

#define SLEN 26
#define TRUE 1
#define FALSE 0

// 前缀树结构体定义
typedef struct trie
{
	char cont;	//节点代表的字母
	int isword;	//从一级节点到节点是否为单词
	char *type;	//存储词性
	char *sent;	//存储例句
	struct trie *next[SLEN];	//存储指针
}Trie;
typedef Trie *Root;	//根指针
typedef Trie *Position;	//位置指针
Root root;	//全局头指针

/*
* 插入一个单词及其对应的词性及例句到前缀树中
* root初始指向树根，word是要插入的单词，inter指向单词对应的意思。
*/
Position init()
{
	int i;	

	// 分配一个树节点
	Position position = (Position)malloc(sizeof(Trie));
	if (!position)
	{
		printf("ERROR: create position error.(malloc)\n");
		exit(1);
	}

	// 树节点初始化
	for (i = 0; i < SLEN; i++)
	{
		position->next[i] = NULL;
	}
	position->cont = 0;
	position->isword = FALSE;
	position->type = NULL;
	position->sent = NULL;

	return position;
}

/*
* 插入一个单词及其对应的词性及例句到前缀树中
* root初始指向插入的根，word是要插入的单词，type指向词性，sent是例句。
*/
void Insert(Position P, char *word, char *type, char *sent)
{
	int i;
	Position p = P, t;
	int len = strlen(word);	//获取单词长度

	if (len == 0) return;	//单词为空无需插入，直接返回

	for (i = 0; i < len; i++)
	{
		// 新增节点，插入字母
		if (p->next[word[i] - 'a'] == NULL)
		{
			t = init();
			t->cont = word[i];
			p->next[word[i] - 'a'] = t;
		}
		p = p->next[word[i] - 'a'];
	}
	p->isword = TRUE;	//改变标记
	// 存入词性和例句
	if (p->type == NULL)
	{
		p->type = (char*)calloc(strlen(type) + 1, sizeof(char));
		if (p->type == NULL)
		{
			fprintf(stderr, "FATAL ERROR: malloc new TrieNode->type (calloc)\n");
			exit(1);
		}
		strcpy(p->type, type);
	}
	if (p->sent == NULL)
	{
		p->sent = (char*)calloc(strlen(sent) + 1, sizeof(char));
		if (p->sent == NULL)
		{
			fprintf(stderr, "FATAL ERROR: malloc new TrieNode->sent (calloc)\n");
			exit(1);
		}
		strcpy(p->sent, sent);
	}
	
	return;
}

/*
* 检查输入合法性，若合法则转成小写。
*/
int TestAndTransform(char *word)
{
	while (*word)
	{
		if ((*word >= 'a' && *word <= 'z') || (*word >= 'A' && *word <= 'Z'))
		{
			// 转换成小写
			if (*word >= 'A' && *word <= 'Z')
				*word += 'a' - 'A';
		}
		else
			return 0;
		word++;
	}
	return 1;
}

/*
* 读取原始文件创建字典，最后返回字典树的根节点。
* 需要调用上面的一些函数。
*/
void CreateDict()
{
	FILE *fp = NULL;
	cJSON *head;
	char json[1200];
	char word[60], type[300], sent[900];
	int wordNumber = 0;

	root = init();	//创建根节点

	fp = fopen("result2.json", "r");
	//fp = fopen("test.json", "r");
	if (!fp)
	{
		printf("FATAL ERROR: raw-dict not exist\n");
		exit(1);
	}

	// 根据文件格式读取单词，存入Trie树中
	while (fgets(json, sizeof(json), fp))
	{
		head = cJSON_Parse(json);

		strcpy(type, head->child->valuestring);
		strcpy(word, head->child->next->valuestring);
		strcpy(sent, head->child->next->next->valuestring);
		Insert(root, word, type, sent);
		wordNumber++;
	}
	fclose(fp);

	printf("******** Total number is %d. ********\n", wordNumber);
}

/*
* 查找单词。
* root初始为根节点，word指向要查找的单词.
*/
Position Find(char *word)
{
	int i;
	Position p = root;
	int len = strlen(word);

	// 要找的单词为空或树为空，则返回
	if (len == 0 || root == NULL)
	{
		printf("The word not exist\n");
		return NULL;
	}
	
	if (!TestAndTransform(word))
		{
			printf("The word not exist\n");
                        return NULL;
		}		
	for (i = 0; i < len; i++)
	{
		// 路上有空节点，则返回
		if (p->next[word[i] - 'a'] == NULL)
		{
			printf("The word not exist\n");
			return NULL;
		}
		p = p->next[word[i] - 'a'];
	}
	// 树里有，但不是单词，返回
	if (!p->isword)
	{
		printf("The word not exist\n");
		return NULL;
	}

	return p;
}

/*
* 删除整颗trie树
*/
void Del(Position position)
{
	int i;
	if (!position)
		return;

	for (i = 0; i < SLEN; i++)
	{
		if (position->next[i])
			Del(position->next[i]);
	}
	free(position);

	return;
}

// 创建字典的包装函数
PyObject *create_dict(PyObject *self, PyObject *args)
{
	clock_t start, end;	

	// 创建字典并打印一些信息
	start = clock();
	CreateDict();
	end = clock();
	printf("******** 建立字典耗时 %.4f s.\t ********\n", (end - start) / 1000000.0);
	printf("******** Input word to inquire (--quit for quit)\t ********\n");

	Py_INCREF(Py_None);
	return Py_None;
}


// 单词查询的包装函数
PyObject *_find(PyObject *self, PyObject *args)
{
	char *word;
	Position p;

	if(!PyArg_ParseTuple(args, "s", &word))
	        return NULL;
	p = Find(word);
	PyObject *result;
	if (p)
	{
		result = Py_BuildValue("[ss]", p->type, p->sent);
	}
	else
		result = Py_BuildValue("[]");
	
	return result;	//返回包含type和sent的list
}

// 导出表函数
static PyMethodDef DictMethod[] = {
	{"createdict", create_dict, METH_VARARGS, "Create the trie dict."},
	{"find", _find, METH_VARARGS, "Find the info of the word, return a list."},
	{NULL, NULL, 0, NULL}
};

// 导出函数，将模块名称与导出表进行连接
void initMydict()
{
	PyObject *m = Py_InitModule("Mydict", DictMethod);
}

