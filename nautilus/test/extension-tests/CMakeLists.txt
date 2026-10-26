# The extension registry has process-wide state (it freezes at the first reader), so its tests are a binary of their own
# with a test-only reset, independent of the order of test cases.
add_executable(nautilus-extension-tests
        ExtensionRegistryTest.cpp
        VerifierExtensionTest.cpp
)
target_link_libraries(nautilus-extension-tests PRIVATE nautilus-nir Catch2::Catch2WithMain)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-extension-tests EXTRA_ARGS --allow-running-no-tests)
