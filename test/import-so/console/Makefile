console:
	g++ -c -fPIC -lv8 -o console.o console.cc
	g++ -o console.so -shared -Wl,-soname=console.so -Wl,--start-group console.o -Wl,--end-group

