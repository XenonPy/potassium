# Potassium
Potassium (.kscript) is an opinionated interpreted programming language. 
## Features
* ⏱️ **Fast**: Potassium is interpreted by a single, compact file

* ⚡ **Batteries Included**: Potassium is run by a lightweight interpreter not relying on any other software

* 📖 **Readable**: Potassium uses a unique syntax format that defines important actions while allowing complex operations
## Build from source
To build Potassium from source, first clone the Git repository: 

`git clone https://github.com/XenonPy/potassium.git`

Then hop into the new directory: 

`cd potassium/`

Build it with `g++` (or your C++ compiler of choice):

`g++ build potassium.cpp -o potassium`

Then add the following command to your shell profile to add Potassium to `$PATH` (varies by system):

`export PATH=/potassium/potassium:$PATH`

To test your installation:

`touch hello_World.kscript`

Add `pln("Hello, World!")` to your Potassium script.
Run it:

`potassium run hello_world.kscript`

You should see `Hello, World!` as the output.
