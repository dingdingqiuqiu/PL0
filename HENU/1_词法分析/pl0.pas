// PL/0 demo

(*
  This is a multi-line
  comment
*)

const limit=10;
var n, f, test, t1, t2;
begin
    n := 0;
    f := 1;
    while n # limit do
    begin
        n := n + 1;
        f := f * n;     
    end;
