# xx2d

this is a simple 2d game engine based by opengl es 3.0 & c++20

performance ~= raylib, much more faster than cocos2dx 3.x


# feature

search path

auto decompress zstd file( detect by header: 28 B5 2F FD )

texture ( current .pkm only, detect by header "PKM 20" ) cache

texture packer ( 7.0 limit support ) loader & frame cache

quad with auto batch

sprite based of quad

bmfont

label

keyboard, mouse

tiled map ( 1.92 full support ) loader & helpers( cam, util funcs )

# compile requires

cmake 3.20+

sub modules: ( put them to sibling directory )

https://github.com/denghe/xxlib.git

https://github.com/glfw/glfw.git

https://github.com/zeux/pugixml.git

https://github.com/facebook/zstd.git

# todo

touch, joystick

button, scroll view

node ( recursive trans calc )

edit box, rich text

Program manager? ready for shader switch

draw no alpha without blend but discard shader ?

texture packer polygon algorithm support?

render texture?

draw instance for particle system?

texture array support?

more texture format support? astc?

spine? webassembly? lua? 
