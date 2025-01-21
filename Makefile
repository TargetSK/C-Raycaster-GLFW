all:
	gcc main.c util.c -Iinclude -Llib -lglfw3 -lopengl32 -lgdi32 