module segment7(
     input  [3:0] bcd,
     output [6:0] seg
    );

    assign seg = (bcd == 0) ? 7'b0000001 : 
                 (bcd == 1) ? 7'b1001111 :
                 (bcd == 2) ? 7'b0010010 :
                 (bcd == 3) ? 7'b0000110 :
                 (bcd == 4) ? 7'b1001100 :
                 (bcd == 5) ? 7'b0100100 :
                 (bcd == 6) ? 7'b0100000 :
                 (bcd == 7) ? 7'b0001111 :
                 (bcd == 8) ? 7'b0000000 :
                 (bcd == 9) ? 7'b0000100 :
                 7'b1111111;
    
endmodule