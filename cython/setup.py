from setuptools import setup
from distutils.sysconfig import get_python_lib
import glob
import os
import sys

if os.path.exists('readme.rst'):
    print("""The setup.py script should be executed from the build directory.

Please see the file 'readme.rst' for further instructions.""")
    sys.exit(1)


setup(
    name = "gloc",
    package_dir = {'': 'src'},
    data_files = [(get_python_lib(), glob.glob('lib/*.so'))],
    author = 'Thomas Latzko',
    description = 'Cpp extension including cuda an pure C++',
    license = 'KIT',
    keywords = 'cmake cython build',
    url = 'http://kit.edu',
    test_require = ['nose'],
    zip_safe = False,
    )
