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

2. The second reason to reorg is so that I can try to use "git" for source code control
