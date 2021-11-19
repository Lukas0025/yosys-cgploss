.PHONY: clean run tests

cgploss.so: yosys/yosys
	yosys/yosys-config --exec --cxx --cxxflags --ldflags -o cgploss.so -shared src/* -I yosys/ -I include/ --ldlibs

yosys/yosys:
	cd yosys && make

run: cgploss.so
	yosys/yosys -m cgploss.so

tests: cgploss.so
	@echo "[info] starting units tests"
	for f in ./tests/*; do \
		if [ -d "$$f" -a $$(echo -n "$$f" | tail -c 1) != "-" ]; then \
			echo "$$f STARTED" && \
			yosys/yosys -m cgploss.so < "$$f/run" > test_run.txt || { echo "$$f FAILED" ; exit 1; } && \
			iverilog -o test_design test_out.v "$$f/tb.v" || { echo "$$f FAILED" ; exit 1; } && \
			vvp test_design || { echo "$$f FAILED" ; exit 1; } && \
			echo "$$f PASS"; \
		fi \
	done
	make clean
	@echo "[info] units tests done"

clean:
	rm -f cgploss.so
	rm -f cgploss.d
	rm -f test_design
	rm -f test_run.txt
	rm -f test_out.v