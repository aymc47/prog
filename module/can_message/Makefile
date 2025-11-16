# インストール
INSDIR_INCLUDES=../../common/include/
INSDIR_BIN=../../common/lib/
INSDIR_HEADER = $(wildcard ex_*.h)

# インクルードパス
INCLUDES= -I./ -I$(INSDIR_INCLUDES)

# ソースファイル
SOURCES=$(wildcard *.c)

# オブジェクトファイル
OBJECTS=$(SOURCES:.c=.o)

# 出力するライブラリ名
TARGET=libcanmessage.a

# コンパイラの指定
CC=gcc

# コンパイルオプション
CFLAGS=-fPIC -Wall -Wextra -O2 -g $(INCLUDES)

# リンクオプション
LDFLAGS=-shared

# デフォルトターゲット
all: $(TARGET)

# ライブラリの生成
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

# オブジェクトファイルの生成
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 'make clean' の定義
clean:
	rm -f $(OBJECTS) $(TARGET)

# '.PHONY' ターゲットは実際のファイルではなく、コマンドとして定義されていることを示す
.PHONY: all clean

# installタスク
install: $(TARGET)
	cp $(TARGET) $(INSDIR_BIN)$(TARGET)
	cp $(INSDIR_HEADER) $(INSDIR_INCLUDES)$(INSDIR_HEADER)

# uninstallタスク
uninstall:
	rm -f $(INSDIR_BIN)$(TARGET) $(INSDIR_INCLUDES)$(INSDIR_HEADER)
