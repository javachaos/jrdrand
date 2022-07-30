# jrdrand
[![CodeQL](https://github.com/javachaos/jrdrand/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/javachaos/jrdrand/actions/workflows/codeql-analysis.yml)
[![Publish package to GitHub Packages](https://github.com/javachaos/jrdrand/actions/workflows/github-packages.yml/badge.svg)](https://github.com/javachaos/jrdrand/actions/workflows/github-packages.yml)

Simple library for java to access RDRAND instruction on supported CPUs

| Currently Supported OS's: | Currently Tested OS's: |
|---------------------------|------------------------|
| Linux                     | OpenSUSE 15.2          |
| Windows                   | Windows 11             |

#### TODO List ####
- [ ] Add support for BSD
- [ ] Add support for MacOSX
- [ ] Add more features
- [x] Publish to GithubPackages

#### Example Usage: ####

Add the following to your pom.xml
```xml
<dependencies>
    <dependency>
      <groupId>com.github.javachaos</groupId>
      <artifactId>jrdrand</artifactId>
      <version>1.0-SNAPSHOT</version>
    </dependency>
</dependencies>
<distributionManagement>
   <repository>
     <id>github</id>
     <name>GitHub Javachaos Apache Maven Packages</name>
     <url>https://maven.pkg.github.com/javachaos/jrdrand</url>
   </repository>
</distributionManagement>
```
or the following to your build.gradle file
```groovy
dependencies {
    implementation 'com.github.javachaos:jrdrand'
}

repositories {
    maven {
        url = uri("https://maven.pkg.github.com/javachaos/jrdrand")
    }
}

```


```java
RdRand rd = new RdRand();
long s = rd.seed();
System.out.println("RDSEED: " + s);
System.out.println("RDRAND: " + rd.rand());
```
