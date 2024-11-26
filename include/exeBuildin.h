// exeBuildin.h
#ifndef EXEBUILDIN_H
#define EXEBUILDIN_H

#include "parse.h"  // 確保能找到 command_t 的定義

void exeBuildin(command_t* cmd);  // 添加函數原型
void exeNonBuildin(command_t* cmd);
void exeNormalPipe(command_t* cmd);

#endif
