

import sys
import argparse
import xml.dom.minidom


def gen_version(inpath, outpath):
    dom = xml.dom.minidom.parse(inpath)
    d = dom.getElementsByTagName("dict")[0]
    data = {}
    name = ""
    value = None
    for node in d.childNodes:
        if node.nodeType == node.ELEMENT_NODE:
            tag = node.tagName
            if tag == "key":
                name = node.firstChild.data
                value = None
                continue
            elif tag == "string":
                value = node.firstChild.data
        if not value is None:
            data[name] = value

    version = data["CFBundleShortVersionString"]
    date = data["CFBundleVersion"]

    s = """
#ifndef VERSION_H
#define VERSION_H

#define AQUASKK_VERSION   "{}"
#define AQUASKK_DATE      "{}"

#endif
""".format(version, date)

    with open(outpath, "w") as f:
        f.write(s)


def main():
    parser = argparse.ArgumentParser(
                description="Reads version and date and generate version.h")
    parser.add_argument("--inputpath",
                help="plist")
    parser.add_argument("--outpath",
                help="version.h")
    args = parser.parse_args()
    gen_version(args.inputpath, args.outpath)


if __name__ == '__main__':
    main()
