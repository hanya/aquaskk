
How to build
----
Please use haikuports to build.

If you are a developer, try these commands to build without haikuports.
```
python build.py gyp --platform=Haiku
```
If you build on x86_gcc2 environment, add --arch=x86_gcc2 .

```
python build.py build -c Release method/method.gyp:aquaskk
```

Search order of path
* /boot/home/config/settings/aquaskk
* /boot/home/config/non-packages/data/aquaskk
* /boot/system/...
