OBJ:= html.o main.o
OUTPUT:=nhtmlc

all: debug

debug: CXXFLAGS = -g -DDEBUG
debug: CFLAGS = -g -DDEBUG
debug: $(OBJ)
	gcc $(OBJ) -g -o $(OUTPUT)

release: $(OBJ)
	gcc $(OBJ) -o $(OUTPUT)

clean:
	rm -rf $(OBJ) $(OUTPUT)


