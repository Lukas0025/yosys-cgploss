module const_tb;
  task assert(input condition);
    if(!condition)
      $fatal;
  endtask

  reg a;
  wire c;
  
  konstanta UUT(
    .a(a), 
    .c(c)
  );
    
  initial begin
    a <= 0;
    #10
    assert(c === 1);

    a <= 1;
    #10
    assert(c === 1);

  end
endmodule