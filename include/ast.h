#ifndef __AST_H__
#define __AST_H__

#include "loc.h"
#include <stdint.h>
#include <iostream>
#include <string>
#include <stack>
#include <map>
#include <cstdio>

/* It's just for reference,
 * you can design your own
 * struct or class to impl inheritance in c/cpp */

typedef enum{ 
	ProgNode, /* types */
	NODE_ROOT,
	NODE_BOOLVAL,
	NODE_NUMBER,
	NODE_ID,
	NODE_PNUMBER,
	NODE_PBOOLVAL,
	NOED_PLUS,
	NODE_MINUS,
	NODE_MULTIPLY,
	NODE_DIVIDE,
	NODE_MODULES,
	NODE_GREATER,
	NODE_SMALLER,
	NODE_EQUAL,
	NODE_AND,
	NODE_OR,
	NODE_NOT,
	NODE_DEFINE,
	NODE_FUN,
	NODE_IF,
	NODE_FUNDEFCALL,
	NODE_FUNCALL,
	NODE_FUNBODY  } NodeType;//將結構擴充為資料型態並命名為NodeType

class NODENode{
public:
	NodeType type;
	NODENode *left, *right;
};

class NODEVal : public NODENode{
public:
	int number;
	bool bool_val;
	std::string id;
};

class NODENumber : public NODENode{
public:
	int number;
};

class NODEBoolVal : public NODENode{
public:
	bool bool_val;
};

class NODEId : public NODENode{
public:
	std::string id;
};

class NODEIf : public NODENode{
public: 
	NODENode *deter;
};

extern std::stack<NodeType> type_stk;
extern std::map<std::string, NODENode*> global_id_map;

NODENode* constructNode(NODENode *exp1, NODENode *exp2, NODENode *exp3);
NODENode* constructNode(NODENode *exp1, NODENode *exp2);
NODEVal* NODEVisit(NODENode *current, std::map<std::string, NODENode*> &local_id_map);

/*typedef struct NodeTag{
  NodeType nt;
  LocType  loc;
  /* some fields */
/*} *Node, NodeStr;

#define Obj void*

typedef struct ConsTag{
  Obj car;
  struct ConsTag *cdr;
} *Cons, ConsStr;*/

#endif
