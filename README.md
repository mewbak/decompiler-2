Educational Decompiler
======================

This is an educational decompiler project based on [this](https://github.com/conmarap/execfile) project.

# Introduction

I have been interested in executable file formats, compilers and decompilers for quite a long time, now. A few years ago I started writing a kernel from scratch, in an attempt to understand how a computer operates on runtime. The one area in which I was stuck was making my kernel extensible; I had tried, multiple times, to add ELF or PE support to it, but with no success.

After many failed attempts and countless segmentation faults later, I decided to write a an API for systems that didn't support ELFs; the idea was that one would be able to run very simple executable files (of various types) on Windows, for instance. This project gave me a lot of knowledge on how to access this type of files, but it was, obviously, not a success. Hence, I decided to write a decompiler that emulated `objdump -d` on all machines with GNU compatibility, which is this project.

This program is implemented using its own ELF binary parser and utilities instead of [ELFUtils](http://www.linuxfromscratch.org/blfs/view/cvs/general/elfutils.html) in order to maintain portability across platforms and operating systems.

# Plans and Notes

My free time is rather limited and I might not spend as much time as I want on this project, but I plan to work on it on an irregular basis. I might even start a blog series about it, in order to document my every commit.

# Capabilities

So far, this decompiler can parse an ELF file and read its opcodes. In the future, and after I read the Intel manual thoroughly, I will implement the actual decompiler that will present the assembly that an executable file runs and, furthermore, a representation of it in C or C++.
