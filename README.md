# jrdrand
[![CodeQL](https://github.com/javachaos/jrdrand/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/javachaos/jrdrand/actions/workflows/codeql-analysis.yml)

Simple library for java to access RDRAND instruction on supported CPUs

| Currently Supported OS's: | Currently Tested OS's: |
|---------------------------|------------------------|
| Linux                     | OpenSUSE 15.2          |
| Windows                   | Windows 11             |

#### TODO List ####
- [ ] Add support for BSD
- [ ] Add support for MacOSX
- [ ] Add more features
- [ ] Publish to GithubPackages

#### Example Usage: ####
```java
RdRand rd = new RdRand();
long s = rd.seed();
System.out.println("RDSEED: " + s);
System.out.println("RDRAND: " + rd.rand(s));
```