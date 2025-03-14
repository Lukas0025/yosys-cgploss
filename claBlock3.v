module top( g0 , p0, g1, p1, g2, p2, cin, cout);
  input g0, p0, g1, p1, g2, p2, cin;
  output cout;
  assign cout = g2 | (p2 & g1) | (p2 & p1 & g0) | (p2 & p1 & p0 & cin);
endmodule
