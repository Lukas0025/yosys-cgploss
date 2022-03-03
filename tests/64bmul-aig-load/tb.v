module mul_tb;
  task assert(input condition);
    if(!condition)
      $fatal;
  endtask

  reg  [63:0] a, b;
  reg  [127:0] mul_exp;

  wire [127:0] mul_out;

  integer i;
  
  mul UUT(
    .a(a),
    .b(b),
    .mul(mul_out)
  );
  
  initial begin
    a    <= 0;
    b    <= 0;

    for (i = 0; i < 100; i = i + 1) begin
      a    <= {$random, $random};
      b    <= {$random, $random};    

      #10

      mul_exp = a * b; 

      #10

      assert(mul_exp === mul_out);
    end
  end
endmodule