SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
EXEC_DIR = bin
EXEC = $(EXEC_DIR)/main
PKGS = gtk+-3.0 libcurl json-glib-1.0
CFLAGS = `pkg-config --cflags $(PKGS)` -g
LIBS = `pkg-config --libs $(PKGS)`

all: $(EXEC)

$(EXEC): $(OBJ)
	@if [ ! -d $(EXEC_DIR) ]; then mkdir -p $(EXEC_DIR); fi;\
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)

clean:
	rm -rf $(EXEC) $(OBJ)
	if [ -d $(EXEC_DIR) ]; then rm -r $(EXEC_DIR); fi;\
