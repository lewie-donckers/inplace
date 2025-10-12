import argparse
import base64
import pickle
import typing

from .execute_command import execute_command_in_docker


class Action:
    def __init__(
        self, child, name, *, help=None, restart_support=False, restart_image=None
    ):
        self._child = child
        self.name = name
        self.help = help
        self._restart_support = restart_support
        self._restart_image = restart_image

    def run_restart(self, *args, **kwargs):
        if self._restart_image is None:
            raise RuntimeError(
                f"run_restart() not implememted and restart_image not specified for action {self.name}"
            )
        self.restart_in_docker(self._restart_image, *args, **kwargs)

    def run(self, *args, **kwargs):
        raise RuntimeError(f"run() not implemented for action {self.name}")

    def setup_parser(self, parser):
        pass

    @typing.final
    def restart_in_docker(self, image, *args, **kwargs):
        execute_command_in_docker(
            image,
            [
                "python3",
                "run",
                self.name,
                "--restarted",
                self._encode(*args, **kwargs),
            ],
        )

    @typing.final
    def _setup_parser(self, parser):
        self.setup_parser(parser)
        if self._restart_support:
            parser.add_argument("--restarted", help=argparse.SUPPRESS)
            parser.add_argument(
                "--here",
                action="store_true",
                help="run this action here; do not automatically restart it in Docker",
            )

    @typing.final
    def _run(self, **args):
        restarted = args.pop("restarted", None)
        here = args.pop("here", None)
        args.pop("action", None)

        if self._restart_support:
            if restarted is not None:
                here_args, here_kwargs = self._decode(restarted)
                return self.run(*here_args, **here_kwargs)
            elif here:
                return self.run(**args)
            return self.run_restart(**args)

        self.run(**args)

    @typing.final
    def _encode(self, *args, **kwargs):
        decoded = (args, kwargs)
        encoded = pickle.dumps(decoded)
        encoded = base64.urlsafe_b64encode(encoded)
        string = encoded.decode(encoding="utf-8")
        return string

    @typing.final
    def _decode(self, string):
        encoded = bytes(string, encoding="utf-8")
        decoded = base64.urlsafe_b64decode(encoded)
        args, kwargs = pickle.loads(decoded)
        return (args, kwargs)
