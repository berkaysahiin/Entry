local entry = Target.new()

entry.name = "entry"
entry.cxx_flags = {"-std=c++20", "-g", "-DENTRY_VERBOSE"}
entry.sources = collect_sources_recursive("src", {".cpp"})
entry.include_dirs = {"include", "/usr/include/lua5.4", "vendor"}
entry.libraries = {"lua5.4"}

build(entry)
