library org.sinkme.misc

TODO
Incorporate the HexOps module into this library.

30 April, 2012
Experiments/lib.org.sinkme.misc
This is a copy of Experiments/src/lib.org.sinkme.misc.  There are 2 development ideas I am experimenting with:

1. Instead of using a structure of:
Experiments
	src
		project1
		project2
	target
		t1
			t1CommonMakery
			project1
			project2
		t2
			t2CommonMakery
			project1
			project2

where Experiments/target/t1/project1/file is a link to Experiments/src/project1/file,
I will try a structure of:
Experiments
	project1
		src
		t1
		t2
	project2
		src
		t1
		t2

where Experiments/project1/t1 contains makery for the t1 platform.  Now, should all t1 platform builds require a common set of make symbols (which is likely), I will need to investigate how those assignments might be factored out of the Experiments/*/t1/makefiles.

2. The second reason to re-org is so that I can try to use "git" for source code control. My idea is that I should be able to maintain a single lib.org.sinkme.misc directory, but git will track a branch for C++ and a separate branch for C.  The experiment will determine how I can retrieve each of these sets for use by a C or C++ user of the code.

Versions and Branches:

I've made a branch called "c++Objects".  I'll be checking in the changes for converting the library to C++ into that branch.  From the docs, it kind of looks like I'm not supposed to keep a branch around.  I'm supposed to do all the work in the branch, then "merge" it back to master, then delete the branch.  However, I'd kind of like to preserve the old C version of the library.

Enter "tags".  It looks like I should use git "tag"s to identify a particular version of a project.  So, I'll use the 1.x.x tags to identify the C version of the org.sinkme.misc library, and used the 2.x.x tags to identify the C++ version of the org.sinkme.misc library.

