- [PR#615](https://github.com/biojppm/rapidyaml/pull/615): add flow style customization:
  - Add `NodeType_e::FLOW_SPC` to force space after comma in flow style containers:
    ```c++
    Tree tree = parse_in_arena("[0,1,2,3,4,5,6]");
    CHECK(emitrs_yaml<std::string>(tree), "[0,1,2,3,4,5,6]");
    tree.rootref().set_container_style(FLOW_SL|FLOW_SPC); // add spaces
    CHECK(emitrs_yaml<std::string>(tree), "[0, 1, 2, 3, 4, 5, 6]");
    ```
  - Add `FLOW_MLN` (flow multiline, multi-values per line, wrapped at max columns). The old `FLOW_ML` was deprecated, now known as `FLOW_ML1` (flow multiline, 1 value per line):
    ```c++
    tree.rootref().set_container_style(FLOW_ML1);
    CHECK(emitrs_yaml<std::string>(tree),
          "[\n"
          "  0,\n"
          "  1,\n"
          "  2,\n"
          "  3,\n"
          "  4,\n"
          "  5,\n"
          "  6\n"
          "]\n");
    tree.rootref().set_container_style(FLOW_MLN);
    CHECK(emitrs_yaml<std::string>(tree),
          "[\n"
          "  0,1,2,3,4,5,6\n"
          "]\n");
    ```
  - Add `FLOW_MLX` mask to match both `FLOW_ML1` and `FLOW_MLN`
  - Add `EmitOptions::max_cols()` to control wrapping of `FLOW_MLN`:
    ```c++
    CHECK(emitrs_yaml<std::string>(tree, EmitOptions{}.max_cols(8)),
          "[\n"
          "  0,1,2,\n"
          "  3,4,5,\n"
          "  6\n"
          "]\n");
    ```
  - Add `EmitOptions::force_flow_spc()` to override all per-node settings of `FLOW_SPC` when emitting:
    ```c++
    CHECK(emitrs_yaml<std::string>(tree, EmitOptions{}.force_flow_spc(true)),
          "[\n"
          "  0, 1, 2, 3, 4, 5, 6\n"
          "]\n");
    ```
  - Add `ParserOptions::flow_ml_style()` to choose the multiline flow style to assign while parsing. The default value is still `FLOW_ML1` (the old behavior), but this option allows picking any of `FLOW_MLN` or `FLOW_MLN`.
  - See `sample_style_flow_formatting()` in the quickstart for more info.
