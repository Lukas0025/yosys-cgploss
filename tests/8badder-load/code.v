module fulladder (input [7:0] a,  
                  input [7:0] b,  
                  input c_in,  
                  output c_out,  
                  output [7:0] sum);
   assign {c_out, sum} = a + b + c_in;  
endmodule