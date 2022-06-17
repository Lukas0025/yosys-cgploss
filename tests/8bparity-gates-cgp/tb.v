module fulladder_tb;
  task assert(input condition);
    if(!condition)
      $fatal;
  endtask

  reg  [7:0] a;
  reg exp_parity;

  wire parity;

  integer i;
  
  xorpar UUT(
    .a(a),
    .parity(parity)
  );
  
  initial begin
    a    <= 0;

    for (i = 0; i < 256; i = i + 1) begin
      a <= i;

      #10

      exp_parity <= a[0] ^ a[1] ^ a[2] ^ a[3] ^ a[4] ^ a[5] ^ a[6] ^ a[7];  

      #10

      assert(parity === exp_parity);
    end
  end
endmodule