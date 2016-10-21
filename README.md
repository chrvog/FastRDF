# FastRDF
FastRDF is a new serialization concept for rdf graphs.

## Background

In the ARVIDA project we analyzed the usage of semantic RDF interfaces in the context of virtual technologies (AR/VR). Many use cases like 3D tracking are latency constrained and are using some kind of high frequency streaming (60+Hz). 

Performance analyses showed that creation of the RDF serialization is a bottleneck in such use cases with the main performance hotspots:
* RDF Graph Creation
* Conversion of native data to string representation for literals
* RDF graph serialization
* RDF graph parsing
* RDF graph queries

Data analyses in the ARVIDA showed that for the given latency constraint use cases the graphs have the following characteristics:
* Small: < 1000 triples 
* The structure of a graph almost constant as the semantic context (e.g. measuring units) does not change 
* Some literal values are highly dynamic (update frequency <10ms)

Most application are written in C++ which allows us to do additional optimizations.

## Concept
The main idea is to split the RDF graph serialization into multiple parts with different representations:
* An RDF graph that is serialization with any kind of standard serialization. The actual lexical representations of the literal value are replaced by subgraphs that describes the access to the binary representation of the literal value. Each literal value is identified by a unique id.
* A binary payload that contains all literal values in a binary representation
* A lookup table that maps the id of the literal to an address in the binary payload and contains the byte size of the value
This separation allows to use different update frequencies for the structure and literal values. Due to the lookup table even partial literal updates are possible.
This repository contains a C++ prototype implementation 

## Features
* In-Memory Triplestore with Literal Callbacks
* Highly Optimized N-Triples Serializer
* Partial Literal Updates

## Dependencies
* Boost
* Redland Raptor (http://librdf.org/)
	
## Compiler-Support
This prototype is built with Visual Studio 2015 Update 2. 
C++11 is required.
Support for other platforms and compilers is in progress.

## Publications
In Progress

## How does it compare to HDT?
HDT optimizes the serialization of the graph itself. It’s an orthogonal problem and can be used in combination with fast rdf.
http://www.rdfhdt.org/#
## Contributing
Contributions are very welcome.

## License
This source distribution is subject to the license terms in the LICENSE file found in the top-level directory of this distribution.
You may not use this file except in compliance with the License.

## Third-party Contents
This source distribution includes the third-party items with respective licenses as listed in the THIRD-PARTY file found in the top-level directory of this distribution.

## Acknowledgements
This work has been supported by the [German Ministry for Education and Research (BMBF)](http://www.bmbf.de/en/index.html) (FZK 01IMI3001 J) as part of the [ARVIDA](http://www.arvida.de/) project.
