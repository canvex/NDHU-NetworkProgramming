CC = gcc
CFLAGS = -Wno-implicit-function-declaration -g
INC = -Iinclude

# 修正 wildcard 語法，避免多餘的空格
SRCS = $(wildcard src/*.c)

# 排除特定檔案
EXCLUDED_SRCS = src/who.c src/name.c
SHELL_SRCS = $(filter-out $(EXCLUDED_SRCS), $(SRCS))

# 修正 patsubst 語法
OBJS = $(patsubst src/%.c, obj/%.o, $(SHELL_SRCS))

.PHONY: clean all

all: shell who name

# 生成可執行檔案 bin/shell
shell: $(OBJS)
	@mkdir -p bin
	@$(CC) $^ -o bin/$@ -lreadline -lhistory

# 生成獨立的 who 可執行檔案
who:
	@mkdir -p bin
	@$(CC) $(CFLAGS) $(INC) src/who.c -o bin/who

# 生成獨立的 name 可執行檔案
name:
	@mkdir -p bin
	@$(CC) $(CFLAGS) $(INC) src/name.c -o bin/name

# 生成中間文件 obj/*.o
obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	@rm -rf bin/* obj/*.o
