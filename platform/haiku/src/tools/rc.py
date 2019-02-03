

import sys
import argparse
import subprocess

def exec_rc(out_path, input_paths):
    # 'rc -o out.rsrc input.rdef'
    args = ['rc', '-o', out_path]
    args.extend(input_paths)
    p = subprocess.Popen(args, stderr=subprocess.PIPE)
    # todo, error
    return p.returncode

def main():
    parser = argparse.ArgumentParser(
                description='Compile rdef file into rsrc file.')
    parser.add_argument('--outpath', 
                help='rsrc file')
    parser.add_argument('inputpath', nargs='+',
                help='rdef files')
    args = parser.parse_args()
    return exec_rc(args.outpath, args.inputpath)

if __name__ == '__main__':
    sys.exit(main())
