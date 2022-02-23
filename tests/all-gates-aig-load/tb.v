module allgates_tb;
  task assert(input condition);
    if(!condition)
      $fatal;
  endtask

  reg  a, b, c, d;
  reg  not_exp, and_exp, nand_exp, andnot_exp, or_exp, nor_exp, ornot_exp, xor_exp, xnor_exp, aoi3_exp, oai3_exp, aoi4_exp, oai4_exp, mux_exp, nmux_exp;

  wire not_o, and_o, nand_o, andnot_o, or_o, nor_o, ornot_o, xor_o, xnor_o, aoi3_o, oai3_o, aoi4_o, oai4_o, mux_o, nmux_o;

  integer i;
  
  allgates UUT(
    .a(a),
    .b(b),
	.c(c),
    .d(d),
    .not_o(not_o),
	.and_o(and_o),
	.nand_o(nand_o),
	.andnot_o(andnot_o),
	.or_o(or_o),
	.nor_o(nor_o),
	.ornot_o(ornot_o),
	.xor_o(xor_o),
	.xnor_o(xnor_o),
	.aoi3_o(aoi3_o),
	.oai3_o(oai3_o),
	.aoi4_o(aoi4_o),
	.oai4_o(oai4_o),
	.mux_o(mux_o),
	.nmux_o(nmux_o)
  );
  
  initial begin
    for (i = 0; i < 15; i = i + 1) begin
		{d, c, b, a} <= i;

		#10
		
		not_exp    =   ~a;  
		and_exp    =    a  &  b;  
		nand_exp   = ~( a  &  b);
		andnot_exp =    a  & ~b;
		or_exp     =    a  |  b;
		nor_exp    = ~( a  |  b);
		ornot_exp  =    a  | ~b;
		xor_exp    =    a  ^  b;
		xnor_exp   = ~( a  ^  b);
		
		aoi3_exp   = ~((a & b) | c);
		oai3_exp   = ~((a | b) & c);
		aoi4_exp   = ~((a & b) | (c & d));
		oai4_exp   = ~((a | b) & (c | d));

		#10

		assert(not_exp === not_o);
		/*assert(and_exp === and_o);
		assert(nand_exp === nand_o);
		assert(andnot_exp === andnot_o);
		assert(or_exp === or_o);
		assert(nor_exp === nor_o);
		assert(ornot_exp === ornot_o);
		assert(xor_exp === xor_o);
		assert(xnor_exp === xnor_o);*/

		//assert(oai3_exp === oai3_o);
		//assert(aoi3_exp === aoi3_o);
		//assert(oai4_exp === oai4_o);
		//assert(aoi4_exp === aoi4_o);
    end
  end
endmodule