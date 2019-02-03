

import argparse
import glob
import os
import sys
import subprocess


ROOT = os.path.abspath(os.path.dirname(__file__))
SRC_DIR = ROOT


def GetBaseName():
    return "haiku"


def GetBuildDirName(platform):
    name = {
        'Haiku': 'out_haiku',
    }.get(platform, None)
    if name is None:
        raise RuntimeError('Unknown platform: ' + platform)
    return name


def GetBuildDirPath(platform):
    return os.path.join(ROOT, GetBuildDirName(platform))


def RunOrRaise(args):
    p = subprocess.Popen(args)
    p.wait()


def GetGypFileNames():
    gyp_file_names = []
    gyp_file_names.extend(glob.glob("{}/method/*.gyp".format(SRC_DIR)))
    gyp_file_names.extend(glob.glob("{}/preference/*.gyp".format(SRC_DIR)))
    gyp_file_names.extend(glob.glob("{}/gyp/test.gyp".format(SRC_DIR)))
    gyp_file_names.sort()
    return gyp_file_names


def GYPArgParse(args):
    parser = argparse.ArgumentParser()
    parser.add_argument('--platform', dest='platform',
                        help='sets target platform')
    parser.add_argument('--arch', dest='arch', default='',
                        help='sets target architecture')
    return parser.parse_args(args)


def BuildArgParse(args):
    parser = argparse.ArgumentParser()
    parser.add_argument('--configuration', '-c', dest='configuration',
                        default='Debug', help='specify the build configuration')
    parser.add_argument('targets', nargs='*',
                        default=[],
                        help='targets to build')
    return parser.parse_args(args)


def CleanArgParse(args):
    parser = argparse.ArgumentParser()
    #parser.add_argument()
    return parser.parse_args(args)


def GYPMain(args):
    gyp_command = ['gyp']
    os.environ['GYP_GENERATORS'] = 'ninja'
    target = GetBuildDirPath(args.platform)

    gyp_options = []

    gyp_options.extend(['--include={}/gyp/common.gypi'.format(SRC_DIR)])
    gyp_options.extend(['-D', 'platform={}'.format(args.platform)])
    gyp_options.extend(['-D', 'arch={}'.format(args.arch)])
    gyp_options.extend(['-D', 'build_base={}'.format(target)])
    gyp_options.extend(['--generator-output=.'])
    gyp_options.extend(['-G', 'output_dir={}'.format(target)])
    gyp_options.extend(GetGypFileNames())

    RunOrRaise(gyp_command + gyp_options)


def BuildMain(args):
    basename = GetBaseName()
    build_config = 'out_{}/{}'.format(basename, args.configuration)
    RunOrRaise(['ninja', '-C', build_config])


def CleanMain(args):
    pass


def PrintUsage():
    usage = """Compile or generates compile scripts.
    Usage:
        python build.py command [arguments]
    command can be one of:
        gyp
        build
        clean
"""
    print(usage)


def main():
    command = sys.argv[1]
    args = sys.argv[2:]

    if command == 'gyp':
        GYPMain(GYPArgParse(args))
    elif command == 'build':
        BuildMain(BuildArgParse(args))
    elif command == 'clean':
        CleanMain(CleanArgParse(args))
    else:
        PrintUsage()


if __name__ == '__main__':
    main()
