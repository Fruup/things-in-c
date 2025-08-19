#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

void append_c_files_of_directory(Cmd *cmd, const char *directory, bool include_main)
{
	if (!file_exists(directory))
	{
		nob_log(ERROR, "Directory '%s' does not exist", directory);
		exit(1);
	}

	File_Paths children = {0};
	nob_read_entire_dir(directory, &children);

	for (size_t i = 0; i < children.count; i++)
	{
		String_View sv = sv_from_cstr(children.items[i]);

		if (!sv_end_with(sv, ".c"))
			continue;

		if (!include_main && sv_end_with(sv, "main.c"))
			continue;

		String_Builder sb = {0};
		sb_append_cstr(&sb, directory);
		sb_append_cstr(&sb, "/");

		const char *file_name = temp_sv_to_cstr(sv);
		sb_append_cstr(&sb, file_name);

		String_View sv2 = sb_to_sv(sb);
		cmd_append(cmd, temp_sv_to_cstr(sv2));
	}
}

int main(int argc, char **argv)
{
	NOB_GO_REBUILD_URSELF(argc, argv);

	if (argc < 2)
	{
		nob_log(ERROR, "No input project specified");
		return 1;
	}

	char **input_projects = argv + 1;
	int input_projects_count = argc - 1;

	mkdir_if_not_exists("build");

	Cmd cmd = {0};

	char *out_file;
	{
		String_Builder sb = {0};
		sb_append_cstr(&sb, "build/");
		sb_append_cstr(&sb, input_projects[0]);

		String_View sv = sb_to_sv(sb);
		out_file = (char *)temp_sv_to_cstr(sv);
	}

	{
		cmd_append(&cmd, "clang", "-Wno-initializer-overrides", "-o", out_file);

		for (int i = 0; i < input_projects_count; i++)
			append_c_files_of_directory(&cmd, input_projects[i], i == 0);

		if (!cmd_run(&cmd))
			return 1;
	}

	{
		cmd_append(&cmd, out_file);
		if (!cmd_run(&cmd))
			return 1;
	}

	return 0;
}
