# Nautilus IR Graphs:

Nautilus supports the vizualization of its intermediate representation using Graphviz or Marimaid:

## Options:

| Option                             | Default  | Description                                                                              |
|------------------------------------|----------|------------------------------------------------------------------------------------------|
| dump.graph=[true,false]            | false    | Dumps a graph representation in either (mermaid or graphvit) to a file.                  |
| dump.graph.type=[mermaid,graphviz] | graphviz | Sets the type of the graph.                                                              |
| dump.graph.full=[true,false]       | false    | Represents both data and control-flow (full=true) or only the control-flow (full=false). |

### Example Nautilus Function:

A simple nautilus function that invokes a native add function for each loop iteration and calculates a final sum.

```Cpp
val<int32_t> loopDirectCall(val<int32_t> c, val<int32_t> x) {
 val<int32_t> sum = 0;
 for (val<int32_t> i = 0; i < c; i = i + 1) {
    sum = invoke<>(add, sum, x);
 }
 return sum;
}
```

Resulting Nautilus IR:

```
NautilusIr {
execute() {
Block_0($1:i32, $2:i32):
	$3 = 0 :i32
	$4 = 0 :i32
	br Block_3($3, $4, $1, $2) :void

Block_3($3:i32, $4:i32, $1:i32, $2:i32):
	$5 = $4 < $1 :bool
	if $5 ? Block_1($1, $4, $3, $2) : Block_2($3) :void

Block_1($1:i32, $4:i32, $3:i32, $2:i32):
	$7 = _ZN8nautilus6engine3addEii($3,$2) :i32
	$8 = 1 :i32
	$9 = $4 + $8 :i32
	br Block_3($7, $9, $1, $2) :void

Block_2($3:i32):
	return ($3) :i32
}
} 
 ```

## Control-flow Graph

If `dump.graph.full=false` nautilus dumps only the control-flow graph.
For the example function, the IR consists of three basic blocks.  
`Block 0` is the function entry, `Block 3` is the loop head, `Block 1` the loop body, and `Block 2` is the final return
block.
This representation is very useful to inspect the connection between individual basic blocks.

```mermaid
flowchart TD
    node0[Block0]
    style node0 fill: #e98693, color: #1a1919
    node3[Block3]
    style node3 fill: #e98693, color: #1a1919
    node1[Block1]
    style node1 fill: #e98693, color: #1a1919
    node2[Block2]
    style node2 fill: #e98693, color: #1a1919
    node0 --> node3
    linkStyle 0 stroke: #da2d4f, stroke-width: 2px
    node3 -- true --> node1
    linkStyle 1 stroke: #da2d4f, stroke-width: 2px
    node3 -- false --> node2
    linkStyle 2 stroke: #da2d4f, stroke-width: 2px
    node1 --> node3
    linkStyle 3 stroke: #da2d4f, stroke-width: 2px

```

## Data/Control-flow Graph

If `dump.graph.full=true` nautilus dumps the complete data and control-flow graph.
This representation contains all IR nodes and represents the data and control dependencies between individual nodes.
In this case we can see in `Block 3` the loop condition `$5 = $4 < $1 :bool` and the invocation of the `add` function
in `Block 1`.

