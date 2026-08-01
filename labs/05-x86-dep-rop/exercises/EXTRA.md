# Extra — ROP lab

## E1 — Compare lab 01 vs 05

Same offset? Same crash type? Document execute-vs-read faults.

## E2 — Bad chars with ROP

Which bytes break gadget addresses? Re-run mona with `-cpb`.

## E3 — ASLR thought experiment

If dynamic base were on, which of your Part A addresses die? What primitive
would you need next (leak)?

## E4 — Linux NX twin

Rebuild lab 03 **without** `-z execstack` and try ret2win against a `win()`
you add — or use lab 03 exercises E1.
