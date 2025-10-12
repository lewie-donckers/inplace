import __main__
import os
import subprocess

from .announce import announce


def execute_command(command, *, cwd=None, announcer=announce.announce):
    announcer(f"running command: {" ".join(command)}")

    try:
        subprocess.run(command, cwd=cwd, check=True, encoding="utf-8")
    except subprocess.CalledProcessError as e:
        raise Exception(
            f"command failed with exit code {e.returncode}: {" ".join(command)}"
        )


def execute_command_in_docker(docker_image, command):
    uid = os.getuid()
    gid = os.getgid()
    pwd = os.path.dirname(os.path.abspath(__main__.__file__))

    announce.alt(f"restarting in Docker image {docker_image}")
    execute_command(
        [
            "docker",
            "run",
            "-it",
            "--rm",
            "--user",
            "{}:{}".format(uid, gid),
            "-e",
            "HOME=/home/user",
            "--workdir={}".format(pwd),
            "--volume={}:{}:rw".format(pwd, pwd),
            "--volume=/etc/group:/etc/group:ro",
            "--volume=/etc/passwd:/etc/passwd:ro",
            "--volume=/etc/shadow:/etc/shadow:ro",
            docker_image,
        ]
        + command,
        announcer=announce.alt,
    )
