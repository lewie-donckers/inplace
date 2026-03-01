class ClangTidy:
    def __init__(self, *, executable="clang-tidy", build_path=None):
        self._executable = executable
        self._build_path = build_path

    def check_command(self, *, files, extra_arguments=[]):
        return (
            [self._executable]
            + (["-p", self._build_path] if self._build_path is not None else [])
            + extra_arguments
            + files
        )
