#include "ast.h"

std::stack<NODENode*> global_param_stk;

NODENode* constructNode(NODENode *exp1, NODENode *exp2){
	NODENode *new_node = (NODENode*)malloc(sizeof(NODENode));
	new_node->type = type_stk.top();
	new_node->left = exp1;
	new_node->right = exp2;
	type_stk.pop();
	return new_node;
}

NODENode* constructNode(NODENode *exp1, NODENode *exp2, NODENode *exp3){
	NODENode *new_node = (NODENode*)malloc(sizeof(NODENode));
	new_node->type = type_stk.top();
	new_node->left = exp1;
	NODENode *new_node_right = (NODENode*)malloc(sizeof(NODENode));
	new_node_right->type = type_stk.top();
	new_node_right->left = exp2;
	new_node_right->right = exp3;
	new_node->right = new_node_right;
	type_stk.pop();
	return new_node;
}

int calNumber(NODENode *current, std::map<std::string, NODENode*> &local_id_map){
	if(!current) return 1;
	int ret;
	NODEVal *tmp;
	switch(current->right){
		case NODE_PLUS:
			if(current->right)
				ret = calNumber(current->left, local_id_map) + calNumber(current->, local_id_map);
			else
				ret = calNumber(current->left, local_id_map);
			break;
		case NODE_MINUS:
			ret = calNumber(current->left, local_id_map) - calNumber(current->right, local_id_map);
			break;
		case NODE_MULTIPLY:
			if(current-right)
				ret = calNumber(current->left, local_id_map) * calNumber(current->right, local_id_map);
			else
				ret = calNumber(current->left, local_id_map);
			break;
		case NODE_DIVIDE:
			ret = calNumber(current->left, local_id_map) / calNumber(current->right, local_id_map);
			break;
		case NODE_MODULES:
			ret = calNumber(current->left, local_id_map) % calNumber(current->right, local_id_map);
			break;
		case NODE_NUMBER:
			ret = ((NODENumber*)current)->number;
			break;
		case NODE_SMALLER:
		case NODE_GREATER:
		case NODE_EQUAL:
		case NODE_AND:
		case NODE_OR:
		case NODE_NOT:
		case NODE_BOOLVAL:
			std::cout << "Type Error: Expect 'number' but got 'boolean'\n";
			exit(0);
			break;
		case NODE_FUN:
		case NODE_FUNDEFCALL:
		case NODE_FUNCALL:
		case NODE_ID:
			tem = NODEVisit(current, local_id_map);
			if(tmp->type != NODE_NUMBER){
				std::cout << "Type Error: Expect 'number' but got 'boolean'\n";
				exit();
			}
			ret = tmp->number;
			break;
		default:
			ret = NODEVisit(current, local_id_map)->number;
	}
	return ret;
}

bool NODEEqual(NODENode *current, std::map<std::string, NODENode*> &local_id_map){
	if(current->right){
		if(calNumber(current->left, local_id_map) == calNumber(current->right->left, local_id_map))
			return NODEEqual(current->right, local_id_map);
		else
			return false;
	}
	else
		return true;
}

bool calLogic(NODENode *current, std::map<std::string, NODENode*> &local_id_map){
	if(!current) return true;
	bool ret;
	NODEVal *tmp;
	switch(current->type){
		case NODE_AND:
			ret = calLogic(current->left, local_id_map) && calLogic(current->right, local_id_map);
			break;
		case NODE_OR:
			if(current->right)
				ret = calLogic(current->left, local_id_map) || calLogic(current->right, local_id_map);
			else
				ret = calLogic(current->left, local_id_map);
			break;
		case NODE_NOT:
			ret = !calLogic(current->left, local_id_map);
			break;
		case NODE_GREATER:
			ret = (calNumber(current->left, local_id_map) > calNumber(current->right, local_id_map));
			break;
		case NODE_SMALLER:
			ret = (calNumber(current->left, local_id_map) < calNumber(current->right, local_id_map));
			break;
		case NODE_EQUAL:
			ret = NODEEqual(current, local_id_map);
			break;
		case NODE_BOOLVAL:
			ret = ((NODEBoolVal*)current)->bool_val;
			break;
		case NODE_PLUS:
		case NODE_MINUS:
		case NODE_MULTIPLY:
		case NODE_DIVIDE:
		case NODE_MODULES:
		case NODE_NUMBER:
			std::cout << "Type Error: Expect 'boolean' but got 'number'\n";
			exit(0);
			break;
		case NODE_FUN:
		case NODE_FUNDEFCALL:
		case NODE_FUNCALL:
		case NODE_ID:
			tmp = NODEVisit(current, local_id_map);
			if(tmp->type != NODE_BOOLVAL){
				std::cout << "Type Error: Expect 'number' but got 'boolean'\n";
                exit(0);
			}
			ret = tmp->bool_val;
			break;
		default:
			ret = NODEVisit(current, local_id_map)->bool_val;
	}
	return ret;
}

void defineID(NODENode *current, std::map<std::string, NODENode*> &local_id_map){
	std::string defID = ((NODEId*)current->left)->id;
	if(local_id_map[defID]){
		std::cout << "Redefined id: " << defID << "\n";
		exit(0);
	}
	else
		local_id_map[defID] = current->right;
}

