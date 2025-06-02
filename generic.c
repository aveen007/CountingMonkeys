#include "generic.h"
classDef* current;
Type* currentGeneric;
int inClass = 0;
classDefInfo* allClasses;
controlFlowGraphBlock* currentCfg;
controlFlowGraphBlock* ret;
int myDrawn = 0;
/// here I match the type key, from the parameters 
///summary
/// let's take an example A<T> is later initialized as A<int>
/// 
/// T is key
/// class is A<T> (A as an unitialized classf
///  
/// generic type is A<int>  (as a type)
/// 
/// 
/// 
Type* matchGenericType(Type* key, classDef* class, Type* genericType) {
	if (key->kind == TYPE_SIMPLE) {
		if (key->data.simpleType.type == TYPE_CUSTOM) {

			for (int k = 0; k < class->parametersCount; k++) {

				if (strcmp(key->data.simpleType.custom_id, class->parameterNames[k]) == 0) {
					//here we found an argument which has a generic type for example T a;
					return  genericType->data.genericType.parameters[k];

				}
				else {

				}

			}
			// it is not a param , must be another class
			return findClass(allClasses, key);
		}
	}
		if (key->kind == TYPE_GENERIC) {

			//for (int k = 0; k < class->parametersCount; k++) {

			//	/*for (int i = 0; i < key->data.genericType.parametersCount; i++) {

			//		key->data.genericType.parameters[i] = matchGenericType(key->data.genericType.parameters[i], class, genericType);

			//	}*/
			//// here I basically treat something like A<T, Z>, T and Z are params, but the base of the type A must be a class
			//}
				return findClass(allClasses, key);

		}

	
}

classDef* initClass(classDef* found, Type* parent , classDefInfo * classes) {
	classDef* newClass = createClassDef(found->name);
	if (found->baseType) {
		newClass->baseType = found->baseType;

	}
	newClass->parametersCount = parent->data.genericType.parametersCount;
	newClass->parameterNames = malloc(sizeof(char*) * newClass->parametersCount);
	for (int i = 0; i < newClass->parametersCount; i++) {
		newClass->parameterNames[i] = found->parameterNames[i];
	//	newClass->parameterNames[i] = get_type_name(parent->data.genericType.parameters[i]);
	}
	newClass->argumentCount = found->argumentCount;
	newClass->arguments = malloc(sizeof(ArgumentInfo*) * found->argumentCount);
	for (int i = 0; i < found->argumentCount; i++) {
		ArgumentDef* argDef = malloc(sizeof(ArgumentDef));
		argDef->name = found->arguments[i]->argument->name;
		argDef->type = matchGenericType(found->arguments[i]->argument->type, found, parent);

		newClass->arguments[i] = createArgumentInfo(found->arguments[i]->modifier, found->arguments[i]->offset, argDef);

	}
	// what to do with the newClasses functions? if they are found withing=

	for (int i = 0; i < found->functionCount; i++) {
		Subroutine* func = createSubroutine();
		SignatureDetails* details = malloc(sizeof(SignatureDetails));
		details->arguments = malloc(sizeof(ArgumentDef*) * found->functions[i]->subroutine->signatureDetails->cntArgs);
		for (int l = 0; l < found->functions[i]->subroutine->signatureDetails->cntArgs; l++) {
			ArgumentDef* argDef = malloc(sizeof(ArgumentDef));
			argDef->name = found->functions[i]->subroutine->signatureDetails->arguments[i]->name;
			argDef->type = matchGenericType(found->functions[i]->subroutine->signatureDetails->arguments[i]->type, found, parent);

			details->arguments[i] = argDef;


		}
		//TODO: if return type does not exist make it null 
	/*	if (found->functions[i]->subroutine->signatureDetails->returnType) {

		details->returnType = matchGenericType(found->functions[i]->subroutine->signatureDetails->returnType, found, parent);
		}*/
		
		func->name = found->functions[i]->subroutine->name;
		func->signatureDetails = details;
		current = newClass;
		currentGeneric = parent;
		inClass = 1;
		ret = NULL;
		// should create the cfg and copy and substitute types
		func->cfg =traverseCfgType(found->functions[i]->subroutine->cfg, NULL, "fileName", classes, NULL);
		FunctionInfo* funcInfo = createFunctionInfo(found->functions[i]->modifier, i, func);
		addFunctionToClass(newClass, funcInfo);

	}
	return newClass;

}
Type* findClass(classDefInfo* classes, Type * parent) {
	allClasses = classes;
	for (int j = 0; j < classes->classCount; j++) {
		char* parentName = get_type_name(parent);
		if (strcmp(parentName, classes->classes[j]->name) == 0) {
			classDef* found = classes->classes[j];

			if (found->parametersCount == 0) {
				//custom
				parent->def = found;
			}
			else {
				//generic
				
			
				parent->def= initClass(found, parent, classes);

			}
		}
	} 
				return parent;
	//TODO: check if I commented correctly
		// here I am considering the case : if the function is inside a class and has a parameter with type T a for example, I need to match it to the initializing type  
	//	return matchGenericType(parent,current,currentGeneric );
		// should the functions inside classes be also amongst the full list of cfgs, when I traverse them if I find a custom type, it could be either a class or also it could be a parameter inside a class
		// I don't want to not traverse them to set the fathers in case it's just a class, 
		// I also don't want to traverse twice and possibly lose information, 




}
//TODO: in handleType in controlGraph.c, do generic handeling
Subroutine** setTypes(Subroutine** subroutines, int cnt, char* fileName, classDefInfo* classes) {

	// TODO: traverse all classes and create a new class for instantiated
	// TODO: set classDef as this created class for custom types 
	
	for (int i = 0; i < classes->classCount; i++)
	{
		// for extends

		if (classes->classes[i]->baseType)
		{
			classes->classes[i]->baseType = findClass(classes, classes->classes[i]->baseType);
		}
	}

	for (int i = 0; i < cnt; i++) {

		subroutines[i]->cfg= traverseCfgType(subroutines[i]->cfg, NULL, fileName, classes);

	}
	return subroutines;
}



