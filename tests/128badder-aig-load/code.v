module fulladder (input [127:0] a,  
                  input [127:0] b,  
                  input c_in,  
                  output c_out,  
                  output [127:0] sum);
   assign {c_out, sum} = a + b + c_in;  
endmodule