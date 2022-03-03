module mul (input [63:0] a,  
            input [63:0] b,  
            output [127:0] mul);
   assign mul = a * b;  
endmodule