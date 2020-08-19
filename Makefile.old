ifeq ($(OS), Windows_NT)
	include Makefile.windows
else
	ifeq ($(shell uname -s),Darwin)
		include Makefile.macos
	else
		include Makefile.linux
	endif
endif
