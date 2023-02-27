# xx2d

this is a simple 2d game engine based by opengl es 3.0 & c++20

GPU & CPU cache friendly, manual control draw order

performance ~= raylib, much more faster than cocos2dx


# feature

search path

auto decompress zstd file( detect by header: 28 B5 2F FD )

shader manager with autobatch

texture ( current .pkm only, detect by header "PKM 20" ) cache

texture packer ( 7.0 limit support ) loader & frame cache

quad

sprite

node

bmfont

label

line strip

keyboard, mouse ( with event listener )

button( simulate )

tiled map ( 1.92 full support ) loader & helpers( cam, util funcs )

# compile requires

cmake 3.20+

pull submodules

# todo

scroll view / scissors

render texture

edit box, rich text? imgui?

texture packer's polygon algorithm support

more texture format support? astc? webp? png? webm -> tex?

spine?

webassembly? lua? 

texture array support?

touch, joystick

crossplatform shell
