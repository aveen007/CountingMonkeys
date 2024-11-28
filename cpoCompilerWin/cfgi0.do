digraph G {
ordering=out;
ranksep=.4;
bgcolor="lightgrey";  node [shape=box, fixedsize=false, fontsize=12, fontname="Helvetica-bold", fontcolor="blue"
width=.25, height=.25, color="black", fillcolor="white", style="filled, solid, bold"];

edge [arrowsize=.5, color="black", style="bold"]    n0000124E2CF22790 [label="BaseBlock\n OT: [ = sum , ) ,  1 , ))]
OT: [ = i , ) ,  0 , ))]
"]
    n0000124E2CF22790 -> n0000124E2CF22850
    n0000124E2CF22850 [label="IfBlock\n OT: [ < i , ) ,  9 , ))]
"]
    n0000124E2CF22850 -> n0000124E2CF228D0 [label="False" color="red"]
    n0000124E2CF228D0 [label="ElseBlock\n OT: [ = sum , ) ,  - sum , ) ,  i , )))]
"]
    n0000124E2CF228D0 -> n0000124E2CF22910
    n0000124E2CF22910 [label="WhileBlock\n OT: [ > i , ) ,  0 , ))]
"]
    n0000124E2CF22910 -> n0000124E2CF22950 [ color="blue"]
    n0000124E2CF22950 [label="WhileBodyBlock\n "]
    n0000124E2CF22950 -> n0000124E2CF22990
    n0000124E2CF22990 [label="IfBlock\n OT: [ >= i , ) ,  sum , ))]
"]
    n0000124E2CF22990 -> n0000124E2CF22A10 [label="False" color="red"]
    n0000124E2CF22A10 [label="ElseBlock\n OT: [ = i , ) ,  + i , ) ,  1 , )))]
"]
    n0000124E2CF22A10 -> n0000124E2CF22A50 [ color="blue"]
    n0000124E2CF22990 -> n0000124E2CF229D0 [label="True" color="green"]
    n0000124E2CF229D0 [label="ThenBlock\n OT: [ = i , ) ,  - i , ) ,  1 , )))]
"]
    n0000124E2CF229D0 -> n0000124E2CF22A50 [ color="blue"]
    n0000124E2CF22A50 [label="WhileExitBlock\n "]
    n0000124E2CF22A50 -> n0000124E2CF22910 [label="True" color="green"]
    n0000124E2CF22910 -> n0000124E2CF22A90 [label="False" color="red"]
    n0000124E2CF22850 -> n0000124E2CF22890 [label="True" color="green"]
    n0000124E2CF22890 [label="ThenBlock\n OT: [ = sum , ) ,  + sum , ) ,  i , )))]
OT: [ = i , ) ,  + i , ) ,  1 , )))]
"]
    n0000124E2CF22890 -> n0000124E2CF22A90 [ color="blue"]
    n0000124E2CF22A90 [label="IfBlock\n OT: [ > i , ) ,  0 , ))]
"]
    n0000124E2CF22A90 -> n0000124E2CF22AD0 [label="True" color="green"]
    n0000124E2CF22AD0 [label="ThenBlock\n OT: [ = sum , ) ,  + sum , ) ,  i , )))]
OT: [ = i , ) ,  + i , ) ,  1 , )))]
"]
    n0000124E2CF22AD0 -> n0000124E2CF22B10 [ color="blue"]
    n0000124E2CF22B10 [label="BaseBlock\n "]
}
