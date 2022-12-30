# xx2d

this is a simple 2d game engine based by opengl es 3.0 & c++20

performance ~= raylib, much more faster than cocos2dx 3.x


# feature

search path

texture format: pkm

texture cache

texture packer frame cache

quad with auto batch

sprite based of quad

bmfont

label

keyboard, mouse


# compile requires

cmake 3.20+

sub modules: ( put them to sibling directory )

https://github.com/denghe/xxlib.git

https://github.com/glfw/glfw.git

https://github.com/zeux/pugixml.git

https://github.com/facebook/zstd.git



# todo

tiledmap loader

touch, joystick

node / translate support( matrix manager )

button, scroll view

Program manager? ready for shader switch

draw no alpha without blend but discard shader ?

texture packer polygon algorithm support?

render texture?

draw instance for particle system?

texture array support?

more texture format support?

spine? webassembly? lua? 
