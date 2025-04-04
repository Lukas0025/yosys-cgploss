module top( g0 , p0, g1, p1, cin, cout1, cout2);
  input g0, p0, g1, p1, g2, p2, cin;
  output cout1, cout2;

  assign cout1 = g0 | (p0 & cin);
  assign cout2 = g1 | (p1 & g0) | (p1 & p0 & cin);
endmodule