//controlFlowGraphBlock* traverseCfgType(controlFlowGraphBlock* old) {
//
//	// here I am just creating the start block
//	//current = cfg;
//	//currentCG = cfg->called;
//	//Stack* openNodes = createStack();
//	//push(openNodes, tree);
//	controlFlowGraphBlock* end = ConstructCFGType( cfg);
//	controlFlowGraphBlock* cfgEnd = createCFGBlock(old->ast, BaseBlock);
//	insertCFGBlock(end, cfgEnd);
//
//}
//

controlFlowGraphBlock* copyNode(controlFlowGraphBlock* node) {
	controlFlowGraphBlock* copy = createCFGBlock(node->ast, node->blocktype);
	copy->instructions = copyInstructions(node);
	copy->drawn = node->drawn;
	copy->outNodeCount = node->outNodeCount;
	copy->called->cnt = node->called->cnt;
	copy->called->calledProcedures = realloc(copy->called->calledProcedures, sizeof(controlFlowGraphBlock*) * node->called->cnt);
	for (int i = 0; i < node->called->cnt; i++) {
		copy->called->calledProcedures = node->called->calledProcedures[i];
		copy->called->calledTokens[i] = node->called->calledTokens[i];
	}
	copy->nodes = realloc(copy->nodes, sizeof(controlFlowGraphBlock) * node->outNodeCount);
	return copy;
}


controlFlowGraphBlock* traverseCfgIfStatementType(controlFlowGraphBlock* node, controlFlowGraphBlock* start,  char* fileName, classDefInfo* classes) {

	controlFlowGraphBlock* copyIfNode = copyNode(node);
	
		//node->nodes[0];
	int isElseNode = node->outNodeCount > 1 ? 1 : 0;
	
	copyIfNode->nodes[0]  = traverseCfgType(node->nodes[0], node, fileName, classes);
	
	if (isElseNode == 1) {
		// ElseNode = node->nodes[1];
		copyIfNode->nodes[1] = traverseCfgType(node->nodes[1], node, fileName, classes);
	}
	node->drawn = 0;
	copyIfNode->drawn = 0;
	
	return copyIfNode;
}
controlFlowGraphBlock* traverseCfgWhileStatementType(controlFlowGraphBlock* node, controlFlowGraphBlock* start,  char* fileName, classDefInfo* classes) {
	controlFlowGraphBlock* WhileNode = copyNode(node);
	// whilebody = node->nodes[0];
	WhileNode->nodes[0] = traverseCfgType(node->nodes[0], node, fileName, classes);
	WhileNode->nodes[1] = traverseCfgType(node->nodes[1], node, fileName, classes);
	node->drawn = 0;
	return WhileNode->nodes[1];
}
Instructions* copyInstructions(controlFlowGraphBlock* node) {
	Instructions* copyInstructions = malloc(sizeof(Instructions));
	copyInstructions->size = node->instructions->size;

	copyInstructions->content = malloc(sizeof(cfgBlockContent*) * node->instructions->size);

	if (node->instructions->size > 0) {
		//TODO : if custom, then handle it's type
		for (int i = 0; i < node->instructions->size; i++) {

			if (node->instructions->content[i]->type == TYPE_VARDECLARATION) {
				copyInstructions->content[i] = copyInstructionsVarStatement(node->instructions->content[i]);

			}
			else {
				copyInstructions->content[i] = copyInstructionsExpression(node->instructions->content[i]);
			}
		}


	}
	return copyInstructions;

}

