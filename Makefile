# --- mos vbcc ---#
CC_VC=vc
C_FLAGS_MOS_FPU_VC=-O3 -speed -final -DNDEBUG -D__MORPHOS__
C_FLAGS_MOS_NOFPU_VC=-O3 -speed -final -DNDEBUG -D__MORPHOS__ -DEXDEV_FP_MATH
LD_FLAGS_MOS_FPU_VC=-lm
LD_FLAGS_MOS_NO_FPU_VC=

#--- mos gcc ---#
CC_GCC=ppc-morphos-gcc-11
C_FLAGS_MOS_FPU_GCC=-Ofast -noixemul -mcpu=G4 -maltivec -mabi=altivec -DNDEBUG -D__MORPHOS__
C_FLAGS_MOS_NO_FPU_GCC=-Ofast -noixemul -mcpu=G4 -maltivec -mabi=altivec -DNDEBUG -D__MORPHOS__ -DEXDEV_FP_MATH
LD_FLAGS_MOS_FPU_GCC=-lm
LD_FLAGS_MOS_NO_FPU_GCC=

#--- aos vc ---#
CC_VC=vc
C_FLAGS_060_FPU_VC=+aos68k -fpu=68060 -cpu=68060 -O3 -speed -final -DNDEBUG -D__AMIGA__ -DLOW_RESOLUTION
C_FLAGS_060_NO_FPU_VC=+aos68k -fpu=0 -cpu=68060 -O3 -speed -final -DNDEBUG -D__AMIGA__ -DEXDEV_FP_MATH -DLOW_RESOLUTION
LD_FLAGS_060_FPU_VC=-lm060
LD_FLAGS_060_NO_FPU_VC=

#--- includes ---#
INCLUDES_MOS_FPU=-Ilibrary/include
INCLUDES_MOS_NO_FPU=-Ilibrary/include -Ilibrary/fpmath/include

INCLUDES_060_FPU=-Ilibrary/include
INCLUDES_060_NO_FPU=-Ilibrary/include -Ilibrary/fpmath/include

PREFIX=ram:

all: voxelspace

#--- voxelspace ---#
voxelspace: voxelspace_mos_ppc_fpu_gcc voxelspace_mos_ppc_nofpu_gcc voxelspace_mos_ppc_fpu_vc voxelspace_mos_ppc_nofpu_vc voxelspace_aos_060_fpu_vc voxelspace_aos_060_nofpu_vc

#--- mos gcc ---#
voxelspace_mos_ppc_fpu_gcc: library/src/vertex3d.c library/src/events.c library/src/vertex2d.c library/src/matrix.c library/src/palette.c library/src/color.c library/src/framebuffer.c library/src/framebuffer_8bit.c library/src_amiga/window_amiga.c library/src/font.c library/src/heightmap.c\
                library/src/voxelspace.c library/src/args.c library/src/helper.c library/src_amiga/exdev_base_amiga.c library/src_amiga/helper_amiga.c examples/voxelspace_main.c
	$(CC_GCC) -o ${@} ${INCLUDES_MOS_FPU} $(^) ${C_FLAGS_MOS_FPU_GCC} ${LD_FLAGS_MOS_FPU_GCC}

voxelspace_mos_ppc_nofpu_gcc: library/src/vertex3d.c library/src/events.c library/src/vertex2d.c library/src/matrix.c library/src/palette.c library/src/color.c library/src/framebuffer.c library/src/framebuffer_8bit.c library/src_amiga/window_amiga.c library/src/font.c library/src/heightmap.c\
                library/src/voxelspace.c library/src/args.c library/src/helper.c library/src_amiga/exdev_base_amiga.c library/src_amiga/helper_amiga.c examples/voxelspace_main.c
	$(CC_GCC) -o ${@} ${INCLUDES_MOS_NO_FPU} $(^) ${C_FLAGS_MOS_NO_FPU_GCC} ${LD_FLAGS_MOS_NO_FPU_GCC}

