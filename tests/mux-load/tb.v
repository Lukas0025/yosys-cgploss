module andgate_tb;
  task assert(input condition);
    if(!condition)
      $fatal;
  endtask

  reg a, b, s;
  wire y;
  
  mux UUT(
    .I0(a),
    .I1(b),
    .S(s),
    .Y(y)
  );
    
  initial begin
    a <= 1;
    b <= 0;
    s <= 0;
    #10
    assert(y === 1);

    a <= 0;
    b <= 1;
    s <= 1;
    #10
    assert(y === 1);

    a <= 1;
    b <= 0;
    s <= 1;
    #10
    assert(y === 0);

    a <= 0;
    b <= 1;
    s <= 0;
    #10
    assert(y === 0);
  end
endmodule