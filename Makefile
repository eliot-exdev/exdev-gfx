C_FLAGS_MOS=-speed -final -DNDEBUG -D__MORPHOS__
C_FLAGS_MOS_GCC=-Ofast -noixemul -mcpu=G4 -maltivec -mabi=altivec -DNDEBUG -D__MORPHOS__

C_FLAGS_060=+aos68k -fpu=68060 -cpu=68060 -speed -final -DNDEBUG -D__AMIGA__
C_FLAGS_030_FPU=+aos68k -fpu=68881 -cpu=68030 -speed -final -DNDEBUG -D__AMIGA__

LD_FLAGS_MOS=-lm
LD_FLAGS_MOS_GCC=-lm

LD_FLAGS_060=-lm060 
LD_FLAGS_030_FPU=-lm881

CC=vc
CC_GCC=ppc-morphos-gcc-11

PREFIX=ram:exdevgfx

INCLUDES_MOS=-Ilibrary/include -Ilibrary/easing/include
INCLUDES_AOS=-Ilibrary/include -Ilibrary/easing/include

all: voxelspace julia test_sprite other

#--- voxelspace ---#
voxelspace: voxelspace_mos voxelspace_mos_gcc voxelspace_060 voxelspace_060_c2p 

voxelspace_mos: library/src/vertex3d.c library/src/events.c library/src/vertex2d.c library/src/matrix.c library/src/palette.c library/src/color.c library/src/framebuffer.c library/src/framebuffer_8bit.c library/src_amiga/window_amiga.c library/src/font.c library/src/heightmap.c\
                library/src/voxelspace.c library/src/args.c library/src/helper.c library/src_amiga/exdev_base_amiga.c library/src_amiga/helper_amiga.c examples/voxelspace_main.c
	$(CC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS} ${LD_FLAGS_MOS}
    
voxelspace_mos_gcc: library/src/vertex3d.c library/src/events.c library/src/vertex2d.c library/src/matrix.c library/src/palette.c library/src/color.c library/src/framebuffer.c library/src/framebuffer_8bit.c library/src_amiga/window_amiga.c library/src/font.c library/src/heightmap.c\
                library/src/voxelspace.c library/src/args.c library/src/helper.c library/src_amiga/exdev_base_amiga.c library/src_amiga/helper_amiga.c examples/voxelspace_main.c
	$(CC_GCC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS_GCC} ${LD_FLAGS_MOS_GCC}

voxelspace_060: library/src/vertex3d.c library/src/events.c library/src/vertex2d.c library/src/matrix.c library/src/palette.c library/src/color.c library/src/framebuffer.c library/src/framebuffer_8bit.c library/src_amiga/window_amiga.c library/src/font.c library/src/heightmap.c\
                library/src/voxelspace.c library/src/args.c library/src/helper.c library/src_amiga/exdev_base_amiga.c library/src_amiga/helper_amiga.c examples/voxelspace_main.c
	$(CC) -o ${@} ${INCLUDES_AOS} $(^) ${C_FLAGS_060} ${LD_FLAGS_060} -DLOW_RESOLUTION

voxelspace_060_c2p: library/src/vertex3d.c library/src/events.c library/src/vertex2d.c library/src/matrix.c library/src/palette.c library/src/color.c library/src/framebuffer.c library/src/framebuffer_8bit.c library/src_amiga/window_amiga.c library/src/font.c library/src/heightmap.c\
                library/src/voxelspace.c library/src/args.c library/src/helper.c library/src_amiga/exdev_base_amiga.c library/src_amiga/helper_amiga.c examples/voxelspace_main.c
	$(CC) -o ${@} ${INCLUDES_AOS} -IWork:workspace/c2plib/sdk/C $(^) ${C_FLAGS_060} ${LD_FLAGS_060} -LWork:workspace/c2plib/sdk -lc2p -DLOW_RESOLUTION -DUSE_C2P

#voxelspace_030_fpu: library/src/vertex3d.c library/src/events.c library/src/vertex2d.c library/src/matrix.c library/src/palette.c library/src/color.c library/src/framebuffer.c library/src/framebuffer_8bit.c library/src_amiga/window_amiga.c library/src/font.c library/src/heightmap.c\
#                library/src/voxelspace.c library/src/args.c library/src/helper.c library/src_amiga/exdev_base_amiga.c library/src_amiga/helper_amiga.c examples/voxelspace_main.c
#	$(CC) -o ${@} ${INCLUDES_AOS} $(^) ${C_FLAGS_030_FPU} ${LD_FLAGS_030_FPU} -DLOW_RESOLUTION

