CC = gcc
CFLAGS = -Wno-implicit-function-declaration -g
INC = -Iinclude

# 修正 wildcard 語法，避免多餘的空格
SRCS = $(wildcard src/*.c)

# 修正 patsubst 語法
OBJS = $(patsubst src/%.c, obj/%.o, $(SRCS))

.PHONY: clean all

all: shell

# 生成可執行檔案 bin/shell
shell: $(OBJS)
	@mkdir -p bin
	@$(CC) $^ -o bin/$@ -lreadline

# 生成中間文件 obj/*.o
obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	@rm -rf bin/* obj/*.o
