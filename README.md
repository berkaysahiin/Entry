# Entry

A tiny, intentionally opinionated C++ build system.

## Pitch

Write your build files in C++ (objectively the best language ever). One language for product code and build logic. Clean, direct, and unapologetically C++.

## Look & Feel

`build.cpp` files should read naturally and look good at a glance.

```cpp
// build.cpp
#include "entry.hpp"

using namespace Entry;

int main()
{
	auto target = Target {
		.name = "BuildMe",
		.sources = CollectSources("src", {".cpp"}),
		.includeDirs = {"include"}
	};

	return Build(target);
}
```

```cpp
// build.cpp
#include "entry.hpp"

using namespace Entry;

int main()
{
	auto target = Target {
		.name = "EntryOuroboros",
		.type = TargetType::Executable,
		.flags = {"-std=c++23", "-Wall", "-Wextra", "-Werror", "-O0", "-g"},
		.sources = {"build.cpp"},
	};

	return Build(target);
}
```
