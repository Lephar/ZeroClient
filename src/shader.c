#include "shader.h"

#include "helper.h"

extern char *path;
extern VkDevice device;

Shader vertex;
Shader fragment;

Shader createModule(const char *name, shaderc_shader_kind kind) {
    Shader shader = {
        .name = name,
        .kind = kind,
        .file = malloc(PATH_MAX)
    };

    debug("Shader: %s", shader.name);
    debug("\tKind: %d", shader.kind);

    const char *extension = "glsl";

    if(shader.kind == shaderc_compute_shader) {
        extension = "comp";
    } else if(shader.kind == shaderc_vertex_shader) {
        extension = "vert";
    } else if(shader.kind == shaderc_fragment_shader) {
        extension = "frag";
    } //TODO: Add other shader types

    sprintf(shader.file, "%s/shaders/%s.%s.spv", path, shader.name, extension);
    debug("\tPath: %s", shader.file);

    FILE *file = fopen(shader.file, "rb");
    fseek(file, 0, SEEK_END);
    shader.size = ftell(file);
    rewind(file);

    shader.code = malloc(shader.size);
    fread(shader.code, 1, shader.size, file);
    fclose(file);

    debug("\tSize: %ld", shader.size);

    VkShaderModuleCreateInfo shaderInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = shader.size,
        .pCode = shader.code
    };

    vkCreateShaderModule(device, &shaderInfo, NULL, &shader.module);
    debug("Successfully created");

    return shader;
}

void createModules() {
    vertex   = createModule("vertex"  , shaderc_vertex_shader  );
    fragment = createModule("fragment", shaderc_fragment_shader);
}

void destroyModules() {

}