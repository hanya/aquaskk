

import sys
import argparse
import subprocess


def exec_compiler(compiler, out_path, input_path, defs, includes, link_libs):
    args = [compiler, input_path, '-o', out_path]
    args.extend(["-I" + i for i in includes])
    args.extend(["-D" + d for d in defs])

    libs = []
    for lib in link_libs:
        if lib.startswith("/"):
            libs.append(lib)
        else:
            libs.append("-l" + lib)
    args.extend(libs)
    p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    if stderr:
        print(stderr)
    #if not p.poll() is None:
    #    print(p.stderr.read())
    p.wait()
    return p.returncode


def main():
    parser = argparse.ArgumentParser(
                description='Compile with x86_gcc2 even on x86 target')
    parser.add_argument('--compiler',
                help='C++ compiler')
    parser.add_argument('--outpath',
                help='output path')
    parser.add_argument('--defs', nargs='+',
                help='definitions')
    parser.add_argument('--includes', nargs='+',
                help='include paths')
    parser.add_argument('--inputpath',
                help='input file')
    parser.add_argument('--libs', nargs='+',
                help='link libs')
    args = parser.parse_args()
    return exec_compiler(args.compiler, args.outpath, args.inputpath,
                         args.defs, args.includes, args.libs)


if __name__ == '__main__':
    sys.exit(main())
