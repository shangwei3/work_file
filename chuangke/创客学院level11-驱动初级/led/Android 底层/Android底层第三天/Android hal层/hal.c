int hw_get_module(char *id,const struct hw_module_t **module);
        |
        |
struct hw_module_t : 描述HAL层一个硬件对象
{
	  char *id;//模块的唯一标识，通过它可以找到这个模块
	  
    struct hw_module_methods_t 
    {
          open =  xxx_open
                    |
                    |
                    |
                    struct hw_device_t{ //硬件操作函数接口
                       xxx_cose
                    }	
    }	
}	


思考:模块id,怎么确定呢?
	
-----------------------------------------------------------------------------------------------------------
struct my_module_t{
	hw_module_t   common;
	
	other info;
};

struct my_module_t HAL_MODULE_INFO_SYM; // HMI , 全局



struct my_hw_device_t{
	hw_device_t common;
	other info;
};

--------------------------------------------------------------------------------------------------------------------


假如 module ID ： led

struct hw_module_t  *led_module;


ret = hw_get_module("led",&led_module);
      |
			|
int hw_get_module(const char *id, const struct hw_module_t **module)
{
    return hw_get_module_by_class(id, NULL, module);
}
			|
			|

@class_id : "led"	
@inst     :  NULL 
@module   :  addr 		
int hw_get_module_by_class(const char *class_id, const char *inst,
                           const struct hw_module_t **module)
{
    int i;
    char prop[PATH_MAX];//property
    char path[PATH_MAX];
    char name[PATH_MAX];
    char prop_name[PATH_MAX];

     
	 //name : "led"
	 strlcpy(name, class_id, PATH_MAX);//解决了越界和'\0'问题      
	 
	 /*
	 char *strcpy(char *dest,const char *src)  bug:源字符串如果比目标长，就会目标越界
	 char *strncpy(char *dest, const char *src, size_t n); bug:解决了目标越界问题，但是如果拷贝的n个字节中不包括
	                                                           '\0',最终目标中也没有'\0',会导致目标不是一个字符串
  */

    /*
     * Here we rely on the fact that calling dlopen multiple times on
     * the same .so will simply increment a refcount (and not load
     * a new copy of the library).
     * We also assume that dlopen() is thread-safe.
     */

    /* First try a property specific to the class and possibly instance */
    snprintf(prop_name, sizeof(prop_name), "ro.hardware.%s", name);
    //prop_name: "ro.hardware.led"
	
	//获取ro.hardware.led属性的值
	if (property_get(prop_name, prop, NULL) > 0) {
					
				//假设属性存在，值为fs4412-led
				//name : led
				//prop : fs4412-led
        if (hw_module_exists(path, sizeof(path), name, prop) == 0) {
            goto found;
        }
    }
	
	
	/
	
	 static const char *variant_keys[] = {
		"ro.hardware",  /* This goes first so that it can pick up a different
						   file on the emulator. */
		"ro.product.board",
		"ro.board.platform",
		"ro.arch"
		 }
	

    /* Loop through the configuration variants looking for a module */
    for (i=0 ; i<HAL_VARIANT_KEYS_COUNT; i++) {
		//ro.hardware  : sun8i
		//ro.product.board : exdroid
		//ro.board.platform : astar
        if (property_get(variant_keys[i], prop, NULL) == 0) {
            continue;
        }
				
				/system/lib/hw/led.sun8i.so
		                   led.exdroid.so
		                   led.astar.so
		                   
		    /vendor/lib/hw/....               
        if (hw_module_exists(path, sizeof(path), name, prop) == 0) {
            goto found;
        }
    }

    /* Nothing found, try the default */
	//led.default.so
	//path:/system/lib/hw/led.default.so
    if (hw_module_exists(path, sizeof(path), name, "default") == 0) {
        goto found;
    }

    return -ENOENT;

found:
    /* load the module, if this fails, we're doomed, and we should not try
     * to load a different variant. */
    return load(class_id, path, module);
}


如何能正确找到hal层库


(1)hal 库有效的命名: 

id.default.so 
id.exdroid.so 
id.astr.so 
id.sun8i.so

(2)module id 要正确
  实际上就是动态库的名字


/*
 * Check if a HAL with given name and subname exists, if so return 0, otherwise
 * otherwise return negative.  On success path will contain the path to the HAL.
 */
static int hw_module_exists(char *path, size_t path_len, const char *name, //
                            const char *subname)
                            
@name      led
@subname   fs4412-led
{
	
	#define HAL_LIBRARY_PATH1 "/system/lib/hw"
	#define HAL_LIBRARY_PATH2 "/vendor/lib/hw"

    snprintf(path, path_len, "%s/%s.%s.so",
             HAL_LIBRARY_PATH2, name, subname); /vendor/lib/hw/led.fs4412-led.so
    if (access(path, R_OK) == 0)
        return 0;

    snprintf(path, path_len, "%s/%s.%s.so",
             HAL_LIBRARY_PATH1, name, subname);  /system/lib/hw/led.fs4412-led.so  
    if (access(path, R_OK) == 0)
        return 0;

    return -ENOENT;
}

---------------------------------------------------------------------------------------------------
HAL 层开发 (在应用层调用驱动的函数接口，向上层提供统一的硬件操作函数接口)

1.定义结构体
struct hw_module_t  HMI;

2.对这个结构体成员做初始化

struct hw_module_methods_t
{
    .open =  xxx_open	
}

3.xxx_open 通过它的参数带回一个结构体:struct hw_device_t (提供硬件操作函数接口)
	
	
4.生成一个动态库文件







