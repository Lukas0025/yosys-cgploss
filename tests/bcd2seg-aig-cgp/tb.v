module fulladder_tb;
  task assert(input condition);
    if(!condition)
      $fatal;
  endtask

  reg  [3:0] a;
  wire [6:0] seg;
  
  segment7 UUT(
    .bcd(a),
    .seg(seg)
  );
  
  initial begin
	a    <= 4'b0000;

	#10

	assert(seg === 7'b0000001);

	a    <= 4'b0001;

	#10

	assert(seg === 7'b1001111);

	a    <= 4'b0010;

	#10

	assert(seg === 7'b0010010);

	a    <= 4'b0011;

	#10

	assert(seg === 7'b0000110);

	a    <= 4'b0100;

	#10

	assert(seg === 7'b1001100);

	a    <= 4'b0101;

	#10

	assert(seg === 7'b0100100);

	a    <= 4'b0110;

	#10

	assert(seg === 7'b0100000);

	a    <= 4'b0111;

	#10

	assert(seg === 7'b0001111);

	a    <= 4'b1000;

	#10

	assert(seg ===  7'b0000000);

	a    <= 4'b1001;

	#10

	assert(seg ===  7'b0000100);

	a    <= 4'b1100;

	#10

	assert(seg ===  7'b1111111);


  end
endmodule