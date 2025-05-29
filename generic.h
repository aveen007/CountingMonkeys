#pragma once
#include "ControlGraph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
Type* matchGenericType(Type* key, classDef* class, Type* genericType);

classDef* initClass(classDef* found, Type* parent, classDefInfo* classes);

Type* findClass(classDefInfo* classes, Type* parent);

Subroutine** setTypes(Subroutine** subroutines, int cnt, char* fileName, classDefInfo* classes);

controlFlowGraphBlock* traverseCfgIfStatementType(controlFlowGraphBlock* node, controlFlowGraphBlock* start, char* fileName, classDefInfo* classes);

controlFlowGraphBlock* traverseCfgWhileStatementType(controlFlowGraphBlock* node, controlFlowGraphBlock* start, char* fileName, classDefInfo* classes);

controlFlowGraphBlock* traverseCfgBaseStatementType(controlFlowGraphBlock* node, controlFlowGraphBlock* start, char* fileName, classDefInfo* classes);

controlFlowGraphBlock* traverseCfgType(controlFlowGraphBlock* cfg, controlFlowGraphBlock* start, char* fileName, classDefInfo* classes);
controlFlowGraphBlock* traverseCfgBaseStatementType(controlFlowGraphBlock* node, controlFlowGraphBlock* start, char* fileName, classDefInfo* classes);
OTNode* copyOperationsTree(OTNode* oldOT);
cfgBlockContent* copyInstructionsExpression(cfgBlockContent* old);
cfgBlockContent* copyInstructionsVarStatement(cfgBlockContent* old);
Instructions* copyInstructions(controlFlowGraphBlock* node);