#--- test sprite ---#
test_sprite: test_sprite_mos test_sprite_mos_gcc

easing_mos_gcc: library/easing/src/Back.c library/easing/src/Bounce.c library/easing/src/Circ.c library/easing/src/Cubic.c library/easing/src/Elastic.c\
                library/easing/src/Expo.c library/easing/src/Linear.c library/easing/src/Quad.c library/easing/src/Quart.c library/easing/src/Quint.c\
                library/easing/src/Sine.c
	$(CC_GCC) -c $(^) ${INCLUDES_MOS} ${C_FLAGS_MOS_GCC}

easing_mos: library/easing/src/Back.c library/easing/src/Bounce.c library/easing/src/Circ.c library/easing/src/Cubic.c library/easing/src/Elastic.c\
                library/easing/src/Expo.c library/easing/src/Linear.c library/easing/src/Quad.c library/easing/src/Quart.c library/easing/src/Quint.c\
                library/easing/src/Sine.c
	$(CC) -c $(^) ${INCLUDES_MOS} ${C_FLAGS_MOS}

EASING_OBJECTS = Back.o Bounce.o Circ.o Cubic.o Elastic.o Expo.o Linear.o Quad.o Quart.o Quint.o Sine.o

test_sprite_mos_gcc: library/src/palette.c library/src/events.c library/src/color.c library/src/framebuffer.c library/src/framebuffer_8bit.c library/src/font.c library/src/vertex2d.c\
                     library/src/helper.c library/src_amiga/exdev_base_amiga.c library/src_amiga/helper_amiga.c library/src_amiga/window_amiga.c examples/test_sprite.c
	$(MAKE) easing_mos_gcc
	$(CC_GCC) -o ${@} ${INCLUDES_MOS} $(^) $(EASING_OBJECTS) ${C_FLAGS_MOS_GCC} ${LD_FLAGS_MOS_GCC}
    
test_sprite_mos: library/src/palette.c library/src/events.c library/src/color.c library/src/framebuffer.c library/src/framebuffer_8bit.c library/src/font.c library/src/vertex2d.c\
                 library/src/helper.c library/src_amiga/exdev_base_amiga.c library/src_amiga/helper_amiga.c library/src_amiga/window_amiga.c examples/test_sprite.c
	$(MAKE) easing_mos
	$(CC) -o ${@} ${INCLUDES_MOS} $(^) $(EASING_OBJECTS) ${C_FLAGS_MOS} ${LD_FLAGS_MOS}

#--- julia ---#
julia: julia_mos julia_mos_gcc julia_060

julia_mos: library/src/vertex2d.c library/src/events.c library/src/framebuffer.c library/src/color.c library/src_amiga/window_amiga.c library/src/julia.c library/src/args.c library/src/palette.c\
           library/src_amiga/exdev_base_amiga.c library/src/helper.c examples/julia_main.c
	$(CC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS} ${LD_FLAGS_MOS}

julia_mos_gcc: library/src/vertex2d.c library/src/events.c library/src/framebuffer.c library/src/color.c library/src/framebuffer_8bit.c library/src/framebuffer_rgba.c library/src/color.c library/src/font.c library/src_amiga/window_amiga.c library/src/julia.c library/src/args.c library/src/palette.c\
           library/src_amiga/exdev_base_amiga.c library/src/helper.c examples/julia_main.c
	$(CC_GCC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS_GCC} ${LD_FLAGS_MOS_GCC}

julia_060: library/src/vertex2d.c library/src/events.c library/src/framebuffer.c library/src/color.c library/src_amiga/window_amiga.c library/src/julia.c library/src/args.c library/src/palette.c\
           library/src_amiga/exdev_base_amiga.c library/src/helper.c examples/julia_main.c
	$(CC) -o ${@} ${INCLUDES_AOS} $(^) ${C_FLAGS_060} ${LD_FLAGS_060}

