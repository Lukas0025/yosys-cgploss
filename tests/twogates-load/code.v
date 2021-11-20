module two_gates (input a,  
                  input b,   
                  output c);
   assign c = (a & b) | a;  
endmodule