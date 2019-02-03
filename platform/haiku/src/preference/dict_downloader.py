

import argparse
import os
import sys
import subprocess
import json
import tempfile
import shutil
import re
import commands


class DictionaryDownloader:
    def __init__(self, dict_dir, dictionaryset_path, host, base_path, types, verbose=False):
        self.verbose = verbose
        self.dict_dir = dict_dir
        self.path = "https://" + host + base_path + "/"
        self.dictionaryset_path = dictionaryset_path
        self.set = self._load_dictionary_set(dictionaryset_path)
        self.temp_dir = None
        self.http_head_exp = re.compile("HTTP/\d.\d (\d\d\d)")
        self.modified = False
        self.types = types
        if self.verbose:
            print("{} initialized".format(self.__class__.__name__))
            print("dict_dir: {}".format(self.dict_dir))
            print("path: {}".format(self.path))
        self.current = None

    def is_modified(self):
        return self.modified

    def store(self):
        if self.modified:
            s = json.dumps(self.set, indent=0)
            with open(self.dictionaryset_path, "w") as f:
                f.write(s)

    def _prepare(self):
        try:
            os.makedirs(self.dict_dir)
        except:
            pass
        self.temp_dir = tempfile.mkdtemp()

    def _closing(self):
        if not self.temp_dir is None:
            shutil.rmtree(self.temp_dir)
        self.temp_dir = None

    def download(self):
        self._prepare()
        try:
            self._download()
        finally:
            self._closing()

    def _download(self):
        types = self.types
        for d in self.set:
            if not isinstance(d, dict):
                continue
            type = int(d.get("type", 0))
            if not type in types:
                continue
            active = d.get("active", False)
            if not active:
                continue
            location = d.get("location", None)
            if location is None:
                continue
            etag = d.get("etag", None)
            self.current = location
            self._download_dictionary(location, etag)
        if self.verbose:
            print("download session done")

    def _download_dictionary(self, location, etag=None):
        if location.find("/") >= 0:
            if location.startswith("zipcode/"):
                self._download_zipcode(location, etag)
        else:
            ext = ".gz"
            if location.endswith("edict"):
                ext = ".tar.gz"
            status = self._download_(location, ext, etag)
            if status is None:
                return
            if not status:
                status = self._download_(location, "", etag)

    def _download_zipcode(self, location, etag=None):
        if not location.startswith("zipcode/"):
            return
        status = self._download_("zipcode", ".tar.gz", etag, no_move=True)
        if status is None:
            return
        if status:
            dict_path = os.path.join(self.dict_dir, location)
            file_path = os.path.join(self.temp_dir, "zipcode.tar.gz")
            path = self._tar_xf(file_path)
            source_path = os.path.join(self.temp_dir, location)
            self._move_file(source_path, dict_path)
        else:
            pass # todo

    def _download_(self, location, extension="", etag=None, no_move=False):
        dict_path = os.path.join(self.dict_dir, location)
        if not os.path.exists(dict_path):
            etag = None
        if self.verbose:
            print("dict_path: {}".format(dict_path))
        try:
            file_path = self._download_file(location + extension, etag)
            if self.verbose:
                print("downloaded: {}".format(file_path))
        except self.NotModified:
            if self.verbose:
                print("not modified: {}".format(location))
            return None
        if not file_path is None:
            md5_path = self._download_file(location + extension + ".md5", store_etag=False)
            if md5_path:
                if not self._check_sum(file_path, md5_path):
                    return False
            if file_path.endswith(".tar.gz"):
                path = self._tar_xf(file_path)
            elif file_path.endswith(".gz"):
                path = self._gunzip(file_path)
            if not no_move:
                self._move_file(path, dict_path)
            return True
        return False

    def _move_file(self, path, location):
        if self.verbose:
            print("moving file: {}, {}".format(path, location))
        try:
            os.renames(path, location)
        except:
            pass

    class NotModified(Exception):
        pass

    def _download_file(self, location, etag=None, store_etag=True):
        update_etag = False
        addr = self.path + location
        if self.verbose:
            print("download from: {}".format(addr))
        args = ["curl", "-I", addr]
        if not etag is None:
            header = 'If-None-Match: "{}"'.format(etag)
            args.extend(["--header", header])
        stdout = self._http_access(args)
        status_code = self._http_status(stdout)
        if status_code == 302:
            trans_addr = self._http_location(stdout)
            args[2] = trans_addr # replace, address
            stdout = self._http_access(args)
            status_code = self._http_status(stdout)
            if status_code == 302:
                return

        if status_code == 200:
            if store_etag:
                new_etag = self._http_etag(stdout)
                update_etag = new_etag != etag
            outpath = os.path.join(self.temp_dir, location)
            args.extend(["--output", outpath])
            args.pop(1) # drop, -I
            stdout = self._http_access(args)
            if store_etag and update_etag:
                self._update_etag(location, new_etag)
            return outpath
        elif status_code == 304:
            # 304 Not Modified
            raise self.NotModified()
        return None

    def _update_etag(self, location, etag):
        if etag:
            for d in self.set:
                if d.get("location") == self.current:
                    d["etag"] = etag
                    self.modified = True
                    break

    def _http_access(self, args):
        p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, _ = p.communicate()
        return stdout

    def _http_status(self, out):
        r = self.http_head_exp.match(out)
        if r:
            return int(r.group(1))
        return 0

    def _http_location(self, out):
        trans_addr = None
        for line in out.split("\n"):
            if line.startswith("Location: "):
                trans_addr = line[10:].strip()
                break
        return trans_addr

    def _http_etag(self, out):
        etag = None
        for line in out.split("\n"):
            if line.startswith("ETag: "):
                etag = line[6:].strip().strip('"')
                break
        return etag

    def _gunzip(self, path):
        args = ["gunzip", path]
        p = subprocess.Popen(args, stderr=subprocess.PIPE)
        _, stderr = p.communicate()
        if stderr:
            return None
        if path.endswith(".gz"):
            return path[0:-3]
        return path

    def _tar_xf(self, path):
        args = ["tar", "xf", path, "-C", os.path.dirname(path)]
        p = subprocess.Popen(args, stderr=subprocess.PIPE)
        _, stderr = p.communicate()
        p.wait()
        if stderr:
            return None
        if path.endswith(".tar.gz"):
            return path[0:-7]
        return path

    def _check_sum(self, file_path, sum_path):
        sum = self._get_check_sum(file_path)
        with open(sum_path, "r") as f:
            verify = f.read().split(" ", 1)[0]
        return sum == verify

    def _get_check_sum(self, file_path):
        args = ["md5sum", file_path]
        p = subprocess.Popen(args, stdout=subprocess.PIPE)
        stdout, _ = p.communicate()
        return stdout.split(" ", 1)[0]

    def _load_dictionary_set(self, path):
        if not os.path.exists(path):
            raise Exception("DictionarySet.json exists: " + path)
        try:
            with open(path, "r") as f:
                j = json.load(f)
        except:
            raise Exception("DictionarySet.json seems broken: " + path)
        if not isinstance(j, list):
            raise Exception("DictionarySet.json contains wrong data: " + path)
        return j


def FindDir(d):
    return commands.getoutput('finddir ' + d)


def main():
    config_settings = FindDir("B_USER_SETTINGS_DIRECTORY")
    aquaskk_settings = os.path.join(config_settings, "aquaskk")

    parser = argparse.ArgumentParser(
                description="Downloads SKK dictionary")
    parser.add_argument("--dictdir", default=os.path.join(aquaskk_settings, "dict"),
                help="Directory path to dictionary to place")
    parser.add_argument("--dictset", default=os.path.join(aquaskk_settings, "DictionarySet.json"),
                help="DictionarySet.json")
    parser.add_argument("--host", default="github.com",
                help="Host address")
    parser.add_argument("--path", default="/skk-dev/dict/raw/gh-pages",
                help="Base path to the dictionary")
    parser.add_argument("--verbose",
                help="Verbose output")
    parser.add_argument("--types",
                help="Dictionary types to download")

    args = parser.parse_args()
    downloader = DictionaryDownloader(
            args.dictdir, args.dictset, args.host, args.path,
            tuple([int(t) for t in args.types.split(",")]), args.verbose)
    downloader.download()
    downloader.store()


if __name__ == '__main__':
    main()
