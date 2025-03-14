module top( g0 , p0, g1, p1, cin, cout);
  input g0, p0, g1, p1, cin;
  output cout;
  assign cout = g1 | (p1 & g0) | (p1 & p0 & cin);
endmodule
