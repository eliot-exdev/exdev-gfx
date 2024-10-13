C_FLAGS_MOS=-O4 -speed -final -DNDEBUG -D__AMIGA__
C_FLAGS_MOS_GCC=-Ofast -noixemul -faltivec -mabi=altivec -DNDEBUG -D__AMIGA__

C_FLAGS_060=+aos68k -c99 -fpu=68060 -cpu=68060 -O4 -speed -final -DNDEBUG -D__AMIGA__
C_FLAGS_060_GCC=-O4 -noixemul -DNDEBUG -march=68060 -mcpu=68060 -mtune=68060 -D__AMIGA__
C_FLAGS_NO_FPU=+aos68k -c99 -cpu=68020 -O4 -speed -final -DNDEBUG -D__AMIGA__

LD_FLAGS_MOS=-lm -lamiga
LD_FLAGS_MOS_GCC=-lm

LD_FLAGS_060=-lm060 -lamiga
LD_FLAGS_060_GCC=-lm
LD_FLAGS_NO_FPU=-lmsoft -lamiga

CC=vc
CC_GCC=ppc-morphos-gcc-10


PREFIX=ram:exdevgfx

INCLUDES_MOS=-Iinclude
INCLUDES_AOS=-Iinclude -IWork:workspace/CGraphX/C/Include

all: voxelspace julia other

#--- voxelspace ---#
voxelspace: voxelspace_mos voxelspace_mos_gcc voxelspace_060

voxelspace_mos: src/vertex3d.c src/matrix.c src/palette.c src/framebuffer.c src/framebuffer_8bit.c src_amiga/window_amiga.c src/font.c src/heightmap.c\
                src/voxelspace.c src/args.c src/helper.c src_amiga/exdev_base_amiga.c src_amiga/helper_amiga.c examples/voxelspace_main.c
	$(CC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS} ${LD_FLAGS_MOS}
    
voxelspace_mos_gcc: src/vertex3d.c src/matrix.c src/palette.c src/framebuffer_8bit.c src/framebuffer.c src/vertex2d.c src_amiga/window_amiga.c src/font.c src/heightmap.c\
                    src/voxelspace.c src/args.c src/helper.c src_amiga/exdev_base_amiga.c src_amiga/helper_amiga.c examples/voxelspace_main.c
	$(CC_GCC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS_GCC} ${LD_FLAGS_MOS_GCC}

voxelspace_060: src/vertex3d.c src/matrix.c src/palette.c src/framebuffer_8bit.c src_amiga/window_amiga.c src/font.c src/heightmap.c\
                src/voxelspace.c src/args.c src/helper.c src_amiga/exdev_base_amiga.c src_amiga/helper_amiga.c examples/voxelspace_main.c
	$(CC) -o ${@} ${INCLUDES_AOS} $(^) ${C_FLAGS_060} ${LD_FLAGS_060} -DLOW_RESOLUTION

#--- julia ---#
julia: julia_mos julia_060 julia_nofpu

julia_mos: src/vertex2d.c src/framebuffer.c src_amiga/window_amiga.c src/julia.c src/args.c src/palette.c\
           src_amiga/exdev_base_amiga.c src/helper.c examples/julia_main.c
	$(CC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS} ${LD_FLAGS_MOS}

julia_mos_gcc: src/vertex2d.c src/framebuffer.c src_amiga/window_amiga.c src/julia.c src/args.c src/palette.c\
               src_amiga/exdev_base_amiga.c src/helper.c examples/julia_main.c
	$(CC_GCC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS_GCC} ${LD_FLAGS_MOS_GCC}

julia_060: src/vertex2d.c src/framebuffer.c src_amiga/window_amiga.c src/julia.c src/args.c src/palette.c\
           src_amiga/exdev_base_amiga.c src/helper.c examples/julia_main.c
	$(CC) -o ${@} ${INCLUDES_AOS} $(^) ${C_FLAGS_060} ${LD_FLAGS_060}

julia_nofpu: src/vertex2d.c src/framebuffer.c src_amiga/window_amiga.c src/julia.c src/args.c src/palette.c\
             src_amiga/exdev_base_amiga.c src/helper.c examples/julia_main.c
	$(CC) -o ${@} ${INCLUDES_AOS} $(^) ${C_FLAGS_NO_FPU} ${LD_FLAGS_NO_FPU}

#--- other ---#
other: render_palette_mos render_palette_060
render_palette_mos: src/framebuffer_8bit.c src/framebuffer.c src_amiga/window_amiga.c src/palette.c\
               src/helper.c src_amiga/exdev_base_amiga.c examples/render_palette.c
	$(CC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS} ${LD_FLAGS_MOS}

render_palette_060: src/framebuffer_8bit.c src/framebuffer.c src_amiga/window_amiga.c src/palette.c\
               src/helper.c src_amiga/exdev_base_amiga.c examples/render_palette.c
	$(CC) -o ${@} ${INCLUDES_AOS} $(^) ${C_FLAGS_060} ${LD_FLAGS_060}

#--- test_3d ---#
test_3d: test_3d_mos_gcc test_3d_mos test_3d_060

test_3d_mos_gcc: src/framebuffer_8bit.c src/framebuffer.c src/matrix.c src_amiga/window_amiga.c src/font.c src/helper.c  src_amiga/helper_amiga.c src_amiga/exdev_base_amiga.c\
                 src/vertex3d.c src/vertex2d.c src/args.c src/sw_renderer_8bit.c src/palette.c examples/test_3d.c
	$(CC_GCC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS_GCC} ${LD_FLAGS_MOS_GCC}
    
test_3d_mos: src/framebuffer_8bit.c src/framebuffer.c src/matrix.c src_amiga/window_amiga.c src/font.c src/helper.c  src_amiga/helper_amiga.c src_amiga/exdev_base_amiga.c\
             src/vertex3d.c src/vertex2d.c src/args.c src/sw_renderer_8bit.c src/palette.c examples/test_3d.c
	$(CC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS} ${LD_FLAGS_MOS}
    
test_3d_060: src/framebuffer_8bit.c src/framebuffer.c src/matrix.c src_amiga/window_amiga.c src/font.c src/helper.c  src_amiga/helper_amiga.c src_amiga/exdev_base_amiga.c\
             src/vertex3d.c src/vertex2d.c src/args.c src/sw_renderer_8bit.c src/palette.c examples/test_3d.c
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
	cp -av examples/README_VOXELSPACE.md ram:voxelspace
	cp -av assets/*palette.dat assets/*height_map.dat assets/*color_map_8bit.dat ram:voxelspace/assets
	cp -av voxelspace_mos voxelspace_mos_gcc voxelspace_060 ram:voxelspace
	lha a -r ram:voxelspace.lha ram:voxelspace

#--- clean ---#
.PHONY: clean
clean: 
	delete julia#? voxelspace#?

