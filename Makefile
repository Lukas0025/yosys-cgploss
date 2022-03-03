.ONESHELL:
.PHONY: clean run tests debugtests

cgploss.so: yosys/yosys
	yosys/yosys-config --exec --cxx --cxxflags --ldflags -o cgploss.so -shared src/* -I yosys/ -I include/ --ldlibs

yosys/yosys:
	cd yosys && make

run: cgploss.so
	yosys/yosys -m cgploss.so

tests: cgploss.so
	@echo "[info] starting implementation tests"
	@EXIT_CODE=0
	for f in ./tests/*; do \
		if [ -d "$$f" -a $$(echo -n "$$f" | tail -c 1) != "-" ]; then \
			#echo "$$f STARTED" && \
			yosys/yosys -m cgploss.so < "$$f/run" > test_run.txt || { echo -e "$$f \e[31mFAILED\e[0m" ; EXIT_CODE=1; continue; } && \
			iverilog -o test_design test_out.v "$$f/tb.v" || { echo -e "$$f \e[31mFAILED\e[0m" ; EXIT_CODE=1; continue; } && \
			vvp test_design || { echo -e "$$f \e[31mFAILED\e[0m" ; EXIT_CODE=1; continue; } && \
			echo -e "$$f \e[32mPASS\e[0m"; \
		fi \
	done
	# remove tests outputs
	rm -f test_design
	rm -f test_run.txt
	rm -f test_out.v
	@echo "[info] implementation tests done"
	@exit $$EXIT_CODE

debugtests: cgploss.so
	@echo "[info] starting implementation tests"
	for f in ./tests/*; do \
		if [ -d "$$f" -a $$(echo -n "$$f" | tail -c 1) != "-" ]; then \
			#echo "$$f STARTED" && \
			yosys/yosys -m cgploss.so < "$$f/run" > test_run.txt || { echo -e "$$f \e[31mFAILED\e[0m" ; exit 1; } && \
			iverilog -o test_design test_out.v "$$f/tb.v" || { echo -e "$$f \e[31mFAILED\e[0m" ; exit 1; } && \
			vvp test_design || { echo -e "$$f \e[31mFAILED\e[0m" ; exit 1; } && \
			echo -e "$$f \e[32mPASS\e[0m"; \
		fi \
	done
	# remove tests outputs
	rm -f test_design
	rm -f test_run.txt
	rm -f test_out.v
	@echo "[info] implementation tests done"

clean:
	rm -f cgploss.so
	rm -f cgploss.d
	rm -f test_design
	rm -f test_run.txt
	rm -f test_out.v
