all: compile_file

compile_file:
	@g++ -std=c++11 -O2 n_body.cpp -o n_body

test-1: compile_file
	@./n_body solar.tsv 200 5000000 1000 solar_out.tsv > case1.txt
	@python3 plot.py solar_out.tsv solar_out.pdf 5000000

test-2: compile_file
	@./n_body 100 1 10000 100 out100.tsv > case2.txt
	@python3 plot.py out100.tsv out100.pdf 10000

test-3: compile_file
	@./n_body 1000 1 10000 100 out1000.tsv > case3.txt
	@python3 plot.py out1000.tsv out1000.pdf 10000