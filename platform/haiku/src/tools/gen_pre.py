

import sys
import argparse
import subprocess

def gen_pre(parser, includes, outpath, input_paths):
    inc = ["-I" + i for i in includes]
    data = []
    args = [parser, '-E', '-x', 'c++', 
            '-DB_COLLECTING_CATKEYS', '-DOS_LINUX', '-DOS_HAIKU']
    args.extend(inc)
    args.extend(input_paths)
    p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if p.poll() is None:
        out = p.stdout.read()
        data.extend([line for line in out.split("\n") 
                        if not line.startswith("#")])
    else:
        print(p.stderr.read())
        return -1
    with open(outpath, "w") as f:
        f.write("\n".join(data))
    return 0


def main():
    parser = argparse.ArgumentParser(
                description='Generates pre file from B_TRANSLATE.')
    parser.add_argument('--parser',
                help='C compiler')
    parser.add_argument('--outpath', 
                help='pre file')
    parser.add_argument('--includes', nargs='+',
                help='Include dirs')
    parser.add_argument('--inputpath', nargs='+',
                help='C++ files to be extracted')
    args = parser.parse_args()
    return gen_pre(args.parser, args.includes, args.outpath, args.inputpath)


if __name__ == '__main__':
    sys.exit(main())
