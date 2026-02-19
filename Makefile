#--- MorphOs vbcc ---#
CC=vc
C_FLAGS_MOS=-speed -final -DNDEBUG -D__MORPHOS__
LD_FLAGS_MOS=-lm

#--- MorphOs gcc ---#
CC_GCC=ppc-morphos-gcc-11
C_FLAGS_MOS_GCC=-Ofast -noixemul -mcpu=G4 -maltivec -mabi=altivec -DNDEBUG -D__MORPHOS__
LD_FLAGS_MOS_GCC=-lm

INCLUDES_MOS=-Ilibrary/include -Ilibrary/easing/include

#--- AmigaOs 060 ---#
C_FLAGS_060=+aos68k -c99 -fpu=68060 -cpu=68060 -speed -final -DNDEBUG -D__AMIGA__
LD_FLAGS_060=-lm060 -lamiga

#--- AmigaOs 030 ---#
C_FLAGS_030=+aos68k -c99 -cpu=68030 -speed -final -DNDEBUG -D__AMIGA__
LD_FLAGS_030=-lmieee -lamiga

INCLUDES_AOS=-Ilibrary/include -Ilibrary/easing/include

#--- Misc ---#
PREFIX=ram:exdevgfx
JOIN=Mossys:C/join

all: voxelspace_all application_all test_sprite_all julia_all

#--- exdev-gfx ---#
EXDEV_GFX_SOURCES=library/src/args.c\
                  library/src/color.c\
                  library/src/events.c\
                  library/src/font.c\
                  library/src/framebuffer.c\
                  library/src/framebuffer_8bit.c\
                  library/src/framebuffer_rgba.c\
                  library/src/heightmap.c\
                  library/src/helper.c\
                  library/src/julia.c\
                  library/src/matrix.c\
                  library/src/palette.c\
                  library/src/vertex2d.c\
                  library/src/vertex3d.c\
                  library/src/voxelspace.c\
                  library/src_amiga/exdev_base_amiga.c\
                  library/src_amiga/helper_amiga.c\
                  library/src_amiga/window_amiga.c

exdev_gfx_mos_gcc.a: $(EXDEV_GFX_SOURCES)
	$(CC_GCC) -c ${INCLUDES_MOS} ${C_FLAGS_MOS_GCC} $(^)
	$(AR) -r $(@) args.o color.o events.o font.o framebuffer.o framebuffer_8bit.o framebuffer_rgba.o heightmap.o helper.o julia.o matrix.o palette.o vertex2d.o vertex3d.o voxelspace.o exdev_base_amiga.o helper_amiga.o window_amiga.o

exdev_gfx_aos_060.lib: $(EXDEV_GFX_SOURCES)
	$(CC) -c ${INCLUDES_AOS} ${C_FLAGS_060} $(^)
	$(JOIN) as $(@) library/src/args.o library/src/color.o library/src/events.o library/src/font.o library/src/framebuffer.o library/src/framebuffer_8bit.o library/src/framebuffer_rgba.o library/src/heightmap.o library/src/helper.o library/src/julia.o library/src/matrix.o library/src/palette.o library/src/vertex2d.o library/src/vertex3d.o library/src/voxelspace.o library/src_amiga/exdev_base_amiga.o library/src_amiga/helper_amiga.o library/src_amiga/window_amiga.o

exdev_gfx_aos_060_c2p.lib: $(EXDEV_GFX_SOURCES)
	$(CC) -c ${INCLUDES_AOS} -IWork:workspace/c2plib/sdk/C ${C_FLAGS_060} $(^) -DUSE_C2P
	$(JOIN) as $(@) library/src/args.o library/src/color.o library/src/events.o library/src/font.o library/src/framebuffer.o library/src/framebuffer_8bit.o library/src/framebuffer_rgba.o library/src/heightmap.o library/src/helper.o library/src/julia.o library/src/matrix.o library/src/palette.o library/src/vertex2d.o library/src/vertex3d.o library/src/voxelspace.o library/src_amiga/exdev_base_amiga.o library/src_amiga/helper_amiga.o library/src_amiga/window_amiga.o

