cc = g++ -Wall
#cc=clang++ -Wall

ifeq ($(mode),release)
		CCFLAGS = -std=c++11 -O3  -Wall
else ifeq ($(mode),debug)
		CCFLAGS = -std=c++11 -O0 -g -Wall
else
		mode = profile
		CCFLAGS = -std=c++11 -O3  -pg -Wall
endif


all: import output

.PHONY: clean tags


import:  
	@echo [cc] import
	make -C stsdb/ import

output:
	@echo [cc] output
	make -C stsdb/ output



tag:
	@echo [tag]
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .


clean:
	rm -f *.o
	rm -f *~
	rm -f *.a
	make -C stsdb/ clean
	rm -f tags
