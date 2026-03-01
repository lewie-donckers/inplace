class ClangFormat:
    def __init__(self, *, executable="clang-format"):
        self._executable = executable

    def format_command(self, *, files, extra_arguments=[]):
        return self._command(False, files, extra_arguments)

    def check_command(self, *, files, extra_arguments=[]):
        return self._command(True, files, extra_arguments)

    def _command(self, check, files, extra_arguments):
        return (
            [self._executable, "--verbose"]
            + (
                ["--Werror", "--Wclang-format-violations", "--dry-run"]
                if check
                else ["-i"]
            )
            + extra_arguments
            + files
        )
