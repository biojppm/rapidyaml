- Ensure parse errors for invalid YAML cases:
  - colon on newline at top level ([PR#585](https://github.com/biojppm/rapidyaml/pull/585)):
    ```yaml
    scalar
    : bad
    ---
    [seq]
    : bad
    ---
    {map: }
    : bad
    ```
  - colon on newline generally ([PR#585](https://github.com/biojppm/rapidyaml/pull/585)):
    ```yaml
    bad cases:
      scalar
        : bad colon
      [seq]
        : bad colon
      {map: }
        : bad colon
    ```
  - tokens after explicit document end ([PR#585](https://github.com/biojppm/rapidyaml/pull/585)):
    ```yaml
    foo: bar
    ... bad tokens
    ```
  - comments directly after comma in flow containers ([PR#586](https://github.com/biojppm/rapidyaml/pull/586)):
    ```yaml
    [a,b,# bad comment
     ]
    ---
    {a: b,# bad comment
     }
    ```
