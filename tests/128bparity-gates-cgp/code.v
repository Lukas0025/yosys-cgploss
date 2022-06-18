module paritydev (input [127:0] a,  
                  output par);
   assign par = ^a;  
endmodule