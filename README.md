# xx2d

this is a simple 2d game engine based by opengl es 3.0 & c++20

GPU & CPU cache friendly, manual control draw order

performance ~= raylib, much more faster than cocos2dx

# tips

convert png to pkm:
tools/png2pkm_astc/etcpak.exe & drag_png_here_convert_pkm_etc2_rgba8.bat

convert png to astc:
tools/png2pkm_astc/astcenc-avx2.exe & drag_png_here_convert_astc6x6.bat

compress pngs to vp9 webm:
ffmpeg.exe -f image2 -framerate 60 -i "%d.png" -c:v libvpx-vp9 -pix_fmt yuva420p -b:v 50k -speed 0 xxx_50k_vp9.webm

compress pkm:
https://github.com/facebook/zstd/releases
zstd.exe --ultra -22 ?????.pkm -o ?????.pkm.zstd

# feature

search path

auto uncompress zstd file( detect by header: 28 B5 2F FD )

shader manager with autobatch

texture ( current support pkm2, astc, png format only, detect by header ) cache

texture packer ( polygon algorithm support. format: cocos3.x plist ) loader & frame cache

quad instance

sprite, polygon sprite

node

bmfont

label, simple label

line strip

keyboard, mouse ( with event listener )

button

tiled map ( 1.92 full support ) loader & helpers( cam, util funcs )

scissors

frame buffer ( render texture )

vp9 alpha webm to texture

audio ( miniaudio + stb vorbise ) ( .exe + 400kb )

imgui ( .exe + 800kb )

# compile requires

cmake 3.20+

pull submodules

# more examples

https://github.com/denghe/xx2d_space_shooter

# todo

edit box, rich text?

more texture format support?

spine?

webassembly? lua? 

touch, joystick?

crossplatform shell
