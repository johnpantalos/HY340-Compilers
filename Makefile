comp:
	bison --defines --output=parser.cpp parser.y
	flex --outfile=scanner.cpp scanner.l;
	g++ -o calc -std=c++20 symbol.cpp actions.cpp types.cpp quad.cpp scanner.cpp parser.cpp tcode_types.cpp generators.cpp;

virt:
	g++ -o vm -std=c++20 VM.cpp machine_code.cpp executors.cpp avm_types.cpp lib_funcs.cpp

clean:
	rm scanner.cpp;
	rm parser.hpp;
	rm parser.cpp;
