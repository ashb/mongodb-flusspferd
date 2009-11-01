# Flusspferd bindings for MongoDB #

To compile this module you'll need [Premake4][industriousone] and obviously
[mongoDB][mongo_qs]. I'm making no guarantees that the compile will work without tweaking
for everyone yet tho.

    $ premake4 --mongodb-root=$HOME/code/assets/mongodb-osx-i386-2009-10-31/ gmake
    $ make
    $ flusspferd -Ibuild
    > mongodb = require('mongodb').MongoDB('localhost')
    ({})
    > quit();
    $ flusspferd install.js

[industriousone]: http://industriousone.com/premake
[mongo_qs]: http://www.mongodb.org/display/DOCS/Quickstart "MongoDB Quick Start"
