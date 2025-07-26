const std = @import("std");

pub fn build(b: *std.Build) void {
    // Create a custom step that runs zig c++ directly
    const compile_step = b.addSystemCommand(&.{ "zig", "c++", "-std=c++17", "-Wall", "-Wextra", "-I", "include", "src/main.cpp", "src/core/audit.cpp", "src/core/asset_manager.cpp", "src/core/asset_indexer.cpp", "src/core/asset_validator.cpp", "src/core/import_manager.cpp", "src/core/material_manager.cpp", "-o", "zig-out/bin/blender_asset_manager" });

    // Make sure the output directory exists
    const mkdir_step = b.addSystemCommand(&.{ "mkdir", "-p", "zig-out/bin" });
    compile_step.step.dependOn(&mkdir_step.step);

    // Run step
    const run_cmd = b.addSystemCommand(&.{"zig-out/bin/blender_asset_manager"});
    run_cmd.step.dependOn(&compile_step.step);
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the application");
    run_step.dependOn(&run_cmd.step);

    const build_step = b.step("build", "Build the application");
    build_step.dependOn(&compile_step.step);

    // Add ImportManager test build (using simple test harness)
    const import_test_compile = b.addSystemCommand(&.{ "zig", "c++", "-std=c++17", "-Wall", "-Wextra", "-I", "include", "-I", "Tests", "src/core/import_manager.cpp", "src/core/asset_manager.cpp", "src/core/asset_indexer.cpp", "src/core/material_manager.cpp", "Tests/test_import_manager.cpp", "-o", "zig-out/bin/test_import_manager" });

    // Add ImportHistory test build
    const history_test_compile = b.addSystemCommand(&.{ "zig", "c++", "-std=c++17", "-Wall", "-Wextra", "-I", "include", "-I", "Tests", "src/core/import_history.cpp", "Tests/test_import_history.cpp", "-o", "zig-out/bin/test_import_history" });
    history_test_compile.step.dependOn(&mkdir_step.step);

    const history_test_build_step = b.step("build-test-history", "Build the import history tests");
    history_test_build_step.dependOn(&history_test_compile.step);

    // Add run test step
    const run_history_test = b.addSystemCommand(&.{"zig-out/bin/test_import_history"});
    run_history_test.step.dependOn(&history_test_compile.step);

    const run_history_test_step = b.step("run-test-history", "Run the import history tests");
    run_history_test_step.dependOn(&run_history_test.step);

    // Add PythonBridge test build (without Python - universal mode)
    const python_bridge_test_compile = b.addSystemCommand(&.{ "zig", "c++", "-std=c++17", "-Wall", "-Wextra", "-I", "include", "-I", "Tests", "src/core/python_bridge.cpp", "src/core/asset_manager.cpp", "src/core/asset_indexer.cpp", "src/core/import_manager.cpp", "src/core/material_manager.cpp", "src/core/import_history.cpp", "Tests/test_python_bridge.cpp", "-o", "zig-out/bin/test_python_bridge" });

    // Add PythonBridge test build (with Python - optional)
    const python_bridge_test_compile_with_python = b.addSystemCommand(&.{ "zig", "c++", "-std=c++17", "-Wall", "-Wextra", "-I", "include", "-I", "Tests", "-I", "/usr/include/python3.13", "-lpython3.13", "-DTAHLIA_ENABLE_PYTHON", "src/core/python_bridge.cpp", "src/core/asset_manager.cpp", "src/core/asset_indexer.cpp", "src/core/import_manager.cpp", "src/core/material_manager.cpp", "src/core/import_history.cpp", "Tests/test_python_bridge.cpp", "-o", "zig-out/bin/test_python_bridge_with_python" });
    python_bridge_test_compile.step.dependOn(&mkdir_step.step);
    python_bridge_test_compile_with_python.step.dependOn(&mkdir_step.step);

    const python_bridge_test_build_step = b.step("build-test-python-bridge", "Build the python bridge tests (universal)");
    python_bridge_test_build_step.dependOn(&python_bridge_test_compile.step);

    const python_bridge_test_build_step_with_python = b.step("build-test-python-bridge-with-python", "Build the python bridge tests (with Python)");
    python_bridge_test_build_step_with_python.dependOn(&python_bridge_test_compile_with_python.step);

    // Add run test step
    const run_python_bridge_test = b.addSystemCommand(&.{"zig-out/bin/test_python_bridge"});
    run_python_bridge_test.step.dependOn(&python_bridge_test_compile.step);

    const run_python_bridge_test_step = b.step("run-test-python-bridge", "Run the python bridge tests (universal)");
    run_python_bridge_test_step.dependOn(&run_python_bridge_test.step);

    const run_python_bridge_test_with_python = b.addSystemCommand(&.{"zig-out/bin/test_python_bridge_with_python"});
    run_python_bridge_test_with_python.step.dependOn(&python_bridge_test_compile_with_python.step);

    const run_python_bridge_test_with_python_step = b.step("run-test-python-bridge-with-python", "Run the python bridge tests (with Python)");
    run_python_bridge_test_with_python_step.dependOn(&run_python_bridge_test_with_python.step);

    import_test_compile.step.dependOn(&mkdir_step.step);

    const import_test_build_step = b.step("build-test-import", "Build the import manager tests");
    import_test_build_step.dependOn(&import_test_compile.step);

    // Add run test step
    const run_import_test = b.addSystemCommand(&.{"zig-out/bin/test_import_manager"});
    run_import_test.step.dependOn(&import_test_compile.step);

    const run_import_test_step = b.step("run-test-import", "Run the import manager tests");
    run_import_test_step.dependOn(&run_import_test.step);

    // Add MaterialManager test build
    const material_test_compile = b.addSystemCommand(&.{ "zig", "c++", "-std=c++17", "-Wall", "-Wextra", "-I", "include", "-I", "Tests", "src/core/material_manager.cpp", "src/core/asset_manager.cpp", "src/core/asset_indexer.cpp", "src/core/import_manager.cpp", "Tests/test_material_manager.cpp", "-o", "zig-out/bin/test_material_manager" });
    material_test_compile.step.dependOn(&mkdir_step.step);

    const material_test_build_step = b.step("build-test-material", "Build the material manager tests");
    material_test_build_step.dependOn(&material_test_compile.step);

    // Add run test step
    const run_material_test = b.addSystemCommand(&.{"zig-out/bin/test_material_manager"});
    run_material_test.step.dependOn(&material_test_compile.step);

    const run_material_test_step = b.step("run-test-material", "Run the material manager tests");
    run_material_test_step.dependOn(&run_material_test.step);
}
