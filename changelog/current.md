- Ensure parse errors for invalid YAML cases:
  - [PR#585](https://github.com/biojppm/rapidyaml/pull/585): colon on newline at top level ([play](https://play.yaml.com/?show=xd#c2NhbGFyCjogYmFkCi0tLQpbc2VxXQo6IGJhZAotLS0KW21hcF0KOiBiYWQK)):
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
  - [PR#586](https://github.com/biojppm/rapidyaml/pull/586): colon on newline generally ([play](https://play.yaml.com/?show=xd#c2NhbGFyCjogYmFkCi0tLQpbc2VxXQo6IGJhZAotLS0KW21hcF0KOiBiYWQK)):
    ```yaml
    bad cases:
      scalar
        : bad
      [seq]
        : bad
      {map: }
        : bad
    ```
