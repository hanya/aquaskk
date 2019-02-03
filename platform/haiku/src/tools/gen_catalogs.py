

import sys
import argparse
import subprocess
import os
from os.path import abspath, basename, join, splitext


def exec_linkcatkeys(sig, out_path, input_paths):
    # 'linkcatkeys -o lang.catalog -s sig -l lang lang.catkeys'
    try:
        os.mkdir(out_path)
    except:
        pass
    for input_path in input_paths:
        filename = basename(input_path)
        name, ext = splitext(filename)
        if ext != '.catkeys':
            continue
        locale = name
        args = ['linkcatkeys', '-s', sig, '-l', locale,
                '-o', join(out_path, locale + '.catalog'),
                join(input_path)]
        p = subprocess.Popen(args)
        p.wait()
    return 0


def main():
    parser = argparse.ArgumentParser(
                description='Compile rdef file into rsrc file.')
    parser.add_argument('--sig',
                help='Application mime-type signature')
    parser.add_argument('--inputpath', nargs='+',
                help='Directory contains .catkeys')
    parser.add_argument('--outpath',
                help='rsrc file')
    args = parser.parse_args()
    return exec_linkcatkeys(args.sig, abspath(args.outpath), map(abspath, args.inputpath))


if __name__ == '__main__':
    sys.exit(main())