```mermaid
flowchart TD
    subgraph cluster_0[Block 0]
        nodestart_0[Start]
        style nodestart_0 fill: #e98693, color: #1a1919
        node0_0[Arg $1]
        style node0_0 fill: #d7ede7, color: #1a1919
        node0_1[Arg $2]
        style node0_1 fill: #d7ede7, color: #1a1919
        node0_2[0]
        style node0_2 fill: #343d46, color: white
        node0_3[0]
        style node0_3 fill: #343d46, color: white
        node0_4[End]
        style node0_4 fill: #e98693, color: #1a1919
        node0_o5_0[Output $3]
        style node0_o5_0 fill: #d7ede7, color: #1a1919
        node0_o5_1[Output $4]
        style node0_o5_1 fill: #d7ede7, color: #1a1919
        node0_o5_2[Output $1]
        style node0_o5_2 fill: #d7ede7, color: #1a1919
        node0_o5_3[Output $2]
        style node0_o5_3 fill: #d7ede7, color: #1a1919
    end
    subgraph cluster_3[Block 3]
        nodestart_3[Start]
        style nodestart_3 fill: #e98693, color: #1a1919
        node3_6[Arg $3]
        style node3_6 fill: #d7ede7, color: #1a1919
        node3_7[Arg $4]
        style node3_7 fill: #d7ede7, color: #1a1919
        node3_8[Arg $1]
        style node3_8 fill: #d7ede7, color: #1a1919
        node3_9[Arg $2]
        style node3_9 fill: #d7ede7, color: #1a1919
        node3_10[$5 = $4 < $1 :bool]
        style node3_10 fill: #3cb4a4, color: #1a1919
        node3_11[If]
        style node3_11 fill: #e98693, color: #1a1919
        node3_o12_0[Output $1]
        style node3_o12_0 fill: #d7ede7, color: #1a1919
        node3_o12_1[Output $4]
        style node3_o12_1 fill: #d7ede7, color: #1a1919
        node3_o12_2[Output $3]
        style node3_o12_2 fill: #d7ede7, color: #1a1919
        node3_o12_3[Output $2]
        style node3_o12_3 fill: #d7ede7, color: #1a1919
        node3_o13_0[Output $3]
        style node3_o13_0 fill: #d7ede7, color: #1a1919
    end
subgraph cluster_1[Block 1]
nodestart_1[Start]
style nodestart_1 fill: #e98693, color: #1a1919
node1_14[Arg $1]
style node1_14 fill: #d7ede7, color: #1a1919
node1_15[Arg $4]
style node1_15 fill: #d7ede7, color: #1a1919
node1_16[Arg $3]
style node1_16 fill: #d7ede7, color: #1a1919
node1_17[Arg $2]
style node1_17 fill: #d7ede7, color: #1a1919
node1_18[_ZN8nautilus6engine3addEii]
style node1_18 fill: #da2d4f, color: white
node1_19[1]
style node1_19 fill: #343d46, color: white
node1_20[#plus;]
style node1_20 fill: #3cb4a4, color:#1a1919
node1_21[End]
style node1_21 fill: #e98693, color: #1a1919
node1_o22_0[Output $3]
style node1_o22_0 fill: #d7ede7, color: #1a1919
node1_o22_1[Output $4]
style node1_o22_1 fill: #d7ede7, color: #1a1919
node1_o22_2[Output $1]
style node1_o22_2 fill: #d7ede7, color: #1a1919
node1_o22_3[Output $2]
style node1_o22_3 fill: #d7ede7, color: #1a1919
end
subgraph cluster_2[Block 2]
nodestart_2[Start]
style nodestart_2 fill: #e98693, color: #1a1919
node2_23[Arg $3]
style node2_23 fill: #d7ede7, color: #1a1919
node2_24[return $3 :i32]
style node2_24 fill: #e98693, color: #1a1919
end
node0_2 --> node0_o5_0
linkStyle 0 stroke: #3cb4a4,stroke-width: 1px
node0_3 --> node0_o5_1
linkStyle 1 stroke: #3cb4a4, stroke-width: 1px
node0_0 --> node0_o5_2
linkStyle 2 stroke: #3cb4a4, stroke-width:1px
node0_1 --> node0_o5_3
linkStyle 3 stroke:#3cb4a4, stroke-width: 1px
node3_7-- $4 --> node3_10
linkStyle 4 stroke: #3cb4a4, stroke-width:1px
node3_8-- $1 --> node3_10
linkStyle 5 stroke: #3cb4a4, stroke-width: 1px
node3_10--$5 --> node3_11
linkStyle 6 stroke: #3cb4a4,stroke-width: 1px
node3_8-- false --> node3_o12_0
linkStyle 7 stroke: #3cb4a4, stroke-width: 1px
node3_7-- false --> node3_o12_1
linkStyle 8 stroke:#3cb4a4, stroke-width: 1px
node3_6-- false --> node3_o12_2
linkStyle 9 stroke: #3cb4a4, stroke-width:1px
node3_9-- false --> node3_o12_3
linkStyle 10 stroke: #3cb4a4, stroke-width: 1px
node3_6--true --> node3_o13_0
linkStyle 11 stroke: #3cb4a4,stroke-width: 1px
node1_16-- $3 --> node1_18
linkStyle 12 stroke: #3cb4a4, stroke-width: 1px
node1_17-- $2 --> node1_18
linkStyle 13 stroke:#3cb4a4, stroke-width: 1px
node1_15-- $4 --> node1_20
linkStyle 14 stroke: #3cb4a4, stroke-width:1px
node1_19-- $8 --> node1_20
linkStyle 15 stroke: #3cb4a4, stroke-width: 1px
node1_18 --> node1_o22_0
linkStyle 16 stroke: #3cb4a4, stroke-width:1px
node1_20 --> node1_o22_1
linkStyle 17 stroke:#3cb4a4, stroke-width: 1px
node1_14 --> node1_o22_2
linkStyle 18 stroke: #3cb4a4, stroke-width: 1px
node1_17 --> node1_o22_3
linkStyle 19 stroke: #3cb4a4,stroke-width: 1px
node2_23-- $3 --> node2_24
linkStyle 20 stroke: #3cb4a4, stroke-width: 1px
nodestart_0 --> node0_4
linkStyle 21 stroke: #da2d4f,stroke-width: 2px
node0_4 --> nodestart_3
linkStyle 22 stroke: #da2d4f, stroke-width: 2px
nodestart_3 --> node3_10
linkStyle 23 stroke: #da2d4f, stroke-width:2px
node3_10 --> node3_11
linkStyle 24 stroke:#da2d4f, stroke-width: 2px
node3_11-- true --> nodestart_1
linkStyle 25 stroke: #da2d4f, stroke-width:2px
node3_11-- false --> nodestart_2
linkStyle 26 stroke: #da2d4f, stroke-width: 2px
nodestart_1 --> node1_18
linkStyle 27 stroke: #da2d4f, stroke-width:2px
node1_18 --> node1_20
linkStyle 28 stroke:#da2d4f, stroke-width: 2px
node1_20 --> node1_21
linkStyle 29 stroke: #da2d4f, stroke-width: 2px
node1_21 --> nodestart_3
linkStyle 30 stroke: #da2d4f,stroke-width: 2px
nodestart_2 --> node2_24
linkStyle 31 stroke: #da2d4f, stroke-width: 2px
```