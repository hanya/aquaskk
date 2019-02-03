

import sys
import argparse
import subprocess
import os.path
import time


class Catkeys:
    def __init__(self):
        self.contexts = {}

    def add(self, context, key, value):
        try:
            c = self.contexts[context]
        except:
            c = []
            self.contexts[context] = c
        c.append((key, value))

    def get_contexts(self):
        for context in sorted(self.contexts.keys()):
            c = self.contexts[context]
            for v in sorted(c):
                yield v


def gen_catkeys(sig, out_path, input_path):
    # collectcatkeys -s sig -o en.catkeys data.pre
    args = ['collectcatkeys', '-s', sig, '-o', out_path, input_path]
    p = subprocess.Popen(args, stderr=subprocess.PIPE)
    while True:
        if p.poll():
            print(p.stderr.readline())
            return p.returncode
        else:
            break
    p.wait()
    return

    # sort entries
    file_path = os.path.abspath(out_path)

    time.sleep(0.01)
    n = 0
    while n < 10:
        try:
            with open(file_path, "r") as f:
                s = f.read()
                break
        except:
            n += 1
            time.sleep(0.01)
    if n == 10:
        return p.returncode

    lines = s.split("\n")
    catkeys = Catkeys()
    for line in lines[1:]:
        parts = line.split("\t")
        if len(parts) > 2:
            catkeys.add(parts[1], parts[0], line)

    data = [lines[0]]
    for k, v in catkeys.get_contexts():
        data.append(v)
    with open(file_path, "w") as f:
        f.write("\n".join(data))
    return p.returncode


def main():
    parser = argparse.ArgumentParser(
                description='Generates en.catkeys file from pre.')
    parser.add_argument('--sig',
                help='Application mime type signature')
    parser.add_argument('--outpath', 
                help='en.catkeys')
    parser.add_argument('--inputpath',
                help='Merged C++ files as .pre')
    args = parser.parse_args()
    return gen_catkeys(args.sig, args.outpath, args.inputpath)


if __name__ == '__main__':
    sys.exit(main())
