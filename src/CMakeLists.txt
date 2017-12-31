add_executable(deadfs
	 
	main.c
	path.c
	base32.c
	fuse/fuse.c 
	ops/create.c ops/getattr.c ops/readdir.c 
	ops/write.c
	ops/read.c
	ops/truncate.c
)

target_link_libraries(deadfs osxfuse)