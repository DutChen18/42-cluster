CFLAGS = -Wall -Wextra -std=c17 -pedantic

srcdir = src
builddir = build
bin = cluster
src = $(wildcard $(srcdir)/*.c)
obj = $(patsubst $(srcdir)/%.c,$(builddir)/%.o,$(src))
dep = $(patsubst $(srcdir)/%.c,$(builddir)/%.d,$(src))

all: $(bin)

$(bin): $(obj)
	$(CC) $(CFLAGS) $^ -o $@

$(builddir)/%.o: $(srcdir)/%.c
	$(CC) $(CFLAGS) $^ -o $@ -c -MMD

clean:
	rm -r $(builddir)
	rm $(bin)

re: clean
	make all

-include $(dep)
