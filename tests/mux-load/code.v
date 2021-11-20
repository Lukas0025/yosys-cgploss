module mux(
	input I0, 
	input I1,
	input S,
	output Y
);

	assign Y = (S) ? I1 : I0;

endmodule