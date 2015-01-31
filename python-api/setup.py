from setuptools import setup, find_packages
from codecs import open
from os import path

import subprocess
version_git = subprocess.check_output('git describe --dirty', shell=True).strip()

here = path.abspath(path.dirname(__file__))

# Get the long description from the relevant file
with open(path.join(here, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

setup(
    name='gst-switch',
    version=version_git,
    description='API for controlling the gst-switch server.',
    long_description=long_description,
    url='https://github.com/timvideos/gst-switch/tree/master/python-api',
    author='TimVideos Developers',
    author_email='timvideos@googlegroups.com',
    license='GPLv3+',

    classifiers="""\
Development Status :: 4 - Beta
Environment :: Console
Environment :: X11 Applications :: GTK
Intended Audience :: Developers
License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)
Operating System :: POSIX :: Linux
Programming Language :: C
Programming Language :: Python :: 2
Programming Language :: Python :: 2.7
Programming Language :: Python :: 3
Programming Language :: Python :: 3.4
Topic :: Multimedia :: Sound/Audio :: Capture/Recording
Topic :: Multimedia :: Sound/Audio :: Mixers
Topic :: Multimedia :: Video
Topic :: Multimedia :: Video :: Capture
Topic :: Multimedia :: Video :: Conversion
Topic :: Software Development :: Libraries :: Python Modules
""".split("\n"),
    keywords='video mixing switching streaming recording gst-switch dvswitch',

    packages=find_packages(exclude=['docs', 'tests*']),
    install_requires=[],
    extras_require = {
        'dev': ['check-manifest'],
        'test': [
            'mock',
            'pytest',
            'pytest-cov',
            'pytest-pep8',
            'pylint',
        ],
    },
)
