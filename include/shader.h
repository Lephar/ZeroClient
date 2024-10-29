#include "zero.h"

struct shader {
    const char *name;
    shaderc_shader_kind kind;
    char *file;
    size_t size;
    uint32_t *code;
    VkShaderModule module;
} typedef Shader;

void createModules();
void destroyModules();
