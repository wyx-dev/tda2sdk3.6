
include $(vision_sdk_PATH)/build/hlos/makerules/linux/common_header_a15.mk

INCLUDE+= $(SGX_DRM_INC) -I$(ipc_PATH)/hlos_common/include -I$(ipc_PATH)/packages -I$(kernel_PATH)/include/generated/uapi $(OSA_INC) -I$(ipc_PATH)/linux/include -I$($(MAKEAPPNAME)_PATH)

#定义模块的名字
MODULE_NAME = hal_can

#定义目标文件生成的路径
LIB_BASE_DIR=$(DEST_ROOT)/hal/$(MODULE_NAME)
OBJ_BASE_DIR=$(DEST_ROOT)/hal/obj
RELEASE_BASE_DIR=$(DEST_ROOT)/hal_release

#指定静态库
LIBS += $(LIB_BASE_DIR)/$(MODULE_NAME).a

#指定动态库编译选项
LD_SO_FLAGS  := -L $(RELEASE_BASE_DIR)
LD_SO_FLAGS  += -lhal_camera
LD_SO_FLAGS  += -lhal_can
LD_SO_FLAGS  += -lhal_sonar

#指定编译动态库目标
hal_can_so:
	@echo \# $(MODULE_NAME): $(PLATFORM): Linking to .so
	-mkdir -p $(RELEASE_BASE_DIR)
	$(LD) $(LD_OPTS) -shared -o $(RELEASE_BASE_DIR)/lib$(MODULE_NAME).so $(OBJ_DIR)/*.o
	cp $(vision_sdk_PATH)/apps/src/hlos/adas/src/usecases/$(MODULE_NAME)/$(MODULE_NAME).h $(RELEASE_BASE_DIR)
	@echo $(LD_SO_FLAGS) > $(DEST_ROOT)/appSolist_$(CORE).txt
	@echo \# Final shared library $(SO_LIB) !!!
	@echo \#

include $(vision_sdk_PATH)/build/hlos/makerules/linux/common_footer_a15.mk
