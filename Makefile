CC = gcc
CFLAGS = -g
BIN_DIR = bin
SRC_DIR = src
OBJ_DIR = obj
LDLIBS = -lglfw -lGLEW -lGL

ifeq ($(OS), "Windows_NT")
	LDFLAGS += -L/mingw64/lib/
else
	LDFLAGS += -L/usr/lib/ -L/usr/local/lib
endif

.PHONY: clean

$(BIN_DIR)/viewer: $(SRC_DIR)/viewer.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $<

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
