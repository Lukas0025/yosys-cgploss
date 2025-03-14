module top( g0 , p0, cin, cout);
  input g0, p0, cin;
  output cout;
  assign cout = g0 | (p0 & cin);
endmodule
