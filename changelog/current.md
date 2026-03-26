- [PR#585](https://github.com/biojppm/rapidyaml/pull/585): ensure parse errors for invalid YAML cases:
  - colon on newline at top level ([play](https://play.yaml.com/?show=xd#c2NhbGFyCjogYmFkCi0tLQpbc2VxXQo6IGJhZAotLS0KW21hcF0KOiBiYWQK)):
    ```yaml
    scalar
    : bad
    ---
    [seq]
    : bad
    ---
    [map]
    : bad
    ```
