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
  - colon on newline generally in block containers ([PR#585](https://github.com/biojppm/rapidyaml/pull/585)):
    ```yaml
    bad cases:
      scalar
        : bad colon
      [seq]
        : bad colon
      {map: }
        : bad colon
    ```
  - colon on newline in flow sequences ([PR#586](https://github.com/biojppm/rapidyaml/pull/586)):
    ```yaml
    [a
      : 
      b]
    ```
  - tokens after explicit document end ([PR#585](https://github.com/biojppm/rapidyaml/pull/585)):
    ```yaml
    foo: bar
    ... bad tokens
    ```
  - `-` is invalid scalar in flow sequences ([PR#586](https://github.com/biojppm/rapidyaml/pull/586)):
    ```yaml
    [-]
    ---
    [-,-]
    ---
    [--,-]
    ---
    [-
     ]
    ```
  - doc start/begin tokens at zero indentation in seq flow ([PR#587](https://github.com/biojppm/rapidyaml/pull/587)):
    ```yaml
    [
    ---,
    --- ,
    ---\t,
    ...,
    ... ,
    ...\t,
    # etc
    ]
    ```
  - nested flow containers now enforce the contextual parent indentation ([PR#587](https://github.com/biojppm/rapidyaml/pull/587)):
    ```yaml
    - - - [
        a  # now this is a parse error
         ]
    ```
  - single/double-quoted scalars now enforce the contextual parent indentation ([PR#587](https://github.com/biojppm/rapidyaml/pull/587)):
    ```yaml
    - - - "a
        b"  # now this is a parse error
    ```
