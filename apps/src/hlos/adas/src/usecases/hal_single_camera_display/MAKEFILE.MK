include $(vision_sdk_PATH)/build/hlos/makerules/linux/common_header_a15.mk

INCLUDE+= $(SGX_DRM_INC) -I$(ipc_PATH)/hlos_common/include -I$(ipc_PATH)/packages -I$(kernel_PATH)/include/generated/uapi $(OSA_INC) -I$(ipc_PATH)/linux/include -I$($(MAKEAPPNAME)_PATH)

#定义模块的名字
MODULE_NAME = hal_single_camera_display

#定义目标文件生成的路径
LIB_BASE_DIR=$(DEST_ROOT)/hal/$(MODULE_NAME)
OBJ_BASE_DIR=$(DEST_ROOT)/hal/obj

#指定静态库
LIBS += $(LIB_BASE_DIR)/$(MODULE_NAME).a

include $(vision_sdk_PATH)/build/hlos/makerules/linux/common_footer_a15.mk
