CC = gcc
CFLAGS = -g
BIN_DIR = bin
SRC_DIR = src
OBJ_DIR = obj
LDLIBS = -lwavefront

ifeq ($(OS), Windows_NT)
	LDFLAGS += -L/clang64/bin/ -L/usr/lib/ -L/usr/local/lib
	CFLAGS += -I/clang64/include/ -DGLFW_DLL
	LDLIBS += -lglfw3 -lglew32 -lopengl32
else
	LDFLAGS += -L/usr/lib/ -L/usr/local/lib
	CFLAGS += -I/usr/include/
	LDLIBS += -lglfw -lGLEW -lGL
endif

.PHONY: clean

ifeq ($(OS), Windows_NT)
$(BIN_DIR)/viewer: $(SRC_DIR)/viewer.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)
	cp /clang64/bin/glfw3.dll /clang64/bin/glew32.dll $(BIN_DIR)/
else
$(BIN_DIR)/viewer: $(SRC_DIR)/viewer.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)
endif

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
