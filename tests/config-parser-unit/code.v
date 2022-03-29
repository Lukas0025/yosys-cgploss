module complex_circ (input a,  
                     input b,
                     output [7:0] sum,
					 output [3:0] spi_data,
					 output [1:0] spi2_data,
					 output uart_rx,
					 output [3:0] uart_tx,
					 output [3:0] uart2_tx);
		assign sum       = {a & b, a & b, a & b, a & b, a & b, a & b, a & b, a & b};
		assign spi_data  = {a & b, a & b, a & b, a & b};
		assign spi2_data = {a & b, a & b};
		assign uart_rx   = a & b;
		assign uart_tx   = {a & b, a & b, a & b, a & b};
		assign uart2_tx   = {a & b, a & b, a & b, a & b};
endmodule