exdev_gfx_aos_030_c2p.lib: $(EXDEV_GFX_SOURCES)
	$(CC) -c ${INCLUDES_AOS} -IWork:workspace/c2plib/sdk/C ${C_FLAGS_030} $(^) -DUSE_C2P
	$(JOIN) as $(@) library/src/args.o library/src/color.o library/src/events.o library/src/font.o library/src/framebuffer.o library/src/framebuffer_8bit.o library/src/framebuffer_rgba.o library/src/heightmap.o library/src/helper.o library/src/julia.o library/src/matrix.o library/src/palette.o library/src/vertex2d.o library/src/vertex3d.o library/src/voxelspace.o library/src_amiga/exdev_base_amiga.o library/src_amiga/helper_amiga.o library/src_amiga/window_amiga.o


#--- exdev-gfx-ui ---#
EXDEV_GFX_UI_SOURCES=library/src/ui/ui_application.c\
                     library/src/ui/ui_component.c\
                     library/src/ui/ui_component_list.c\
                     library/src/ui/ui_scroll.c\
                     library/src/ui/ui_horizontal_scroll_bar.c\
                     library/src/ui/ui_vertical_scroll_bar.c\
                     library/src/ui/ui_icon.c\
                     library/src/ui/ui_text.c

exdev_gfx_ui_mos_gcc.a: $(EXDEV_GFX_UI_SOURCES)
	$(CC_GCC) -c ${INCLUDES_MOS} ${C_FLAGS_MOS_GCC} $(^)
	$(AR) -r $(@) ui_application.o ui_component.o ui_component_list.o ui_scroll.o ui_horizontal_scroll_bar.o ui_vertical_scroll_bar.o ui_icon.o ui_text.o

exdev_gfx_ui_aos_060.lib: $(EXDEV_GFX_UI_SOURCES)
	$(CC) -c ${INCLUDES_AOS} ${C_FLAGS_060} $(^)
	$(JOIN) as $(@) library/src/ui/ui_application.o library/src/ui/ui_component.o library/src/ui/ui_component_list.o library/src/ui/ui_scroll.o library/src/ui/ui_horizontal_scroll_bar.o library/src/ui/ui_vertical_scroll_bar.o library/src/ui/ui_icon.o library/src/ui/ui_text.o

exdev_gfx_ui_aos_030.lib: $(EXDEV_GFX_UI_SOURCES)
	$(CC) -c ${INCLUDES_AOS} ${C_FLAGS_030} $(^)
	$(JOIN) as $(@) library/src/ui/ui_application.o library/src/ui/ui_component.o library/src/ui/ui_component_list.o library/src/ui/ui_scroll.o library/src/ui/ui_horizontal_scroll_bar.o library/src/ui/ui_vertical_scroll_bar.o library/src/ui/ui_icon.o library/src/ui/ui_text.o


#--- exdev-gfx-easing ---#
EXDEV_GFX_EASING_SOURCES=library/easing/src/Back.c\
                         library/easing/src/Bounce.c\
                         library/easing/src/Circ.c\
                         library/easing/src/Cubic.c\
                         library/easing/src/Elastic.c\
                         library/easing/src/Expo.c\
                         library/easing/src/Linear.c\
                         library/easing/src/Quad.c\
                         library/easing/src/Quart.c\
                         library/easing/src/Quint.c\
                         library/easing/src/Sine.c

exdev_gfx_easing_mos_gcc.a: $(EXDEV_GFX_EASING_SOURCES)
	$(CC_GCC) -c ${INCLUDES_MOS} ${C_FLAGS_MOS_GCC} $(^)
	$(AR) -r $(@) Back.o Bounce.o Circ.o Cubic.o Elastic.o Expo.o Linear.o Quad.o Quart.o Quint.o Sine.o

exdev_gfx_easing_aos_060.lib: $(EXDEV_GFX_EASING_SOURCES)
	$(CC) -c ${INCLUDES_AOS} ${C_FLAGS_060} $(^)
	$(JOIN) as $(@) library/easing/src/Back.o library/easing/src/Bounce.o library/easing/src/Circ.o library/easing/src/Cubic.o library/easing/src/Elastic.o library/easing/src/Expo.o library/easing/src/Linear.o library/easing/src/Quad.o library/easing/src/Quart.o library/easing/src/Quint.o library/easing/src/Sine.o

