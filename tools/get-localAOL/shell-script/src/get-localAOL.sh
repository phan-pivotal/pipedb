#!/bin/sh

DIR=$(dirname $0)
if test "$DIR" = "."
then
    DIR=$PWD
fi

if test $# -ne 1
then
    echo "usage $0 <output file>"
    exit 1
fi

cd $DIR/../../c-nar-project 
mvn clean install 
localAOL=$(ls target/nar/gnu/)

cat<<EOF >$1
<project xmlns="http://maven.apache.org/POM/4.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>

    <parent>
        <groupId>info.pipedb</groupId>
        <artifactId>pipedb</artifactId>
        <version>0.0.1</version>
    </parent>

    <groupId>info.pipedb</groupId>
    <artifactId>src</artifactId>
    <packaging>pom</packaging>
    <version>0.0.1</version>

    <name>PipeDB</name>
    <description>
      PipeDB NOSQL database block repository - automatically generated pom
    </description>

    <properties>
        <localAOL>${localAOL}</localAOL>
    </properties>

    <modules>
        <module>thirdparty</module>
        <module>core</module>
    </modules>
</project>
EOF

