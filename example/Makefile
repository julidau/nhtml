
all: entchen.html

NHTMLC:=../build/nhtmlc

%.html: %.nhtml
	$(NHTMLC) $< -o $@

index.html: head.nhtml entchen.nhtml
	$(NHTMLC) entchen.nhtml -o $@

clean: 
	rm -f entchen.html

.PHONY: all clean
