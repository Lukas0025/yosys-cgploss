module fulladder_tb;
  task assert(input condition);
    if(!condition)
      $fatal;
  endtask

  reg  a, b, sum_exp;
  reg  c_in, c_out_exp;

  wire sum;
  wire c_out;

  integer i, j, k;
  
  fulladder UUT(
    .a(a),
    .b(b),
    .c_in(c_in),
    .c_out(c_out),
    .sum(sum)
  );
  
  initial begin
	a    <= 0;
	b    <= 0;    
	c_in <= 0;

	#10

	assert(sum === 0);
	assert(c_out === 0);

	a    <= 1;
	b    <= 0;    
	c_in <= 0;

	#10

	assert(sum === 1);
	assert(c_out === 0);

	a    <= 0;
	b    <= 1;    
	c_in <= 0;

	#10

	assert(sum === 1);
	assert(c_out === 0);

	a    <= 1;
	b    <= 1;    
	c_in <= 0;

	#10

	assert(sum === 0);
	assert(c_out === 1);

	a    <= 1;
	b    <= 0;    
	c_in <= 1;

	#10

	assert(sum === 0);
	assert(c_out === 1);

	a    <= 0;
	b    <= 1;    
	c_in <= 1;

	#10

	assert(sum === 0);
	assert(c_out === 1);

	a    <= 1;
	b    <= 1;    
	c_in <= 1;

	#10

	assert(sum === 1);
	assert(c_out === 1);
  end
endmodule