#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

int main(int argc, char **argv)
{
	NOB_GO_REBUILD_URSELF(argc, argv);

	if (argc < 2)
	{
		nob_log(ERROR, "No input project specified");
		return 1;
	}

	const char *input_project = argv[1];
	if (!file_exists(input_project))
	{
		nob_log(ERROR, "Input project '%s' does not exist", input_project);
		return 1;
	}

	mkdir_if_not_exists("build");

	Cmd cmd = {0};

	char *out_file;
	{
		String_Builder sb = {0};
		sb_append_cstr(&sb, "build/");
		sb_append_cstr(&sb, input_project);

		String_View sv = sb_to_sv(sb);
		out_file = (char *)temp_sv_to_cstr(sv);
	}

	{
		File_Paths children = {0};
		nob_read_entire_dir(input_project, &children);

		cmd_append(&cmd, "clang", "-o", out_file);

		// Compile each child .c file
		for (size_t i = 0; i < children.count; i++)
		{
			String_View sv = sv_from_cstr(children.items[i]);

			if (!sv_end_with(sv, ".c"))
				continue;

			String_Builder sb = {0};
			sb_append_cstr(&sb, input_project);
			sb_append_cstr(&sb, "/");

			const char *file_name = temp_sv_to_cstr(sv);
			sb_append_cstr(&sb, file_name);

			String_View sv2 = sb_to_sv(sb);
			cmd_append(&cmd, temp_sv_to_cstr(sv2));
		}

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
