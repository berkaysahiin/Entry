#include "../../entry/entry.hpp" // IWYU pragma: keep

int main(ENTRY_MAYBE_UNUSED int argc, ENTRY_MAYBE_UNUSED char** argv)
{
	Entry::Target target;
	target.name = "Hello_World";
	target.sources = {"main.cpp"};

	Entry::Build(target);
}
