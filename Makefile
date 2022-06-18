.ONESHELL:
.PHONY: clean run tests debugtests

TEST_SRC = src/aig-sim.cpp src/aig-rtlil.cpp src/test.cpp src/genome.cpp src/aig-genome.cpp src/generation.cpp src/config-parse.cpp

cgploss.so: yosys/yosys
	yosys/yosys-config --exec --cxx --cxxflags --ldflags -o cgploss.so -shared src/* -I yosys/ -I include/ --ldlibs

multicore: yosys/yosys
	yosys/yosys-config --exec --cxx --cxxflags --ldflags -fopenmp -o cgploss.so -shared src/* -I yosys/ -I include/ --ldlibs

yosys/yosys:
	cd yosys && make

run: cgploss.so
	yosys/yosys -m cgploss.so

test: cgploss.so
	@echo "[info] starting implementation test $(dir)"
	@EXIT_CODE=0
	for f in ./tests/$(dir); do \
		if [ -d "$$f" -a $$(echo -n "$$f" | tail -c 1) != "-" -a $$(echo -n "$$f" | tail -c 4) != "unit" ]; then \
			#echo "$$f STARTED" && \
			yosys/yosys -m cgploss.so < "$$f/run" > test_run.txt || { echo -e "$$f \e[31mFAILED\e[0m" ; EXIT_CODE=1; continue; } && \
			iverilog -o test_design test_out.v "$$f/tb.v" || { echo -e "$$f \e[31mFAILED\e[0m" ; EXIT_CODE=1; continue; } && \
			vvp test_design || { echo -e "$$f \e[31mFAILED\e[0m" ; EXIT_CODE=1; continue; } && \
			echo -e "$$f \e[32mPASS\e[0m"; \
		elif [ -d "$$f" -a $$(echo -n "$$f" | tail -c 1) != "-" -a $$(echo -n "$$f" | tail -c 4) == "unit" ]; then \
			#echo "$$f STARTED UNIT TEST" && \
			cp $$f/test.cpp src/test.cpp  && \
			yosys/yosys-config --exec --cxx --cxxflags --ldflags -o cgplossUnit.so -shared $(TEST_SRC) -I yosys/ -I include/ --ldlibs > test_run.txt 2>&1 || { echo -e "$$f \e[31mFAILED\e[0m" ; EXIT_CODE=1; continue; } && \
			rm -f src/test.cpp && \
			yosys/yosys -m cgplossUnit.so < "$$f/run" >> test_run.txt || { echo -e "$$f \e[31mFAILED\e[0m" ; EXIT_CODE=1; continue; } && \
			echo -e "$$f \e[32mPASS\e[0m"; \
		fi \
	done
	# remove tests outputs
	@echo "[info] implementation test $(dir) done"
	@exit $$EXIT_CODE

tests:
	$(make) test dir=*
	rm -f test_design
	rm -f test_run.txt
	rm -f test_out.v

clean:
	rm -f cgploss.so
	rm -f cgploss.d
	rm -f test_design
	rm -f test_run.txt
	rm -f test_out.v
	rm -f cgplossUnit.d
	rm -f cgplossUnit.so
	rm -f src/test.cpp
