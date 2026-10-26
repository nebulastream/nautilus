# NIR conformance material

Inputs and expected results that every implementation of NIR 1.0 must agree on
(`docs/plans/nautilus-0.4/13-nir-spec.md` §11). They are read by `nautilus-nir-tests` and, for the text files, run one
by one by ctest through `nautilus-nir-opt`.

| Path | Content |
|---|---|
| `text/*.nir` | valid modules in the printer's canonical form: `print(parse(t)) == t`. Together they use every core opcode, every table, both levels, landing pads and every constant spelling |
| `text/invalid/*.nir` | modules that must be rejected; the first line is `; expect: NIR-XNNN`, the code the parser or verifier reports |
| `images/valid/*.bin` | canonical images of the non-process-local texts (pre-SSA ones are written as they are); `images/valid.txt` lists each with its structural hash (default options) |
| `images/invalid/*.bin` | damaged images; `images/invalid.txt` lists the `NIR-L` code the loader answers with |
| `images/example2.hexdump.txt` | the second example of §9, canonicalized: its structural hash and an annotated hex dump of the image, checked byte for byte |

The golden images change only with the format. After an intentional change (a new minor version, §8.2), regenerate them
with `NIR_UPDATE_GOLDEN=1 nautilus-nir-tests` and review the diff.

Not covered yet: `NIR-L010`-`L012` and `NIR-L015` (they need the extension registry and the pointer resolver, M1.7),
`NIR-L008` (limits are options, tested in `ImageTest.cpp`), `NIR-L013` (reserved), the extension productions and
`NIR-P005` of the text form (M1.3a), and the verifier codes `NIR-V070`-`V072` (M1.7). Most verifier codes have their
positive and negative tests in `nautilus/test/nir-tests/Verifier*Test.cpp`; only a selection have a text file here.
