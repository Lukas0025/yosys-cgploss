module top( g0 , p0, g1, p1, g2, p2, p3, g3, cin, cout);
  input g0, p0, g1, p1, g2, p2, cin;
  output cout;
  assign cout = g3 | (p3 & g2) | (p3 & p2 & g1) | (p3 & p2 & p1 & g0) | (p3 & p2 & p1 & p0 & cin);
endmodule
