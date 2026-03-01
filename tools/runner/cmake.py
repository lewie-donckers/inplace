class CMake:
    def __init__(
        self,
        *,
        source_path,
        build_path,
        generator="Ninja",
        cxx_compiler=None,
        build_type=None,
        toolchain=None,
        variables={},
    ):
        self._source_path = source_path
        self._build_path = build_path
        self._generator = generator
        self._cxx_compiler = cxx_compiler
        self._build_type = build_type
        self._toolchain = toolchain
        self._variables = variables

    def generate_command(self, *, extra_arguments=[]):
        return (
            [
                "cmake",
                "-S",
                self._source_path,
                "-B",
                self._build_path,
                "-G",
                self._generator,
            ]
            + self._optional_generate_arguments()
            + [f"-D{k}={v}" for k, v in self._variables.items()]
            + extra_arguments
        )

    def build_command(self, *, targets=[], extra_arguments=[]):
        return (
            ["cmake", "--build", self._build_path]
            + (["--target"] + targets if targets else [])
            + extra_arguments
        )

    def _optional_generate_arguments(self):
        result = []
        if self._cxx_compiler is not None:
            result.append(f"-DCMAKE_CXX_COMPILER={self._cxx_compiler}")
        if self._build_type is not None:
            result.append(f"-DCMAKE_BUILD_TYPE={self._build_type}")
        if self._toolchain is not None:
            result.extend(["--toolchain", self._toolchain])
        return result
