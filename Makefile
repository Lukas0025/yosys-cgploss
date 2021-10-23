.PHONY: clean run

cgploss.so: yosys/yosys
	yosys/yosys-config --exec --cxx --cxxflags --ldflags -o cgploss.so -shared src/main.cpp -I yosys/ --ldlibs

yosys/yosys:
	cd yosys && make

run: cgploss.so
	yosys/yosys -m cgploss.so

clean:
	rm -f cgploss.so
	rm -f cgploss.d