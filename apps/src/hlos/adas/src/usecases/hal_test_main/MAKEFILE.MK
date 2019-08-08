include $(vision_sdk_PATH)/build/hlos/makerules/linux/common_header_a15.mk

INCLUDE+= $(SGX_DRM_INC) -I$(ipc_PATH)/hlos_common/include -I$(ipc_PATH)/packages -I$(kernel_PATH)/include/generated/uapi $(OSA_INC) -I$(ipc_PATH)/linux/include -I$($(MAKEAPPNAME)_PATH)

#定义模块的名字
MODULE_NAME = hal_test

#定义目标文件生成的路径
LIB_BASE_DIR=$(DEST_ROOT)/hal/$(MODULE_NAME)
OBJ_BASE_DIR=$(DEST_ROOT)/hal/$(MODULE_NAME)/obj
RELEASE_BASE_DIR=$(DEST_ROOT)/hal_release

#指定静态库
LIBS += $(LIB_BASE_DIR)/$(MODULE_NAME).a

#指定编译动态库目标
hal_test_main_exe:
	@echo \# $(MODULE_NAME): $(PLATFORM): Linking to .elf
	-mkdir -p $(RELEASE_BASE_DIR)
	$(AR) -x $(LIB_SRV)
	-mv *.o $(DEST_ROOT)/hal/obj/
	$(AR)	$(AR_OPTS) $(RELEASE_BASE_DIR)/libhal.a $(DEST_ROOT)/hal/obj/*.o
	$(LD) $(LD_OPTS) -o $(RELEASE_BASE_DIR)/$(MODULE_NAME).out $(LIB_BASE_DIR)/$(MODULE_NAME).a $(RELEASE_BASE_DIR)/libhal.a $(LIB_SRV)
	cp $(vision_sdk_PATH)/apps/src/hlos/adas/src/usecases/hal_camera/hal_camera.h $(RELEASE_BASE_DIR)
	cp $(vision_sdk_PATH)/apps/src/hlos/adas/src/usecases/hal_camera/camera.conf $(RELEASE_BASE_DIR)
	cp $(vision_sdk_PATH)/apps/src/hlos/adas/src/usecases/hal_can/hal_can.h $(RELEASE_BASE_DIR)
	#cp $(vision_sdk_PATH)/apps/src/hlos/adas/src/usecases/hal_sonar/hal_sonar.h $(RELEASE_BASE_DIR)
	cp $(vision_sdk_PATH)/apps/src/hlos/adas/src/usecases/hal_arch/hal_arch.h $(RELEASE_BASE_DIR)
	cp $(vision_sdk_PATH)/apps/src/hlos/adas/src/usecases/hal_imu/hal_imu.h $(RELEASE_BASE_DIR)
	#cp $(vision_sdk_PATH)/apps/src/hlos/adas/src/usecases/hal_system/hal_system.h $(RELEASE_BASE_DIR)
	#cp $(vision_sdk_PATH)/apps/src/hlos/adas/src/usecases/hal_warp_affine/hal_warp_affine.h $(RELEASE_BASE_DIR)
	#cp $(vision_sdk_PATH)/apps/src/hlos/adas/src/usecases/hal_match_template/hal_match_template.h $(RELEASE_BASE_DIR)
	#cp $(vision_sdk_PATH)/apps/src/hlos/adas/src/usecases/hal_match_template_classic/hal_match_template_classic.h $(RELEASE_BASE_DIR)
	#cp $(vision_sdk_PATH)/apps/src/hlos/adas/src/usecases/hal_sparse_optical_flow/hal_sparse_optical_flow.h $(RELEASE_BASE_DIR)
	#cp $(vision_sdk_PATH)/apps/src/hlos/adas/src/usecases/hal_modl/hal_modl.h $(RELEASE_BASE_DIR)
	@echo \# Final gen exe hal_main.out !!!
	@echo \#

include $(vision_sdk_PATH)/build/hlos/makerules/linux/common_footer_a15.mk
