# Nautilus IR Graphs:

Nautilus support the vizualization of its intermediate representation using Graphviz or Marimaid:

[![](https://mermaid.ink/img/pako:eNqNklFrwjAUhf9KiD4qNKnMNQ-Dga976t7skNjctsE0KWmCE_G_L62z00nRp5t7c87HCblHnBsBmOFCmX1ecevQ5yrTCLV-W1reVChXvnVgNxHqxkgHeeuCbhOt065-9Wp3UHB9hwqpFJtA8vqSxLPcKGPZhHCSkGTARAHxbks0JegfJBoAYgkClqMAstZQ3pnJrznOtwu-GDXTtQXnbRhMY4SYjOkdiT54B2jRlUvm-fztEqEbK6l3aU-LAtWaHQyZzu18L4WrGGm-_yjkikJvKeRZyuUTRvPQgSQ4FYvilkSfzBM_ouAZrsHWXIqwYcfOnGFXQQ0ZZuEooOBeuQxn-hSk3DuTHnSOmbMeZtgaX1aYFVy1ofON4A5Wkoe1rIcpCOmM_TjvcL_Kpx-_cuzO?type=png)](https://mermaid.live/edit#pako:eNqNklFrwjAUhf9KiD4qNKnMNQ-Dga976t7skNjctsE0KWmCE_G_L62z00nRp5t7c87HCblHnBsBmOFCmX1ecevQ5yrTCLV-W1reVChXvnVgNxHqxkgHeeuCbhOt065-9Wp3UHB9hwqpFJtA8vqSxLPcKGPZhHCSkGTARAHxbks0JegfJBoAYgkClqMAstZQ3pnJrznOtwu-GDXTtQXnbRhMY4SYjOkdiT54B2jRlUvm-fztEqEbK6l3aU-LAtWaHQyZzu18L4WrGGm-_yjkikJvKeRZyuUTRvPQgSQ4FYvilkSfzBM_ouAZrsHWXIqwYcfOnGFXQQ0ZZuEooOBeuQxn-hSk3DuTHnSOmbMeZtgaX1aYFVy1ofON4A5Wkoe1rIcpCOmM_TjvcL_Kpx-_cuzO)

```mermaid
  graph TD;
      A-->B;
      A-->C;
      B-->D;
      C-->D;
```
```mairmaid
graph TD;
  subgraph cluster_0;
   nodestart_0[Start];
  style nodestart_0 fill:#e98693,color:#1a1919;
   node0_0[Arg $1 ];
  style node0_0 fill:#d7ede7,color:#1a1919;
   node0_1[neg];
  style node0_1 fill:#3cb4a4,color:#1a1919;
   node0_2[return  $3  :i32];
  style node0_2 fill:#e98693,color:#1a1919;
  end;
  node0_0 --> node0_1;
  linkStyle 0 stroke:#3cb4a4,stroke-width:1px;
  node0_1 --> node0_2;
  linkStyle 1 stroke:#3cb4a4,stroke-width:1px;
  nodestart_0 --> node0_1;
  linkStyle 2 stroke:#da2d4f,stroke-width:2px;
  node0_1 --> node0_2;
  linkStyle 3 stroke:#da2d4f,stroke-width:2px;
```