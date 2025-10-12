import argparse
import sys

try:
    import argcomplete

    use_argcomplete = True
except:
    use_argcomplete = False

from .announce import announce


class Runner:
    def __init__(self):
        self._parser = argparse.ArgumentParser()
        self._subparsers = self._parser.add_subparsers(
            title="actions",
            required=True,
        )

    def register_action(self, action_class):
        action = action_class()
        subparser = self._subparsers.add_parser(
            action.name, help=action.help, description=action.help
        )
        action._setup_parser(subparser)
        subparser.set_defaults(action=action)

    def run(self):
        if use_argcomplete:
            argcomplete.autocomplete(self._parser)
        args = self._parser.parse_args()
        args.action._run(**vars(args))


def run_with_actions(actions):
    try:
        runner = Runner()
        for action in actions:
            runner.register_action(action)
        runner.run()
    except KeyboardInterrupt:
        announce.error("keyboard interrupt received")
        sys.exit(2)
    except Exception as e:
        announce.error("error: {}".format(str(e)))
        sys.exit(1)
