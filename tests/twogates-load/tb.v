module two_gates_tb;
  task assert(input condition);
    if(!condition)
      $fatal;
  endtask

  reg a, b;
  wire c;
  
  two_gates UUT(
    .a(a), 
    .b(b),
    .c(c)
  );
    
  initial begin
    a <= 0;
    b <= 0;
    #10
    assert(c === 0);

    a <= 1;
    b <= 0;
    #10
    assert(c === 1);

    a <= 1;
    b <= 1;
    #10
    assert(c === 1);

    a <= 0;
    b <= 1;
    #10
    assert(c === 0);

  end
endmodule