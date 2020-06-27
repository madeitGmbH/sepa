CFLAGS = -g -O3 -std=gnu99 -pedantic -Wall -Wextra -Wno-unused-parameter

ERLANG_PATH = $(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
CFLAGS += -I$(ERLANG_PATH)

# ifeq ($(wildcard deps/hoedown),)
# 	HOEDOWN_PATH = ../hoedown
# else
# 	HOEDOWN_PATH = deps/hoedown
# endif

# CFLAGS += -I$(HOEDOWN_PATH)/src

ifneq ($(OS),Windows_NT)
	CFLAGS += -fPIC

	ifeq ($(shell uname),Darwin)
		LDFLAGS += -dynamiclib -undefined dynamic_lookup
	endif
endif

.PHONY: clean

priv/sepa.so: src/sepa.c
	$(CC) $(CFLAGS) -shared $(LDFLAGS) -o $@ src/sepa.c src/libsepa.a

clean:
	$(RM) priv/sepa.so