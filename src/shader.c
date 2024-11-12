#include "shader.h"
#include "helper.h"

shaderc_compiler_t shaderCompiler;
shaderc_compile_options_t shaderCompileOptions;

extern VkDevice device;

Shader vertex;
Shader fragment;

extern Shader *shaderReferences[];

extern uint32_t shaderCount;

VkDescriptorPool descriptorPool;
VkDescriptorSetLayout descriptorSetLayout;
VkDescriptorSet descriptorSet;

VkPipelineLayout pipelineLayout;

void createDescriptors() {
    VkDescriptorPoolSize poolSize = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1
    };

    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .maxSets = 1,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize
    };

    vkCreateDescriptorPool(device, &poolInfo, NULL, &descriptorPool);
    debug("Descriptor pool created");

    VkDescriptorSetLayoutBinding layoutBinding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = NULL
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = 1,
        .pBindings = &layoutBinding
    };

    vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &descriptorSetLayout);
    debug("Descriptor set layout created");

    VkDescriptorSetAllocateInfo setInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayout
    };

    vkAllocateDescriptorSets(device, &setInfo, &descriptorSet);
    debug("Descriptor set allocated");

    // TODO: Carry it away!
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL
    };

    vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &pipelineLayout);
    debug("Pipeline layout created");
}

void createModule(Shader *shader) {
    debug("Shader: %s", shader->name);

    const char *extension = ".glsl";

    VkShaderStageFlags stage = 0;

    if(shader->kind == shaderc_compute_shader) {
        extension = ".comp";
        stage = VK_SHADER_STAGE_COMPUTE_BIT;
    } else if(shader->kind == shaderc_vertex_shader) {
        extension = ".vert";
        stage = VK_SHADER_STAGE_VERTEX_BIT;
    } else if(shader->kind == shaderc_fragment_shader) {
        extension = ".frag";
        stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    } //TODO: Add other shader types

    char shaderFile[PATH_MAX];
    sprintf(shaderFile, "shaders/%s%s%s", shader->name, extension, shader->intermediate ? ".spv" : "");
    debug("\tPath:          %s", shaderFile);

    if(shader->intermediate) {
        readFile(shaderFile, 1, &shader->size, &shader->data);
        debug("\tSize:          %ld", shader->size);
    } else {
        size_t shaderCodeSize;
        char *shaderCode;

        readFile(shaderFile, 0, &shaderCodeSize, &shaderCode);
        debug("\tCode size:     %ld", shaderCodeSize);

        shaderc_compilation_result_t result = shaderc_compile_into_spv(shaderCompiler, shaderCode, shaderCodeSize - 1, shader->kind, shaderFile, "main", shaderCompileOptions);
        shaderc_compilation_status status = shaderc_result_get_compilation_status(result);

        if(status != shaderc_compilation_status_success) {
            debug("%s", shaderc_result_get_error_message(result));
            shaderc_result_release(result);
            assert(status == shaderc_compilation_status_success);
        }

        debug("\tSuccessfully compiled");

        shader->size = shaderc_result_get_length(result);
        shader->data = malloc(shader->size);

        memcpy(shader->data, shaderc_result_get_bytes(result), shader->size);
        debug("\tCompiled size: %ld", shader->size);

        shaderc_result_release(result);
        free(shaderCode);
    }

    VkShaderCreateInfoEXT shaderCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT,
        .pNext = NULL,
        .flags = 0,
        .stage = stage,
        .nextStage = 0,
        .codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT,
        .codeSize = shader->size,
        .pCode = shader->data,
        .pName = "main",
        .setLayoutCount = 1,
        .pSetLayouts = &descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
        .pSpecializationInfo = NULL
    };

    PFN_vkCreateShadersEXT createShaders = loadFunction("vkCreateShadersEXT");
    assert(createShaders);

    createShaders(device, 1, &shaderCreateInfo, NULL, &shader->module);
    debug("\tSuccessfully created");
}

void createModules() {
    shaderCompiler = shaderc_compiler_initialize();

    // TODO: Add actual compiler options depending on compilation profile
    shaderCompileOptions = shaderc_compile_options_initialize();

    debug("Shader compiler and shader compile options set");

    debug("Shader count: %d", shaderCount);

    for(uint32_t shaderIndex = 0; shaderIndex < shaderCount; shaderIndex++) {
        createModule(shaderReferences[shaderIndex]);
    }

    debug("Shader modules created");
}

void destroyDescriptors() {
    vkDestroyPipelineLayout(device, pipelineLayout, NULL);
    debug("Pipeline layout destroyed");

    vkDestroyDescriptorPool(device, descriptorPool, NULL);
    debug("Descriptor pool destroyed");

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, NULL);
    debug("Descriptor set layout destroyed");
}

void destroyModule(Shader *shader) {
    PFN_vkDestroyShaderEXT destroyShader = loadFunction("vkDestroyShaderEXT");
    assert(destroyShader);
    destroyShader(device, shader->module, NULL);

    free(shader->data);

    debug("Shader module %s destroyed", shader->name);
}

void destroyModules() {
    shaderc_compiler_release(shaderCompiler);
    shaderc_compile_options_release(shaderCompileOptions);

    debug("Shader compiler and shader compile options released");

    for(uint32_t shaderIndex = 0; shaderIndex < shaderCount; shaderIndex++) {
        destroyModule(shaderReferences[shaderIndex]);
    }

    debug("Shader modules destroyed");
}
