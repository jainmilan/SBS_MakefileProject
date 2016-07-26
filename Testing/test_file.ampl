model test_file.mod;
data test_file.dat;

let a:= 0;
let b:= 100;
display a, b;

solve;

display a, b;
