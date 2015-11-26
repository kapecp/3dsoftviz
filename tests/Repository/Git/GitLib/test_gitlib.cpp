#include <igloo/igloo_alt.h>
using namespace igloo;

#include "GitLib/GitFile.h"

Describe(a_git_file)
{
	It(should_be_NONE)
	{
		Assert::That(file.getType(), Equals(Repository::Git::GitType::MODIFIED));
	}

	Repository::Git::GitFile file;
};

int main(int argc, char *argv[])
{
	return TestRunner::RunAllTests(argc, argv);
}
