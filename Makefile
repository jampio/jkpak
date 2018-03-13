bindir ?= /bin
PREFIX ?= /usr/local

.PHONY: build
build:
	@mkdir -p build && \
	conan install -if build . --build missing && \
	conan build -bf build .

.PHONY: install
install:
	@install build/package/bin/jkpak -D -t $(DESTDIR)$(PREFIX)$(bindir)/

.PHONY: clean
clean:
	@rm -rf build
