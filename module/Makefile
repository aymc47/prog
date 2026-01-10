# Makefile (top)
SUBDIRS := $(wildcard */)

.PHONY: all clean
all:
	@for d in $(SUBDIRS); do \
		echo "==> build $$d"; \
		$(MAKE) -C $$d all || exit $$?; \
	done

clean:
	@for d in $(SUBDIRS); do \
		echo "==> cleaning $$d"; \
		$(MAKE) -C $$d clean || exit $$?; \
	done

install:
	@for d in $(SUBDIRS); do \
		echo "==> build $$d"; \
		$(MAKE) -C $$d install || exit $$?; \
	done

uninstall:
	@for d in $(SUBDIRS); do \
		echo "==> build $$d"; \
		$(MAKE) -C $$d uninstall || exit $$?; \
	done