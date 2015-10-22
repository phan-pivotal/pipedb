PipeDB
=========


PipeDB is a work-in-progress block repository based on existing key/value database.


Version
----

0.0.1

Installation
--------------

First recover the code :
git clone https://github.com/juanmaneo/pipedb.git
cd pipedb
git submodule update --init --recursive

To build just do :

```sh
mvn clean install
```

On first build this will generate "src/pom.xml" with "localAOL" variable depending on your system/machine. 
After first build "tools" directory can be avoided and you can only work within "src" directory like any maven project. 

License
----

APACHE 2.0

