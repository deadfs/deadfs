add_executable(deadfs
	 
	main.c
	path.c
	fuse/fuse.c 
	ops/create.c ops/getattr.c ops/readdir.c 
)

target_link_libraries(deadfs osxfuse)