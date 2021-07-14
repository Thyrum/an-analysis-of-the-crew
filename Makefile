.PHONY: debug release all clean
all: release

release: release/Makefile
	cd release && make -j --no-print-directory

debug: debug/Makefile
	cd debug && make -j --no-print-directory

clean:
	rm -rf release debug bin
	rm -rf deps/lib/* deps/include/*

%/Makefile:
	mkdir -p $* && cd $* && cmake .. -DCMAKE_BUILD_TYPE=$* -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
