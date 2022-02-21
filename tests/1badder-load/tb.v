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

    for (i = 0; i < 2; i = i + 1) begin
		for (j = 0; i < 2; i = i + 1) begin
			for (k = 0; i < 2; i = i + 1) begin
				a    <= i;
				b    <= j;    
				c_in <= k;

				#10

				{c_out_exp, sum_exp} = a + b + c_in; 

				#10

				assert(sum === sum_exp);
				assert(c_out === c_out_exp);
			end
		end
    end
  end
endmodule