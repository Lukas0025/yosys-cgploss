module fulladder_tb;
  task assert(input condition);
    if(!condition)
      $fatal;
  endtask

  reg  [7:0] a, b, sum_exp;

  wire [7:0] sum;

  integer i;
  integer error_total;
  integer diff;
  integer WCE;
  real    MAE;
  
  fulladder UUT(
    .a(a),
    .b(b),
    .sum(sum)
  );
  
  initial begin
    a    <= 0;
    b    <= 0;

    error_total <= 0;
    WCE         <= 0;

    for (i = 0; i < 65536; i = i + 1) begin
      {a, b} <= i;

      #100

      sum_exp <= a + b; 

      #100

      diff = sum_exp - sum;

      #100

      if (diff < 0)
	diff = -diff;

      #100

      if (WCE < diff)
	WCE = diff;

      #100

      error_total = error_total + diff;
    end

    MAE = error_total / 65536;

    assert(MAE <= 50);
    assert(WCE <= 50);
    assert(WCE > 0);
  end
endmodule