# --- mos vbcc ---#
voxelspace_mos_ppc_fpu_vc: library/src/vertex3d.c library/src/events.c library/src/vertex2d.c library/src/matrix.c library/src/palette.c library/src/color.c library/src/framebuffer.c library/src/framebuffer_8bit.c library/src_amiga/window_amiga.c library/src/font.c library/src/heightmap.c\
                library/src/voxelspace.c library/src/args.c library/src/helper.c library/src_amiga/exdev_base_amiga.c library/src_amiga/helper_amiga.c examples/voxelspace_main.c
	$(CC_VC) -o ${@} ${INCLUDES_MOS_FPU} $(^) ${C_FLAGS_MOS_FPU_VC} ${LD_FLAGS_MOS_FPU_VC}

voxelspace_mos_ppc_nofpu_vc: library/src/vertex3d.c library/src/events.c library/src/vertex2d.c library/src/matrix.c library/src/palette.c library/src/color.c library/src/framebuffer.c library/src/framebuffer_8bit.c library/src_amiga/window_amiga.c library/src/font.c library/src/heightmap.c\
                library/src/voxelspace.c library/src/args.c library/src/helper.c library/src_amiga/exdev_base_amiga.c library/src_amiga/helper_amiga.c examples/voxelspace_main.c
	$(CC_VC) -o ${@} ${INCLUDES_MOS_NO_FPU} $(^) ${C_FLAGS_MOS_NOFPU_VC} ${LD_FLAGS_MOS_NO_FPU_VC}

# --- aos 060 vbcc ---#
voxelspace_aos_060_fpu_vc: library/src/vertex3d.c library/src/events.c library/src/vertex2d.c library/src/matrix.c library/src/palette.c library/src/color.c library/src/framebuffer.c library/src/framebuffer_8bit.c library/src_amiga/window_amiga.c library/src/font.c library/src/heightmap.c\
                library/src/voxelspace.c library/src/args.c library/src/helper.c library/src_amiga/exdev_base_amiga.c library/src_amiga/helper_amiga.c examples/voxelspace_main.c
	$(CC_VC) -o ${@} ${INCLUDES_060_FPU} $(^) ${C_FLAGS_060_FPU_VC} ${LD_FLAGS_060_FPU_VC}

voxelspace_aos_060_nofpu_vc: library/src/vertex3d.c library/src/events.c library/src/vertex2d.c library/src/matrix.c library/src/palette.c library/src/color.c library/src/framebuffer.c library/src/framebuffer_8bit.c library/src_amiga/window_amiga.c library/src/font.c library/src/heightmap.c\
                library/src/voxelspace.c library/src/args.c library/src/helper.c library/src_amiga/exdev_base_amiga.c library/src_amiga/helper_amiga.c examples/voxelspace_main.c
	$(CC_VC) -o ${@} ${INCLUDES_060_NO_FPU} $(^) ${C_FLAGS_060_NO_FPU_VC} ${LD_FLAGS_060_NO_FPU_VC}

#--- dist voxelspace ---#
dist_voxelspace: voxelspace
	rm -rf ${PREFIX}voxelspace
	rm -f ${PREFIX}voxelspace.lha
	MakeDir ${PREFIX}voxelspace
	MakeDir ${PREFIX}voxelspace/assets
	Copy examples/README_VOXELSPACE.md ${PREFIX}voxelspace
	Copy assets/#?palette.dat assets/#?height_map.dat assets/#?color_map_8bit.dat assets/#?sky.dat assets/#?sky_lowres.dat ${PREFIX}voxelspace/assets
	Copy voxelspace#? ${PREFIX}voxelspace
	lha a -r ${PREFIX}voxelspace.lha ${PREFIX}voxelspace

#--- clean ---#
.PHONY: clean
clean: 
	delete voxelspace#? #?.o
