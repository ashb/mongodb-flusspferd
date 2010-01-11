# Flusspferd bindings for MongoDB #

To compile this module you'll need [Cmake 2.6.4+][cmake] (as required for
building Flusspferd itself) and obviously [mongoDB][mongo_qs]. I'm making no
guarantees that the compile will work without tweaking for everyone yet tho.

    $ cmake -H. -Bbuild

If this fails to find the mongoDB library or header files you'll need to
specify the directory containing `lib/libmongoclient.a` (or similar) and
`include/mongo` by adding `-DMONGO_ROOT=/path/to/mongo` to the above cmake
command.

    $ make -C build
    $ flusspferd -Ibuild
    > mongodb = require('mongodb').MongoDB('localhost')
    ({})
    > quit();
    $ make -C build install

[cmake]: http://www.cmake.org/cmake/resources/software.html
[mongo_qs]: http://www.mongodb.org/display/DOCS/Quickstart "MongoDB Quick Start"