controlFlowGraphBlock* traverseCfgBaseStatementType(controlFlowGraphBlock* node, controlFlowGraphBlock* start,  char* fileName, classDefInfo* classes) {
	controlFlowGraphBlock* copyCfg = copyNode(node);

	copyCfg->nodes[0]= traverseCfgType(node->nodes[0], node, fileName, classes);
	return copyCfg;

}

cfgBlockContent* copyInstructionsVarStatement(cfgBlockContent* old) {
	// add a var instruction
	cfgBlockContent* content = malloc(sizeof(cfgBlockContent));
	char** ids = (char*)malloc(sizeof(char*) * old->varDec->cntId);
	for (int k = 0; k < old->varDec->cntId; k++) {

		ids[k] = old->varDec->Ids[k];
	}

	//TODO: Handle the type , I actually need to see if it's a generic or a custom , I need the parameters of the class that brought it in, if brought by a class
	Type* instructionType = matchGenericType(old->varDec->type, current, currentGeneric);
	varDeclaration* varDecl = CreateVarDeclaration(ids, old->varDec->cntId, instructionType);
	content->type = TYPE_VARDECLARATION;
	content->varDec = varDecl;  // Assign var declaration to the union
	return content;
}
cfgBlockContent* copyInstructionsExpression(cfgBlockContent*  old) {
	// add an operations tree
	cfgBlockContent* content = malloc(sizeof(cfgBlockContent));
	OTNode* otNode = copyOperationsTree(old->ot);
	content->type = TYPE_OTNODE;
	content->ot = otNode;  // Assign OTNode to the union
	return content;

}
OTNode* copyOperationsTree(OTNode* oldOT) {
	OTNode* OT= malloc(sizeof(OTNode));
	
	if (OT->type== NODE_TYPE_OPERATOR) {

		OT = createOperatorNode(OT->value.operator);

		//insertCGToken(base->children[1]->token);

		//base = base->children[0];
	}
	
		else {
			OT = createOperandNode(oldOT->value.operand);
			OT->cntOperands = oldOT->cntOperands;
			OT->operands = malloc(sizeof(OTNode*)* OT->cntOperands);

			for (int i = 0; i < OT->cntOperands; i++) {

				OT->operands[i] = copyOperationsTree(oldOT->operands[i]);
			}
		
		}
	
	//OT->cntOperands = base->childrenCount;
	//OT->operands = malloc(sizeof(OTNode*) * base->childrenCount);

	return OT;

}

controlFlowGraphBlock* traverseCfgType(controlFlowGraphBlock* cfg, controlFlowGraphBlock* start,  char* fileName, classDefInfo* classes) {
	
	switch (cfg->blocktype)
	{
	case IfBlock:
		ret= traverseCfgIfStatementType(cfg, start, fileName, classes);
		break;
	case WhileBlock:
		if (cfg->drawn == 0) {
			cfg->drawn = 1;
			ret= traverseCfgWhileStatementType(cfg, start, fileName, classes);
		}
		else if (cfg->drawn == 1) {
			cfg->drawn = 2;
			return copyNode(cfg);
		}
		else {
			return copyNode(cfg);
		}
		break;
	case BreakBlock:


		return copyNode(cfg);
		break;
	case BaseBlock:
		if (cfg->outNodeCount <= 0) {


			return copyNode(cfg);
		}
		ret= traverseCfgBaseStatementType(cfg, start, fileName, classes);
		break;
	default:
		if (cfg->blocktype == IfExitBlock) {
			cfg->drawn++;
		}
		if (cfg->blocktype == IfExitBlock && cfg->drawn > 1) {
			cfg->drawn = 0;
			
			//ret = cfg;//TODO: hot fix, check what to do with if exit drawn maybe I need to set it to zero somewhere 
			break;
		}

		if (cfg->outNodeCount <= 0) {


			return copyNode(cfg);
		}
		ret= traverseCfgBaseStatementType(cfg, start, fileName, classes);
		break;
	}
	return ret;

}