julia_nofpu: library/src/vertex2d.c library/src/events.c library/src/framebuffer.c library/src_amiga/window_amiga.c library/src/julia.c library/src/args.c library/src/palette.c\
           library/src_amiga/exdev_base_amiga.c library/src/helper.c examples/julia_main.c
	$(CC) -o ${@} ${INCLUDES_AOS} $(^) ${C_FLAGS_NO_FPU} ${LD_FLAGS_NO_FPU}

#--- other ---#
other: render_palette_mos render_palette_060
render_palette_mos: library/src/framebuffer_8bit.c library/src/events.c  library/src/framebuffer.c library/src/framebuffer_rgba.c library/src/color.c library/src/vertex2d.c library/src_amiga/window_amiga.c library/src/palette.c\
               library/src/helper.c library/src_amiga/exdev_base_amiga.c library/src/font.c examples/render_palette.c
	$(CC_GCC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS_GCC} ${LD_FLAGS_MOS_GCC}

render_palette_060: library/src/framebuffer_8bit.c library/src/events.c  library/src/framebuffer.c library/src_amiga/window_amiga.c library/src/palette.c\
               library/src/helper.c library/src_amiga/exdev_base_amiga.c library/src/font.c examples/render_palette.c
	$(CC) -o ${@} ${INCLUDES_AOS} $(^) ${C_FLAGS_060} ${LD_FLAGS_060}

#--- test_3d ---#
test_3d: test_3d_mos_gcc test_3d_mos test_3d_060

test_3d_mos_gcc: library/src/framebuffer_8bit.c library/src/events.c  library/src/framebuffer.c library/src/matrix.c library/src_amiga/window_amiga.c library/src/font.c library/src/helper.c library/src_amiga/helper_amiga.c library/src_amiga/exdev_base_amiga.c\
                 library/src/vertex3d.c library/src/vertex2d.c library/src/args.c library/src/sw_renderer_8bit.c library/src/palette.c examples/test_3d.c
	$(CC_GCC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS_GCC} ${LD_FLAGS_MOS_GCC}
    
test_3d_mos: library/src/framebuffer_8bit.c library/src/events.c  library/src/framebuffer.c library/src/matrix.c library/src_amiga/window_amiga.c library/src/font.c library/src/helper.c library/src_amiga/helper_amiga.c library/src_amiga/exdev_base_amiga.c\
             library/src/vertex3d.c library/src/vertex2d.c library/src/args.c library/src/sw_renderer_8bit.c library/src/palette.c examples/test_3d.c
	$(CC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS} ${LD_FLAGS_MOS}
    
test_3d_060: library/src/framebuffer_8bit.c library/src/events.c  library/src/framebuffer.c library/src/matrix.c library/src_amiga/window_amiga.c library/src/font.c library/src/helper.c library/src_amiga/helper_amiga.c library/src_amiga/exdev_base_amiga.c\
             library/src/vertex3d.c library/src/vertex2d.c library/src/args.c library/src/sw_renderer_8bit.c library/src/palette.c examples/test_3d.c
	$(CC) -o ${@} ${INCLUDES_AOS} $(^) ${C_FLAGS_060} ${LD_FLAGS_060} -DLOW_RESOLUTION

#--- dist julia ---#
dist_julia: julia_060 julia_mos julia_nofpu
	$(RM) julia.lha
	lha a ram:julia.lha $(^)

#--- dist voxelspace ---#
dist_voxelspace: voxelspace
	rm -rf ram:voxelspace 
	rm -f ram:voxelspace.lha
	mkdir ram:voxelspace
	mkdir ram:voxelspace/assets
	mkdir ram:voxelspace/screenshots
	cp -av screenshots/voxelspace*.png ram:voxelspace/screenshots
	cp -av examples/README_VOXELSPACE.md ram:voxelspace
	cp -av assets/*palette.dat assets/*height_map.dat assets/*color_map_8bit.dat assets/*sky.dat assets/*sky_lowres.dat ram:voxelspace/assets
	cp -av voxelspace_mos voxelspace_060 voxelspace_060_c2p ram:voxelspace
	cp voxelspace.info ram:voxelspace.info
	lha a -r ram:voxelspace.lha ram:voxelspace ram:voxelspace.info

#--- clean ---#
.PHONY: clean
clean: 
	delete julia#? voxelspace_#? test_sprite#? #?.o

