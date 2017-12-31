add_executable(deadfs
	 
	main.c
	path.c
	fuse/fuse.c 
	ops/create.c ops/getattr.c ops/readdir.c 
	ops/write.c
	ops/read.c
	ops/truncate.c
	ops/rmdir.c
	ops/mkdir.c
	ops/unlink.c
	ops/rename.c
	
	utils/base32.c
	
)

target_link_libraries(deadfs osxfuse)