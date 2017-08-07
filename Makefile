OBJ:= main.o
OUTPUT:=nhtmlc

debug: CXXFLAGS = -g 
debug: CFLAGS = -g 
debug: $(OBJ)
	gcc $(OBJ) -g -o $(OUTPUT)

release: $(OBJ)
	gcc $(OBJ) -o $(OUTPUT)

clean:
	rm -rf $(OBJ) $(OUTPUT)


