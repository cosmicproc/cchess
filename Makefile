CFLAGS += -O2

cchess: src/*
	$(CC) $(CFLAGS) -o cchess src/*.c

debug: CFLAGS += -DDEBUG -g -Wall
debug: cchess

test: debug tests/*
	cd tests; \
	mkdir bin; \
	for f in *.c; \
	do \
		path="bin/$$(echo $$f | cut -f 1 -d '.')"; \
		$(CC) $(CFLAGS) -o "$$path" "$$f"; \
		echo "Running $$path"; \
		chmod +x "$$path"; \
		"./$$path"; \
	done

.PHONY: clean

clean:
	rm -rf cchess tests/bin