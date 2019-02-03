

import os
import stat
import sys
import argparse
import subprocess
import shutil

def exec_xres(input_path, out_path, rsrc, make_executable=False):
    # copy 
    shutil.copyfile(input_path, out_path)
    
    # merge
    # 'xres -o out input.rsrc'
    args = ['xres', '-o', out_path, rsrc]
    p = subprocess.Popen(args, stderr=subprocess.PIPE)
    p.wait()
    if p.returncode == 0 and make_executable:
        os.chmod(out_path,
            os.stat(out_path).st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
    return p.returncode


def main():
    parser = argparse.ArgumentParser(
                description='Merge rsrc file into executable file.')
    parser.add_argument('--outpath', 
                help='executable file with resource')
    parser.add_argument('--inputpath',
                help='input executable file')
    parser.add_argument('--rsrc',
                help='rsrc file')
    parser.add_argument('--exe', action='store_true', default=False,
                help='set executable mode')
    args = parser.parse_args()
    return exec_xres(args.inputpath, args.outpath, args.rsrc, args.exe)

if __name__ == '__main__':
    sys.exit(main())