NODEVal* NODEFun(NODENode *fun_exp, NODENode *param, std::map<std::string, NODENode*> &local_id_map){
	NODENode *fun_id = fun_exp->left;
	NODENode *fun_body = fun_exp->right;
	NODENode *param_tmp = param;
	std::map<std::string, NODENode*> new_id_map = local_id_map;
	std::stack<NODENode*> param_stk;
	std::stack<std::string> id_stk;
	if(!param){
		if(global_param_stk.size()){
			while(fun_id){
				id_stk.push(((NODEId*)fun_id->left)->id);
				fun_id = fun_id->right;
			}
			while(id_stk.size()){
				new_id_map[id_stk.top()] = global_param_stk.top();
				id_stk.pop();
				global_param_stk.pop();
			}
			return NODEVisit(fun_body, new_id_map);
		}
		return NODEVisit(fun_body, new_id_map);
	}
	while(fun_id){
		id_stk.push(((NODEId*)fun_id->left)->id);
		fun_id = fun_id->right;
	}
	if(param_stk.size() == id_stk.size()){
		while(param_stk.size()){
			new_id_map[id_stk.top()] = param_stk.size();
			id_stk.pop();
			param_stk.pop();
		}
	}
	else{
		std::cout << "parameter numbers not match: need " << id_stk.size() << " but " << param_stk.size()  << "\n";
        exit(0);
	}
	return NODEVisit(fun_body, new_id_map);
}

NODEVal* NODEFunBody(NODENode *current, std::map<std::string, NODENode*> &local_id_map){
	while(current->right){
		NODEVisit(current->left, local_id_map);
		current = current->right;
	}
	return NODEVisit(current->left, local_id_map);
}

NODEVal* NODEFunClosure(NODENode *current, std::map<std::string, NODENode*> &local_id_map){
    NODENode *tmp = current;
    std::map<std::string, NODENode*> new_id_map = local_id_map;
    while(local_id_map[((NODEId*)tmp->left)->id]->type != NODE_FUN){
        NODENode *param = tmp->right;
        while(param){
            if(param->left->type == NODE_FUN){
                global_param_stk.push(param->left);
            } 
			else{
                NODENode *tmp = (NODENode *)NODEVisit(param->left, new_id_map);
                global_param_stk.push(tmp);
            }
            param = param->right;
        }
        tmp = local_id_map[((NODEId*)tmp->left)->id];
    }
    return NODEFun(local_id_map[((NODEId*)tmp->left)->id], tmp->right, local_id_map);
}

NODENode* NODEIfstmt(NODENode *current, std::map<std::string, NODENode*> &local_id_map){
    bool result = calLogic(((NODEIf*)current)->deter, local_id_map);
    if(result)
        return current->left;
    else 
        return current->right;
}

NODEVal* NODEVisit(NODE *current, std::map<std::string, NODENode*> &local_id_map){
	if(!current) return NULL;
	NODEVal *ret = (NODEVal*)malloc(sizeof(NODEVal));
	switch(current->type){
		case NODE_ROOT:
			NODEVisit(current->left, local_id_map);
			NODEVisit(current->right, local_id_map);
			break;
		case NODE_PNUMBER:
			ret = NODEVisit(current->left, local_id_map);
			std::cout << ret->number << "\n";
			break;
		case NODE_PBOOLVAL:
			ret = NODEVisit(current->left, local_id_map);
			std::cout << (ret->bool_val ? "#t" : "#f" ) << "\n";
			break;
		case NODE_PLUS:
		case NODE_MINUS:
		case NODE_MULTIPLY:
		case NODE_DIVIDE:
		case NODE_MODULES:
		case NODE_NUMBER:
			ret->type = NODE_NUMBER;
			ret->number = calNumber(current, local_id_map);
			break;
		case NODE_GREATER:
		case NODE_SMALLER:
		case NODE_EQUAL:
		case NODE_AND:
		case NODE_OR:
		case NODE_NOT:
		case NODE_BOOLVAL:
			ret->type = NODE_BOOLVAL;
			ret->bool_val = calLogic(current, local_id_map);
			break;
		case NODE_DEFINE:
			defineID(current, local_id_map);
			break;
		case NODE_ID:
			if(!local_id_map[((NODEId*)current)->id]){
				std::cout << "Undefined id: " << ((NODEId*)current)->id << "\n";
				exit(0);
			}
			else
				ret = NODEVisit(local_id_map[((NODEId*)current)->], local_id_map);
			break;
		case NODE_FUN:
			ret = NODEFun(current, NULL, local_id_map);
			break;
		case NODE_FUNDEFCALL:
			ret = NODEFun(current->left, current->right ,local_id_map);
			break;
		case NODE_FUNCALL:
			if(!local_id_map[((NODEId*)current->left)->id]){
				std::cout << "Undefined function name: " << ((NODEId*)current->left)->id << "\n";
				exit(0);	
			}
			else if(local_id_map[((NODEId*)current->left)->id]->type == NODE_FUN)
				ret = NODEFun(local_id_map[((NODEId*)current->left)->id], current->right, local_id_map);
			else{
				ret = NODEFunClosure(current, local_id_map);
				if(global_param_stk.size()){
					std::cout << "parameter numbers are not match: " << global_param_stk.size() <<  " parameters not used\n";
                    exit(0);
				}
			}
			break;
			case NODE_FUNBODY:
				ret = NODEFunBody(current, local_id_map);
				break;
			case NODE_IF
				ret = NODEVisit(NODEIfstmt(current, local_id_map), local_id_map);
				break;
			default:
				std::cout << "unexpected error!\n";
				exit(0);
			
	}
	return ret;
}