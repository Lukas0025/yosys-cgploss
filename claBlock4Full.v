module top( g0 , p0, g1, p1, g2, p2, p3, g3, cin, cout1, cout2, cout3, cout4);
  input g0, p0, g1, p1, g2, p2, g3, p3, cin;
  output cout1, cout2, cout3, cout4;

  assign cout1 = g0 | (p0 & cin);
  assign cout2 = g1 | (p1 & g0) | (p1 & p0 & cin);
  assign cout3 = g2 | (p2 & g1) | (p2 & p1 & g0) | (p2 & p1 & p0 & cin);
  assign cout4 = g3 | (p3 & g2) | (p3 & p2 & g1) | (p3 & p2 & p1 & g0) | (p3 & p2 & p1 & p0 & cin);
endmodule
