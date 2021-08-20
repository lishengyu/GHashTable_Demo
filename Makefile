
all: GHashTable_Demo

GHashTable_Demo: main.c
	gcc -o $@ $< `pkg-config --libs --cflags glib-2.0`

clean:
	rm GHashTable_Demo
