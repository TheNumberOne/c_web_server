# Rosetta's Web Server

Features implemented:
 * Implemented http spec
    * Sends 401 if file not found
    * Sends correct error for various bad requests
    * Content length implemented.
    * Content type implemented for 6 file types.
    * Accepts GET requests
 * Multi-threaded with configurable thread pool size
 * Logging with separate dedicated thread for logging
 * Multi-thread safe caching
 * Hidden files and files outside webroot are kept hidden
 * Accepts settings file specifying parameters.

How to compile

```bash
cd <project_directory>
mkdir build
cd build
cmake ..
make
```

How to run:

```bash
./server <name of settings file>
```

You can use the example settings.txt file as a template for what you want or you can pass an invalid file
to have it display a helpful message explaining what needs to be in the settings file.

To use, navigate in the browser to the specified files. Note that this webserver does not automatically
resolve the forward slash to index.html. Enjoy!