# QtSpeechRecognition
Qt wrapper around pocketsphinx with basic functionality focused on keywords. Currently only tested and working on Unix systems. Windows is planned for the future

# Requirements
This requires CMUSphinx to be compiled/installed on your system. This takes inspiration from the abandoned QtSpeech project to include Speech-To-Text functionality (currently it only has Text-To-Speech functionality). 

While you can just compile and install CMUSphinx without any modications, I highly recommend applying the __pocketsphinx_qt.patch__ located in the main directory. 
It adds the functionality to use keywords from memory  rather than requiring a file to read from.

You can follow the installation from the [**CMUSphinx Tutorial**](https://cmusphinx.github.io/wiki/tutorialpocketsphinx/) to install PocketSphinx on your system. Or follow the instructions in the next section.

# Basic CMUSphinx Installation Tutorial
## Installing SphinxBase
1. clone https://github.com/cmusphinx/sphinxbase
2. go into the `sphinxbase` folder and run `./autogen.sh`
3. `./configure` then `make` and `make install`

## Installing PocketSphinx
1. clone https://github.com/cmusphinx/pocketsphinx
2. copy the `pocketsphinx_qt.patch` located in the  `QtSpeechRecognition`  folder, into the `pocketsphinx` folder
3. run `patch -p0 < pocketsphinx_qt.patch` to patch the files. 
4. `./configure` then `make` and `make install`

## Post Installation

#### Setup Library Paths
CMUSphinx installs the libraries by default in the `/usr/local/lib/` directory. Most systems do not load libraries from this directory. You can either move them to the normal library location, or do something like:

1. Editing `/etc/ld.so.conf` to contain `/usr/local/lib` directory, and then `sudo ldconfig` to update the config
2. Or by exporting: 
```bash
export LD_LIBRARY_PATH=/usr/local/lib
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
```

See the [**CMUSphinx Tutorial**](https://cmusphinx.github.io/wiki/tutorialpocketsphinx/)  if you have more questions regarding setting up the libraries.

#### CMUSphinx Models

The `pocketsphinx` folder contains a `model` folder, containing the models needed for speech recognition. 
Drop them in the same directory as the `DLLTestLoader` application (I should've renamed it to `Demo` Instead). It should load the model just fine.

If the model doesn't load, check the path to laod the model when initializing the speech recognition library.
