module fulladder_tb;
  task assert(input condition);
    if(!condition)
      $fatal;
  endtask

  reg  [127:0] a;
  reg  par_exp;
  wire par;

  integer i;
  
  paritydev UUT(
    .a(a),
    .par(par)
  );
  
  initial begin
    a    <= 0;

    for (i = 0; i < 131072; i = i + 1) begin
      a    <= {$random, $random, $random, $random};

      #10

      par_exp = ^a; 

      #10

      assert(par === par_exp);
    end
  end
endmodule