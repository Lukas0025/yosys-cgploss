module xorpar (input [6:0] a, output parity);
   assign parity = a[0] ^ a[1] ^ a[2] ^ a[3] ^ a[4] ^ a[5] ^ a[6];  
endmodule