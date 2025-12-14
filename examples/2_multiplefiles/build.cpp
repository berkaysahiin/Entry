#include "../../entry/entry.hpp" // IWYU pragma: keep

int main(ENTRY_MAYBE_UNUSED int argc, ENTRY_MAYBE_UNUSED char** argv)
{
	Entry::Target target;
	target.name = "MultipleFiles";
	target.sources = Entry::CollectSources("src", {".cpp"});
	target.includeDirs = {"include"};

	Entry::Build(target);
}
