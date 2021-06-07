7 June 2021:

The vulkan loader library cannot be statically linked, however it is not consistently available
as a system library on Linux or Windows.

I made branches to test three seperate imperfect work-arounds for this:

1) Build the vulkan loader from source and ship it with the executable(s).
- If you use a fixed version/tag of the loader, it will not be able to load newer ICDs, even if in 
the future they completely replace the ones it works with now.
- If you use the latest commit of the loader source, CI only guarantees it builds in isolation, not
as a submodule of my project, with whatever hackery I've used to drive the build.
- Even if the versioning works out, in practice building the loader on Windows turns out
 to involve more dll installations and other system fiddling than just downloading the Vulkan SDK
 itself. 

2) Create a Linux installer and a Windows installer to install the loader directly onto the host
system.
- Would probably end up being about ten times as complicated as the source code and build system
combined, and take a hundred times longer to write.
- Even if I was a genius, would a user really want to have to install hundreds of dlls/sos
onto their system just to run some example code?

3) Dynamically load the vulkan loader so it is possible to give a helpful error message if it is
not on the system.
- Doesn't actually solve the fact the loader is not there.
- There is no platform independent way to dynamically load libraries, so have to use preprocessor
 and add extra complexity to the build.

In the end I chose to dynamically load the vulkan loader, as it seemed the least awful option, 
however I am not happy with the horrific C preprocessor code I had to write to get it all to
work...