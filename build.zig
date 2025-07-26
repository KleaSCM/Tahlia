const std = @import("std");

pub fn build(b: *std.Build) void {
    // Create a custom step that runs zig c++ directly
    const compile_step = b.addSystemCommand(&.{ "zig", "c++", "-std=c++17", "-Wall", "-Wextra", "-I", "include", "src/main.cpp", "src/core/audit.cpp", "src/core/asset_manager.cpp", "src/core/asset_indexer.cpp", "-o", "zig-out/bin/blender_asset_manager" });

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

    // Add test build (Catch2 must be available)
    const test_compile = b.addSystemCommand(&.{ "zig", "c++", "-std=c++17", "-Wall", "-Wextra", "-I", "include", "src/core/audit.cpp", "Tests/test_audit.cpp", "-o", "zig-out/bin/test_audit", "-lcatch2" });
    test_compile.step.dependOn(&mkdir_step.step);

    const test_build_step = b.step("build-test-audit", "Build the audit tests");
    test_build_step.dependOn(&test_compile.step);
}
