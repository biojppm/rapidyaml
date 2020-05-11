# Changelog

## Changes in master

* 2020/May
  * ***Breaking change***: the error callback now receives a source location object:
    ```c++
    // previous
    using pfn_error = void (*)(const char* msg, size_t msg_len, void *user_data);
    // now:
    using pfn_error = void (*)(const char* msg, size_t msg_len, Location location, void *user_data);
    ```
