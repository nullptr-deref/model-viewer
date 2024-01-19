CC = gcc
CFLAGS = -g
BIN_DIR = bin
SRC_DIR = src
OBJ_DIR = obj
LDLIBS = -lwavefront

LDFLAGS += -L/usr/lib/ -L/usr/local/lib
CFLAGS += -I/usr/include/
LDLIBS += -lglfw -lGLEW -lGL

.PHONY: clean

$(BIN_DIR)/viewer: $(SRC_DIR)/viewer.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