#--- application ---#
application_all: application_mos_gcc application_060

application_mos_gcc: examples/test_application.c exdev_gfx_ui_mos_gcc.a exdev_gfx_mos_gcc.a
	$(CC_GCC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS_GCC} ${LD_FLAGS_MOS_GCC}

application_060: examples/test_application.c exdev_gfx_ui_aos_060.lib exdev_gfx_aos_060.lib
	$(CC) -o ${@} ${INCLUDES_AOS} $(^) ${C_FLAGS_060} ${LD_FLAGS_060}

#--- voxelspace ---#
voxelspace_all: voxelspace_mos_gcc voxelspace_060 voxelspace_060_c2p voxelspace_030_c2p

voxelspace_mos_gcc: examples/voxelspace_main.c exdev_gfx_mos_gcc.a
	$(CC_GCC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS_GCC} ${LD_FLAGS_MOS_GCC}

voxelspace_060: examples/voxelspace_main.c exdev_gfx_aos_060.lib
	$(CC) -o ${@} ${INCLUDES_AOS} $(^) ${C_FLAGS_060} ${LD_FLAGS_060} -DLOW_RESOLUTION

voxelspace_060_c2p: examples/voxelspace_main.c exdev_gfx_aos_060_c2p.lib
	$(CC) -o ${@} ${INCLUDES_AOS} -IWork:workspace/c2plib/sdk/C $(^) ${C_FLAGS_060} ${LD_FLAGS_060} -LWork:workspace/c2plib/sdk -lc2p -DLOW_RESOLUTION -DUSE_C2P

voxelspace_030_c2p: examples/voxelspace_main.c exdev_gfx_aos_030_c2p.lib
	$(CC) -o ${@} ${INCLUDES_AOS} -IWork:workspace/c2plib/sdk/C $(^) ${C_FLAGS_030} ${LD_FLAGS_030} -LWork:workspace/c2plib/sdk -lc2p -DLOW_RESOLUTION -DUSE_C2P


#--- test sprite ---#
test_sprite_all: test_sprite_mos_gcc test_sprite_060

test_sprite_mos_gcc: examples/test_sprite.c exdev_gfx_easing_mos_gcc.a exdev_gfx_mos_gcc.a
	$(CC_GCC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS_GCC} ${LD_FLAGS_MOS_GCC}

test_sprite_060: examples/test_sprite.c exdev_gfx_easing_aos_060.lib exdev_gfx_aos_060.lib
	$(CC) -o ${@} ${INCLUDES_AOS} $(^) ${C_FLAGS_060} ${LD_FLAGS_060}

#--- julia ---#
julia_all: julia_mos_gcc

julia_mos_gcc: examples/julia_main.c exdev_gfx_mos_gcc.a
	$(CC_GCC) -o ${@} ${INCLUDES_MOS} $(^) ${C_FLAGS_MOS_GCC} ${LD_FLAGS_MOS_GCC}

#--- dist voxelspace ---#
dist_voxelspace: voxelspace_all
	rm -rf ram:voxelspace
	rm -f ram:voxelspace.lha
	mkdir ram:voxelspace
	mkdir ram:voxelspace/assets
	mkdir ram:voxelspace/screenshots
	cp -av screenshots/voxelspace*.png ram:voxelspace/screenshots
	cp -av examples/voxelspace.readme ram:voxelspace
	cp -av assets/*color_map_palette.dat assets/*height_map.dat assets/*color_map_8bit.dat assets/*_sky.dat assets/*_sky_lowres.dat ram:voxelspace/assets
	cp -av voxelspace_mos_gcc voxelspace_060 voxelspace_060_c2p voxelspace_030_c2p ram:voxelspace
	cp examples/voxelspace.info ram:voxelspace.info
	lha a -r ram:voxelspace.lha ram:voxelspace ram:voxelspace.info

#--- clean ---#
.PHONY: clean
clean:
	$(RM) -f julia_* voxelspace_* application_* test_sprite_* *.o library/easing/src/*.o *.lib *.a library/src/*.o library/src_amiga/*.o
