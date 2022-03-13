CFLAGS = -Wall -Wextra -std=c17 -pedantic #-fsanitize=address -Og -g

botfile = test.c
srcdir = src
builddir = build
mlxdir = mlx
bin = cluster
mlx = $(mlxdir)/libmlx42.a
src = $(wildcard $(srcdir)/*.c)
obj = $(patsubst $(srcdir)/%.c,$(builddir)/%.o,$(src))
dep = $(patsubst $(srcdir)/%.c,$(builddir)/%.d,$(src))
inc = -I include -I mlx/include
CC = gcc

all: $(bin)

$(bin): $(obj) $(mlx)
	@mkdir -p $(builddir)
	$(CC) $(CFLAGS) $^ -o $@ -lglfw -L /Users/$(USER)/.brew/opt/glfw/lib -lm -ldl

$(builddir)/%.o: $(srcdir)/%.c
	@mkdir -p $(builddir)
	$(CC) $(CFLAGS) $< -o $@ -c -MMD $(inc)

$(mlx):
	make -C $(mlxdir)

clean:
	rm -rf $(builddir)
	rm -f $(bin)

test: all
	./cluster

bot_test: all bot
	./cluster ./bot ./bot

bot: $(botfile)
	$(CC) $(botfile) -o $@

re: clean
	make re -C mlx
	make all

-include $(dep)
