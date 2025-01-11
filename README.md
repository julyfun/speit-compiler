## cmd

```fish
gcc -Wall test_analex.c analex.c vector.c -o 1 && ./1
gcc -Wall anasynt.c analex.c vector.c test_anasynt.c -o 1 && ./1
```

## TODO

```
⇒	U+21D2
→	U+2192
¬	U+00AC
∧	U+2227
∨	U+2228
```

## BNF

```
<digit> ::= "0" | ... | "9"
<letter> ::= "a" | ... | "z"
<id> ::= <lettre> { <lettre> | <chiffre> }

<primary_expr> ::= <id> | "(" <arrow_expr> ")"
<unary_expr> ::= <primary_expr> | ¬ <unary_expr>
<and_expr> ::= <unary_expr> | <unary_expr> ∧ <and_expr>
<or_expr> ::= <and_expr> | <and_expr> ∨ <or_expr>
<arrow_expr> ::= <or_expr> | <or_expr> ⇒ <arrow_expr>
<regle> ::= <arrow_expr> → <arrow_expr>
<expr> ::= <regle> | <arrow_expr>
```

## 困难
