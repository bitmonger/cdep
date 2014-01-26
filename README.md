cdep
====

A simple dependency resolver with a .ini file format

There are many dependency resolving tools, but they can be very complicated, and perhaps do more than in necessary.
This tool is designed to be the simplest way possible to generate a build order for a project.

Example:

deps.ini:
    
    [some_component]
    cmd = ./some_component/build.sh
    dep = some_other_component
    
    [some_other_component]
    cmd = ./some_other_component/pre_build.sh
    cmd = ./some_other_component/build.sh

Run cdep:
    
    $ cdep deps.ini > build_script.sh
    $ cat build_script.sh
    #!/bin/bash -e
    ./some_other_component/pre_build.sh
    ./some_other_component/build.sh
    ./some_component/build.sh

