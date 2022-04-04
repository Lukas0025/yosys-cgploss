module allgates ( input a,  
                  input b,  
                  input c,
                  input d,  
                  output not_o,
                  output and_o,
                  output nand_o,
                  output andnot_o,
                  output or_o,
                  output nor_o,
                  output ornot_o,
                  output xor_o,
                  output xnor_o,
                  output aoi3_o,
                  output oai3_o,
                  output aoi4_o,
                  output oai4_o,
                  output mux_o,
                  output nmux_o,
               );
   assign not_o    =   ~a;  
   assign and_o    =    a  &  b; 
   assign nand_o   = ~( a  &  b);
   assign andnot_o =    a  & ~b;
   assign or_o     =    a  |  b;
   assign nor_o    = ~( a  |  b);
   assign ornot_o  =    a  | ~b;
   assign xor_o    =    a  ^  b;
   assign xnor_o   = ~( a  ^  b);

   assign aoi3_o   = ~((a & b) | c);
   assign oai3_o   = ~((a | b) & c);
   assign aoi4_o   = ~((a & b) | (c & d));
   assign oai4_o   = ~((a | b) & (c | d));
endmodule